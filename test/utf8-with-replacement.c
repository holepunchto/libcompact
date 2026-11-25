#include <assert.h>
#include <string.h>
#include <utf.h>

#include "../include/compact.h"

int
main () {
  int err;

  // A count of two, then a lead byte whose continuation never arrives
  uint8_t ill_formed[] = {0x02, 0xc3, 0x28};

  compact_state_t state = {0, sizeof(ill_formed), ill_formed};

  utf8_string_t replaced;
  utf8_string_init(&replaced);

  err = compact_decode_utf8_with_replacement(&state, &replaced);
  assert(err == 0);

  assert(replaced.len == 4);
  assert(memcmp(replaced.data, "\xef\xbf\xbd\x28", 4) == 0);
  assert(state.start == sizeof(ill_formed));

  utf8_string_destroy(&replaced);

  // The plain decoder hands back the two bytes as they came
  state.start = 0;

  utf8_string_view_t borrowed;
  err = compact_decode_utf8(&state, &borrowed);
  assert(err == 0);

  assert(borrowed.len == 2);
  assert(memcmp(borrowed.data, "\xc3\x28", 2) == 0);

  // Well formed input comes back unchanged, in its own memory
  uint8_t well_formed[] = {0x03, 0x61, 0x62, 0x63};

  state = (compact_state_t){0, sizeof(well_formed), well_formed};

  utf8_string_t copied;
  utf8_string_init(&copied);

  err = compact_decode_utf8_with_replacement(&state, &copied);
  assert(err == 0);

  assert(copied.len == 3);
  assert(memcmp(copied.data, "abc", 3) == 0);
  assert(copied.data != &well_formed[1]);

  utf8_string_destroy(&copied);

  // A count past the end of the input is rejected
  uint8_t truncated[] = {0x05, 0x61};

  state = (compact_state_t){0, sizeof(truncated), truncated};

  utf8_string_t unused;
  utf8_string_init(&unused);

  err = compact_decode_utf8_with_replacement(&state, &unused);
  assert(err < 0);

  utf8_string_destroy(&unused);
}
