#include <stddef.h>
#include <utf.h>

#include "../include/compact.h"

int
compact_decode_utf8_with_replacement (compact_state_t *state, utf8_string_t *result) {
  uint64_t len;
  int err = compact_decode_uint(state, &len);
  if (err < 0) return err;

  if (state->end - state->start < len) return -1;

  if (result) {
    const utf8_t *bytes = &state->buffer[state->start];

    size_t size = utf8_length_with_replacement(bytes, len);

    err = utf8_string_reserve(result, size);
    if (err < 0) return err;

    utf8_with_replacement(bytes, len, result->data);

    result->len = size;
  }

  state->start += len;

  return 0;
}
