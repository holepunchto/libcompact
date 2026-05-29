#include <stdint.h>
#include <string.h>

#include "../include/compact.h"

int
compact_preencode_float64 (compact_state_t *state, double n) {
  state->end += 8;

  return 0;
}

int
compact_encode_float64 (compact_state_t *state, double n) {
  uint64_t bits;
  memcpy(&bits, &n, sizeof(bits));

  return compact_encode_uint64(state, bits);
}

int
compact_decode_float64 (compact_state_t *state, double *result) {
  uint64_t bits;
  int err = compact_decode_uint64(state, result ? &bits : NULL);
  if (err < 0) return err;

  if (result) memcpy(result, &bits, sizeof(*result));

  return 0;
}
