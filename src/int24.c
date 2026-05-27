#include <stdint.h>

#include "../include/compact.h"

int
compact_preencode_int24 (compact_state_t *state, int32_t n) {
  state->end += 3;

  return 0;
}

int
compact_encode_int24 (compact_state_t *state, int32_t n) {
  uint32_t z = n < 0 ? ((uint32_t) (-n - 1) << 1) | 1 : (uint32_t) n << 1;
  return compact_encode_uint24(state, z);
}

int
compact_decode_int24 (compact_state_t *state, int32_t *result) {
  uint32_t n;
  int err = compact_decode_uint24(state, result ? &n : NULL);
  if (err < 0) return err;

  if (result) *result = (n & 1) ? -(int32_t) (n >> 1) - 1 : (int32_t) (n >> 1);

  return 0;
}
