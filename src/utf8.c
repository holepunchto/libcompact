#include <stddef.h>
#include <utf.h>

#include "../include/compact.h"
#include "utf/string.h"

int
compact_preencode_utf8 (compact_state_t *state, const utf8_string_view_t string) {
  int err = compact_preencode_uint(state, string.len);
  if (err < 0) return 0;

  state->end += string.len;

  return 0;
}

int
compact_encode_utf8 (compact_state_t *state, const utf8_string_view_t string) {
  int err = compact_encode_uint(state, string.len);
  if (err < 0) return 0;

  memcpy(&state->buffer[state->start], string.data, string.len);

  state->start += string.len;

  return 0;
}

int
compact_decode_utf8 (compact_state_t *state, utf8_string_view_t *result) {
  uintmax_t len;
  int err = compact_decode_uint(state, &len);
  if (err < 0) return err;

  if (state->end - state->start < len) return -1;

  if (result) *result = utf8_string_view_init(&state->buffer[state->start], len);

  state->start += len;

  return 0;
}
