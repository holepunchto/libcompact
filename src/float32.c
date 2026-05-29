#include <stdint.h>
#include <string.h>

#include "../include/compact.h"

int
compact_preencode_float32 (compact_state_t *state, float n) {
  state->end += 4;

  return 0;
}

int
compact_encode_float32 (compact_state_t *state, float n) {
  uint32_t bits;
  memcpy(&bits, &n, sizeof(bits));

  return compact_encode_uint32(state, bits);
}

int
compact_decode_float32 (compact_state_t *state, float *result) {
  uint32_t bits;
  int err = compact_decode_uint32(state, result ? &bits : NULL);
  if (err < 0) return err;

  if (result) memcpy(result, &bits, sizeof(*result));

  return 0;
}
