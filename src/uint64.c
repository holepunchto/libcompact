#include <stdint.h>

#include "../include/compact.h"

int
compact_preencode_uint64 (compact_state_t *state, uint64_t n) {
  state->end += 8;

  return 0;
}

int
compact_encode_uint64 (compact_state_t *state, uint64_t n) {
  state->buffer[state->start] = n;
  state->buffer[state->start + 1] = n >> 8;
  state->buffer[state->start + 2] = n >> 16;
  state->buffer[state->start + 3] = n >> 24;
  state->buffer[state->start + 4] = n >> 32;
  state->buffer[state->start + 5] = n >> 40;
  state->buffer[state->start + 6] = n >> 48;
  state->buffer[state->start + 7] = n >> 56;

  state->start += 8;

  return 0;
}

int
compact_decode_uint64 (compact_state_t *state, uint64_t *result) {
  if (state->end - state->start < 8) return -1;

  if (result) *result = ((uint64_t) state->buffer[state->start]) | ((uint64_t) state->buffer[state->start + 1] << 8) | ((uint64_t) state->buffer[state->start + 2] << 16) | ((uint64_t) state->buffer[state->start + 3] << 24) | ((uint64_t) state->buffer[state->start + 4] << 32) | ((uint64_t) state->buffer[state->start + 5] << 40) | ((uint64_t) state->buffer[state->start + 6] << 48) | ((uint64_t) state->buffer[state->start + 7] << 56);

  state->start += 8;

  return 0;
}
