#include <stddef.h>
#include <string.h>

#include "../include/compact.h"

int
compact_preencode_fixed64 (compact_state_t *state, const uint8_t buffer[64]) {
  state->end += 64;
  return 0;
}

int
compact_encode_fixed64 (compact_state_t *state, const uint8_t buffer[64]) {
  memcpy(&state->buffer[state->start], buffer, 64);
  state->start += 64;
  return 0;
}

int
compact_decode_fixed64 (compact_state_t *state, uint8_t result[64]) {
  if (state->end - state->start < 64) return -1;

  if (result) memcpy(result, &state->buffer[state->start], 64);

  state->start += 64;
  return 0;
}
