#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/compact.h"

#define test_int32(n) \
  { \
    int err; \
\
    compact_state_t state = {0, 0}; \
\
    err = compact_preencode_int32(&state, n); \
    assert(err == 0); \
\
    state.buffer = malloc(state.end); \
\
    err = compact_encode_int32(&state, n); \
    assert(err == 0); \
\
    state.start = 0; \
\
    int32_t decoded; \
    err = compact_decode_int32(&state, &decoded); \
    assert(err == 0); \
\
    assert(n == decoded); \
  }

int
main () {
  test_int32(0);
  test_int32(1);
  test_int32(2);
  test_int32(-1);
  test_int32(-2);

  test_int32(0x0102);
  test_int32(-0x0102);

  test_int32(0x01020304);
  test_int32(-0x01020304);

  test_int32(INT32_MAX);
  test_int32(INT32_MIN);
}
