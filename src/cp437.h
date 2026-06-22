/** @file */
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Decode an IBM437 (CP437) encoded byte buffer into a newly allocated,
 * NUL-terminated UTF-8 string.
 *
 * Every CP437 byte maps to a single Unicode code point in the Basic
 * Multilingual Plane, so each input byte expands to at most three UTF-8 bytes.
 *
 * @param input  Pointer to the CP437 bytes. May be NULL only when length is 0.
 * @param length Number of bytes to read from input.
 * @return A heap-allocated UTF-8 string that the caller must release with
 *         free(), or NULL if allocation fails.
 */
char *cp437_to_utf8(const unsigned char *input, size_t length);

#ifdef __cplusplus
}
#endif
