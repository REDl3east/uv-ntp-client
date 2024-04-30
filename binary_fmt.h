#ifndef _BINARY_FMT_H_
#define _BINARY_FMT_H_

#define INT8_BINARY_FMT "%c%c%c%c%c%c%c%c"
#define INT8_BINARY_ARG(num)                   \
  ((num) & ((uint8_t)1 << 7) ? '1' : '0'),     \
      ((num) & ((uint8_t)1 << 6) ? '1' : '0'), \
      ((num) & ((uint8_t)1 << 5) ? '1' : '0'), \
      ((num) & ((uint8_t)1 << 4) ? '1' : '0'), \
      ((num) & ((uint8_t)1 << 3) ? '1' : '0'), \
      ((num) & ((uint8_t)1 << 2) ? '1' : '0'), \
      ((num) & ((uint8_t)1 << 1) ? '1' : '0'), \
      ((num) & ((uint8_t)1 << 0) ? '1' : '0')

#define INT16_BINARY_FMT "%c%c%c%c%c%c%c%c" \
                         "%c%c%c%c%c%c%c%c"

#define INT16_BINARY_ARG(num)                    \
  ((num) & ((uint16_t)1 << 15) ? '1' : '0'),     \
      ((num) & ((uint16_t)1 << 14) ? '1' : '0'), \
      ((num) & ((uint16_t)1 << 13) ? '1' : '0'), \
      ((num) & ((uint16_t)1 << 12) ? '1' : '0'), \
      ((num) & ((uint16_t)1 << 11) ? '1' : '0'), \
      ((num) & ((uint16_t)1 << 10) ? '1' : '0'), \
      ((num) & ((uint16_t)1 << 9) ? '1' : '0'),  \
      ((num) & ((uint16_t)1 << 8) ? '1' : '0'),  \
      INT8_BINARY_ARG(num)

#define INT32_BINARY_FMT "%c%c%c%c%c%c%c%c" \
                         "%c%c%c%c%c%c%c%c" \
                         "%c%c%c%c%c%c%c%c" \
                         "%c%c%c%c%c%c%c%c"

#define INT32_BINARY_ARG(num)                    \
  ((num) & ((uint32_t)1 << 31) ? '1' : '0'),     \
      ((num) & ((uint32_t)1 << 30) ? '1' : '0'), \
      ((num) & ((uint32_t)1 << 29) ? '1' : '0'), \
      ((num) & ((uint32_t)1 << 28) ? '1' : '0'), \
      ((num) & ((uint32_t)1 << 27) ? '1' : '0'), \
      ((num) & ((uint32_t)1 << 26) ? '1' : '0'), \
      ((num) & ((uint32_t)1 << 25) ? '1' : '0'), \
      ((num) & ((uint32_t)1 << 24) ? '1' : '0'), \
      ((num) & ((uint32_t)1 << 23) ? '1' : '0'), \
      ((num) & ((uint32_t)1 << 22) ? '1' : '0'), \
      ((num) & ((uint32_t)1 << 21) ? '1' : '0'), \
      ((num) & ((uint32_t)1 << 20) ? '1' : '0'), \
      ((num) & ((uint32_t)1 << 19) ? '1' : '0'), \
      ((num) & ((uint32_t)1 << 18) ? '1' : '0'), \
      ((num) & ((uint32_t)1 << 17) ? '1' : '0'), \
      ((num) & ((uint32_t)1 << 16) ? '1' : '0'), \
      INT16_BINARY_ARG(num)

#define INT64_BINARY_FMT "%c%c%c%c%c%c%c%c" \
                         "%c%c%c%c%c%c%c%c" \
                         "%c%c%c%c%c%c%c%c" \
                         "%c%c%c%c%c%c%c%c" \
                         "%c%c%c%c%c%c%c%c" \
                         "%c%c%c%c%c%c%c%c" \
                         "%c%c%c%c%c%c%c%c" \
                         "%c%c%c%c%c%c%c%c"

#define INT64_BINARY_ARG(num)                    \
  ((num) & ((uint64_t)1 << 63) ? '1' : '0'),     \
      ((num) & ((uint64_t)1 << 62) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 61) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 60) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 59) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 58) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 57) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 56) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 55) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 54) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 53) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 52) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 51) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 50) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 49) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 48) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 47) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 46) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 45) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 44) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 43) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 42) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 41) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 40) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 39) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 38) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 37) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 36) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 35) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 34) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 33) ? '1' : '0'), \
      ((num) & ((uint64_t)1 << 32) ? '1' : '0'), \
      INT32_BINARY_ARG(num)

#endif // _BINARY_FMT_H_