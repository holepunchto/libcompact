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

#define test_uintbe(n) \
  { \
    int err; \
\
    compact_state_t state = {0, 0}; \
    compact_state_t ref = {0, 0}; \
\
    err = compact_preencode_uintbe(&state, n); \
    assert(err == 0); \
\
    state.buffer = malloc(state.end); \
\
    err = compact_encode_uintbe(&state, n); \
    assert(err == 0); \
\
    state.start = 0; \
\
    uintmax_t decoded; \
    err = compact_decode_uintbe(&state, &decoded); \
    assert(err == 0); \
\
    assert(n == decoded); \
\
    err = compact_preencode_uint(&ref, n); \
    assert(err == 0); \
\
    ref.buffer = malloc(ref.end); \
\
    err = compact_encode_uint(&ref, n); \
    assert(err == 0); \
\
    assert(state.end == ref.end); \
    for(int i = 0; i < state.end; i++) { \
      assert(state.buffer[i] == ref.buffer[state.end - 1 - i]); \
    } \
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

  test_uintbe(0);
  test_uintbe(1);
  test_uintbe(2);
  test_uintbe(UINT8_MAX);
  test_uintbe(UINT16_MAX);
  test_uintbe(UINT32_MAX);
  test_uintbe(UINT64_MAX);
}
