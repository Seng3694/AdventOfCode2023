#include <aoc/common.h>
#include <aoc/filesystem.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <aoc/aoc.h>
#include <aoc/mem.h>

#define MAP_COUNT 7

typedef struct {
  i64 dest;
  i64 src;
  i64 length;
} map;

#define AOC_T u32
#define AOC_T_NAME seed
#include <aoc/vector.h>

#define AOC_T map
#include <aoc/vector.h>

static inline i32 sort_map_by_src(const map *const a, const map *const b) {
  return (i32)(a->src - b->src);
}

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

static i64 calculate_location(const u32 seed,
                              const aoc_vector_map maps[const MAP_COUNT]) {
  i64 current = seed;
  for (u8 j = 0; j < MAP_COUNT; ++j) {
    const aoc_vector_map *const m = &maps[j];
    for (size_t k = 0; k < m->length; ++k) {
      const map *const mp = &m->items[k];
      if (mp->src <= current && (mp->src + mp->length - 1) >= current) {
        current = mp->dest + (current - mp->src);
        break;
      }
    }
  }
  return current;
}

static i64 solve_part1(const aoc_vector_seed *const seeds,
                       const aoc_vector_map maps[const MAP_COUNT]) {
  i64 solution = INT32_MAX;
  for (size_t i = 0; i < seeds->length; ++i) {
    const i64 location = calculate_location(seeds->items[i], maps);
    solution = AOC_MIN(solution, location);
  }
  return solution;
}

static i64 solve_part2(const aoc_vector_seed *const seeds,
                       const aoc_vector_map maps[const MAP_COUNT]) {
  // TODO: bruteforce approach takes around 3m. fix it
  i64 solution = INT32_MAX;
  const u32 *const s = seeds->items;
  for (size_t i = 0; i < seeds->length; i += 2) {
    for (u32 seed = s[i], to = s[i] + s[i + 1]; seed < to; ++seed) {
      const i64 location = calculate_location(seed, maps);
      solution = AOC_MIN(solution, location);
    }
  }
  return solution;
}

int main(void) {
  char *input = aoc_file_read_all2("day05/input.txt");
  aoc_vector_seed seeds = {0};
  aoc_vector_map maps[MAP_COUNT] = {0};

  aoc_vector_seed_create(&seeds, 32);
  for (u8 i = 0; i < MAP_COUNT; ++i)
    aoc_vector_map_create(&maps[i], 64);

  parse(input, &seeds, maps);
  aoc_free(input);

  for (u8 i = 0; i < MAP_COUNT; ++i)
    qsort(maps[i].items, maps[i].length, sizeof(map),
          (int (*)(const void *, const void *))sort_map_by_src);

  const i64 part1 = solve_part1(&seeds, maps);
  const i64 part2 = solve_part2(&seeds, maps);
  printf("%ld\n%ld\n", part1, part2);

  aoc_vector_seed_destroy(&seeds);
  for (size_t i = 0; i < MAP_COUNT; ++i)
    aoc_vector_map_destroy(&maps[i]);
}
