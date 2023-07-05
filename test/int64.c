#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/compact.h"

#define test_int64(n) \
  { \
    int err; \
\
    compact_state_t state = {0, 0}; \
\
    err = compact_preencode_int64(&state, n); \
    assert(err == 0); \
\
    state.buffer = malloc(state.end); \
\
    err = compact_encode_int64(&state, n); \
    assert(err == 0); \
\
    state.start = 0; \
\
    int64_t decoded; \
    err = compact_decode_int64(&state, &decoded); \
    assert(err == 0); \
    printf("decoded=%lld\n", decoded); \
\
    assert(n == decoded); \
  }

int
main () {
  test_int64(0);
  test_int64(1);
  test_int64(2);
  test_int64(-1);
  test_int64(-2);

  test_int64(0x0102);
  test_int64(-0x0102);

  test_int64(0x01020304);
  test_int64(-0x01020304);

  test_int64(0x0102030405060708);
  test_int64(-0x0102030405060708);

  test_int64(INT64_MAX);
  test_int64(INT64_MIN);
}
