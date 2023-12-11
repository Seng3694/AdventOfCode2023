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

typedef struct {
  tile_type type : 7;
  bool partOfLoop : 1;
} tile;

typedef struct __attribute__((packed)) {
  u8 size;
  u8 realSize;
  u32 start;
  tile data[];
} map;

static void begin(char *line, map **mp) {
  // this length includes 2 extra for another layer around the map to avoid
  // bounds checking later
  const size_t length = strlen(line) + 1;
  map *m = aoc_calloc(1, sizeof(map) + (length * length * sizeof(tile)));
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
    case '|': m->data[index] = (tile){TILE_VERTICAL, false}; break;
    case '-': m->data[index] = (tile){TILE_HORIZONTAL, false}; break;
    case 'L': m->data[index] = (tile){TILE_BL, false}; break;
    case 'J': m->data[index] = (tile){TILE_BR, false}; break;
    case '7': m->data[index] = (tile){TILE_TR, false}; break;
    case 'F': m->data[index] = (tile){TILE_TL, false}; break;
    case '.': m->data[index] = (tile){TILE_GROUND, false}; break;
    case 'S': m->start = index; break;
    }
    // clang-format on
  }
}

typedef enum {
  DIR_L = 1 << 0,
  DIR_R = 1 << 1,
  DIR_T = 1 << 2,
  DIR_B = 1 << 3,
} direction;

static const tile_type tile_mapping[] = {
    [DIR_L | DIR_R] = TILE_HORIZONTAL, [DIR_L | DIR_T] = TILE_BR,
    [DIR_L | DIR_B] = TILE_TR,         [DIR_R | DIR_T] = TILE_BL,
    [DIR_R | DIR_B] = TILE_TL,         [DIR_T | DIR_B] = TILE_VERTICAL,
};

static void end(char *line, map **mp, const size_t lineNumber) {
  (void)line;
  (void)lineNumber;
  map *m = *mp;
  // find tile type of start
  const tile_type left = m->data[m->start - 1].type;
  const tile_type right = m->data[m->start + 1].type;
  const tile_type top = m->data[m->start - m->realSize].type;
  const tile_type bottom = m->data[m->start + m->realSize].type;
  const bool l = left == TILE_HORIZONTAL || left == TILE_TL || left == TILE_BL;
  const bool r =
      right == TILE_HORIZONTAL || right == TILE_TR || right == TILE_BR;
  const bool t = top == TILE_VERTICAL || top == TILE_TL || top == TILE_TR;
  const bool b =
      bottom == TILE_VERTICAL || bottom == TILE_BL || bottom == TILE_BR;
  m->data[m->start].type = tile_mapping[l | (r << 1) | (t << 2) | (b << 3)];
}

static u32 move(const u32 before, const u32 index, const map *const m) {
  switch (m->data[index].type) {
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
  default:
    return index;
  }
}

static u32 solve_part1(map *const m) {
  const u32 start = m->start;
  u32 current = start;
  u32 before = start;
  u32 length = 0;
  m->data[current].partOfLoop = true;

  do {
    const u32 b = current;
    current = move(before, current, m);
    m->data[current].partOfLoop = true;
    before = b;
    length++;
  } while (current != start);

  return (length >> 1) + (length & 1);
}

static u32 solve_part2(const map *const m) {
  u32 solution = 0;
  for (u8 y = 0; y < m->realSize; ++y) {
    u8 count = 0;
    for (u8 x = 0; x < m->realSize; ++x) {
      const tile t = m->data[y * m->realSize + x];
      if (t.partOfLoop) {
        count +=
            (t.type == TILE_VERTICAL || t.type == TILE_BL || t.type == TILE_BR);
      } else {
        solution += count & 1;
      }
    }
  }
  return solution;
}

int main(void) {
  map *m = {0};
  aoc_file_read_lines3("day10/input.txt", (aoc_line_func)begin,
                       (aoc_line_num_func)parse, (aoc_line_num_func)end, &m);

  const u32 part1 = solve_part1(m);
  const u32 part2 = solve_part2(m);

  printf("%u\n%u\n", part1, part2);

  aoc_free(m);
}
