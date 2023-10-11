#define _POSIX_C_SOURCE 200809L

#include <err.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>

// readline's header requires including stdio beforehand
#include <readline/readline.h>

#include "attributes.h"
#include "error.h"
/*
** This mess must be the worst part of the whole project.
** Here's what the following code does:
**  - Set a flag when Ctrl+C occurs by catching SIGINT.
**  - Use the asynchronous readline API, wait for key presses using select
**  - call rl_callback_read_char() when needed, which will call process_line
**    when a full line was read
**  - periodically check for the flag. When present, reset readline, print ^C
*and redraw the prompt.
*/

/** When the user presses CTRL + C, this flag is set by the signal handler */
static volatile bool interrupted;

/** The newly completed line (NULL encodes CTRL + D) */
static char *user_input;
static bool user_input_complete;

/** This is called by readline when it has read an entire line */
static void readline_callback(char *line)
{
  user_input = line;
  user_input_complete = true;
  rl_callback_handler_remove();
}

/** Checks whether user input was interupted */
static enum error check_interrupt() __warn_unused;

enum error interruptible_readline(char *prompt, char **line)
{
  int rc;
  enum error error;

  fd_set rfds;
  FD_ZERO(&rfds);
  FD_SET(fileno(stdin), &rfds);

  interrupted = false;
  rl_callback_handler_install(prompt, readline_callback);
  free(prompt);

  while (true)
  {
    // wait for data to be available on stdin. if the user presses CTRL + C,
    // select will instead fail and errno be set to EINTR
    if ((rc = select(1, &rfds, NULL, NULL, NULL)) < 0)
    {
      if (errno == EAGAIN)
        continue;

      if (errno == EINTR)
      {
        if ((error = check_interrupt()))
          return error;
        continue;
      }

      return error_warn(ERROR, "select failed in interruptible_readline");
    }

    rl_callback_read_char();

    // if the user completed the line,
    if (user_input_complete)
    {
      user_input_complete = false;
      *line = user_input;
      return NO_ERROR;
    }

    if ((error = check_interrupt()))
      return error;
  }
}

static void sigint_handler(int signal __unused)
{
  interrupted = true;
}

void interruptible_readline_setup(void)
{
  signal(SIGINT, sigint_handler);
}

static enum error check_interrupt()
{
  if (!interrupted)
    return NO_ERROR;
  interrupted = false;

  // reset the prompt, eventhough the user input isn't complete
  rl_free_line_state();
  rl_cleanup_after_signal();
  RL_UNSETSTATE(RL_STATE_ISEARCH | RL_STATE_NSEARCH | RL_STATE_VIMOTION
                | RL_STATE_NUMERICARG | RL_STATE_MULTIKEY);
  rl_done = 1;
  rl_callback_handler_remove();

  // let the user know what happened
  fputs("^C\n", stderr);

  return KEYBOARD_INTERUPT;
}
