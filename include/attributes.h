#pragma once

/**
 * \page Attributes
 *
 * Attributes are non-standard annotations used to tell the compiler
 * about some property of the code.
 */

/* The compiler will generate a warning when this valid isn't used */
#define __warn_unused __attribute__((warn_unused_result))

/* The compiler will not generate a warning if this value isn't used */
#define __unused __attribute__((unused))

/* Tells the compiler this function allocates memory */
#define __malloc __attribute__((malloc))

/* Tells the compiler the size of the memory returned by this function
   can be found in the Nth argument */
#define __alloc_size(...) __attribute__((__alloc_size__(__VA_ARGS__)))
