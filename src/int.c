#include <assert.h>
#include <stdint.h>

#include "../include/compact.h"
#include "zig-zag.h"

int
compact_preencode_int (compact_state_t *state, intmax_t n) {
  assert(sizeof(intmax_t) == 8);

  return compact_preencode_uint(state, compact_encode_zig_zag(64, n));
}

int
compact_encode_int (compact_state_t *state, intmax_t n) {
  assert(sizeof(intmax_t) == 8);

  return compact_encode_uint(state, compact_encode_zig_zag(64, n));
}

int
compact_decode_int (compact_state_t *state, intmax_t *result) {
  assert(sizeof(intmax_t) == 8);

  uintmax_t n;
  int err = compact_decode_uint(state, result ? &n : NULL);
  if (err < 0) return err;

  if (result) *result = compact_decode_zig_zag(64, n);

  return 0;
}
