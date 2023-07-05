#include <stdbool.h>

#include "../include/compact.h"

int
compact_preencode_bool (compact_state_t *state, bool value) {
  state->end++;

  return 0;
}

int
compact_encode_bool (compact_state_t *state, bool value) {
  state->buffer[state->start] = value ? 1 : 0;

  state->start++;

  return 0;
}

int
compact_decode_bool (compact_state_t *state, bool *result) {
  if (state->start >= state->end) return -1;

  if (result) *result = state->buffer[state->start] == 1;

  state->start++;

  return 0;
}
