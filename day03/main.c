#include <aoc/aoc.h>
#include <ctype.h>
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

typedef struct {
  u8 partCount;
  u32 ratio;
} gear;

#define HASH_INDEX(x) (((x) ^ 43947373) * 57428611)

#define AOC_KEY_T size_t
#define AOC_KEY_T_NAME index
#define AOC_VALUE_T gear
#define AOC_KEY_T_EMPTY ((size_t){0})
#define AOC_KEY_T_HASH(x) HASH_INDEX(*(x))
#define AOC_KEY_T_EQUALS(a, b) (*(a) == *(b))
#define AOC_BASE2_CAPACITY
#include <aoc/map.h>

// assumes width and height to be the same
typedef struct {
  u8 size;
  aoc_vector_number numbers;
  aoc_map_index_gear gears;
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
  aoc_map_index_gear_create(&m->gears, 512);
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

static void add_or_update_gear(map *const m, const size_t index,
                               const u16 number) {
  const u32 hash = HASH_INDEX(index);
  if (!aoc_map_index_gear_contains_pre_hashed(&m->gears, index, hash))
    aoc_map_index_gear_insert_pre_hashed(&m->gears, index, (gear){0, 1}, hash);
  gear g;
  aoc_map_index_gear_get_pre_hashed(&m->gears, index, &g, hash);
  ++g.partCount;
  g.ratio *= number;
  aoc_map_index_gear_put_pre_hashed(&m->gears, index, g, hash);
}

static u32 solve_part1(map *const m) {
  u32 sum = 0;

  for (size_t i = 0; i < m->numbers.length; ++i) {
    const number *const n = &m->numbers.items[i];
    bool hasSpecialChar = false;
    // top and bottom row
    for (i8 x = -1; x < n->length + 1; ++x) {
      const size_t topIndex = n->index - m->size + x;
      const size_t bottomIndex = n->index + m->size + x;
      if (is_special_character(m->data[topIndex]) ||
          is_special_character(m->data[bottomIndex])) {
        hasSpecialChar = true;
        if (m->data[topIndex] == '*')
          add_or_update_gear(m, topIndex, n->value);
        if (m->data[bottomIndex] == '*')
          add_or_update_gear(m, bottomIndex, n->value);
      }
    }

    // left and right
    if (is_special_character(m->data[n->index - 1]) ||
        is_special_character(m->data[n->index + n->length])) {
      hasSpecialChar = true;
      if (m->data[n->index - 1] == '*')
        add_or_update_gear(m, n->index - 1, n->value);
      if (m->data[n->index + n->length] == '*')
        add_or_update_gear(m, n->index + n->length, n->value);
    }

    if (hasSpecialChar)
      sum += n->value;
  }

  return sum;
}

static u32 solve_part2(map *const m) {
  u32 solution = 0;
  aoc_map_iter_index_gear iter = {0};
  aoc_map_iter_index_gear_init(&iter, &m->gears);
  size_t i;
  gear g;
  while (aoc_map_index_gear_iterate(&iter, &i, &g)) {
    if (g.partCount == 2)
      solution += g.ratio;
  }
  return solution;
}

int main(void) {
  map m = {0};
  aoc_file_read_lines3("day03/input.txt", (aoc_line_func)begin,
                       (aoc_line_num_func)parse, (aoc_line_num_func)end, &m);

  const u32 part1 = solve_part1(&m);
  const u32 part2 = solve_part2(&m);

  printf("%u\n%u\n", part1, part2);

  aoc_vector_number_destroy(&m.numbers);
  aoc_map_index_gear_destroy(&m.gears);
  aoc_free(m.data);
}
