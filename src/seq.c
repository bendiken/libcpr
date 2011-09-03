/* This is free and unencumbered software released into the public domain. */

#include "build.h"

seq_t*
seq_alloc() {
  return calloc(1, sizeof(seq_t));
}

void
seq_free(seq_t* const seq) {
  validate_with_void_return(is_nonnull(seq));
  seq_reset(seq);
  free(seq);
}

int
seq_init(seq_t* const restrict seq,
         const seq_vtable_t* restrict vtable,
         const compare_func_t compare_func,
         const free_func_t free_func, ...) {
  validate_with_errno_return(is_nonnull(seq));

  bzero(seq, sizeof(seq_t));

  seq->vtable       = vtable = (is_nonnull(vtable) ? vtable : NULL); // FIXME
  seq->compare_func = compare_func;
  seq->free_func    = free_func;

  if (is_nonnull(vtable->init)) {
    va_list args;
    va_start(args, free_func);
    const int rc = vtable->init(seq, args);
    va_end(args);
    return rc;
  }

  return 0;
}

int
seq_reset(seq_t* const seq) {
  validate_with_errno_return(is_nonnull(seq));

  const seq_vtable_t* const vtable = seq->vtable;
  if (is_nonnull(vtable->reset)) {
    return vtable->reset(seq);
  }

#ifndef NDEBUG
  bzero(seq, sizeof(seq_t));
#endif

  return 0;
}

int
seq_clear(seq_t* const seq) {
  validate_with_errno_return(is_nonnull(seq));

  const seq_vtable_t* const vtable = seq->vtable;
  if (is_nonnull(vtable->clear)) {
    return vtable->clear(seq);
  }

  return -(errno = ENOTSUP); // operation not supported
}

bool
seq_is_empty(const seq_t* const seq) {
  validate_with_true_return(is_nonnull(seq));

  return unlikely(is_null(seq->data) || seq_length((seq_t*)seq) == 0);
}

long
seq_length(seq_t* const seq) {
  validate_with_zero_return(is_nonnull(seq));

  const seq_vtable_t* const vtable = seq->vtable;
  if (is_nonnull(vtable->length)) {
    return vtable->length(seq);
  }

  return (errno = ENOTSUP), 0; // operation not supported
}

long
seq_count(seq_t* const restrict seq, const void* const restrict elt) {
  validate_with_zero_return(is_nonnull(seq) && is_nonnull(elt));

  const seq_vtable_t* const vtable = seq->vtable;
  if (is_nonnull(vtable->count)) {
    return vtable->count(seq, elt);
  }

  return (errno = ENOTSUP), 0; // operation not supported
}

bool
seq_lookup(seq_t* const restrict seq, const void* const restrict elt) {
  validate_with_false_return(is_nonnull(seq) && is_nonnull(elt));

  const seq_vtable_t* const vtable = seq->vtable;
  if (is_nonnull(vtable->lookup)) {
    return vtable->lookup(seq, elt);
  }

  return (errno = ENOTSUP), FALSE; // operation not supported
}

int
seq_insert(seq_t* const restrict seq, const void* const restrict elt) {
  validate_with_errno_return(is_nonnull(seq) && is_nonnull(elt));

  const seq_vtable_t* const vtable = seq->vtable;
  if (is_nonnull(vtable->insert)) {
    return vtable->insert(seq, elt);
  }

  return -(errno = ENOTSUP); // operation not supported
}

int
seq_remove(seq_t* const restrict seq, const void* const restrict elt) {
  validate_with_errno_return(is_nonnull(seq) && is_nonnull(elt));

  const seq_vtable_t* const vtable = seq->vtable;
  if (is_nonnull(vtable->remove)) {
    return vtable->remove(seq, elt);
  }

  return -(errno = ENOTSUP); // operation not supported
}

int
seq_replace(seq_t* const restrict seq, const void* const restrict elt1,
                                       const void* const restrict elt2) {
  validate_with_errno_return(is_nonnull(seq) && is_nonnull(elt1));

  const seq_vtable_t* const vtable = seq->vtable;
  if (is_nonnull(vtable->replace)) {
    return vtable->replace(seq, elt1, elt2);
  }

  return -(errno = ENOTSUP); // operation not supported
}

int
seq_reverse(seq_t* const seq) {
  validate_with_zero_return(is_nonnull(seq));

  const seq_vtable_t* const vtable = seq->vtable;
  if (is_nonnull(vtable->reverse)) {
    return vtable->reverse(seq);
  }

  return -(errno = ENOTSUP); // operation not supported
}

int
seq_sort(seq_t* const seq, const seq_sort_type_t how) {
  validate_with_errno_return(is_nonnull(seq));
  validate_with_errno_return(how > SEQ_SORT_NONE);

  const seq_vtable_t* const vtable = seq->vtable;
  if (is_nonnull(vtable->sort)) {
    return vtable->sort(seq, how);
  }

  return -(errno = ENOTSUP); // operation not supported
}