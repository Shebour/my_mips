#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cstream.h"
#include "interruptible_readline.h"
#include "utils.h"

struct cstream_readline
{
  struct cstream base;

  bool line_start;

  char *current_line;
  // offset in current_line
  size_t line_position;
};

static char *prompt_get(struct cstream_readline *cs)
{
  if (cs->line_start)
    return strdup("dbg$ ");
  return strdup("> ");
}

static enum error cstream_readline_read(struct cstream *base_cs, int *c)
{
  enum error err;
  struct cstream_readline *cs = (struct cstream_readline *)base_cs;
  char *str = cs->current_line;

  if (!str)
  {
    char *prompt = prompt_get(cs);
    if ((err = interruptible_readline(prompt, &cs->current_line)))
      return err;
    str = cs->current_line;
    cs->line_position = 0;
  }

  if (str && str[cs->line_position] == '\0')
  {
    free(cs->current_line);
    cs->current_line = NULL;
    *c = '\n';
    return NO_ERROR;
  }

  if (str == NULL)
  {
    *c = EOF;
    return NO_ERROR;
  }

  /* using an unsigned char is required to avoid sign extension */
  unsigned char res = str[cs->line_position];
  if (res == '\0')
  {
    *c = EOF;
    return NO_ERROR;
  }

  cs->line_position++;
  *c = res;
  return NO_ERROR;
}

static enum error cstream_readline_free(struct cstream *cstream_base)
{
  struct cstream_readline *cs = (struct cstream_readline *)cstream_base;
  free(cs->current_line);
  free(cs);
  return NO_ERROR;
}

static void cstream_readline_reset(struct cstream *base_cs)
{
  struct cstream_readline *cs = (struct cstream_readline *)base_cs;
  free(cs->current_line);
  cs->current_line = NULL;
  cs->line_position = 0;
  cs->line_start = true;
}

static const struct cstream_type cstream_readline_type = {
  .read = cstream_readline_read,
  .free = cstream_readline_free,
  .reset = cstream_readline_reset,
  .interactive = true,
};

struct cstream *cstream_readline_create()
{
  struct cstream_readline *cstream = malloc(sizeof(*cstream));
  cstream->base.type = &cstream_readline_type;
  cstream->current_line = NULL;
  cstream->line_position = 0;
  cstream->line_start = true;
  interruptible_readline_setup();
  return &cstream->base;
}
