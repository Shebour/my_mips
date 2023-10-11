#pragma once

#include <stdio.h>

/**
 * \page errors
 *
 * Usualy, error handling in C is performed using integer codes:
 * If a function performs as expected, it returns 0. Otherwise, it returns
 * an error code.
 *
 * This error code usualy contains information about what kind of error occured.
 *
 * This pattern has many variants:
 *  - linux syscalls return a negative value on error, which encodes the error
 *  - the C standard library returns -1 on error, and stores the exact error
 * code in a "global" variable
 */

enum error
{
  // when a function returns NO_ERROR, everything went well.
  // because NO_ERROR == 0, testing if an error occured is easy
  NO_ERROR = 0,

  // these errors signal something went wrong in
  // one of these modules
  ERROR,

  // these aren't really errors, but behave in a similar way:
  // they interrupt normal program operation. The only difference
  // with other error kinds is that they are part of normal.
  KEYBOARD_INTERUPT,
  BREAK_INTERUPT,
  CONTINUE_INTERUPT,
  EXIT_INTERUPT,
};

/** \brief Calls warn and return err */
enum error error_warn(enum error err, const char *fmt, ...);

/** \brief Calls warnx and return err */
enum error error_warnx(enum error err, const char *fmt, ...);
