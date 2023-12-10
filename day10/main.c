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

static const u8 upscale_map[][9] = {
    [TILE_GROUND] = {0, 0, 0, 0, 0, 0, 0, 0, 0},
    [TILE_VERTICAL] = {0, 1, 0, 0, 1, 0, 0, 1, 0},
    [TILE_HORIZONTAL] = {0, 0, 0, 1, 1, 1, 0, 0, 0},
    [TILE_TL] = {0, 0, 0, 0, 1, 1, 0, 1, 0},
    [TILE_TR] = {0, 0, 0, 1, 1, 0, 0, 1, 0},
    [TILE_BR] = {0, 1, 0, 1, 1, 0, 0, 0, 0},
    [TILE_BL] = {0, 1, 0, 0, 1, 1, 0, 0, 0},
};

typedef struct {
  u32 x;
  u32 y;
} point;

#define AOC_T point
#include <aoc/vector.h>

static u32 solve_part2(const map *const m) {
  u32 solution = 0;
  const u32 size = m->realSize * 3;
  u8 *stretchedMap = aoc_calloc(1, size * size * sizeof(u8));

  // scale map up by 3 in each direction and interpolate pipe pieces
  for (u8 y = 0; y < m->realSize; ++y) {
    for (u8 x = 0; x < m->realSize; ++x) {
      const u32 from = y * m->realSize + x;
      const tile_type t = m->data[from].type;
      if (!m->data[from].partOfLoop)
        continue;
      for (u8 ys = 0; ys < 3; ++ys) {
        for (u8 xs = 0; xs < 3; ++xs) {
          const u32 to = (y * 3) * size + (x * 3) + ys * size + xs;
          stretchedMap[to] = upscale_map[t][ys * 3 + xs];
        }
      }
    }
  }

  // flood fill
  // fill outer border to avoid bounds checks
  for (u32 i = 0; i < size; ++i) {
    stretchedMap[i] = 1;
    stretchedMap[(size * (size - 1)) + i] = 1;
    stretchedMap[i * size] = 1;
    stretchedMap[i * size + size - 1] = 1;
  }
  aoc_vector_point points = {0};
  aoc_vector_point_create(&points, 512);

  aoc_vector_point_push(&points, (point){1, 1});

  while (points.length > 0) {
    const size_t len = points.length;
    for (size_t i = 0; i < len; ++i) {
      point p = points.items[i];
      const u32 index = p.y * size + p.x;
      if (stretchedMap[index - 1] == 0) {
        stretchedMap[index - 1] = 1;
        aoc_vector_point_push(&points, (point){p.x - 1, p.y});
      }
      if (stretchedMap[index + 1] == 0) {
        stretchedMap[index + 1] = 1;
        aoc_vector_point_push(&points, (point){p.x + 1, p.y});
      }
      if (stretchedMap[index - size] == 0) {
        stretchedMap[index - size] = 1;
        aoc_vector_point_push(&points, (point){p.x, p.y - 1});
      }
      if (stretchedMap[index + size] == 0) {
        stretchedMap[index + size] = 1;
        aoc_vector_point_push(&points, (point){p.x, p.y + 1});
      }
    }
    const size_t newLength = points.length - len;
    for (size_t k = 0; k < newLength; ++k)
      points.items[k] = points.items[len + k];
    points.length = newLength;
  }

  // go through all original indices and count the corresponding 9 "pixels" in
  // the scaled version. if the count is 0 add it to the solution
  for (u8 y = 0; y < m->realSize; ++y) {
    for (u8 x = 0; x < m->realSize; ++x) {
      u8 count = 0;
      for (u8 ys = 0; ys < 3; ++ys) {
        for (u8 xs = 0; xs < 3; ++xs) {
          const u32 index = (y * 3) * size + (x * 3) + ys * size + xs;
          count += stretchedMap[index];
        }
      }
      solution += count == 0;
    }
  }

  aoc_vector_point_destroy(&points);
  aoc_free(stretchedMap);
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
