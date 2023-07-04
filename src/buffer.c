#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../include/compact.h"

int
compact_preencode_buffer (compact_state_t *state, const uint8_t *buffer, size_t len) {
  int err = compact_preencode_uint(state, len);
  if (err < 0) return 0;

  state->end += len;
  return 0;
}

int
compact_encode_buffer (compact_state_t *state, const uint8_t *buffer, size_t len) {
  int err = compact_encode_uint(state, len);
  if (err < 0) return 0;

  memcpy(&state->buffer[state->start], buffer, len);

  state->start += len;
  return 0;
}

int
compact_decode_buffer (compact_state_t *state, uint8_t **result, size_t *len) {
  uintmax_t size;
  int err = compact_decode_uint(state, &size);
  if (err < 0) return err;

  if (state->end - state->start < size) return -1;

  if (result) {
    uint8_t *buffer = malloc(size);

    memcpy(buffer, &state->buffer[state->start], size);

    *result = buffer;
  }

  if (len) *len = size;

  state->start += size;
  return 0;
}
