#pragma once

#include <stdbool.h>

#include "attributes.h"
#include "error.h"

/**
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * !!!!  Please read the inheritance and dynamic dispatch  !!!
 * !!!!          sections of the project guide             !!!
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */

/**
 * This is a promise that cstream_type will be defined later on (a forward
 * declaration). It is needed because two types are interdependant:
 *  - cstream has a type field
 *  - cstream_type references cstream in method arguments
 */
struct cstream_type;

struct cstream
{
  /** Points to a structure which describes the stream implementation. */
  const struct cstream_type *type;
  /** When this boolean is true, buffer contains the next character in the
   * stream */
  bool has_buffer;
  /** The next character in the stream, if has_buffer is true */
  int buffer;
};

struct cstream_type
{
  /** Reads the next character from the stream, storing it to c */
  enum error (*read)(struct cstream *stream, int *c);
  /** Releases all resources associated with the stream */
  enum error (*free)(struct cstream *stream);
  /** If the stream is interactive, this function discards previously read
   * user input */
  void (*reset)(struct cstream *stream);
  bool interactive;
};

/**
 * \brief Returns the next character, without advancing the stream.
 * Calling this function multiple times in a row returns the same thing.
 */
enum error cstream_peek(struct cstream *cstream, int *c) __warn_unused;

/**
 * \brief Returns the next character and advances the stream.
 * Calling this function in a loop until EOF reads all the stream.
 */
enum error cstream_pop(struct cstream *cstream, int *c) __warn_unused;

/** \brief Releases all resources associated with the stream */
enum error cstream_free(struct cstream *cstream);

/**
 * \brief Creates a stream which read from the given file.
 * Freeing the stream closes the stream.
 */
struct cstream *cstream_file_create(FILE *file, bool fclose_on_free);

/**
 * \brief Creates a stream which read from the given string.
 * Freeing the stream does not free the string.
 */
struct cstream *cstream_string_create(const char *str);

/** \brief Creates a stream which read user input with readline */
struct cstream *cstream_readline_create();
