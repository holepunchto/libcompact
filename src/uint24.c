#include <stdint.h>

#include "../include/compact.h"

int
compact_preencode_uint24 (compact_state_t *state, uint32_t n) {
  state->end += 3;

  return 0;
}

int
compact_encode_uint24 (compact_state_t *state, uint32_t n) {
  state->buffer[state->start] = n;
  state->buffer[state->start + 1] = n >> 8;
  state->buffer[state->start + 2] = n >> 16;

  state->start += 3;

  return 0;
}

int
compact_decode_uint24 (compact_state_t *state, uint32_t *result) {
  if (state->end - state->start < 3) return -1;

  if (result) *result = (uint32_t) state->buffer[state->start] | ((uint32_t) state->buffer[state->start + 1] << 8) | ((uint32_t) state->buffer[state->start + 2] << 16);

  state->start += 3;

  return 0;
}
