#include <stdio.h>
#include <aoc/aoc.h>
#include <aoc/mem.h>

static inline u8 calculate_hash(const char *const str, const u8 length) {
  u8 hash = 0;
  for (u8 i = 0; i < length; ++i)
    hash = (u8)(((u16)hash + (u16)str[i]) * 17u);
  return hash;
}

static u32 solve_part1(const char *input) {
  u32 sum = 0;
  while (*input != '\0') {
    const char *start = input;
    while (*input != ',' && *input != '\n')
      input++;
    sum += (u32)calculate_hash(start, input - start);
    input++;
  }
  return sum;
}

int main(void) {
  char *input = aoc_file_read_all2("day15/input.txt");
  printf("%u\n", solve_part1(input));
  aoc_free(input);
}
