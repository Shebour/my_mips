#include "vec.h"

#include <stdlib.h>

void vec_init(struct vec *vec)
{
  vec->data = NULL;
  vec->size = 0;
  vec->capacity = 0;
}

void vec_destroy(struct vec *vec)
{
  free(vec->data);
  vec->data = NULL;
}

static void vec_grow(struct vec *vec)
{
  size_t new_capacity;
  if (vec->capacity == 0)
    new_capacity = 10;
  else
    new_capacity = vec->capacity * 2;
  char *new_data;
  if (!vec->data)
    new_data = malloc(new_capacity);
  else
    new_data = realloc(vec->data, new_capacity);
  vec->data = new_data;
  vec->capacity = new_capacity;
}

void vec_reset(struct vec *vec)
{
  vec->size = 0;
}

void vec_push(struct vec *vec, char c)
{
  if (vec->size == vec->capacity)
    vec_grow(vec);

  vec->data[vec->size++] = c;
}

char *vec_cstring(struct vec *vec)
{
  if (vec->size == 0 || vec->data[vec->size - 1] != '\0')
    vec_push(vec, '\0');
  return vec->data;
}
