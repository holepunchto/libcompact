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

#define test_uint64be(n) \
  { \
    int err; \
\
    compact_state_t state = {0, 0}; \
    compact_state_t ref = {0, 0}; \
\
    err = compact_preencode_uint64be(&state, n); \
    assert(err == 0); \
\
    state.buffer = malloc(state.end); \
\
    err = compact_encode_uint64be(&state, n); \
    assert(err == 0); \
\
    state.start = 0; \
\
    uint64_t decoded; \
    err = compact_decode_uint64be(&state, &decoded); \
    assert(err == 0); \
\
    assert(n == decoded); \
\
    err = compact_preencode_uint64(&ref, n); \
    assert(err == 0); \
\
    ref.buffer = malloc(ref.end); \
\
    err = compact_encode_uint64(&ref, n); \
    assert(err == 0); \
\
    for (int i = 0U; i < 8; i++) {\
        assert(state.buffer[i] == ref.buffer[7 - i]);\
    }\
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

  // big endian
  test_uint64be(0);
  test_uint64be(1);
  test_uint64be(2);

  test_uint64be(0x0102);

  test_uint64be(0x01020304);

  test_uint64be(0x0102030405060708);

  test_uint64be(UINT64_MAX);
}
