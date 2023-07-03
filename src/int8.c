#include <stdint.h>

#include "../include/compact.h"
#include "zig-zag.h"

int
compact_preencode_int8 (compact_state_t *state, int8_t n) {
  state->end++;
  return 0;
}

int
compact_encode_int8 (compact_state_t *state, int8_t n) {
  return compact_encode_uint8(state, compact_encode_zig_zag(n));
}

int
compact_decode_int8 (compact_state_t *state, int8_t *result) {
  uint8_t n;
  int err = compact_decode_uint8(state, &n);
  if (err < 0) return err;

  if (result) *result = compact_decode_zig_zag(n);

  return 0;
}
