#include <stdint.h>

#include "../include/compact.h"
#include "zig-zag.h"

int
compact_preencode_int (compact_state_t *state, intmax_t n) {
  return compact_preencode_uint(state, compact_encode_zig_zag(n));
}

int
compact_encode_int (compact_state_t *state, intmax_t n) {
  return compact_encode_uint(state, compact_encode_zig_zag(n));
}

int
compact_decode_intmax (compact_state_t *state, intmax_t *result) {
  uintmax_t n;
  int err = compact_decode_uint(state, &n);
  if (err < 0) return err;

  *result = compact_decode_zig_zag(n);
  return 0;
}
