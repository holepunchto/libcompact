#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/compact.h"

#define test_int56(n) \
  { \
    int err; \
\
    compact_state_t state = {0, 0}; \
\
    err = compact_preencode_int56(&state, n); \
    assert(err == 0); \
    assert(state.end == 7); \
\
    state.buffer = malloc(state.end); \
\
    err = compact_encode_int56(&state, n); \
    assert(err == 0); \
\
    state.start = 0; \
\
    int64_t decoded; \
    err = compact_decode_int56(&state, &decoded); \
    assert(err == 0); \
\
    assert(n == decoded); \
  }

int
main () {
  test_int56(0);
  test_int56(1);
  test_int56(-1);
  test_int56(36028797018963967LL);
  test_int56(-36028797018963968LL);
}
