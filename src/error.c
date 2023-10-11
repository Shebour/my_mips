#include "error.h"

#include <err.h>
#include <stdarg.h>

enum error error_warn(enum error err, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  vwarn(fmt, ap);

  va_end(ap);
  return err;
}

enum error error_warnx(enum error err, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  vwarnx(fmt, ap);

  va_end(ap);
  return err;
}
