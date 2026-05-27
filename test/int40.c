#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/compact.h"

#define test_int40(n) \
  { \
    int err; \
\
    compact_state_t state = {0, 0}; \
\
    err = compact_preencode_int40(&state, n); \
    assert(err == 0); \
    assert(state.end == 5); \
\
    state.buffer = malloc(state.end); \
\
    err = compact_encode_int40(&state, n); \
    assert(err == 0); \
\
    state.start = 0; \
\
    int64_t decoded; \
    err = compact_decode_int40(&state, &decoded); \
    assert(err == 0); \
\
    assert(n == decoded); \
  }

int
main () {
  test_int40(0);
  test_int40(1);
  test_int40(-1);
  test_int40(549755813887LL);
  test_int40(-549755813888LL);
}
