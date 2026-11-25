#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <utf.h>

#include "../include/compact.h"

static void *
on_alloc (size_t len, void *data) {
  return malloc(len);
}

int
main () {
  int err;

  // A count of two, then a lead byte whose continuation never arrives
  uint8_t ill_formed[] = {0x02, 0xc3, 0x28};

  compact_state_t state = {0, sizeof(ill_formed), ill_formed};

  utf8_string_view_t replaced;
  err = compact_decode_utf8_with_replacement(&state, &replaced, NULL, on_alloc);
  assert(err == 0);

  assert(replaced.len == 4);
  assert(memcmp(replaced.data, "\xef\xbf\xbd\x28", 4) == 0);
  assert(state.start == sizeof(ill_formed));

  // The plain decoder hands back the two bytes as they came
  state.start = 0;

  utf8_string_view_t borrowed;
  err = compact_decode_utf8(&state, &borrowed);
  assert(err == 0);

  assert(borrowed.len == 2);
  assert(memcmp(borrowed.data, "\xc3\x28", 2) == 0);

  // Well formed input is unchanged, though it is still copied out
  uint8_t well_formed[] = {0x03, 0x61, 0x62, 0x63};

  state = (compact_state_t){0, sizeof(well_formed), well_formed};

  err = compact_decode_utf8_with_replacement(&state, &replaced, NULL, on_alloc);
  assert(err == 0);

  assert(replaced.len == 3);
  assert(memcmp(replaced.data, "abc", 3) == 0);
  assert(replaced.data != &well_formed[1]);

  // A short input is rejected before anything is allocated
  uint8_t truncated[] = {0x05, 0x61};

  state = (compact_state_t){0, sizeof(truncated), truncated};

  err = compact_decode_utf8_with_replacement(&state, &replaced, NULL, on_alloc);
  assert(err < 0);
}
