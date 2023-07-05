#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/compact.h"

#define test_int(n) \
  { \
    int err; \
\
    compact_state_t state = {0, 0}; \
\
    err = compact_preencode_int(&state, n); \
    assert(err == 0); \
\
    state.buffer = malloc(state.end); \
\
    err = compact_encode_int(&state, n); \
    assert(err == 0); \
\
    state.start = 0; \
\
    intmax_t decoded; \
    err = compact_decode_int(&state, &decoded); \
    assert(err == 0); \
\
    assert(n == decoded); \
  }

int
main () {
  test_int(0);
  test_int(1);
  test_int(2);
  test_int(-1);
  test_int(-2);

  test_int(INT8_MAX);
  test_int(INT8_MIN);

  test_int(INT16_MAX);
  test_int(INT16_MIN);

  test_int(INT32_MAX);
  test_int(INT32_MIN);

  test_int(INT64_MAX);
  test_int(INT64_MIN);
}
