#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <utf.h>

#include "../include/compact.h"

int
on_preencode (compact_state_t *state, void *array, size_t i, void *data) {
  utf8_t **strings = (utf8_t **) array;

  return compact_preencode_utf8(state, strings[i], strlen((const char *) strings[i]));
}

int
on_encode (compact_state_t *state, void *array, size_t i, void *data) {
  utf8_t **strings = (utf8_t **) array;

  return compact_encode_utf8(state, strings[i], strlen((const char *) strings[i]));
}

int
on_decode (compact_state_t *state, void *array, size_t i, void *data) {
  utf8_t **strings = (utf8_t **) array;

  return compact_decode_utf8(state, &strings[i], NULL);
}

void *
on_alloc (size_t len, void *data) {
  return malloc(len * sizeof(utf8_t *));
}

int
main () {
  int err;

  utf8_t *array[] = {
    (utf8_t *) "hello",
    (utf8_t *) "world",
  };

  compact_state_t state = {0, 0};

  err = compact_preencode_array(&state, array, 2, NULL, on_preencode);
  assert(err == 0);

  state.buffer = malloc(state.end);

  err = compact_encode_array(&state, array, 2, NULL, on_encode);
  assert(err == 0);

  state.start = 0;

  utf8_t **decoded;
  size_t len;
  err = compact_decode_array(&state, (void **) &decoded, &len, NULL, on_alloc, on_decode);
  assert(err == 0);
  assert(len == 2);

  for (size_t i = 0; i < len; i++) {
    assert(strcmp((char *) array[i], (char *) decoded[i]) == 0);
  }
}
