#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/compact.h"

#define test_uint8(n) \
  { \
    int err; \
\
    compact_state_t state = {0, 0}; \
\
    err = compact_preencode_uint8(&state, n); \
    assert(err == 0); \
\
    state.buffer = malloc(state.end); \
\
    err = compact_encode_uint8(&state, n); \
    assert(err == 0); \
\
    state.start = 0; \
\
    uint8_t decoded; \
    err = compact_decode_uint8(&state, &decoded); \
    assert(err == 0); \
\
    assert(n == decoded); \
  }

int
main () {
  test_uint8(0);
  test_uint8(1);
  test_uint8(2);

  test_uint8(UINT8_MAX);
}
