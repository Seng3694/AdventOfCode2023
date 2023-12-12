#include <aoc/aoc.h>
#include <aoc/common.h>
#include <aoc/mem.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
  char data[32 * 5];
  u8 counts[6 * 5];
  u8 dataLength;
  u8 countsLength;
  u8 wildcardCount;
} record;

#define AOC_T record
#include <aoc/vector.h>

static void parse(char *line, aoc_vector_record *const records) {
  char *current = line;
  record r = {0};
  while (*current != ' ') {
    r.wildcardCount += (*current == '?');
    current++;
  }
  r.dataLength = current - line;
  aoc_mem_copy(r.data, line, r.dataLength);

  while (*current != '\n') {
    current++;
    r.counts[r.countsLength++] = strtoul(current, &current, 10);
  }
  aoc_vector_record_push(records, r);
}

static bool is_valid_arrangement(const record *const r) {
  u8 current = 0;
  for (u8 i = 0; i < r->dataLength; ++i)
    if (r->data[i] == '?')
      return false;
  u8 found = 0;
  for (u8 i = 0; i < r->countsLength && current < r->dataLength; ++i) {
    while (r->data[current] != '#') {
      current++;
      if (current >= r->dataLength)
        return false;
    }
    u8 count = 0;
    for (u8 j = 0; j < r->counts[i] && (current + j) < r->dataLength; ++j) {
      count++;
      if (r->data[current + j] != '#')
        return false;
    }
    current += r->counts[i];
    found += (count == r->counts[i]);
    if (r->data[current] == '#')
      return false;
  }
  if (found != r->countsLength)
    return false;

  for (u8 i = current; i < r->dataLength; ++i)
    if (r->data[i] == '#')
      return false;

  return true;
}

static u32 count_arrangements(record r, const u8 start,
                              const u8 wildcardsLeft) {
  u32 count = 0;
  if (wildcardsLeft == 0)
    count = is_valid_arrangement(&r);

  for (u8 i = start; i < r.dataLength; ++i) {
    if (r.data[i] == '?') {
      r.data[i] = '#';
      count += count_arrangements(r, i + 1, wildcardsLeft - 1);
      r.data[i] = '.';
      count += count_arrangements(r, i + 1, wildcardsLeft - 1);
      break;
    }
  }

  return count;
}

static u32 solve(aoc_vector_record *const records) {
  u32 sum = 0;
  for (size_t i = 0; i < records->length; ++i)
    sum += count_arrangements(records->items[i], 0,
                              records->items[i].wildcardCount);
  return sum;
}

static void unfold(aoc_vector_record *const dest,
                   const aoc_vector_record *const src) {
  for (size_t i = 0; i < src->length; ++i) {
    record r = {.dataLength = src->items[i].dataLength * 5 + 4,
                .countsLength = src->items[i].countsLength * 5};
    for (u8 j = 0; j < 5; ++j) {
      size_t destIndex = j * (src->items[i].dataLength + 1);
      aoc_mem_copy(&r.data[destIndex], src->items[i].data,
                   src->items[i].dataLength);
      r.data[destIndex + src->items[i].dataLength] = '?';

      aoc_mem_copy(&r.counts[j * (src->items[i].countsLength)],
                   src->items[i].counts, src->items[i].countsLength);
    }
    aoc_vector_record_push(dest, r);
  }
}

int main(void) {
  aoc_vector_record records = {0};
  aoc_vector_record_create(&records, 1000);
  aoc_file_read_lines1("day12/input.txt", (aoc_line_func)parse, &records);

  aoc_vector_record unfolded = {0};
  aoc_vector_record_create(&unfolded, 1000);
  unfold(&unfolded, &records);

  const u32 part1 = solve(&records);
  const u32 part2 = solve(&unfolded);
  printf("%u\n%u\n", part1, part2);

  aoc_vector_record_destroy(&unfolded);
  aoc_vector_record_destroy(&records);
}
