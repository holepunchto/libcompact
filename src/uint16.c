#include <stdint.h>

#include "../include/compact.h"

int
compact_preencode_uint16 (compact_state_t *state, uint16_t n) {
  state->end += 2;
  return 0;
}

int
compact_encode_uint16 (compact_state_t *state, uint16_t n) {
  state->buffer[state->start++] = n;
  state->buffer[state->start++] = n >> 8;
  return 0;
}

int
compact_decode_uint16 (compact_state_t *state, uint16_t *result) {
  if (state->end - state->start < 2) return -1;

  uint16_t n = state->buffer[state->start++] + state->buffer[state->start++] * 0x100;

  if (result) *result = n;

  return 0;
}
