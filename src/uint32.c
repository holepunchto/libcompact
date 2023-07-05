#include <stdint.h>

#include "../include/compact.h"

int
compact_preencode_uint32 (compact_state_t *state, uint32_t n) {
  state->end += 4;
  return 0;
}

int
compact_encode_uint32 (compact_state_t *state, uint32_t n) {
  state->buffer[state->start++] = n;
  state->buffer[state->start++] = n >> 8;
  state->buffer[state->start++] = n >> 16;
  state->buffer[state->start++] = n >> 24;
  return 0;
}

int
compact_decode_uint32 (compact_state_t *state, uint32_t *result) {
  if (state->end - state->start < 4) return -1;

  uint32_t n = state->buffer[state->start++] + state->buffer[state->start++] * 0x100 + state->buffer[state->start++] * 0x10000 + state->buffer[state->start++] * 0x1000000;

  if (result) *result = n;

  return 0;
}
