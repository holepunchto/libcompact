#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/compact.h"

#define test_uint24(n) \
  { \
    int err; \
\
    compact_state_t state = {0, 0}; \
\
    err = compact_preencode_uint24(&state, n); \
    assert(err == 0); \
    assert(state.end == 3); \
\
    state.buffer = malloc(state.end); \
\
    err = compact_encode_uint24(&state, n); \
    assert(err == 0); \
\
    state.start = 0; \
\
    uint32_t decoded; \
    err = compact_decode_uint24(&state, &decoded); \
    assert(err == 0); \
\
    assert(n == decoded); \
  }

int
main () {
  test_uint24(0);
  test_uint24(1);
  test_uint24(255);
  test_uint24(256);
  test_uint24(65535);
  test_uint24(65536);
  test_uint24(0xFFFFFF);
}
