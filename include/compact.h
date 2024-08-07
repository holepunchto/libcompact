#ifndef COMPACT_H
#define COMPACT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <utf.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct compact_state_s compact_state_t;

typedef void *(*compact_alloc_cb)(size_t len, void *data);

typedef int (*compact_array_cb)(compact_state_t *state, void *array, size_t i, void *data);

struct compact_state_s {
  size_t start;
  size_t end;
  uint8_t *buffer;
};

int
compact_preencode_bool (compact_state_t *state, bool value);

int
compact_encode_bool (compact_state_t *state, bool value);

int
compact_decode_bool (compact_state_t *state, bool *result);

int
compact_preencode_uint (compact_state_t *state, uintmax_t n);

int
compact_encode_uint (compact_state_t *state, uintmax_t n);

int
compact_decode_uint (compact_state_t *state, uintmax_t *result);

int
compact_preencode_uint8 (compact_state_t *state, uint8_t n);

int
compact_encode_uint8 (compact_state_t *state, uint8_t n);

int
compact_decode_uint8 (compact_state_t *state, uint8_t *result);

int
compact_preencode_uint16 (compact_state_t *state, uint16_t n);

int
compact_encode_uint16 (compact_state_t *state, uint16_t n);

int
compact_decode_uint16 (compact_state_t *state, uint16_t *result);

int
compact_preencode_uint32 (compact_state_t *state, uint32_t n);

int
compact_encode_uint32 (compact_state_t *state, uint32_t n);

int
compact_decode_uint32 (compact_state_t *state, uint32_t *result);

int
compact_preencode_uint64 (compact_state_t *state, uint64_t n);

int
compact_encode_uint64 (compact_state_t *state, uint64_t n);

int
compact_decode_uint64 (compact_state_t *state, uint64_t *result);

int
compact_preencode_int (compact_state_t *state, intmax_t n);

int
compact_encode_int (compact_state_t *state, intmax_t n);

int
compact_decode_int (compact_state_t *state, intmax_t *result);

int
compact_preencode_int8 (compact_state_t *state, int8_t n);

int
compact_encode_int8 (compact_state_t *state, int8_t n);

int
compact_decode_int8 (compact_state_t *state, int8_t *result);

int
compact_preencode_int16 (compact_state_t *state, int16_t n);

int
compact_encode_int16 (compact_state_t *state, int16_t n);

int
compact_decode_int16 (compact_state_t *state, int16_t *result);

int
compact_preencode_int32 (compact_state_t *state, int32_t n);

int
compact_encode_int32 (compact_state_t *state, int32_t n);

int
compact_decode_int32 (compact_state_t *state, int32_t *result);

int
compact_preencode_int64 (compact_state_t *state, int64_t n);

int
compact_encode_int64 (compact_state_t *state, int64_t n);

int
compact_decode_int64 (compact_state_t *state, int64_t *result);

int
compact_preencode_int64 (compact_state_t *state, int64_t n);

int
compact_encode_int64 (compact_state_t *state, int64_t n);

int
compact_decode_int64 (compact_state_t *state, int64_t *result);

int
compact_preencode_float32 (compact_state_t *state, float n);

int
compact_encode_float32 (compact_state_t *state, float n);

int
compact_decode_float32 (compact_state_t *state, float *result);

int
compact_preencode_float64 (compact_state_t *state, double n);

int
compact_encode_float64 (compact_state_t *state, double n);

int
compact_decode_float64 (compact_state_t *state, double *result);

int
compact_preencode_uint8array (compact_state_t *state, const uint8_t *array, size_t len);

int
compact_encode_uint8array (compact_state_t *state, const uint8_t *array, size_t len);

int
compact_decode_uint8array (compact_state_t *state, uint8_t **result, size_t *len);

int
compact_preencode_fixed32 (compact_state_t *state, const uint8_t array[32]);

int
compact_encode_fixed32 (compact_state_t *state, const uint8_t array[32]);

int
compact_decode_fixed32 (compact_state_t *state, uint8_t result[32]);

int
compact_preencode_fixed64 (compact_state_t *state, const uint8_t array[64]);

int
compact_encode_fixed64 (compact_state_t *state, const uint8_t array[64]);

int
compact_decode_fixed64 (compact_state_t *state, uint8_t result[64]);

int
compact_preencode_utf8 (compact_state_t *state, const utf8_string_view_t string);

int
compact_encode_utf8 (compact_state_t *state, const utf8_string_view_t string);

int
compact_decode_utf8 (compact_state_t *state, utf8_string_view_t *result);

int
compact_preencode_array (compact_state_t *state, void *array, size_t len, void *data, compact_array_cb on_preencode);

int
compact_encode_array (compact_state_t *state, void *array, size_t len, void *data, compact_array_cb on_encode);

int
compact_decode_array (compact_state_t *state, void **result, size_t *len, void *data, compact_alloc_cb on_alloc, compact_array_cb on_decode);

#ifdef __cplusplus
}
#endif

#endif // COMPACT_H
