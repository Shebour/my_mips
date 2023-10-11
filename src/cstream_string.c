#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "attributes.h"
#include "cstream.h"

struct cstream_string
{
  struct cstream base;
  const char *str;
};

static enum error cstream_string_read(struct cstream *cstream_base, int *c)
{
  struct cstream_string *cstream = (struct cstream_string *)cstream_base;
  if (*cstream->str == '\0')
  {
    *c = EOF;
  }
  else
  {
    *c = *cstream->str;
    cstream->str++;
  }
  return NO_ERROR;
}

static enum error cstream_string_free(struct cstream *cstream_base __unused)
{
  return NO_ERROR;
}

static const struct cstream_type cstream_string_type = {
  .read = cstream_string_read,
  .free = cstream_string_free,
  .reset = NULL,
  .interactive = false,
};

struct cstream *cstream_string_create(const char *str)
{
  struct cstream_string *cstream = calloc(1, sizeof(*cstream));
  cstream->base.type = &cstream_string_type;
  cstream->str = str;
  return &cstream->base;
}
