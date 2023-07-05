#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/compact.h"

#define test_uint(n) \
  { \
    int err; \
\
    compact_state_t state = {0, 0}; \
\
    err = compact_preencode_uint(&state, n); \
    assert(err == 0); \
\
    state.buffer = malloc(state.end); \
\
    err = compact_encode_uint(&state, n); \
    assert(err == 0); \
\
    state.start = 0; \
\
    uintmax_t decoded; \
    err = compact_decode_uint(&state, &decoded); \
    assert(err == 0); \
\
    assert(n == decoded); \
  }

int
main () {
  test_uint(0);
  test_uint(1);
  test_uint(2);

  test_uint(UINT8_MAX);

  test_uint(UINT16_MAX);

  test_uint(UINT32_MAX);

  test_uint(UINT64_MAX);
}
