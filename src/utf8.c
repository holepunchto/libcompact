#include <stddef.h>
#include <utf.h>

#include "../include/compact.h"
#include "utf/string.h"

int
compact_preencode_utf8 (compact_state_t *state, const utf8_string_view_t string) {
  int err = compact_preencode_uint(state, string.len);
  if (err < 0) return 0;

  state->end += string.len;

  return 0;
}

int
compact_encode_utf8 (compact_state_t *state, const utf8_string_view_t string) {
  int err = compact_encode_uint(state, string.len);
  if (err < 0) return 0;

  memcpy(&state->buffer[state->start], string.data, string.len);

  state->start += string.len;

  return 0;
}

int
compact_decode_utf8 (compact_state_t *state, utf8_string_view_t *result, void *data, compact_alloc_cb on_alloc) {
  uint64_t len;
  int err = compact_decode_uint(state, &len);
  if (err < 0) return err;

  if (state->end - state->start < len) return -1;

  if (result) {
    const utf8_t *bytes = &state->buffer[state->start];

    // Ill-formed bytes become U+FFFD, which is wider than what it stands for,
    // so the result cannot be a view into the buffer it was decoded from.
    size_t size = utf8_length_with_replacement(bytes, (size_t) len);

    utf8_t *copy = on_alloc(size, data);
    if (copy == NULL && size > 0) return -1;

    utf8_with_replacement(bytes, (size_t) len, copy);

    *result = utf8_string_view_init(copy, size);
  }

  state->start += len;

  return 0;
}
