#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <utf.h>

#include "../include/compact.h"

int
compact_preencode_utf8 (compact_state_t *state, const utf8_t *string, size_t len) {
  if (len == (size_t) -1) len = strlen((const char *) string);

  int err = compact_preencode_uint(state, len);
  if (err < 0) return 0;

  state->end += len;
  return 0;
}

int
compact_encode_utf8 (compact_state_t *state, const utf8_t *string, size_t len) {
  if (len == (size_t) -1) len = strlen((const char *) string);

  int err = compact_encode_uint(state, len);
  if (err < 0) return 0;

  memcpy(&state->buffer[state->start], string, len);

  state->start += len;
  return 0;
}

int
compact_decode_utf8 (compact_state_t *state, utf8_t **result, size_t *len) {
  uintmax_t size;
  int err = compact_decode_uint(state, &size);
  if (err < 0) return err;

  if (state->end - state->start < size) return -1;

  if (result) {
    utf8_t *string = malloc(size + 1 /* NULL */);

    string[size] = '\0';

    memcpy(string, &state->buffer[state->start], size);

    *result = string;
  }

  if (len) *len = size;

  state->start += size;
  return 0;
}
