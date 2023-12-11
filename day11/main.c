#include <aoc/aoc.h>
#include <aoc/filesystem.h>
#include <stdio.h>

#define AOC_T i32
#include <aoc/point.h>
typedef aoc_point2_i32 point;

#define AOC_T point
#include <aoc/vector.h>

typedef struct {
  aoc_vector_point *points;
  u32 y;
} parsing_context;

static void parse(char *line, parsing_context *const ctx) {
  bool hadAnyStars = false;
  for (u32 x = 0; *line != '\n'; ++x) {
    if (*line == '#') {
      aoc_vector_point_push(ctx->points, (point){x, ctx->y});
      hadAnyStars = true;
    }
    line++;
  }
  ctx->y += hadAnyStars ? 1 : 2;
}

static inline i32 compare_point(const point *const a, const point *const b) {
  const i32 diff = a->x - b->x;
  return diff == 0 ? a->y - b->y : diff;
}

static void expand_horizontally(aoc_vector_point *const points) {
  qsort(points->items, points->length, sizeof(point),
        (__compar_fn_t)compare_point);
  i32 dist = points->items[0].x - 0;
  if (dist > 1) {
    for (size_t i = 0; i < points->length; ++i) {
      points->items[i].x += (dist - 1);
    }
  }

  for (size_t i = 1; i < points->length; ++i) {
    dist = points->items[i].x - points->items[i - 1].x;
    if (dist > 1) {
      for (size_t j = i; j < points->length; ++j) {
        points->items[j].x += (dist - 1);
      }
    }
  }
}

static u32 solve_part1(const aoc_vector_point *const points) {
  u32 sum = 0;
  for (size_t i = 0; i < points->length - 1; ++i) {
    for (size_t j = i + 1; j < points->length; ++j) {
      sum += aoc_point2_i32_taxicab(&points->items[i], &points->items[j]);
    }
  }
  return sum;
}

int main(void) {
  aoc_vector_point points = {0};
  aoc_vector_point_create(&points, 128);
  parsing_context ctx = {.points = &points};
  aoc_file_read_lines1("day11/input.txt", (aoc_line_func)parse, &ctx);
  expand_horizontally(&points);

  const u32 part1 = solve_part1(&points);

  printf("%u\n", part1);

  aoc_vector_point_destroy(&points);
}
