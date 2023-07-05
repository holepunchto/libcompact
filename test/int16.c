#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/compact.h"

#define test_int16(n) \
  { \
    int err; \
\
    compact_state_t state = {0, 0}; \
\
    err = compact_preencode_int16(&state, n); \
    assert(err == 0); \
\
    state.buffer = malloc(state.end); \
\
    err = compact_encode_int16(&state, n); \
    assert(err == 0); \
\
    state.start = 0; \
\
    int16_t decoded; \
    err = compact_decode_int16(&state, &decoded); \
    assert(err == 0); \
\
    assert(n == decoded); \
  }

int
main () {
  test_int16(0);
  test_int16(1);
  test_int16(2);
  test_int16(-1);
  test_int16(-2);

  test_int16(0x0102);
  test_int16(-0x0102);

  test_int16(INT16_MAX);
  test_int16(INT16_MIN);
}
