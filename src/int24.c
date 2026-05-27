#include <stdint.h>

#include "../include/compact.h"
#include "zig-zag.h"

int
compact_preencode_int24 (compact_state_t *state, int32_t n) {
  state->end += 3;

  return 0;
}

int
compact_encode_int24 (compact_state_t *state, int32_t n) {
  return compact_encode_uint24(state, compact_encode_zig_zag(32, n));
}

int
compact_decode_int24 (compact_state_t *state, int32_t *result) {
  uint32_t n;
  int err = compact_decode_uint24(state, result ? &n : NULL);
  if (err < 0) return err;

  if (result) *result = compact_decode_zig_zag(32, n);

  return 0;
}
