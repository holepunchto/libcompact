#include <stdint.h>

#include "../include/compact.h"

int
compact_preencode_float64 (compact_state_t *state, double n) {
  state->end += 8;
  return 0;
}

int
compact_encode_float64 (compact_state_t *state, double n) {
  return compact_encode_uint64(state, (uint64_t) n);
}

int
compact_decode_float64 (compact_state_t *state, double *result) {
  uint64_t n;
  int err = compact_decode_uint64(state, &n);
  if (err < 0) return err;

  if (result) *result = (double) n;

  return 0;
}
