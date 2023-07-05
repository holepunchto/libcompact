#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/compact.h"

#define test_int8(n) \
  { \
    int err; \
\
    compact_state_t state = {0, 0}; \
\
    err = compact_preencode_int8(&state, n); \
    assert(err == 0); \
\
    state.buffer = malloc(state.end); \
\
    err = compact_encode_int8(&state, n); \
    assert(err == 0); \
\
    state.start = 0; \
\
    int8_t decoded; \
    err = compact_decode_int8(&state, &decoded); \
    assert(err == 0); \
\
    assert(n == decoded); \
  }

int
main () {
  test_int8(0);
  test_int8(1);
  test_int8(2);
  test_int8(-1);
  test_int8(-2);

  test_int8(INT8_MAX);
  test_int8(INT8_MIN);
}
