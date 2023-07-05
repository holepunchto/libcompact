#include <stdint.h>

#include "../include/compact.h"

int
compact_preencode_uint (compact_state_t *state, uintmax_t n) {
  state->end += n <= 0xfc         ? 1
                : n <= 0xffff     ? 3
                : n <= 0xffffffff ? 5
                                  : 9;
  return 0;
}

int
compact_encode_uint (compact_state_t *state, uintmax_t n) {
  if (n <= 0xfc) {
    return compact_encode_uint8(state, n & 0xff);
  }

  if (n <= 0xffff) {
    state->buffer[state->start++] = 0xfd;

    return compact_encode_uint16(state, n & 0xffff);
  }

  if (n <= 0xffffffff) {
    state->buffer[state->start++] = 0xfe;

    return compact_encode_uint32(state, n & 0xffffffff);
  }

  state->buffer[state->start++] = 0xff;

  return compact_encode_uint64(state, n);
}

int
compact_decode_uint (compact_state_t *state, uintmax_t *result) {
  int err;

  uint8_t uint8;
  err = compact_decode_uint8(state, &uint8);
  if (err < 0) return err;

  if (uint8 <= 0xfc) {
    if (result) *result = uint8;

    return 0;
  }

  if (uint8 == 0xfd) {
    uint16_t uint16;
    err = compact_decode_uint16(state, result ? &uint16 : NULL);
    if (err < 0) return err;

    if (result) *result = uint16;

    return 0;
  }

  if (uint8 == 0xfe) {
    uint32_t uint32;
    err = compact_decode_uint32(state, result ? &uint32 : NULL);
    if (err < 0) return err;

    if (result) *result = uint32;

    return 0;
  }

  uint64_t uint64;
  err = compact_decode_uint64(state, result ? &uint64 : NULL);
  if (err < 0) return err;

  if (result) *result = uint64;

  return 0;
}
