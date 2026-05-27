#include <stdint.h>

#include "../include/compact.h"

int
compact_preencode_uint40 (compact_state_t *state, uint64_t n) {
  state->end += 5;

  return 0;
}

int
compact_encode_uint40 (compact_state_t *state, uint64_t n) {
  state->buffer[state->start] = n;
  state->buffer[state->start + 1] = n >> 8;
  state->buffer[state->start + 2] = n >> 16;
  state->buffer[state->start + 3] = n >> 24;
  state->buffer[state->start + 4] = n >> 32;

  state->start += 5;

  return 0;
}

int
compact_decode_uint40 (compact_state_t *state, uint64_t *result) {
  if (state->end - state->start < 5) return -1;

  if (result) *result = (uint64_t) state->buffer[state->start] | ((uint64_t) state->buffer[state->start + 1] << 8) | ((uint64_t) state->buffer[state->start + 2] << 16) | ((uint64_t) state->buffer[state->start + 3] << 24) | ((uint64_t) state->buffer[state->start + 4] << 32);

  state->start += 5;

  return 0;
}
