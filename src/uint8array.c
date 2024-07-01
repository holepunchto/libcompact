#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "../include/compact.h"

int
compact_preencode_uint8array (compact_state_t *state, const uint8_t *array, size_t len) {
  int err = compact_preencode_uint(state, len);
  if (err < 0) return 0;

  state->end += len;

  return 0;
}

int
compact_encode_uint8array (compact_state_t *state, const uint8_t *array, size_t len) {
  int err = compact_encode_uint(state, len);
  if (err < 0) return 0;

  memcpy(&state->buffer[state->start], array, len);

  state->start += len;

  return 0;
}

int
compact_decode_uint8array (compact_state_t *state, uint8_t **result, size_t *len) {
  uintmax_t size;
  int err = compact_decode_uint(state, &size);
  if (err < 0) return err;

  if (state->end - state->start < size) return -1;

  if (result) *result = &state->buffer[state->start];

  if (len) *len = size;

  state->start += size;

  return 0;
}
