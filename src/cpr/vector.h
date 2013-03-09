/* This is free and unencumbered software released into the public domain. */

#ifndef CPR_VECTOR_H
#define CPR_VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h> /* for bool */
#include <stddef.h>  /* for size_t */

typedef struct cpr_vector cpr_vector_t;

/**
 * The size of the cpr_vector_t opaque type, i.e., sizeof(cpr_vector_t).
 */
extern const size_t cpr_vector_sizeof;

cpr_vector_t* cpr_vector_alloc(void);

void cpr_vector_free(cpr_vector_t* vector);

void cpr_vector_init(cpr_vector_t* vector);

void cpr_vector_dispose(cpr_vector_t* vector);

/**
 * Tests whether a vector is empty, i.e., whether its size is 0.
 */
bool cpr_vector_empty(const cpr_vector_t* vector);

/**
 * Returns the number of elements in a vector.
 */
size_t cpr_vector_size(const cpr_vector_t* vector);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CPR_VECTOR_H */