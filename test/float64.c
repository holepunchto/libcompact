#undef NDEBUG
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../include/compact.h"

#define test_float64(n) \
  { \
    int err; \
\
    compact_state_t state = {0, 0}; \
\
    err = compact_preencode_float64(&state, n); \
    assert(err == 0); \
    assert(state.end == 8); \
\
    state.buffer = malloc(state.end); \
\
    err = compact_encode_float64(&state, n); \
    assert(err == 0); \
\
    state.start = 0; \
\
    double decoded; \
    err = compact_decode_float64(&state, &decoded); \
    assert(err == 0); \
\
    assert(n == decoded); \
\
    free(state.buffer); \
  }

int
main () {
  test_float64(0.0);
  test_float64(1.0);
  test_float64(-1.0);
  test_float64(0.5);
  test_float64(-0.5);
  test_float64(3.141592653589793);
  test_float64(1e300);
}
