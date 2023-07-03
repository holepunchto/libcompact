#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../include/compact.h"

int
main () {
  int err;

  uint8_t value[32];

  for (size_t i = 0; i < 32; i++) {
    value[i] = i;
  }

  compact_state_t state = {0, 0};

  err = compact_preencode_fixed32(&state, value);
  assert(err == 0);

  state.buffer = malloc(state.end);

  err = compact_encode_fixed32(&state, value);
  assert(err == 0);

  state.start = 0;

  uint8_t decoded[32];
  err = compact_decode_fixed32(&state, decoded);
  assert(err == 0);

  assert(memcmp(value, decoded, 32) == 0);
}
