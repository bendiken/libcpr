/* This is free and unencumbered software released into the public domain. */

#include "build.h"
#include "set/nullset.h"
#include "set/vectorset.h"
#include "set/listset.h"
#include "set/treeset.h"
#include "set/hashset.h"

const class_t set_class = {
  .name    = "set",
  .super   = NULL,
  .vtable  = NULL, /* abstract class */
};

set_t*
set_alloc() {
  return calloc(1, sizeof(set_t));
}

void
set_free(set_t* const set) {
  validate_with_void_return(is_nonnull(set));
  set_reset(set);
  free(set);
}

int
set_init(set_t* const restrict set,
         const class_t* restrict klass,
         const hash_func_t hash_func,
         const compare_func_t compare_func,
         const free_func_t free_func, ...) {
  validate_with_errno_return(is_nonnull(set));

  bzero(set, sizeof(set_t));

  const set_vtable_t* const vtable = is_nonnull(klass) ?
    (set_vtable_t*)klass->vtable : &listset_vtable;

  set->vtable       = vtable;
  set->hash_func    = hash_func;
  set->compare_func = compare_func;
  set->free_func    = free_func;

  if (is_nonnull(vtable->init)) {
    va_list args;
    va_start(args, free_func);
    const int rc = vtable->init(set, args);
    va_end(args);
    return rc;
  }

  return SUCCESS;
}

int
set_reset(set_t* const set) {
  validate_with_errno_return(is_nonnull(set));

  const set_vtable_t* const vtable = set->vtable;
  if (is_nonnull(vtable->reset)) {
    return vtable->reset(set);
  }

#ifndef NDEBUG
  bzero(set, sizeof(set_t));
#endif

  return SUCCESS;
}

int
set_clear(set_t* const set) {
  validate_with_errno_return(is_nonnull(set));

  const set_vtable_t* const vtable = set->vtable;
  if (is_nonnull(vtable->clear)) {
    return vtable->clear(set);
  }

  return FAILURE(ENOTSUP); // operation not supported
}

bool
set_is_empty(const set_t* const set) {
  validate_with_true_return(is_nonnull(set));

  return unlikely(is_null(set->instance) || set_count((set_t*)set, NULL) == 0);
}

long
set_count(set_t* const restrict set, const void* const restrict elt) {
  validate_with_zero_return(is_nonnull(set));

  const set_vtable_t* const vtable = set->vtable;
  if (is_nonnull(vtable->count)) {
    return vtable->count(set, elt);
  }

  return (void)FAILURE(ENOTSUP), 0; // operation not supported
}

bool
set_lookup(set_t* const restrict set, const void* const restrict elt) {
  validate_with_false_return(is_nonnull(set) && is_nonnull(elt));

  const set_vtable_t* const vtable = set->vtable;
  if (is_nonnull(vtable->lookup)) {
    return vtable->lookup(set, elt);
  }

  return (void)FAILURE(ENOTSUP), FALSE; // operation not supported
}

int
set_insert(set_t* const restrict set, const void* const restrict elt) {
  validate_with_errno_return(is_nonnull(set) && is_nonnull(elt));

  const set_vtable_t* const vtable = set->vtable;
  if (is_nonnull(vtable->insert)) {
    return vtable->insert(set, elt);
  }

  return FAILURE(ENOTSUP); // operation not supported
}

int
set_remove(set_t* const restrict set, const void* const restrict elt) {
  validate_with_errno_return(is_nonnull(set) && is_nonnull(elt));

  const set_vtable_t* const vtable = set->vtable;
  if (is_nonnull(vtable->remove)) {
    return vtable->remove(set, elt);
  }

  return FAILURE(ENOTSUP); // operation not supported
}

int
set_replace(set_t* const restrict set, const void* const restrict elt1,
                                       const void* const restrict elt2) {
  validate_with_errno_return(is_nonnull(set) && is_nonnull(elt1));

  const set_vtable_t* const vtable = set->vtable;
  if (is_nonnull(vtable->replace)) {
    return vtable->replace(set, elt1, elt2);
  }

  return FAILURE(ENOTSUP); // operation not supported
}
