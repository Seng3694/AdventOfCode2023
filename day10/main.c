#include <aoc/aoc.h>
#include <aoc/mem.h>
#include <stdio.h>
#include <string.h>

typedef enum {
  TILE_GROUND,
  TILE_VERTICAL,
  TILE_HORIZONTAL,
  TILE_TL,
  TILE_TR,
  TILE_BR,
  TILE_BL,
} tile_type;

typedef struct __attribute__((packed)) {
  u8 size;
  u8 realSize;
  u32 start;
  u8 data[];
} map;

static void begin(char *line, map **mp) {
  // this length includes 2 extra for another layer around the map to avoid
  // bounds checking later
  const size_t length = strlen(line) + 1;
  map *m = aoc_calloc(1, sizeof(map) + (length * length * sizeof(u8)));
  m->realSize = (u8)length;
  m->size = m->realSize - 2;
  *mp = m;
}

static void parse(char *line, map **mp, const size_t lineNumber) {
  map *m = *mp;
  u32 index = m->realSize + lineNumber * m->realSize + 1;
  for (u8 i = 0; i < m->size; ++i, ++index) {
    // clang-format off
    switch (line[i]) {
    case '|': m->data[index] = TILE_VERTICAL; break;
    case '-': m->data[index] = TILE_HORIZONTAL; break;
    case 'L': m->data[index] = TILE_BL; break;
    case 'J': m->data[index] = TILE_BR; break;
    case '7': m->data[index] = TILE_TR; break;
    case 'F': m->data[index] = TILE_TL; break;
    case '.': m->data[index] = TILE_GROUND; break;
    case 'S': m->start = index; break;
    }
    // clang-format on
  }
}

static const tile_type tile_mapping[] = {
    // bits correspond to left right top bottom
    [0b0011] = TILE_HORIZONTAL, // left right
    [0b0101] = TILE_BR,         // left top
    [0b1001] = TILE_TR,         // left bottom
    [0b0110] = TILE_BL,         // right top
    [0b1010] = TILE_TL,         // right bottom
    [0b1100] = TILE_VERTICAL,   // top bottom
};

static void end(char *line, map **mp, const size_t lineNumber) {
  (void)line;
  (void)lineNumber;
  map *m = *mp;
  // find tile type of start
  const tile_type left = m->data[m->start - 1];
  const tile_type right = m->data[m->start + 1];
  const tile_type top = m->data[m->start - m->realSize];
  const tile_type bottom = m->data[m->start + m->realSize];
  const bool l = left == TILE_HORIZONTAL || left == TILE_TL || left == TILE_BL;
  const bool r =
      right == TILE_HORIZONTAL || right == TILE_TR || right == TILE_BR;
  const bool t = top == TILE_VERTICAL || top == TILE_TL || top == TILE_TR;
  const bool b =
      bottom == TILE_VERTICAL || bottom == TILE_BL || bottom == TILE_BR;
  m->data[m->start] = tile_mapping[l | (r << 1) | (t << 2) | (b << 3)];
}

static const char *tile_char_map[] = {
    [TILE_GROUND] = " ", [TILE_VERTICAL] = "║", [TILE_HORIZONTAL] = "═",
    [TILE_TL] = "╔",     [TILE_TR] = "╗",       [TILE_BR] = "╝",
    [TILE_BL] = "╚",
};

static void print_map(const map *const m, const u32 self) {
  for (u8 y = 0; y < m->size; ++y) {
    const u32 offset = m->realSize + m->realSize * y + 1;
    for (u8 x = 0; x < m->size; ++x) {
      const u32 index = offset + x;
      const char *c = tile_char_map[m->data[index]];
      if (index == m->start) {
        printf("\e[0;35m%s\e[0m", c);
      } else if (index == self) {
        printf("\e[0;33m%s\e[0m", c);
      } else {
        printf("\e[0;36m%s\e[0m", c);
      }
    }
    printf("\n");
  }
}

static u32 move(const u32 before, const u32 index, const map *const m) {
  switch (m->data[index]) {
  case TILE_VERTICAL:
    return index > before ? index + m->realSize : index - m->realSize;
  case TILE_HORIZONTAL:
    return index > before ? index + 1 : index - 1;
  case TILE_TL:
    return before - index == 1 ? index + m->realSize : index + 1;
  case TILE_TR:
    return index - before == 1 ? index + m->realSize : index - 1;
  case TILE_BR:
    return index - before == 1 ? index - m->realSize : index - 1;
  case TILE_BL:
    return before - index == 1 ? index - m->realSize : index + 1;
  }
  return index;
}

static u32 solve_part1(const map *const m) {
  const u32 start = m->start;
  u32 current = start;
  u32 before = start;
  u32 length = 0;
  print_map(m, current);

  do {
    const u32 b = current;
    current = move(before, current, m);
    before = b;
    length++;
  } while (current != start);

  return (length >> 1) + (length & 1);
}

int main(void) {
  map *m = {0};
  aoc_file_read_lines3("day10/input.txt", (aoc_line_func)begin,
                       (aoc_line_num_func)parse, (aoc_line_num_func)end, &m);

  const u32 part1 = solve_part1(m);

  printf("%u\n", part1);

  aoc_free(m);
}
