#include <ctype.h>
#include <stdio.h>
#include <aoc/aoc.h>
#include <string.h>

// all functions assume that the input string is longer than "length". currently
// aoc_file_read_lines1 uses a 255 long buffer for reading lines

#define DIGIT_STRINGS(f)                                                       \
  f("one"), f("two"), f("three"), f("four"), f("five"), f("six"), f("seven"),  \
      f("eight"), f("nine")
#define IDENTITY(x) x
#define STRING_LENGTH(x) (AOC_ARRAY_SIZE(x) - 1)
static const char *digit_strings[] = {DIGIT_STRINGS(IDENTITY)};
static const size_t digit_string_lengths[] = {DIGIT_STRINGS(STRING_LENGTH)};

static inline uint8_t first_digit(const char *const str, const size_t length,
                                  size_t *const index) {
  for (size_t i = 0; i < length; ++i) {
    if (isdigit(str[i])) {
      *index = i;
      return str[i] - '0';
    }
  }
  return 0;
}

static inline uint8_t first_string_digit(const char *const str,
                                         const size_t length,
                                         size_t *const index) {
  for (size_t i = 0, len = length; i < len; ++i) {
    for (size_t j = 0, digits = AOC_ARRAY_SIZE(digit_strings); j < digits;
         ++j) {
      if (str[i] == digit_strings[j][0] &&
          memcmp(str + i, digit_strings[j], digit_string_lengths[j]) == 0) {
        *index = i;
        return (uint8_t)(j + 1); // 0th string is "one"
      }
    }
  }
  return 0;
}

static inline uint8_t last_digit(const char *const str, const size_t length,
                                 size_t *const index) {
  for (int i = (int)length - 1; i >= 0; --i) {
    if (isdigit(str[i])) {
      *index = i;
      return str[i] - '0';
    }
  }
  return 0;
}

static inline uint8_t last_string_digit(const char *const str,
                                        const size_t length,
                                        size_t *const index) {
  for (int i = length - 3; i >= 0; --i) {
    for (size_t j = 0, digits = AOC_ARRAY_SIZE(digit_strings); j < digits;
         ++j) {
      if (str[i] == digit_strings[j][0] &&
          memcmp(str + i, digit_strings[j], digit_string_lengths[j]) == 0) {
        *index = (i + digit_string_lengths[j] - 1);
        return (uint8_t)(j + 1); // 0th string is "one"
      }
    }
  }
  return 0;
}

typedef struct {
  int part1;
  int part2;
} context;

static void parse(char *s, context *const ctx) {
  const size_t len = (int)strlen(s) - 1;
  size_t fi1 = len, fi2 = len, li1 = 0, li2 = 0;

  const uint8_t fd1 = first_digit(s, len, &fi1);
  const uint8_t ld1 = last_digit(s, len, &li1);
  ctx->part1 += (fd1 * 10 + ld1);

  const uint8_t fd2 = first_string_digit(s, len, &fi2);
  const uint8_t ld2 = last_string_digit(s, len, &li2);

  ctx->part2 += ((fi1 <= fi2 ? fd1 : fd2) * 10 + (li1 >= li2 ? ld1 : ld2));
}

int main(void) {
  context ctx = {0};
  aoc_file_read_lines1("day01/input.txt", (aoc_line_func)parse, &ctx);
  printf("%d\n%d\n", ctx.part1, ctx.part2);
}
