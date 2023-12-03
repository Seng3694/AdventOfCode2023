#include <aoc/aoc.h>
#include <aoc/common.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aoc/mem.h>

typedef struct {
  u16 value;
  size_t index;
  u8 length;
} number;

#define AOC_T number
#include <aoc/vector.h>

#define AOC_T size_t
#define AOC_T_NAME index
#include <aoc/vector.h>

// assumes width and height to be the same
typedef struct {
  u8 size;
  aoc_vector_number numbers;
  char *data;
} map;

static void begin(char *line, map *const m) {
  // add extra layer to avoid bounds checking
  const size_t size = strlen(line) + 1;
  m->size = (u8)size;
  m->data = aoc_alloc(size * size);
  for (u8 x = 0; x < size; ++x) {
    m->data[x] = '.';
    m->data[size * (size - 1) + x] = '.';
  }
  for (u8 y = 1; y < size - 1; ++y) {
    m->data[size * y] = '.';
    m->data[size * y + (size - 1)] = '.';
  }
  aoc_vector_number_create(&m->numbers, 1300);
}

static void parse(char *line, map *const m, const size_t lineNumber) {
  const size_t mapOffset = (lineNumber + 1) * m->size;
  for (u8 x = 0; x < m->size - 2; ++x) {
    if (isdigit(line[x])) {
      number n = {0};
      n.index = mapOffset + (x + 1);
      char *end = NULL;
      n.value = strtoul(line + x, &end, 10);
      n.length = end - (line + x);
      aoc_vector_number_push(&m->numbers, n);
      aoc_mem_copy(&m->data[n.index], &line[x], n.length);
      x += n.length - 1;
    } else {
      m->data[mapOffset + (x + 1)] = line[x];
    }
  }
}

static void end(char *line, map *const m, const size_t lineNumber) {
  (void)line;
  (void)m;
  (void)lineNumber;
}

static inline bool is_special_character(const char c) {
  return c != '.';
}

static u32 solve_part1(const map *const m) {
  u32 sum = 0;

  for (size_t i = 0; i < m->numbers.length; ++i) {
    const number *const n = &m->numbers.items[i];

    // top and bottom row
    for (i8 x = -1; x < n->length + 1; ++x) {
      if (is_special_character(m->data[n->index - m->size + x]) ||
          is_special_character(m->data[n->index + m->size + x])) {
        sum += n->value;
        goto next;
      }
    }

    // left and right
    if (is_special_character(m->data[n->index - 1]) ||
        is_special_character(m->data[n->index + n->length])) {
      sum += n->value;
    }

  next:;
  }

  return sum;
}

int main(void) {
  map m = {0};
  aoc_file_read_lines3("day03/input.txt", (aoc_line_func)begin,
                       (aoc_line_num_func)parse, (aoc_line_num_func)end, &m);

  printf("%u\n", solve_part1(&m));

  aoc_vector_number_destroy(&m.numbers);
  aoc_free(m.data);
}
