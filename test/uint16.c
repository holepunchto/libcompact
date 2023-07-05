#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/compact.h"

#define test_uint16(n) \
  { \
    int err; \
\
    compact_state_t state = {0, 0}; \
\
    err = compact_preencode_uint16(&state, n); \
    assert(err == 0); \
\
    state.buffer = malloc(state.end); \
\
    err = compact_encode_uint16(&state, n); \
    assert(err == 0); \
\
    state.start = 0; \
\
    uint16_t decoded; \
    err = compact_decode_uint16(&state, &decoded); \
    assert(err == 0); \
\
    assert(n == decoded); \
  }

int
main () {
  test_uint16(0);
  test_uint16(1);
  test_uint16(2);

  test_uint16(0x0102);

  test_uint16(UINT16_MAX);
}
