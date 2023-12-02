#include <aoc/aoc.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct __attribute__((packed)) {
  u8 red;
  u8 green;
  u8 blue;
} round;

typedef struct {
  round rounds[6];
  u8 roundCount;
} game;

// could have just used an array because it's exactly 100 games
// but this works better for test inputs of variable lengths
#define AOC_T game
#include <aoc/vector.h>

static inline void skip_game_name(char *line, char **out) {
  while (*line != ':')
    line++;
  *out = line;
}

static void parse_field(char *line, char **out, round *const r) {
  const u8 count = strtoul(line, &line, 10);
  // clang-format off
  switch(line[1]) {
    case 'r': r->red   = count; *out = line + 4; break;
    case 'g': r->green = count; *out = line + 6; break;
    case 'b': r->blue  = count; *out = line + 5; break;
  }
  // clang-format on
}

static round parse_round(char *line, char **out) {
  round r = {0};
  // starts with ": 123 field"
  // continues with ", 123 field"
  do {
    parse_field(line + 2, &line, &r);
  } while (*line == ',');
  *out = line;
  return r;
}

static void parse(char *line, aoc_vector_game *const games) {
  game g = {0};
  skip_game_name(line, &line);
  while (*line != '\n')
    g.rounds[g.roundCount++] = parse_round(line, &line);
  aoc_vector_game_push(games, g);
}

static u32 solve_part1(const aoc_vector_game *const games) {
  const game *const g = games->items;
  u32 solution = 0;
  for (size_t i = 0; i < games->length; ++i) {
    for (u8 j = 0; j < g[i].roundCount; ++j) {
      const round *const r = &g[i].rounds[j];
      if (r->red > 12 || r->green > 13 || r->blue > 14)
        goto next;
    }
    solution += (i + 1);
  next:;
  }
  return solution;
}

static u32 solve_part2(const aoc_vector_game *const games) {
  const game *const g = games->items;
  u32 solution = 0;
  for (size_t i = 0; i < games->length; ++i) {
    u8 maxR = 0, maxG = 0, maxB = 0;
    for (u8 j = 0; j < g[i].roundCount; ++j) {
      maxR = AOC_MAX(maxR, g[i].rounds[j].red);
      maxG = AOC_MAX(maxG, g[i].rounds[j].green);
      maxB = AOC_MAX(maxB, g[i].rounds[j].blue);
    }
    solution += (maxR * maxG * maxB);
  }
  return solution;
}

int main(void) {
  aoc_vector_game games = {0};
  aoc_vector_game_create(&games, 128);
  aoc_file_read_lines1("day02/input.txt", (aoc_line_func)parse, &games);
  printf("%u\n%u\n", solve_part1(&games), solve_part2(&games));
  aoc_vector_game_destroy(&games);
}
