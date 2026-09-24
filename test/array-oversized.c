#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/compact.h"

static int alloc_called = 0;

void *
on_alloc (size_t len, void *data) {
  alloc_called = 1;

  return NULL;
}

int
on_decode (compact_state_t *state, void *array, size_t i, void *data) {
  return -1;
}

int
main () {
  // varint 0xff + little-endian uint64 = a count of 2^40, in a 9 byte buffer
  uint8_t buffer[] = {0xff, 0, 0, 0, 0, 0, 0x01, 0, 0};

  compact_state_t state = {0, sizeof(buffer), buffer};

  void *decoded;
  size_t len;
  int err = compact_decode_array(&state, &decoded, &len, NULL, on_alloc, on_decode);

  assert(err < 0);
  assert(!alloc_called);
}
