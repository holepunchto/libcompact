#include <stdint.h>

#include "../include/compact.h"

int
compact_preencode_uint8 (compact_state_t *state, uint8_t n) {
  state->end++;

  return 0;
}

int
compact_encode_uint8 (compact_state_t *state, uint8_t n) {
  state->buffer[state->start] = n;

  state->start++;

  return 0;
}

int
compact_decode_uint8 (compact_state_t *state, uint8_t *result) {
  if (state->start >= state->end) return -1;

  if (result) *result = state->buffer[state->start];

  state->start++;

  return 0;
}
