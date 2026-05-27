#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/compact.h"

#define test_uint56(n) \
  { \
    int err; \
\
    compact_state_t state = {0, 0}; \
\
    err = compact_preencode_uint56(&state, n); \
    assert(err == 0); \
    assert(state.end == 7); \
\
    state.buffer = malloc(state.end); \
\
    err = compact_encode_uint56(&state, n); \
    assert(err == 0); \
\
    state.start = 0; \
\
    uint64_t decoded; \
    err = compact_decode_uint56(&state, &decoded); \
    assert(err == 0); \
\
    assert(n == decoded); \
  }

int
main () {
  test_uint56(0);
  test_uint56(1);
  test_uint56(0xFFFFFFFFFFFFULL);
  test_uint56(0xFFFFFFFFFFFFFFULL);
}
