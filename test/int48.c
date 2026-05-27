#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/compact.h"

#define test_int48(n) \
  { \
    int err; \
\
    compact_state_t state = {0, 0}; \
\
    err = compact_preencode_int48(&state, n); \
    assert(err == 0); \
    assert(state.end == 6); \
\
    state.buffer = malloc(state.end); \
\
    err = compact_encode_int48(&state, n); \
    assert(err == 0); \
\
    state.start = 0; \
\
    int64_t decoded; \
    err = compact_decode_int48(&state, &decoded); \
    assert(err == 0); \
\
    assert(n == decoded); \
  }

int
main () {
  test_int48(0);
  test_int48(1);
  test_int48(-1);
  test_int48(140737488355327LL);
  test_int48(-140737488355328LL);
}
