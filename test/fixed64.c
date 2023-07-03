#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../include/compact.h"

int
main () {
  int err;

  uint8_t value[64];

  for (size_t i = 0; i < 64; i++) {
    value[i] = i;
  }

  compact_state_t state = {0, 0};

  err = compact_preencode_fixed64(&state, value);
  assert(err == 0);

  state.buffer = malloc(state.end);

  err = compact_encode_fixed64(&state, value);
  assert(err == 0);

  state.start = 0;

  uint8_t decoded[64];
  err = compact_decode_fixed64(&state, decoded);
  assert(err == 0);

  assert(memcmp(value, decoded, 64) == 0);
}
