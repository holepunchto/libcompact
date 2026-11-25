#include <stddef.h>
#include <utf.h>

#include "../include/compact.h"

int
compact_decode_utf8_with_replacement (compact_state_t *state, utf8_string_view_t *result, void *data, compact_alloc_cb on_alloc) {
  uint64_t len;
  int err = compact_decode_uint(state, &len);
  if (err < 0) return err;

  if (state->end - state->start < len) return -1;

  if (result == NULL) {
    state->start += len;

    return 0;
  }

  const utf8_t *bytes = &state->buffer[state->start];

  size_t size = utf8_length_with_replacement(bytes, len);

  utf8_t *replaced = (utf8_t *) on_alloc(size, data);
  if (replaced == NULL) return -1;

  utf8_with_replacement(bytes, len, replaced);

  *result = utf8_string_view_init(replaced, size);

  state->start += len;

  return 0;
}
