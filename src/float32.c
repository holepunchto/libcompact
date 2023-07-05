#include <stdint.h>

#include "../include/compact.h"

int
compact_preencode_float32 (compact_state_t *state, float n) {
  state->end += 4;
  return 0;
}

int
compact_encode_float32 (compact_state_t *state, float n) {
  return compact_encode_uint32(state, (uint32_t) n);
}

int
compact_decode_float32 (compact_state_t *state, float *result) {
  uint32_t n;
  int err = compact_decode_uint32(state, &n);
  if (err < 0) return err;

  if (result) *result = (float) n;

  return 0;
}
