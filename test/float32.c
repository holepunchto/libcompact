#undef NDEBUG
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../include/compact.h"

#define test_float32(n) \
  { \
    int err; \
\
    compact_state_t state = {0, 0}; \
\
    err = compact_preencode_float32(&state, n); \
    assert(err == 0); \
    assert(state.end == 4); \
\
    state.buffer = malloc(state.end); \
\
    err = compact_encode_float32(&state, n); \
    assert(err == 0); \
\
    state.start = 0; \
\
    float decoded; \
    err = compact_decode_float32(&state, &decoded); \
    assert(err == 0); \
\
    assert(n == decoded); \
\
    free(state.buffer); \
  }

int
main () {
  test_float32(0.0f);
  test_float32(1.0f);
  test_float32(-1.0f);
  test_float32(0.5f);
  test_float32(-0.5f);
  test_float32(3.5f);
  test_float32(1e30f);
}
