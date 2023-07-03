#include <stdint.h>
#include <stdio.h>

#include "../include/compact.h"

int
compact_preencode_array (compact_state_t *state, void *array, size_t len, void *data, compact_array_cb on_preencode) {
  int err;

  err = compact_preencode_uint(state, len);
  if (err < 0) return err;

  for (size_t i = 0; i < len; i++) {
    err = on_preencode(state, array, i, data);
    if (err < 0) return err;
  }

  return 0;
}

int
compact_encode_array (compact_state_t *state, void *array, size_t len, void *data, compact_array_cb on_encode) {
  int err;

  err = compact_encode_uint(state, len);
  if (err < 0) return err;

  for (size_t i = 0; i < len; i++) {
    err = on_encode(state, array, i, data);
    if (err < 0) return err;
  }

  return 0;
}

int
compact_decode_array (compact_state_t *state, void **result, size_t *len, void *data, compact_alloc_cb on_alloc, compact_array_cb on_decode) {
  int err;

  size_t size;
  err = compact_decode_uint(state, &size);
  if (err < 0) return err;

  void *array = on_alloc(size, data);

  if (result) *result = array;
  if (len) *len = 0;

  for (size_t i = 0; i < size; i++) {
    err = on_decode(state, array, i, data);
    if (err < 0) return err;

    if (len) *len += 1;
  }

  return 0;
}
