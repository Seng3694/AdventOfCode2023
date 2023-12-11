#include <aoc/aoc.h>
#include <stdio.h>

#define AOC_T i64
#include <aoc/point.h>
typedef aoc_point2_i64 point;

#define AOC_T point
#include <aoc/vector.h>

static void parse(char *line, aoc_vector_point *const points,
                  const size_t lineNumber) {
  for (u32 x = 0; *line != '\n'; ++x, ++line) {
    if (*line == '#')
      aoc_vector_point_push(points, (point){x, lineNumber});
  }
}

static inline i32 sort_by_x(const point *const a, const point *const b) {
  return a->x - b->x;
}

static inline i32 sort_by_y(const point *const a, const point *const b) {
  return a->y - b->y;
}

static void expand_horizontally(aoc_vector_point *const points,
                                const u32 factor) {
  qsort(points->items, points->length, sizeof(point), (__compar_fn_t)sort_by_x);
  i64 dist = points->items[0].x - 0;
  // this handles the case for any empty rows between 0 and the first point
  if (dist > 1) {
    for (size_t i = 0; i < points->length; ++i) {
      points->items[i].x += ((factor - 1) * (dist - 1));
    }
  }
  for (size_t i = 1; i < points->length; ++i) {
    dist = points->items[i].x - points->items[i - 1].x;
    if (dist > 1) {
      for (size_t j = i; j < points->length; ++j) {
        points->items[j].x += ((factor - 1) * (dist - 1));
      }
    }
  }
}

static void expand_vertically(aoc_vector_point *const points,
                              const u32 factor) {
  qsort(points->items, points->length, sizeof(point), (__compar_fn_t)sort_by_y);
  i64 dist = points->items[0].y - 0;
  if (dist > 1) {
    for (size_t i = 0; i < points->length; ++i) {
      points->items[i].y += ((factor - 1) * (dist - 1));
    }
  }
  for (size_t i = 1; i < points->length; ++i) {
    dist = points->items[i].y - points->items[i - 1].y;
    if (dist > 1) {
      for (size_t j = i; j < points->length; ++j) {
        points->items[j].y += ((factor - 1) * (dist - 1));
      }
    }
  }
}

static u64 solve(const aoc_vector_point *const points) {
  u64 sum = 0;
  for (size_t i = 0; i < points->length - 1; ++i) {
    for (size_t j = i + 1; j < points->length; ++j) {
      sum += aoc_point2_i64_taxicab(&points->items[i], &points->items[j]);
    }
  }
  return sum;
}

int main(void) {
  aoc_vector_point points = {0};
  aoc_vector_point_create(&points, 512);
  aoc_file_read_lines2("day11/input.txt", (aoc_line_num_func)parse, &points);

  expand_vertically(&points, 2);
  expand_horizontally(&points, 2);
  const u64 part1 = solve(&points);
  expand_vertically(&points, 500000);
  expand_horizontally(&points, 500000);
  const u64 part2 = solve(&points);

  printf("%lu\n%lu\n", part1, part2);

  aoc_vector_point_destroy(&points);
}
