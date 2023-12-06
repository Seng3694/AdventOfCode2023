#include <aoc/aoc.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <aoc/mem.h>

#define RACES 4

static void parse(char *input, i64 times[const RACES], i64 distances[RACES]) {
  input += 5;
  for (u8 i = 0; i < RACES; ++i) {
    while (*input == ' ')
      input++;
    times[i] = strtoul(input, &input, 10);
  }
  input += 10;
  for (u8 i = 0; i < RACES; ++i) {
    while (*input == ' ')
      input++;
    distances[i] = strtoul(input, &input, 10);
  }
}

static inline i64 first_win(const i64 t, const i64 x) {
  return (i64)(floor((-t + sqrt((t * t) - (4 * x))) / -2) + 1);
}

static inline i64 last_win(const i64 t, const i64 x) {
  return (i64)(ceil((-t - sqrt((t * t) - (4 * x))) / -2) - 1);
}

static inline i64 count_wins(const i64 t, const i64 x) {
  return last_win(t, x) - first_win(t, x) + 1;
}

static i64 solve_part1(const i64 *const times, const i64 *const distances) {
  i64 solution = 1;
  for (u8 i = 0; i < RACES; ++i)
    solution *= count_wins(times[i], distances[i]);
  return solution;
}

static i64 combine_numbers(const i64 numbers[const RACES]) {
  i64 result = 0, pos = 1;
  for (i8 i = RACES - 1; i >= 0; --i) {
    i64 current = numbers[i];
    for (; current > 0; pos *= 10) {
      result += (current % 10) * pos;
      current /= 10;
    }
  }
  return result;
}

int main(void) {
  char *input = aoc_file_read_all2("day06/input.txt");
  i64 times[RACES] = {0};
  i64 distances[RACES] = {0};
  parse(input, times, distances);
  aoc_free(input);

  const i64 part1 = solve_part1(times, distances);
  const i64 time = combine_numbers(times);
  const i64 distance = combine_numbers(distances);
  const i64 part2 = count_wins(time, distance);

  printf("%lu\n%lu\n", part1, part2);
}
