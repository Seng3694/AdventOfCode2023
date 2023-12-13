#include <aoc/aoc.h>
#include <aoc/mem.h>
#include <stdio.h>

// max length found with:
// paste -sd '+' day13/input.txt | sed -r 's/\+\+/\n/g' | sed -r 's/\+//g' | awk
// '{print length}' | sort -nu | tail -n
typedef struct {
  u8 width;
  u8 height;
  char data[300]; // biggest has 289
} pattern;

#define AOC_T pattern
#include <aoc/vector.h>

static void parse(char *input, aoc_vector_pattern *const patterns) {
  input--;
  do {
    input++;
    pattern p = {0};
    u32 x = 0, y = 0;
    for (y = 0; *input != '\n' && *input != '\0'; ++y) {
      for (x = 0; *input != '\n'; ++x) {
        // p.width is 0 for the first row which is fine because the height
        // offset isn't needed in the first row
        p.data[y * p.width + x] = *input;
        input++;
      }
      p.width = x;
      input++;
    }
    p.height = y;
    aoc_vector_pattern_push(patterns, p);
  } while (*input != '\0');
}

static void print_pattern(const pattern *const p) {
  printf("w:%u h:%u\n", p->width, p->height);
  for (u32 y = 0; y < p->height; ++y) {
    for (u32 x = 0; x < p->width; ++x) {
      putchar(p->data[y * p->width + x]);
    }
    printf("\n");
  }
  printf("\n");
}

static bool try_find_mirror_horizontally(const pattern *const p,
                                         u32 *const row) {
  for (i32 y1 = 0, y2 = 1; y2 < p->height; ++y1, ++y2) {
    if (p->data[y1 * p->width] == p->data[y2 * p->width]) {
      bool match = true;
      for (i32 y11 = y1, y22 = y2; y11 >= 0 && y22 < p->height; --y11, ++y22) {
        for (i32 x = 0; x < p->width; ++x) {
          if (p->data[y11 * p->width + x] != p->data[y22 * p->width + x]) {
            match = false;
            goto next;
          }
        }
      }
      if (match) {
        *row = y2;
        return true;
      }
    next:;
    }
  }
  return false;
}

static bool try_find_mirror_vertically(const pattern *const p, u32 *const row) {
  for (i32 x1 = 0, x2 = 1; x2 < p->width; ++x1, ++x2) {
    if (p->data[x1] == p->data[x2]) {
      bool match = true;
      for (i32 x11 = x1, x22 = x2; x11 >= 0 && x22 < p->width; --x11, ++x22) {
        for (i32 y = 0; y < p->height; ++y) {
          if (p->data[y * p->width + x11] != p->data[y * p->width + x22]) {
            match = false;
            goto next;
          }
        }
      }
      if (match) {
        *row = x2;
        return true;
      }
    next:;
    }
  }
  return false;
}

static u32 solve_part1(const aoc_vector_pattern *const patterns) {
  u32 sum = 0;

  for (size_t i = 0; i < patterns->length; ++i) {
    u32 count = 0;
    const pattern *const p = &patterns->items[i];
    if (try_find_mirror_horizontally(p, &count)) {
      sum += (count * 100);
    } else if (try_find_mirror_vertically(p, &count)) {
      sum += count;
    }
  }

  return sum;
}

int main(void) {
  char *input = aoc_file_read_all2("day13/input.txt");
  aoc_vector_pattern patterns = {0};
  aoc_vector_pattern_create(&patterns, 100);
  parse(input, &patterns);
  aoc_free(input);

  const u32 part1 = solve_part1(&patterns);

  printf("%u\n", part1);

  aoc_vector_pattern_destroy(&patterns);
}
