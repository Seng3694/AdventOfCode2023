#include <aoc/aoc.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static inline void set_bit(u32 bits[const 8], const size_t bit) {
  const size_t arrIndex = bit >> 5;
  const size_t bitIndex = bit & 31;
  bits[arrIndex] |= (1lu << bitIndex);
}

static inline u32 popcount(const u32 bits[const 8]) {
  return aoc_popcount(bits[0]) + aoc_popcount(bits[1]) + aoc_popcount(bits[2]) +
         aoc_popcount(bits[3]) + aoc_popcount(bits[4]) + aoc_popcount(bits[5]) +
         aoc_popcount(bits[6]) + aoc_popcount(bits[7]);
}

static inline void and
    (u64 dest[const 4], const u64 a[const 4], const u64 b[const 4]) {
  dest[0] = a[0] & b[0];
  dest[1] = a[1] & b[1];
  dest[2] = a[2] & b[2];
  dest[3] = a[3] & b[3];
}

static inline char *skip_spaces(char *line) {
  while (*line == ' ')
    line++;
  return line;
}

static inline char *skip_until(char *line, const char c) {
  while (*line != c)
    line++;
  return line;
}

static char *parse_numbers(char *line, u64 numbers[const 4], const char end) {
  line += 2;
  while (*line != end) {
    set_bit((u32 *)numbers, strtoul(line, &line, 10));
    line = skip_spaces(line);
  }
  return line;
}

static u64 temp[4];

static void parse(char *line, u32 *const part1) {
  u64 win[4] = {0};
  u64 actual[4] = {0};
  line = skip_until(line, ':');
  line = parse_numbers(line, win, '|');
  line = parse_numbers(line, actual, '\n');
  and(temp, win, actual);
  *part1 += (u32)pow(2, popcount((u32 *)temp) - 1);
}

int main(void) {
  u32 part1 = 0;
  aoc_file_read_lines1("day04/input.txt", (aoc_line_func)parse, &part1);
  printf("%u\n", part1);
}
