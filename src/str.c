/* This is free and unencumbered software released into the public domain. */

#include "build.h"
#include <inttypes.h> /* for strtoimax() */
#include <regex.h>    /* for regex_t, regcomp(), regexec(), regfree() */
#include <stdarg.h>   /* for va_list, va_start(), va_end() */
#include <stdio.h>    /* for vasprintf() */
#include <stdlib.h>   /* for atoi(), atol(), strtod(), strtof() */
#include <string.h>   /* for strlen(), strcmp(), strncmp(), strstr() */

public char*
str_dup(const char* const restrict str) {
  validate_with_null_return(str != NULL);

  return strdup(str);
}

public char*
str_ndup(const char* const restrict str, const size_t size) {
  validate_with_null_return(str != NULL);

#ifdef HAVE_STRNDUP
  return strndup(str, size);
#else
  char* const restrict str2 = malloc(size + 1);
  bcopy(str, str2, size), str2[size] = '\0';
  return str2;
#endif /* HAVE_STRNDUP */
}

public char*
str_format(const char* const restrict fmt, ...) {
  validate_with_null_return(fmt != NULL);

#ifndef HAVE_VASPRINTF
  return (void)FAILURE(ENOTSUP), NULL; // operation not supported
#else
  char* result = NULL;
  va_list args;
  va_start(args, fmt);
  {
    const int rc = vasprintf(&result, fmt, args);
    if (unlikely(rc == -1)) {
      return (void)FAILURE(ENOMEM), NULL;
    }
  }
  va_end(args);
  return result;
#endif /* HAVE_VASPRINTF */
}

public long
str_size(const char* const restrict str) {
  validate_with_zero_return(str != NULL);

  if (unlikely(*str == '\0'))
    return 0; // the string is empty

  return strlen(str);
}

public long
str_length(const char* const restrict str) {
  validate_with_zero_return(str != NULL);

  if (unlikely(*str == '\0'))
    return 0; // the string is empty

#ifdef DISABLE_UNICODE
  return strlen(str); // ASCII only
#else
  long length = 0;
  const char* data = str;
  while (likely(*data != '\0')) {
    length++;
    data = UTF8_SKIP_CHAR(data);
  }
  return length;
#endif /* DISABLE_UNICODE */
}

public hash_t
str_hash(const char* const restrict str) {
  validate_with_zero_return(str != NULL);

  hash_t hash = 5381;
  const int8_t* data = (int8_t*)str;
  while (likely(*data != '\0')) {
    hash = ((hash << 5) + hash) + *data++; // hash * 33 + c
  }
  return hash;
}

public int
str_compare(const char* const str1, const char* const str2) {
  validate_with_errno_return(str1 != NULL && str2 != NULL);

  if (unlikely(str1 == str2))
    return COMPARE_EQ;

  return strcmp(str1, str2);
}

public bool
str_equal(const char* const str1, const char* const str2) {
  validate_with_false_return(str1 != NULL && str2 != NULL);

  if (unlikely(str1 == str2))
    return TRUE;

  return (strcmp(str1, str2) == 0) ? TRUE : FALSE;
}

public bool
str_is_empty(const char* const restrict str) {
  if (unlikely(str == NULL))
    return (void)FAILURE(EINVAL), TRUE;

  return unlikely(*str == '\0') ? TRUE : FALSE;
}

public bool
str_is_ascii(const char* const restrict str) {
  validate_with_false_return(str != NULL);

  const byte_t* chr = (byte_t*)str;
  while (likely(*chr != '\0')) {
    if (unlikely(!CHAR_IS_ASCII(chr[0]))) {
      return FALSE;
    }
    chr++;
  }
  return TRUE;
}

public bool
str_is_utf8(const char* const restrict str) {
  validate_with_false_return(str != NULL);

#ifdef DISABLE_UNICODE
  return (void)FAILURE(ENOTSUP), FALSE;
#else
  const char* chr = str;
  while (likely(*chr != '\0')) {
    // TODO: UTF8_DECODE()
    chr = UTF8_SKIP_CHAR(chr);
  }
  return TRUE;
#endif /* DISABLE_UNICODE */
}

static inline bool
str_is(const char* const restrict str, int (*predicate)(const char_t chr)) {
  validate_with_false_return(str != NULL && predicate != NULL);

  const char* chr = str;
  while (likely(*chr != '\0')) {
#ifdef DISABLE_UNICODE
    if (unlikely(predicate(chr[0]) != TRUE)) {
      return FALSE;
    }
    chr++; // ASCII only
#else
    if (unlikely(predicate(chr[0]) != TRUE)) { // TODO: UTF8_DECODE()
      return FALSE;
    }
    chr = UTF8_SKIP_CHAR(chr);
#endif /* DISABLE_UNICODE */
  }
  return TRUE;
}

public bool
str_is_alnum(const char* const restrict str) {
  return str_is(str, char_is_alnum);
}

public bool
str_is_alpha(const char* const restrict str) {
  return str_is(str, char_is_alpha);
}

public bool
str_is_blank(const char* const restrict str) {
  return str_is(str, char_is_blank);
}

public bool
str_is_cntrl(const char* const restrict str) {
  return str_is(str, char_is_cntrl);
}

public bool
str_is_digit(const char* const restrict str) {
  return str_is(str, char_is_digit);
}

public bool
str_is_graph(const char* const restrict str) {
  return str_is(str, char_is_graph);
}

public bool
str_is_lower(const char* const restrict str) {
  return str_is(str, char_is_lower);
}

public bool
str_is_print(const char* const restrict str) {
  return str_is(str, char_is_print);
}

public bool
str_is_punct(const char* const restrict str) {
  return str_is(str, char_is_punct);
}

public bool
str_is_space(const char* const restrict str) {
  return str_is(str, char_is_space);
}

public bool
str_is_upper(const char* const restrict str) {
  return str_is(str, char_is_upper);
}

public bool
str_is_xdigit(const char* const restrict str) {
  return str_is(str, char_is_xdigit);
}

static regex_t str_is_uuid_regex;
static once_t  str_is_uuid_once = ONCE_INIT;

static void
str_is_uuid_init() {
  const int rc = regcomp(&str_is_uuid_regex, UUID_PATTERN, REG_EXTENDED | REG_NOSUB);
  assert(rc == 0);
}

public bool
str_is_uuid(const char* const restrict str) {
  validate_with_false_return(str != NULL);

  once(&str_is_uuid_once, str_is_uuid_init);

  return (regexec(&str_is_uuid_regex, str, (size_t)0, NULL, 0) == 0) ? TRUE : FALSE;
}

public bool
str_has_prefix(const char* const restrict str, const char* const restrict prefix) {
  validate_with_false_return(str != NULL && prefix != NULL);

  const size_t string_size = strlen(str);
  const size_t prefix_size = strlen(prefix);

  if (unlikely(string_size < prefix_size)) {
    return FALSE;
  }

  return unlikely(strncmp(str, prefix, prefix_size) == 0) ? TRUE : FALSE;
}

public bool
str_has_suffix(const char* const restrict str, const char* const restrict suffix) {
  validate_with_false_return(str != NULL && suffix != NULL);

  const size_t string_size = strlen(str);
  const size_t suffix_size = strlen(suffix);

  if (unlikely(string_size < suffix_size)) {
    return FALSE;
  }

  return unlikely(strcmp(str + string_size - suffix_size, suffix) == 0) ? TRUE : FALSE;
}

public bool
str_contains(const char* const restrict str, const char* const restrict substr) {
  validate_with_false_return(str != NULL && substr != NULL);

  return unlikely(strstr(str, substr) != NULL) ? TRUE : FALSE;
}

public bool
str_matches(const char* const restrict str, const char* const restrict pattern) {
  validate_with_false_return(str != NULL && pattern != NULL);

  regex_t regex;
  {
    const int rc = regcomp(&regex, pattern, REG_EXTENDED | REG_NOSUB);
    if (unlikely(rc != 0)) {
      return (void)FAILURE(EINVAL), FALSE; // invalid pattern
    }
  }

  bool result = TRUE;
  {
    const int rc = regexec(&regex, str, (size_t)0, NULL, 0);
    if (unlikely(rc != 0)) {
      result = FALSE;
    }
  }

  regfree(&regex);

  return result;
}

public int
str_to_int(const char* const restrict str) {
  validate_with_zero_return(str != NULL && *str != '\0');

  return atoi(str);
}

public long
str_to_long(const char* const restrict str) {
  validate_with_zero_return(str != NULL && *str != '\0');

  return atol(str);
}

public intmax_t
str_to_intmax(const char* const restrict str) {
  validate_with_errno_return(str != NULL && *str != '\0');

  return strtoimax(str, NULL, 10);
}

public float
str_to_float(const char* const restrict str) {
  validate_with_errno_return(str != NULL && *str != '\0');

  return strtof(str, NULL);
}

public double
str_to_double(const char* const restrict str) {
  validate_with_errno_return(str != NULL && *str != '\0');

  return strtod(str, NULL);
}
