#include <stdint.h>

#include "../include/compact.h"

int
compact_preencode_int40 (compact_state_t *state, int64_t n) {
  state->end += 5;

  return 0;
}

int
compact_encode_int40 (compact_state_t *state, int64_t n) {
  uint64_t z = n < 0 ? ((uint64_t) (-n - 1) << 1) | 1 : (uint64_t) n << 1;
  return compact_encode_uint40(state, z);
}

int
compact_decode_int40 (compact_state_t *state, int64_t *result) {
  uint64_t n;
  int err = compact_decode_uint40(state, result ? &n : NULL);
  if (err < 0) return err;

  if (result) *result = (n & 1) ? -(int64_t) (n >> 1) - 1 : (int64_t) (n >> 1);

  return 0;
}
