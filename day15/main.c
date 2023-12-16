#include <stdio.h>
#include <stdlib.h>
#include <aoc/aoc.h>
#include <aoc/mem.h>
#include <string.h>

typedef struct {
  const char *str;
  u8 length;
} slice;

typedef struct {
  slice label;
  u8 value;
} entry;

typedef struct {
  entry entries[10];
  u8 count;
} bucket;

typedef struct {
  bucket buckets[0x100];
} hashmap;

static inline u8 calculate_hash(const char *const str, const u8 length) {
  u8 hash = 0;
  for (u8 i = 0; i < length; ++i)
    hash = (u8)(((u16)hash + (u16)str[i]) * 17u);
  return hash;
}

static inline bool slice_equals(const slice *const a, const slice *const b) {
  return a->length == b->length && memcmp(a->str, b->str, a->length) == 0;
}

static entry *hashmap_find(hashmap *const hm, const slice *const label,
                           const u8 hash) {
  for (u8 i = 0; i < hm->buckets[hash].count; ++i) {
    entry *e2 = &hm->buckets[hash].entries[i];
    if (slice_equals(&e2->label, label))
      return e2;
  }
  return NULL;
}

static void hashmap_put(hashmap *const hm, const entry *const e) {
  const u8 hash = calculate_hash(e->label.str, e->label.length);
  bucket *const b = &hm->buckets[hash];
  entry *const existent = hashmap_find(hm, &e->label, hash);
  if (existent != NULL) {
    existent->value = e->value;
  } else {
    b->entries[b->count++] = *e;
  }
}

static void hashmap_remove(hashmap *const hm, const slice *const label) {
  const u8 hash = calculate_hash(label->str, label->length);
  bucket *const b = &hm->buckets[hash];
  entry *const existent = hashmap_find(hm, label, hash);
  if (existent == NULL)
    return;
  const u8 index = existent - b->entries;
  for (u8 i = index; i < b->count - 1; ++i)
    b->entries[i] = b->entries[i + 1];
  b->count--;
}

static u32 solve_part1(char *input, hashmap *const hm) {
  u32 sum = 0;
  while (*input != '\0') {
    const char *start = input;
    while (*input != '=' && *input != '-')
      input++;

    entry e = {.label = {.str = start, .length = input - start}};
    if (*input == '=') {
      e.value = strtoul(input + 1, &input, 10);
      hashmap_put(hm, &e);
    } else {
      hashmap_remove(hm, &e.label);
      input++;
    }

    sum += (u32)calculate_hash(start, input - start);
    input++;
  }
  return sum;
}

static u32 solve_part2(const hashmap *const hm) {
  u32 sum = 0;
  for (u32 i = 0; i <= 0xff; ++i) {
    const bucket *const b = &hm->buckets[i];
    for (u32 j = 0; j < b->count; ++j)
      sum += ((i + 1) * (j + 1) * (u32)b->entries[j].value);
  }
  return sum;
}

int main(void) {
  char *input = aoc_file_read_all2("day15/input.txt");
  hashmap hm = {0};
  const u32 part1 = solve_part1(input, &hm);
  const u32 part2 = solve_part2(&hm);
  printf("%u\n%u\n", part1, part2);
  aoc_free(input);
}
