#include <aoc/aoc.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <aoc/mem.h>

#define RACES 4

static inline char *skip_spaces(char *str) {
  while (*str == ' ')
    str++;
  return str;
}

static void parse_part1(char *input, i64 times[const RACES],
                        i64 distances[RACES]) {
  input += 5;
  for (u8 i = 0; i < RACES; ++i) {
    input = skip_spaces(input);
    times[i] = strtoul(input, &input, 10);
  }
  input += 10;
  for (u8 i = 0; i < RACES; ++i) {
    input = skip_spaces(input);
    distances[i] = strtoul(input, &input, 10);
  }
}

static i64 parse_number_with_spaces(char *input, char **out) {
  char buffer[255] = {0};
  size_t length = 0;
  for (u8 i = 0; i < RACES; ++i) {
    input = skip_spaces(input);
    while (*input != '\n' && *input != '\0' && *input != ' ')
      buffer[length++] = *input++;
  }
  *out = input;
  return strtoull(buffer, NULL, 10);
}

static void parse_part2(char *input, i64 *const time, i64 *const distance) {
  input += 5;
  *time = parse_number_with_spaces(input, &input);
  input += 10;
  *distance = parse_number_with_spaces(input, &input);
}

// x(t) = -t² + totalTime * t + 0
// find both intersections with distance
// -t² + totalTime * t = dist
// -t² + totalTime * t - dist = 0
// t1 = (-totalTime + sqrt(totalTime² - 4*(-1)*(-dist))) / (2 * (-1))
// t2 = (-totalTime - sqrt(totalTime² - 4*(-1)*(-dist))) / (2 * (-1))
static inline i64 first_win(const i64 totalTime, const i64 distance) {
  return (-totalTime + sqrt((totalTime * totalTime) - (4 * distance))) / -2;
}

static inline i64 last_win(const i64 totalTime, const i64 distance) {
  return (-totalTime - sqrt((totalTime * totalTime) - (4 * distance))) / -2;
}

static inline i64 count_wins(const i64 time, const i64 distance) {
  return last_win(time, distance) - first_win(time, distance);
}

static i64 solve_part1(const i64 *const times, const i64 *const distances) {
  i64 solution = 1;
  for (u8 i = 0; i < RACES; ++i)
    solution *= count_wins(times[i], distances[i]);
  return solution;
}

int main(void) {
  char *input = aoc_file_read_all2("day06/input.txt");
  i64 times[RACES] = {0};
  i64 distances[RACES] = {0};
  parse_part1(input, times, distances);
  i64 time = 0;
  i64 distance = 0;
  parse_part2(input, &time, &distance);
  aoc_free(input);

  const i64 part1 = solve_part1(times, distances);
  const i64 part2 = count_wins(time, distance);

  printf("%lu\n%lu\n", part1, part2);
}
