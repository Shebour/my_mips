#ifndef VEC_H
#define VEC_H

#include <stddef.h>
/**
 * \brief An array of characters which grows as needed.
 */
struct vec
{
  char *data;
  size_t size;
  size_t capacity;
};

/** Initialize a new vector */
void vec_init(struct vec *vec);

/** Releases the memory allocated for the vector */
void vec_destroy(struct vec *vec);

/** Remove all characters from the vector, without releasing memory */
void vec_reset(struct vec *vec);

/** Add a character at the end of the vector */
void vec_push(struct vec *vec, char c);

/** Ensures the array has a NUL byte at the end, and returns it */
char *vec_cstring(struct vec *vec);

#endif /* VEC_H */
