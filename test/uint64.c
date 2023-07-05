#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/compact.h"

#define test_uint64(n) \
  { \
    int err; \
\
    compact_state_t state = {0, 0}; \
\
    err = compact_preencode_uint64(&state, n); \
    assert(err == 0); \
\
    state.buffer = malloc(state.end); \
\
    err = compact_encode_uint64(&state, n); \
    assert(err == 0); \
\
    state.start = 0; \
\
    uint64_t decoded; \
    err = compact_decode_uint64(&state, &decoded); \
    assert(err == 0); \
\
    assert(n == decoded); \
  }

int
main () {
  test_uint64(0);
  test_uint64(1);
  test_uint64(2);

  test_uint64(0x0102);

  test_uint64(0x01020304);

  test_uint64(0x0102030405060708);

  test_uint64(UINT64_MAX);
}
