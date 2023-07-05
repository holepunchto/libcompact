#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/compact.h"

#define test_uint32(n) \
  { \
    int err; \
\
    compact_state_t state = {0, 0}; \
\
    err = compact_preencode_uint32(&state, n); \
    assert(err == 0); \
\
    state.buffer = malloc(state.end); \
\
    err = compact_encode_uint32(&state, n); \
    assert(err == 0); \
\
    state.start = 0; \
\
    uint32_t decoded; \
    err = compact_decode_uint32(&state, &decoded); \
    assert(err == 0); \
\
    assert(n == decoded); \
  }

int
main () {
  test_uint32(0);
  test_uint32(1);
  test_uint32(2);

  test_uint32(0x0102);

  test_uint32(0x01020304);

  test_uint32(UINT32_MAX);
}
