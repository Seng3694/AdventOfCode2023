#include <aoc/aoc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

typedef struct {
  u32 part1;
  // there are only around 200 used numbers
  u32 matches[0xff];
  i32 counts[0xff];
  u32 cards;
} context;

static u64 temp[4];

static void parse(char *line, context *const ctx, const size_t lineNumber) {
  u64 win[4] = {0};
  u64 actual[4] = {0};
  line = skip_until(line, ':');
  line = parse_numbers(line, win, '|');
  line = parse_numbers(line, actual, '\n');
  and(temp, win, actual);
  const u32 matches = popcount((u32 *)temp);
  ctx->matches[lineNumber + 1] = matches;
  ctx->part1 += matches == 0 ? 0 : 1 << (matches - 1);
  ctx->cards++;
}

static u32 get_card_count(context *const ctx, const u32 id) {
  if (ctx->counts[id] != -1)
    return ctx->counts[id];

  u32 sum = 1;
  for (u32 i = 1; i <= ctx->matches[id]; ++i)
    sum += get_card_count(ctx, id + i);

  ctx->counts[id] = sum;
  return sum;
}

static inline u32 solve(context *const ctx) {
  u32 count = 0;
  for (u32 i = 0; i < ctx->cards; ++i)
    count += get_card_count(ctx, i);
  return count;
}

int main(void) {
  context ctx = {0};
  memset(ctx.counts, -1, sizeof(u32) * 0xff);
  aoc_file_read_lines2("day04/input.txt", (aoc_line_num_func)parse, &ctx);
  const u32 part2 = solve(&ctx);
  printf("%u\n%u\n", ctx.part1, part2);
}
