#include <stdint.h>

#include "../include/compact.h"
#include "zig-zag.h"

int
compact_preencode_int32 (compact_state_t *state, int32_t n) {
  state->end += 4;
  return 0;
}

int
compact_encode_int32 (compact_state_t *state, int32_t n) {
  return compact_encode_uint32(state, compact_encode_zig_zag(n));
}

int
compact_decode_int32 (compact_state_t *state, int32_t *result) {
  uint32_t n;
  int err = compact_decode_uint32(state, &n);
  if (err < 0) return err;

  if (result) *result = compact_decode_zig_zag(n);

  return 0;
}
