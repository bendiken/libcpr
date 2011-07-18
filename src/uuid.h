/* This is free and unencumbered software released into the public domain. */

#ifndef _CPRIME_UUID_H
#define _CPRIME_UUID_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h> /* for uuid_t on Darwin */
#endif /* HAVE_UNISTD_H */

#ifndef _UUID_T
#define _UUID_T
#include <stdint.h> /* for uint8_t */
typedef uint8_t uuid_t[16];
#endif /* _UUID_T */

/**
 * The canonical all-zeroes UUID.
 */
extern const uuid_t uuid_zero;

/**
 * Allocates heap memory for a new UUID.
 */
extern uuid_t* uuid_alloc();

/**
 * Releases the heap memory used by a UUID.
 */
extern void uuid_free(uuid_t* uuid);

/**
 * Initializes a stack-allocated UUID.
 */
#define UUID_INIT {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}

/**
 * Initializes a UUID.
 */
extern int uuid_init(uuid_t* uuid);

/**
 * Disposes of a UUID.
 */
extern int uuid_dispose(uuid_t* uuid);

/**
 * Returns `TRUE` if a UUID contains all zeroes.
 */
extern int uuid_is_zero(const uuid_t* const uuid);

#ifdef __cplusplus
}
#endif

#endif /* _CPRIME_UUID_H */
