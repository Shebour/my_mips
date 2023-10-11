#include "cstream.h"

static inline enum error cstream_read(struct cstream *cstream, int *c)
{
  return cstream->type->read(cstream, c);
}

enum error cstream_peek(struct cstream *cstream, int *c)
{
  // If the next character in the stream is unknown, read it into the buffer
  if (!cstream->has_buffer)
  {
    enum error err;
    if ((err = cstream_read(cstream, &cstream->buffer)))
      return err;
    cstream->has_buffer = true;
  }

  // Otherwise, return the existing one
  *c = cstream->buffer;
  return NO_ERROR;
}

enum error cstream_pop(struct cstream *cstream, int *c)
{
  // If there if previously read character, read a new one
  if (!cstream)
    return ERROR;
  if (!cstream->has_buffer)
    return cstream_read(cstream, c);

  // Otherwise, return the buffer
  *c = cstream->buffer;
  cstream->has_buffer = false;
  return NO_ERROR;
}

enum error cstream_free(struct cstream *cstream)
{
  return cstream->type->free(cstream);
}
