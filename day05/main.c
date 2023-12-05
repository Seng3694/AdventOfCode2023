#include <aoc/filesystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <aoc/aoc.h>
#include <aoc/mem.h>

#define MAP_COUNT 7

typedef struct {
  u32 dest;
  u32 src;
  u32 length;
} map;

#define AOC_T u32
#define AOC_T_NAME seed
#include <aoc/vector.h>

#define AOC_T map
#include <aoc/vector.h>

static char *parse_seeds(char *input, aoc_vector_seed *const seeds) {
  input += 7;
  while (*input != '\n') {
    const u32 seed = strtoul(input, &input, 10);
    aoc_vector_seed_push(seeds, seed);
    input++;
  }
  return input + 1;
}

static char *skip_line(char *input) {
  while (*input != '\n')
    input++;
  return input + 1;
}

static char *parse_map(char *input, aoc_vector_map *const maps) {
  input = skip_line(input);
  while (*input != '\n' && *input != '\0') {
    map m = {0};
    m.dest = strtoul(input, &input, 10);
    m.src = strtoul(input + 1, &input, 10);
    m.length = strtoul(input + 1, &input, 10);
    aoc_vector_map_push(maps, m);
    input++;
  }
  return input + 1;
}

static void parse(char *input, aoc_vector_seed *const seeds,
                  aoc_vector_map maps[const MAP_COUNT]) {
  input = parse_seeds(input, seeds);
  for (size_t i = 0; i < MAP_COUNT; ++i)
    input = parse_map(input, &maps[i]);
}

int main(void) {
  char *input = aoc_file_read_all2("day05/input.txt");
  aoc_vector_seed seeds = {0};
  aoc_vector_map maps[MAP_COUNT] = {0};

  aoc_vector_seed_create(&seeds, 32);
  for (size_t i = 0; i < MAP_COUNT; ++i)
    aoc_vector_map_create(&maps[i], 64);

  parse(input, &seeds, maps);
  aoc_free(input);

  aoc_vector_seed_destroy(&seeds);
  for (size_t i = 0; i < MAP_COUNT; ++i)
    aoc_vector_map_destroy(&maps[i]);
}
