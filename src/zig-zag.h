#ifndef COMPACT_ZIG_ZAG_H
#define COMPACT_ZIG_ZAG_H

#define compact_encode_zig_zag(n) \
  n < 0    ? (2 * -n) - 1 \
  : n == 0 ? 0 \
           : 2 * n

#define compact_decode_zig_zag(n) \
  n == 0         ? n \
  : (n & 1) == 0 ? n / 2 \
                 : -(n + 1) / 2

#endif // COMPACT_ZIG_ZAG_H
