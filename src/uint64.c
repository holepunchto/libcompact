#include <stdint.h>

#include "../include/compact.h"

int
compact_preencode_uint64 (compact_state_t *state, uint64_t n) {
  state->end += 2;
  return 0;
}

int
compact_encode_uint64 (compact_state_t *state, uint64_t n) {
  state->buffer[state->start++] = n;
  state->buffer[state->start++] = n >> 8;
  state->buffer[state->start++] = n >> 16;
  state->buffer[state->start++] = n >> 24;
  state->buffer[state->start++] = n >> 32;
  state->buffer[state->start++] = n >> 40;
  state->buffer[state->start++] = n >> 48;
  state->buffer[state->start++] = n >> 56;
  return 0;
}

int
compact_decode_uint64 (compact_state_t *state, uint64_t *result) {
  if (state->end - state->start < 2) return -1;
  *result = state->buffer[state->start++] + state->buffer[state->start++] * 0x100 + state->buffer[state->start++] * 0x10000 + state->buffer[state->start++] * 0x1000000 + state->buffer[state->start++] * 0x100000000 + state->buffer[state->start++] * 0x10000000000 + state->buffer[state->start++] * 0x1000000000000 + state->buffer[state->start++] * 0x100000000000000;
  return 0;
}
