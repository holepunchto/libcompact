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

#define test_uint16be(n) \
  { \
    int err; \
\
    compact_state_t state = {0, 0}; \
    compact_state_t ref = {0, 0}; \
\
    err = compact_preencode_uint16be(&state, n); \
    assert(err == 0); \
\
    state.buffer = malloc(state.end); \
\
    err = compact_encode_uint16be(&state, n); \
    assert(err == 0); \
\
    state.start = 0; \
\
    uint16_t decoded; \
    err = compact_decode_uint16be(&state, &decoded); \
    assert(err == 0); \
\
    assert(n == decoded); \
\
    err = compact_preencode_uint16(&ref, n); \
    assert(err == 0); \
\
    ref.buffer = malloc(ref.end); \
\
    err = compact_encode_uint16(&ref, n); \
    assert(err == 0); \
\
    assert(state.buffer[0] == ref.buffer[1]); \
    assert(state.buffer[1] == ref.buffer[0]); \
  }

int
main () {
  test_uint16(0);
  test_uint16(1);
  test_uint16(2);

  test_uint16(0x0102);

  test_uint16(UINT16_MAX);

  test_uint16be(0);
  test_uint16be(1);
  test_uint16be(2);
  test_uint16be(0x0102);
  test_uint16be(UINT16_MAX);
}
