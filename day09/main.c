#include <stdlib.h>
#include <stdio.h>
#include <aoc/aoc.h>
#include <aoc/mem.h>
#include <aoc/bump.h>

#define AOC_T i32
#include <aoc/vector.h>

#define AOC_T aoc_vector_i32
#define AOC_T_NAME numbers
#include <aoc/vector.h>

static void parse(char *line, aoc_vector_numbers *const numbers) {
  aoc_vector_i32 n = {0};
  aoc_vector_i32_create(&n, 22);
  line--;
  do {
    line++;
    aoc_vector_i32_push(&n, strtol(line, &line, 10));
  } while (*line != '\n');
  aoc_vector_numbers_push(numbers, n);
}

typedef struct {
  i32 first;
  i32 second;
} tuple;

static tuple extrapolated_value(const i32 *const numbers, const u32 length) {
  i32 n[length - 1];
  i32 notZero = 0;
  for (u32 i = 0; i < length - 1; ++i) {
    n[i] = numbers[i + 1] - numbers[i];
    notZero |= n[i];
  }
  if (!notZero)
    return (tuple){0};

  const tuple t = extrapolated_value(n, length - 1);
  return (tuple){t.first + n[length - 2], n[0] - t.second};
}

static tuple solve(const aoc_vector_numbers *const numbers) {
  tuple solutions = {0};
  const u32 baseLen = numbers->items[0].length;
  for (size_t i = 0; i < numbers->length; ++i) {
    const tuple t = extrapolated_value(numbers->items[i].items, baseLen);
    solutions.first += numbers->items[i].items[baseLen - 1] + t.first;
    solutions.second += (numbers->items[i].items[0] - t.second);
  }
  return solutions;
}

int main(void) {
  aoc_bump bump = {0};
  aoc_bump_init(&bump, 1 << 15);
  aoc_allocator allocator = aoc_bump_create_allocator(&bump);
  aoc_set_allocator(&allocator);

  aoc_vector_numbers numbers = {0};
  aoc_vector_numbers_create(&numbers, 0xff);

  aoc_file_read_lines1("day09/input.txt", (aoc_line_func)parse, &numbers);

  const tuple solutions = solve(&numbers);
  printf("%d\n%d\n", solutions.first, solutions.second);

  aoc_bump_destroy(&bump);
}
