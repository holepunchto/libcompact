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

#define test_intbe(n) \
  { \
    int err; \
\
    compact_state_t state = {0, 0}; \
    compact_state_t ref = {0, 0}; \
\
    err = compact_preencode_intbe(&state, n); \
    assert(err == 0); \
\
    state.buffer = malloc(state.end); \
\
    err = compact_encode_intbe(&state, n); \
    assert(err == 0); \
\
    state.start = 0; \
\
    intmax_t decoded; \
    err = compact_decode_intbe(&state, &decoded); \
    assert(err == 0); \
\
    assert(n == decoded); \
\
    err = compact_preencode_int(&ref, n); \
    assert(err == 0); \
\
    ref.buffer = malloc(ref.end); \
\
    err = compact_encode_int(&ref, n); \
    assert(err == 0); \
\
    assert(state.end == ref.end); \
    for(int i = 0; i < state.end; i++) { \
      assert(state.buffer[i] == ref.buffer[i - state.end - 1]); \
    } \
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

  test_intbe(0);
  test_intbe(1);
  test_intbe(2);
  test_intbe(-1);
  test_intbe(-2);
  test_intbe(INT8_MAX);
  test_intbe(INT8_MIN);
  test_intbe(INT16_MAX);
  test_intbe(INT16_MIN);
  test_intbe(INT32_MAX);
  test_intbe(INT32_MIN);
  test_intbe(INT64_MAX);
  test_intbe(INT64_MIN);
}
