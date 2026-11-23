#include <errno.h>
#include <inttypes.h>
#include <json.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utf.h>

#include "../include/compact.h"

// A harness in the shape CONFORMANCE.md describes, run against the corpus that
// compact-encoding-test ships. A codec the corpus covers and this library does
// not provide is a declaration rather than a failure, and its cases are counted
// as unrun.

#define SAFE_INTEGER UINT64_C(9007199254740991)

#define FIXTURES "node_modules/compact-encoding-test/fixtures"

typedef enum {
  answer_hex,
  answer_refused,
  answer_decodes,
  answer_rejects,
  answer_unrepresentable,
} answer_type_t;

typedef struct {
  answer_type_t type;
  uint8_t *hex;
  size_t hex_len;
  json_t *decodes;
  uint64_t read;
  uint8_t *bits;
  size_t bits_len;
} answer_t;

typedef void (*encode_cb)(json_t *value, answer_t *given);

typedef void (*decode_cb)(const uint8_t *bytes, size_t len, answer_t *given);

typedef struct {
  const char *name;
  encode_cb encode;
  decode_cb decode;
} codec_t;

typedef struct {
  const char **values;
  size_t len;
  size_t capacity;
} list_t;

typedef struct {
  const char *id;
  answer_t expected;
  answer_t given;
} divergence_t;

static const char *fixtures = FIXTURES;

static list_t matched;
static list_t unrun;
static list_t unsupported;
static list_t uncovered;
static list_t reasons;

static divergence_t *divergences;
static size_t divergences_len;

static json_t *unrepresentable_rules;

// The divergences this library is known to produce. Each one is a finding
// recorded in TODO.md, and anything outside this list fails the run.
static const char *expected[] = {
  // the corpus marks a wrong size unrepresentable from the release after 0.3.1
  "fixed-32-wrong-size",
  "fixed-64-wrong-size",
  // utf8 decode hands back the bytes without substituting
  "utf8-above-max-code-point",
  "utf8-count-cuts-a-sequence-short",
  "utf8-encoded-surrogate",
  "utf8-invalid-bytes",
  "utf8-overlong",
  "utf8-overlong-four-byte",
  "utf8-stranded-continuations",
  "utf8-subpart-completable",
  "utf8-subpart-uncompletable",
};

static bool
diverged (const char *id) {
  for (size_t i = 0; i < divergences_len; i++) {
    if (strcmp(divergences[i].id, id) == 0) return true;
  }

  return false;
}

static bool
is_expected (const char *id) {
  for (size_t i = 0; i < sizeof(expected) / sizeof(const char *); i++) {
    if (strcmp(expected[i], id) == 0) return true;
  }

  return false;
}

static void
push (list_t *list, const char *value) {
  if (list->len == list->capacity) {
    list->capacity = list->capacity ? list->capacity * 2 : 16;
    list->values = realloc(list->values, list->capacity * sizeof(const char *));
  }

  list->values[list->len++] = value;
}

static int
compare (const void *a, const void *b) {
  return strcmp(*(const char **) a, *(const char **) b);
}

static void
fatal (const char *message, const char *detail) {
  fprintf(stderr, "conformance: %s%s\n", message, detail);
  exit(1);
}

/**
 * Reading the corpus
 */

static json_t *
read_json (const char *path) {
  FILE *file = fopen(path, "rb");

  if (file == NULL) fatal("cannot open ", path);

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);

  utf8_t *source = malloc(size + 1);

  if (fread(source, 1, size, file) != (size_t) size) fatal("cannot read ", path);

  fclose(file);

  source[size] = '\0';

  json_t *result;
  if (json_decode_utf8(source, size, &result) < 0) fatal("cannot parse ", path);

  free(source);

  return result;
}

static json_t *
read_fixture (const char *name, const char *file) {
  char path[4096];

  snprintf(path, sizeof(path), "%s/%s/%s", fixtures, name, file);

  return read_json(path);
}

static json_t *
field (const json_t *object, const char *name) {
  if (object == NULL || !json_is_object(object)) return NULL;

  return json_object_get_literal_utf8(object, (const utf8_t *) name, strlen(name));
}

static const char *
text (const json_t *value) {
  if (value == NULL || !json_is_string(value)) return NULL;

  return (const char *) json_string_value_utf8(value);
}

/**
 * Notation, as CODECS.md writes values a harness cannot hold in JSON
 */

static json_t *
number (double value) {
  json_t *result;
  json_create_number(value, &result);
  return result;
}

static json_t *
string (const char *value) {
  json_t *result;
  json_create_string_utf8((const utf8_t *) value, strlen(value), &result);
  return result;
}

static json_t *
notation_unsigned (uint64_t n) {
  if (n <= SAFE_INTEGER) return number((double) n);

  char digits[32];
  snprintf(digits, sizeof(digits), "%" PRIu64 "n", n);

  return string(digits);
}

static json_t *
notation_signed (int64_t n) {
  if (n >= -(int64_t) SAFE_INTEGER && n <= (int64_t) SAFE_INTEGER) return number((double) n);

  char digits[32];
  snprintf(digits, sizeof(digits), "%" PRId64 "n", n);

  return string(digits);
}

static json_t *
notation_double (double n) {
  if (isnan(n)) return string("NaN");
  if (isinf(n)) return string(n > 0 ? "Infinity" : "-Infinity");
  if (n == 0 && signbit(n)) return string("-0");

  return number(n);
}

static json_t *
notation_bytes (const uint8_t *data, size_t len) {
  char *digits = malloc(len * 2 + 1);

  for (size_t i = 0; i < len; i++) {
    snprintf(&digits[i * 2], 3, "%02x", data[i]);
  }

  digits[len * 2] = '\0';

  json_t *result = string(digits);

  free(digits);

  return result;
}

static json_t *
notation_boolean (bool value) {
  json_t *result;
  json_create_boolean(value, &result);
  return result;
}

static bool
notation_equal (const json_t *a, const json_t *b) {
  if (a == NULL || b == NULL) return a == b;

  if (json_typeof(a) != json_typeof(b)) return false;

  switch (json_typeof(a)) {
  case json_boolean:
    return json_boolean_value(a) == json_boolean_value(b);

  case json_number:
    return json_number_value(a) == json_number_value(b);

  case json_string:
    return strcmp(text(a), text(b)) == 0;

  case json_array: {
    size_t len = json_array_size(a);

    if (len != json_array_size(b)) return false;

    for (size_t i = 0; i < len; i++) {
      if (!notation_equal(json_array_get(a, i), json_array_get(b, i))) return false;
    }

    return true;
  }

  case json_null:
    return true;

  default:
    return false;
  }
}

/**
 * Reading a value out of the notation and into a type this library carries
 */

static bool
wide (const json_t *value, const char **digits) {
  const char *source = text(value);

  if (source == NULL) return false;

  size_t len = strlen(source);

  if (len < 2 || source[len - 1] != 'n') return false;

  *digits = source;

  return true;
}

static bool
as_uint64 (const json_t *value, uint64_t *result) {
  const char *digits;

  if (value != NULL && json_is_number(value)) {
    double n = json_number_value(value);

    if (isnan(n) || isinf(n) || n < 0 || n >= 0x1p64) return false;

    *result = (uint64_t) n;

    // a fraction truncates, so the value is an integer only if it survives
    return (double) *result == n;
  }

  if (wide(value, &digits)) {
    if (digits[0] == '-') return false;

    char *end;
    errno = 0;
    unsigned long long n = strtoull(digits, &end, 10);

    if (*end != 'n' || errno == ERANGE) return false;

    *result = (uint64_t) n;

    return true;
  }

  return false;
}

static bool
as_int64 (const json_t *value, int64_t *result) {
  const char *digits;

  if (value != NULL && json_is_number(value)) {
    double n = json_number_value(value);

    if (isnan(n) || isinf(n) || n < -0x1p63 || n >= 0x1p63) return false;

    *result = (int64_t) n;

    return (double) *result == n;
  }

  if (wide(value, &digits)) {
    char *end;
    errno = 0;
    long long n = strtoll(digits, &end, 10);

    if (*end != 'n' || errno == ERANGE) return false;

    *result = (int64_t) n;

    return true;
  }

  return false;
}

static bool
as_double (const json_t *value, double *result) {
  if (value == NULL) return false;

  if (json_is_number(value)) {
    *result = json_number_value(value);

    return true;
  }

  const char *token = text(value);

  if (token == NULL) return false;

  if (strcmp(token, "NaN") == 0) *result = NAN;
  else if (strcmp(token, "Infinity") == 0) *result = INFINITY;
  else if (strcmp(token, "-Infinity") == 0) *result = -INFINITY;
  else if (strcmp(token, "-0") == 0) *result = -0.0;
  else return false;

  return true;
}

static bool
nibble (char digit, uint8_t *result) {
  if (digit >= '0' && digit <= '9') *result = digit - '0';
  else if (digit >= 'a' && digit <= 'f') *result = digit - 'a' + 10;
  else if (digit >= 'A' && digit <= 'F') *result = digit - 'A' + 10;
  else return false;

  return true;
}

static bool
as_bytes (const json_t *value, uint8_t **result, size_t *len) {
  const char *digits = text(value);

  if (digits == NULL) return false;

  size_t size = strlen(digits);

  if (size % 2 != 0) return false;

  *len = size / 2;
  *result = malloc(*len + 1);

  for (size_t i = 0; i < *len; i++) {
    uint8_t high, low;

    if (!nibble(digits[i * 2], &high) || !nibble(digits[i * 2 + 1], &low)) return false;

    (*result)[i] = (uint8_t) (high << 4 | low);
  }

  return true;
}

// Text is a sequence of UTF-16 code units, which this library does not carry:
// its string view is UTF-8. Where the units do not all pair the corpus writes
// them out, and libutf is what converts them.
static bool
as_text (const json_t *value, utf8_t **result, size_t *len) {
  const char *source = text(value);

  if (source != NULL) {
    *len = strlen(source);
    *result = malloc(*len + 1);
    memcpy(*result, source, *len + 1);

    return true;
  }

  const json_t *units = field(value, "units");

  if (units == NULL || !json_is_array(units)) return false;

  size_t size = json_array_size(units);
  utf16_t *utf16 = malloc(size * sizeof(utf16_t) + 1);

  for (size_t i = 0; i < size; i++) {
    utf16[i] = (utf16_t) json_number_value(json_array_get(units, i));
  }

  *result = malloc(utf8_length_from_utf16le(utf16, size) + 1);
  *len = utf16le_convert_to_utf8(utf16, size, *result);

  (*result)[*len] = '\0';

  free(utf16);

  return true;
}

/**
 * The outcomes a case can have
 */

static void
hex (answer_t *given, uint8_t *bytes, size_t len) {
  given->type = answer_hex;
  given->hex = bytes;
  given->hex_len = len;
}

static void
refused (answer_t *given) {
  given->type = answer_refused;
}

static void
decoded (answer_t *given, json_t *value, size_t read) {
  given->type = answer_decodes;
  given->decodes = value;
  given->read = read;
}

static void
rejected (answer_t *given) {
  given->type = answer_rejects;
}

static void
unrepresentable (answer_t *given) {
  given->type = answer_unrepresentable;
}

/**
 * The codecs
 *
 * A value this library's parameter cannot hold is unrepresentable, which the
 * corpus admits wherever the case cites a rule that says so. A value it can
 * hold is handed over, and whatever comes back is the answer.
 */

#define encode_with(codec, value) \
  compact_state_t state = {0, 0, NULL}; \
\
  if (compact_preencode_##codec(&state, value) < 0) return refused(given); \
\
  state.buffer = malloc(state.end); \
\
  if (compact_encode_##codec(&state, value) < 0) return refused(given); \
\
  hex(given, state.buffer, state.end);

#define decode_with(codec, result) \
  compact_state_t state = {0, len, (uint8_t *) bytes}; \
\
  if (compact_decode_##codec(&state, &result) < 0) return rejected(given);

#define declare_unsigned(codec, type, limit) \
  static void \
  encode_##codec (json_t *value, answer_t *given) { \
    uint64_t n; \
\
    if (!as_uint64(value, &n) || n > (uint64_t) (limit)) return unrepresentable(given); \
\
    encode_with(codec, (type) n) \
  } \
\
  static void \
  decode_##codec (const uint8_t *bytes, size_t len, answer_t *given) { \
    type n; \
\
    decode_with(codec, n) \
\
    decoded(given, notation_unsigned(n), state.start); \
  }

#define declare_signed(codec, type, min, max) \
  static void \
  encode_##codec (json_t *value, answer_t *given) { \
    int64_t n; \
\
    if (!as_int64(value, &n) || n < (int64_t) (min) || n > (int64_t) (max)) return unrepresentable(given); \
\
    encode_with(codec, (type) n) \
  } \
\
  static void \
  decode_##codec (const uint8_t *bytes, size_t len, answer_t *given) { \
    type n; \
\
    decode_with(codec, n) \
\
    decoded(given, notation_signed(n), state.start); \
  }

#define declare_float(codec, type) \
  static void \
  encode_##codec (json_t *value, answer_t *given) { \
    double n; \
\
    if (!as_double(value, &n)) return unrepresentable(given); \
\
    encode_with(codec, (type) n) \
  } \
\
  static void \
  decode_##codec (const uint8_t *bytes, size_t len, answer_t *given) { \
    type n; \
\
    decode_with(codec, n) \
\
    decoded(given, notation_double((double) n), state.start); \
\
    if (isnan(n)) { \
      compact_state_t bits = {0, 0, NULL}; \
\
      compact_preencode_##codec(&bits, n); \
      bits.buffer = malloc(bits.end); \
      compact_encode_##codec(&bits, n); \
\
      given->bits = bits.buffer; \
      given->bits_len = bits.end; \
    } \
  }

#define declare_fixed(codec, width) \
  static void \
  encode_##codec (json_t *value, answer_t *given) { \
    uint8_t *array; \
    size_t len; \
\
    if (!as_bytes(value, &array, &len) || len != width) return unrepresentable(given); \
\
    encode_with(codec, array) \
  } \
\
  static void \
  decode_##codec (const uint8_t *bytes, size_t len, answer_t *given) { \
    uint8_t array[width]; \
\
    compact_state_t state = {0, len, (uint8_t *) bytes}; \
\
    if (compact_decode_##codec(&state, array) < 0) return rejected(given); \
\
    decoded(given, notation_bytes(array, width), state.start); \
  }

declare_unsigned(uint, uint64_t, UINT64_MAX)
declare_unsigned(uintbe, uint64_t, UINT64_MAX)
declare_unsigned(uint8, uint8_t, UINT8_MAX)
declare_unsigned(uint16, uint16_t, UINT16_MAX)
declare_unsigned(uint16be, uint16_t, UINT16_MAX)
declare_unsigned(uint24, uint32_t, UINT32_MAX)
declare_unsigned(uint32, uint32_t, UINT32_MAX)
declare_unsigned(uint32be, uint32_t, UINT32_MAX)
declare_unsigned(uint40, uint64_t, UINT64_MAX)
declare_unsigned(uint48, uint64_t, UINT64_MAX)
declare_unsigned(uint56, uint64_t, UINT64_MAX)
declare_unsigned(uint64, uint64_t, UINT64_MAX)
declare_unsigned(uint64be, uint64_t, UINT64_MAX)

declare_signed(int, int64_t, INT64_MIN, INT64_MAX)
declare_signed(intbe, int64_t, INT64_MIN, INT64_MAX)
declare_signed(int8, int8_t, INT8_MIN, INT8_MAX)
declare_signed(int16, int16_t, INT16_MIN, INT16_MAX)
declare_signed(int16be, int16_t, INT16_MIN, INT16_MAX)
declare_signed(int24, int32_t, INT32_MIN, INT32_MAX)
declare_signed(int32, int32_t, INT32_MIN, INT32_MAX)
declare_signed(int32be, int32_t, INT32_MIN, INT32_MAX)
declare_signed(int40, int64_t, INT64_MIN, INT64_MAX)
declare_signed(int48, int64_t, INT64_MIN, INT64_MAX)
declare_signed(int56, int64_t, INT64_MIN, INT64_MAX)
declare_signed(int64, int64_t, INT64_MIN, INT64_MAX)
declare_signed(int64be, int64_t, INT64_MIN, INT64_MAX)

declare_float(float32, float)
declare_float(float64, double)

declare_fixed(fixed32, 32)
declare_fixed(fixed64, 64)

static void
encode_bool (json_t *value, answer_t *given) {
  if (value == NULL || !json_is_boolean(value)) return unrepresentable(given);

  encode_with(bool, json_boolean_value(value))
}

static void
decode_bool (const uint8_t *bytes, size_t len, answer_t *given) {
  bool n;

  decode_with(bool, n)

  decoded(given, notation_boolean(n), state.start);
}

static void
encode_uint8array (json_t *value, answer_t *given) {
  uint8_t *array;
  size_t size;

  if (!as_bytes(value, &array, &size)) return unrepresentable(given);

  compact_state_t state = {0, 0, NULL};

  if (compact_preencode_uint8array(&state, array, size) < 0) return refused(given);

  state.buffer = malloc(state.end);

  if (compact_encode_uint8array(&state, array, size) < 0) return refused(given);

  hex(given, state.buffer, state.end);
}

static void
decode_uint8array (const uint8_t *bytes, size_t len, answer_t *given) {
  uint8_t *array;
  size_t size;

  compact_state_t state = {0, len, (uint8_t *) bytes};

  if (compact_decode_uint8array(&state, &array, &size) < 0) return rejected(given);

  decoded(given, notation_bytes(array, size), state.start);
}

static void
encode_utf8 (json_t *value, answer_t *given) {
  utf8_t *data;
  size_t size;

  if (!as_text(value, &data, &size)) return unrepresentable(given);

  utf8_string_view_t view = utf8_string_view_init(data, size);

  encode_with(utf8, view)
}

static void
decode_utf8 (const uint8_t *bytes, size_t len, answer_t *given) {
  utf8_string_view_t view;

  compact_state_t state = {0, len, (uint8_t *) bytes};

  if (compact_decode_utf8(&state, &view) < 0) return rejected(given);

  utf8_t *data = malloc(view.len + 1);
  memcpy(data, view.data, view.len);
  data[view.len] = '\0';

  decoded(given, string((const char *) data), state.start);
}

static int
on_preencode_uint (compact_state_t *state, void *array, size_t i, void *data) {
  return compact_preencode_uint(state, ((uint64_t *) array)[i]);
}

static int
on_encode_uint (compact_state_t *state, void *array, size_t i, void *data) {
  return compact_encode_uint(state, ((uint64_t *) array)[i]);
}

static int
on_decode_uint (compact_state_t *state, void *array, size_t i, void *data) {
  return compact_decode_uint(state, &((uint64_t *) array)[i]);
}

static void *
on_alloc_uint (size_t len, void *data) {
  return malloc(len * sizeof(uint64_t));
}

static void
encode_array_uint (json_t *value, answer_t *given) {
  if (value == NULL || !json_is_array(value)) return unrepresentable(given);

  size_t size = json_array_size(value);
  uint64_t *array = malloc(size * sizeof(uint64_t) + 1);

  for (size_t i = 0; i < size; i++) {
    if (!as_uint64(json_array_get(value, i), &array[i])) return unrepresentable(given);
  }

  compact_state_t state = {0, 0, NULL};

  if (compact_preencode_array(&state, array, size, NULL, on_preencode_uint) < 0) return refused(given);

  state.buffer = malloc(state.end);

  if (compact_encode_array(&state, array, size, NULL, on_encode_uint) < 0) return refused(given);

  hex(given, state.buffer, state.end);
}

static void
decode_array_uint (const uint8_t *bytes, size_t len, answer_t *given) {
  uint64_t *array;
  size_t size;

  compact_state_t state = {0, len, (uint8_t *) bytes};

  if (compact_decode_array(&state, (void **) &array, &size, NULL, on_alloc_uint, on_decode_uint) < 0) {
    return rejected(given);
  }

  json_t *values;
  json_create_array(size, &values);

  for (size_t i = 0; i < size; i++) {
    json_array_set(values, i, notation_unsigned(array[i]));
  }

  decoded(given, values, state.start);
}

#define codec(name, id) \
  {name, encode_##id, decode_##id}

static const codec_t codecs[] = {
  codec("array-uint", array_uint),
  codec("bool", bool),
  codec("fixed-32", fixed32),
  codec("fixed-64", fixed64),
  codec("float32", float32),
  codec("float64", float64),
  codec("int", int),
  codec("int16", int16),
  codec("int16be", int16be),
  codec("int24", int24),
  codec("int32", int32),
  codec("int32be", int32be),
  codec("int40", int40),
  codec("int48", int48),
  codec("int56", int56),
  codec("int64", int64),
  codec("int64be", int64be),
  codec("int8", int8),
  codec("intbe", intbe),
  codec("uint", uint),
  codec("uint16", uint16),
  codec("uint16be", uint16be),
  codec("uint24", uint24),
  codec("uint32", uint32),
  codec("uint32be", uint32be),
  codec("uint40", uint40),
  codec("uint48", uint48),
  codec("uint56", uint56),
  codec("uint64", uint64),
  codec("uint64be", uint64be),
  codec("uint8", uint8),
  codec("uint8array", uint8array),
  codec("uintbe", uintbe),
  codec("utf8", utf8),
};

#undef codec

static const codec_t *
find (const char *name) {
  for (size_t i = 0; i < sizeof(codecs) / sizeof(codec_t); i++) {
    if (strcmp(codecs[i].name, name) == 0) return &codecs[i];
  }

  return NULL;
}

/**
 * Running a case and answering it
 */

static void
answer (const codec_t *codec, const json_t *input, answer_t *given) {
  const json_t *bytes = field(input, "bytes");

  if (bytes != NULL) {
    uint8_t *data;
    size_t len;

    if (!as_bytes(bytes, &data, &len)) return unrepresentable(given);

    return codec->decode(data, len, given);
  }

  codec->encode((json_t *) field(input, "value"), given);
}

static bool
parse_answer (const json_t *json, answer_t *expected) {
  const json_t *written = field(json, "hex");

  if (written != NULL) {
    expected->type = answer_hex;

    return as_bytes(written, &expected->hex, &expected->hex_len);
  }

  if (field(json, "refused") != NULL) {
    expected->type = answer_refused;

    return true;
  }

  const json_t *decodes = field(json, "decodes");

  if (decodes != NULL) {
    const json_t *bits = field(json, "bits");

    expected->type = answer_decodes;
    expected->decodes = (json_t *) decodes;
    expected->read = (uint64_t) json_number_value(field(json, "read"));

    return bits == NULL || as_bytes(bits, &expected->bits, &expected->bits_len);
  }

  if (field(json, "rejects") != NULL) {
    expected->type = answer_rejects;

    return true;
  }

  return false;
}

static bool
answer_equal (const answer_t *a, const answer_t *b) {
  if (a->type != b->type) return false;

  switch (a->type) {
  case answer_hex:
    return a->hex_len == b->hex_len && memcmp(a->hex, b->hex, a->hex_len) == 0;

  case answer_decodes:
    if (a->read != b->read) return false;
    if ((a->bits == NULL) != (b->bits == NULL)) return false;
    if (a->bits != NULL && (a->bits_len != b->bits_len || memcmp(a->bits, b->bits, a->bits_len) != 0)) return false;

    return notation_equal(a->decodes, b->decodes);

  default:
    return true;
  }
}

static bool
cites_unrepresentable (const json_t *rules) {
  if (rules == NULL || !json_is_array(rules)) return false;

  for (size_t i = 0; i < json_array_size(rules); i++) {
    const char *rule = text(json_array_get(rules, i));

    for (size_t j = 0; j < json_array_size(unrepresentable_rules); j++) {
      if (strcmp(rule, text(json_array_get(unrepresentable_rules, j))) == 0) return true;
    }
  }

  return false;
}

static bool
conformant (const answer_t *given, const answer_t *expected, const json_t *example) {
  if (answer_equal(given, expected)) return true;

  const json_t *alternatives = field(example, "alternatives");

  if (alternatives != NULL && json_is_array(alternatives)) {
    for (size_t i = 0; i < json_array_size(alternatives); i++) {
      answer_t alternative = {0};

      if (parse_answer(json_array_get(alternatives, i), &alternative) && answer_equal(given, &alternative)) {
        return true;
      }
    }
  }

  if (given->type == answer_unrepresentable) return cites_unrepresentable(field(example, "rules"));

  return false;
}

static void
diverge (const char *id, const answer_t *expected, const answer_t *given) {
  divergences = realloc(divergences, (divergences_len + 1) * sizeof(divergence_t));

  divergences[divergences_len].id = id;
  divergences[divergences_len].expected = *expected;
  divergences[divergences_len].given = *given;

  divergences_len++;
}

static void
run (const char *name) {
  json_t *cases = read_fixture(name, "cases.json");
  json_t *examples = field(cases, "cases");

  size_t len = examples == NULL ? 0 : json_array_size(examples);

  if (len == 0) {
    push(&uncovered, name);
    push(&reasons, text(field(cases, "reason")));

    return;
  }

  const codec_t *codec = find(name);

  if (codec == NULL) {
    push(&unsupported, name);

    for (size_t i = 0; i < len; i++) {
      push(&unrun, text(field(json_array_get(examples, i), "id")));
    }

    return;
  }

  json_t *answers = read_fixture(name, "answers.json");

  for (size_t i = 0; i < len; i++) {
    const json_t *example = json_array_get(examples, i);
    const char *id = text(field(example, "id"));

    answer_t expected = {0};

    if (!parse_answer(field(answers, id), &expected)) fatal("no answer for ", id);

    answer_t given = {0};

    answer(codec, field(example, "input"), &given);

    if (conformant(&given, &expected, example)) push(&matched, id);
    else diverge(id, &expected, &given);
  }
}

/**
 * The report
 */

static const utf32_t smallest[] = {0, 0, 0x80, 0x800, 0x10000};

static size_t
sequence (const unsigned char *c, utf32_t *code) {
  size_t width;

  if ((*c & 0xe0) == 0xc0) width = 2;
  else if ((*c & 0xf0) == 0xe0) width = 3;
  else if ((*c & 0xf8) == 0xf0) width = 4;
  else return 0;

  *code = *c & (0xff >> (width + 1));

  for (size_t i = 1; i < width; i++) {
    if ((c[i] & 0xc0) != 0x80) return 0;

    *code = (*code << 6) | (c[i] & 0x3f);
  }

  if (*code < smallest[width] || *code > 0x10ffff) return 0;

  if (*code >= 0xd800 && *code <= 0xdfff) return 0;

  return width;
}

// CODECS.md writes every code unit outside ASCII as an escape, so a divergence
// in text reads as the corpus writes it. A byte belonging to no well formed
// sequence has no code unit of its own and is written as the low surrogate
// 0xdc00 + the byte, which keeps the report both lossless and parseable.
static void
print_string (const char *value) {
  putchar('"');

  for (const unsigned char *c = (const unsigned char *) value; *c != '\0';) {
    if (*c == '"' || *c == '\\') {
      printf("\\%c", *c++);
      continue;
    }

    if (*c >= 0x20 && *c < 0x7f) {
      putchar(*c++);
      continue;
    }

    utf32_t code;
    size_t width = *c < 0x80 ? 1 : sequence(c, &code);

    if (*c < 0x80) code = *c;

    if (width == 0) {
      printf("\\udc%02x", *c++);
      continue;
    }

    if (code > 0xffff) {
      code -= 0x10000;
      printf("\\u%04x\\u%04x", 0xd800 + (code >> 10), 0xdc00 + (code & 0x3ff));
    } else {
      printf("\\u%04x", code);
    }

    c += width;
  }

  putchar('"');
}

static void
print_number (double value) {
  char digits[32];

  for (int precision = 1; precision <= 17; precision++) {
    snprintf(digits, sizeof(digits), "%.*g", precision, value);

    if (strtod(digits, NULL) == value) break;
  }

  printf("%s", digits);
}

static void
print_notation (const json_t *value) {
  switch (json_typeof(value)) {
  case json_boolean:
    printf(json_boolean_value(value) ? "true" : "false");
    break;

  case json_number:
    print_number(json_number_value(value));
    break;

  case json_string:
    print_string(text(value));
    break;

  case json_array:
    printf("[");

    for (size_t i = 0; i < json_array_size(value); i++) {
      if (i > 0) printf(", ");
      print_notation(json_array_get(value, i));
    }

    printf("]");
    break;

  default:
    printf("null");
  }
}

static void
print_bytes (const uint8_t *data, size_t len) {
  putchar('"');

  for (size_t i = 0; i < len; i++) {
    printf("%02x", data[i]);
  }

  putchar('"');
}

static void
print_answer (const answer_t *answer) {
  switch (answer->type) {
  case answer_hex:
    printf("{\"hex\":");
    print_bytes(answer->hex, answer->hex_len);
    printf("}");
    break;

  case answer_refused:
    printf("{\"refused\":true}");
    break;

  case answer_decodes:
    printf("{\"decodes\":");
    print_notation(answer->decodes);
    printf(",\"read\":%" PRIu64, answer->read);

    if (answer->bits != NULL) {
      printf(",\"bits\":");
      print_bytes(answer->bits, answer->bits_len);
    }

    printf("}");
    break;

  case answer_rejects:
    printf("{\"rejects\":true}");
    break;

  case answer_unrepresentable:
    printf("{\"unrepresentable\":true}");
    break;
  }
}

static void
print_list (const list_t *list) {
  printf("[");

  for (size_t i = 0; i < list->len; i++) {
    if (i > 0) printf(", ");
    print_string(list->values[i]);
  }

  printf("]");
}

static void
print_report (const char *corpus) {
  printf("{\n");
  printf("  \"corpus\": ");
  print_string(corpus);
  printf(",\n  \"implementation\": \"libcompact\",\n");
  printf("  \"summary\": { \"matched\": %zu, \"diverged\": %zu, \"unrun\": %zu, \"unsupported\": %zu, \"uncovered\": %zu },\n", matched.len, divergences_len, unrun.len, unsupported.len, uncovered.len);

  printf("  \"unsupported\": ");
  print_list(&unsupported);

  printf(",\n  \"uncovered\": {");

  for (size_t i = 0; i < uncovered.len; i++) {
    printf(i > 0 ? ", " : " ");
    print_string(uncovered.values[i]);
    printf(": ");
    print_string(reasons.values[i]);
  }

  printf(" },\n  \"diverged\": {");

  for (size_t i = 0; i < divergences_len; i++) {
    printf(i > 0 ? ", " : " ");
    print_string(divergences[i].id);
    printf(": { \"corpus\": ");
    print_answer(&divergences[i].expected);
    printf(", \"port\": ");
    print_answer(&divergences[i].given);
    printf(" }");
  }

  printf(" },\n  \"unrun\": ");
  print_list(&unrun);

  printf(",\n  \"matched\": ");
  print_list(&matched);

  printf("\n}\n");
}

static void
print_summary (void) {
  for (size_t i = 0; i < divergences_len; i++) {
    printf("DIVERGES %s\n", divergences[i].id);

    printf("  corpus: ");
    print_answer(&divergences[i].expected);
    printf("\n  port:   ");
    print_answer(&divergences[i].given);
    printf("\n");
  }

  printf("%zu cases matched, %zu diverged, %zu not run\n", matched.len, divergences_len, unrun.len);
  printf("%zu codecs unsupported", unsupported.len);

  for (size_t i = 0; i < unsupported.len; i++) {
    printf("%s%s", i == 0 ? ": " : ", ", unsupported.values[i]);
  }

  printf("\n%zu codecs the corpus has no cases for\n", uncovered.len);
}

// A case that stops diverging is reported rather than failed, so that a fix
// landing upstream does not turn the build red on its way in.
static size_t
print_gate (void) {
  size_t count = 0;

  for (size_t i = 0; i < divergences_len; i++) {
    if (is_expected(divergences[i].id)) continue;

    printf("UNEXPECTED %s\n", divergences[i].id);

    count++;
  }

  for (size_t i = 0; i < sizeof(expected) / sizeof(const char *); i++) {
    if (diverged(expected[i])) continue;

    printf("RESOLVED %s, drop it from the expected list\n", expected[i]);
  }

  return count;
}

int
main (int argc, char *argv[]) {
  bool as_json = false;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--json") == 0) as_json = true;
    else fixtures = argv[i];
  }

  char path[4096];

  snprintf(path, sizeof(path), "%s/index.json", fixtures);
  json_t *index = read_json(path);

  snprintf(path, sizeof(path), "%s/unrepresentable.json", fixtures);
  unrepresentable_rules = read_json(path);

  snprintf(path, sizeof(path), "%s/../package.json", fixtures);
  const char *corpus = text(field(read_json(path), "version"));

  for (size_t i = 0; i < json_object_size(index); i++) {
    const json_t *category = json_object_value(index, i);

    for (size_t j = 0; j < json_object_size(category); j++) {
      run(text(json_object_key(category, j)));
    }
  }

  qsort(matched.values, matched.len, sizeof(const char *), compare);
  qsort(unrun.values, unrun.len, sizeof(const char *), compare);
  qsort(unsupported.values, unsupported.len, sizeof(const char *), compare);

  if (as_json) print_report(corpus);
  else print_summary();

  return print_gate() == 0 ? 0 : 1;
}
