#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/compact.h"

#define test_uint40(n) \
  { \
    int err; \
\
    compact_state_t state = {0, 0}; \
\
    err = compact_preencode_uint40(&state, n); \
    assert(err == 0); \
    assert(state.end == 5); \
\
    state.buffer = malloc(state.end); \
\
    err = compact_encode_uint40(&state, n); \
    assert(err == 0); \
\
    state.start = 0; \
\
    uint64_t decoded; \
    err = compact_decode_uint40(&state, &decoded); \
    assert(err == 0); \
\
    assert(n == decoded); \
  }

int
main () {
  test_uint40(0);
  test_uint40(1);
  test_uint40(0xFFFFFF);
  test_uint40(0xFFFFFFFF);
  test_uint40(0xFFFFFFFFFFULL);
}
