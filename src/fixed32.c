#include <stddef.h>
#include <string.h>

#include "../include/compact.h"

int
compact_preencode_fixed32 (compact_state_t *state, const uint8_t array[32]) {
  state->end += 32;
  return 0;
}

int
compact_encode_fixed32 (compact_state_t *state, const uint8_t array[32]) {
  memcpy(&state->buffer[state->start], array, 32);
  state->start += 32;
  return 0;
}

int
compact_decode_fixed32 (compact_state_t *state, uint8_t result[32]) {
  if (state->end - state->start < 32) return -1;

  if (result) memcpy(result, &state->buffer[state->start], 32);

  state->start += 32;
  return 0;
}
