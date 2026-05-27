#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/compact.h"

#define test_int24(n) \
  { \
    int err; \
\
    compact_state_t state = {0, 0}; \
\
    err = compact_preencode_int24(&state, n); \
    assert(err == 0); \
    assert(state.end == 3); \
\
    state.buffer = malloc(state.end); \
\
    err = compact_encode_int24(&state, n); \
    assert(err == 0); \
\
    state.start = 0; \
\
    int32_t decoded; \
    err = compact_decode_int24(&state, &decoded); \
    assert(err == 0); \
\
    assert(n == decoded); \
  }

int
main () {
  test_int24(0);
  test_int24(1);
  test_int24(-1);
  test_int24(127);
  test_int24(-128);
  test_int24(8388607);
  test_int24(-8388608);
}
