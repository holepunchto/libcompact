#include <stdint.h>

#include "../include/compact.h"
#include "zig-zag.h"

int
compact_preencode_int64 (compact_state_t *state, int64_t n) {
  state->end += 8;
  return 0;
}

int
compact_encode_int64 (compact_state_t *state, int64_t n) {
  return compact_encode_uint64(state, compact_encode_zig_zag(n));
}

int
compact_decode_int64 (compact_state_t *state, int64_t *result) {
  uint64_t n;
  int err = compact_decode_uint64(state, &n);
  if (err < 0) return err;

  *result = compact_decode_zig_zag(n);
  return 0;
}
