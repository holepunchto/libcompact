#ifndef COMPACT_ZIG_ZAG_H
#define COMPACT_ZIG_ZAG_H

// See https://stackoverflow.com/a/48924178

#define compact_encode_zig_zag(width, n) \
  ((uint##width##_t) n << 1) ^ -((uint##width##_t) n >> (width - 1))

#define compact_decode_zig_zag(width, n) \
  (int##width##_t)((n >> 1) ^ -(n & 1))

#endif // COMPACT_ZIG_ZAG_H
