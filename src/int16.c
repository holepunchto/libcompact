#include <stdint.h>

#include "../include/compact.h"
#include "zig-zag.h"

int
compact_preencode_int16 (compact_state_t *state, int16_t n) {
  state->end += 2;

  return 0;
}

int
compact_encode_int16 (compact_state_t *state, int16_t n) {
  return compact_encode_uint16(state, compact_encode_zig_zag(16, n));
}

int
compact_decode_int16 (compact_state_t *state, int16_t *result) {
  uint16_t n;
  int err = compact_decode_uint16(state, result ? &n : NULL);
  if (err < 0) return err;

  if (result) *result = compact_decode_zig_zag(16, n);

  return 0;
}
