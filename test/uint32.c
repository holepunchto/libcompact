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

#define test_uint32be(n) \
  { \
    int err; \
\
    compact_state_t state = {0, 0}; \
    compact_state_t ref = {0, 0}; \
\
    err = compact_preencode_uint32be(&state, n); \
    assert(err == 0); \
\
    state.buffer = malloc(state.end); \
\
    err = compact_encode_uint32be(&state, n); \
    assert(err == 0); \
\
    state.start = 0; \
\
    uint32_t decoded; \
    err = compact_decode_uint32be(&state, &decoded); \
    assert(err == 0); \
\
    assert(n == decoded); \
\
    err = compact_preencode_uint32be(&ref, n); \
    assert(err == 0); \
\
    ref.buffer = malloc(ref.end); \
\
    err = compact_encode_uint32be(&ref, n); \
    assert(err == 0); \
\
    for(int i = 0; i < 4; i++) { \
      assert(state.buffer[i] == ref.buffer[4-i]); \
    } \
  }

int
main () {
  test_uint32(0);
  test_uint32(1);
  test_uint32(2);

  test_uint32(0x0102);

  test_uint32(0x01020304);

  test_uint32(UINT32_MAX);

  test_uint32be(0);
  test_uint32be(1);
  test_uint32be(2);
  test_uint32be(0x0102);
  test_uint32be(0x01020304);
  test_uint32be(UINT32_MAX);
}
