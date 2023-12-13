#include <aoc/aoc.h>
#include <aoc/mem.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
  char state[32 * 5];
  u8 groups[6 * 5];
  u8 stateLength;
  u8 groupsLength;
} record;

#define AOC_T record
#include <aoc/vector.h>

static void parse(char *line, aoc_vector_record *const records) {
  char *current = line;
  record r = {0};
  while (*current != ' ')
    current++;
  r.stateLength = current - line;
  aoc_mem_copy(r.state, line, r.stateLength);

  while (*current != '\n') {
    current++;
    r.groups[r.groupsLength++] = strtoul(current, &current, 10);
  }
  aoc_vector_record_push(records, r);
}

typedef struct {
  u32 statePos, groupPos, matched;
} tuple;

static inline u32 tuple_hash(const tuple *const t) {
  u32 hash = 59238281;
  hash = (hash << 5) + t->statePos;
  hash = (hash << 5) + t->groupPos;
  hash = (hash << 5) + t->matched;
  return hash;
}

static inline bool tuple_equals(const tuple *const a, const tuple *const b) {
  return a->groupPos == b->groupPos && a->statePos == b->statePos &&
         a->matched == b->matched;
}

#define AOC_KEY_T tuple
#define AOC_VALUE_T u64
#define AOC_KEY_T_EMPTY ((tuple){UINT32_MAX, UINT32_MAX, UINT32_MAX})
#define AOC_KEY_T_HASH tuple_hash
#define AOC_KEY_T_EQUALS tuple_equals
#define AOC_BASE2_CAPACITY
#include <aoc/map.h>

static aoc_map_tuple_u64 cache = {0};

static u64 count(const record *const r, const u32 statePos, const u32 groupPos,
                 const u32 matched);

static inline u64 count_inner(const record *const r, const u32 statePos,
                              const u32 groupPos, const u32 matched) {
#define DOT()                                                                  \
  (matched == 0 ? count(r, statePos + 1, groupPos, 0)                          \
   : (groupPos < r->groupsLength && r->groups[groupPos] == matched)            \
       ? count(r, statePos + 1, groupPos + 1, 0)                               \
       : 0)

#define POUND() (count(r, statePos + 1, groupPos, matched + 1))
  if (statePos == r->stateLength)
    return (groupPos == r->groupsLength && matched == 0) ||
           (groupPos == r->groupsLength - 1u &&
            matched == r->groups[r->groupsLength - 1]);

  switch (r->state[statePos]) {
  case '#':
    return POUND();
  case '.':
    return DOT();
  default:
    return DOT() + POUND();
  }
}

static u64 count(const record *const r, const u32 statePos, const u32 groupPos,
                 const u32 matched) {
  u64 c = 0;
  const tuple t = {statePos, groupPos, matched};
  u32 hash = tuple_hash(&t);
  if (!aoc_map_tuple_u64_contains_pre_hashed(&cache, t, hash)) {
    c = count_inner(r, statePos, groupPos, matched);
    aoc_map_tuple_u64_insert_pre_hashed(&cache, t, c, hash);
  } else {
    aoc_map_tuple_u64_get_pre_hashed(&cache, t, &c, hash);
  }
  return c;
}

static u64 solve(const aoc_vector_record *const records) {
  u64 sum = 0;
  for (size_t i = 0; i < records->length; ++i) {
    aoc_map_tuple_u64_clear(&cache);
    sum += count(&records->items[i], 0, 0, 0);
  }
  return sum;
}

static void unfold(aoc_vector_record *const records) {
  for (size_t i = 0; i < records->length; ++i) {
    record *const r = &records->items[i];
    r->state[r->stateLength++] = '?';
    for (u8 j = 1; j < 5; ++j) {
      aoc_mem_copy(&r->state[j * r->stateLength], r->state, r->stateLength);
      aoc_mem_copy(&r->groups[j * r->groupsLength], r->groups, r->groupsLength);
    }
    r->stateLength = (r->stateLength * 5) - 1;
    r->groupsLength *= 5;
  }
}

int main(void) {
  aoc_vector_record records = {0};
  aoc_vector_record_create(&records, 1000);
  aoc_file_read_lines1("day12/input.txt", (aoc_line_func)parse, &records);

  aoc_map_tuple_u64_create(&cache, 1 << 15);

  u64 part1 = solve(&records);
  unfold(&records);
  u64 part2 = solve(&records);

  printf("%lu\n%lu\n", part1, part2);

  aoc_map_tuple_u64_destroy(&cache);
  aoc_vector_record_destroy(&records);
}
