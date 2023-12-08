#include <stdio.h>
#include <aoc/aoc.h>
#include <aoc/mem.h>
#include <string.h>
#include <aoc/bump.h>

#define AOC_T u8
#include <aoc/vector.h>

typedef enum {
  NODE_TYPE_NORMAL,
  NODE_TYPE_START,
  NODE_TYPE_DESTINATION,
} node_type;

typedef struct {
  u32 id;
  node_type type;
  union {
    struct {
      u32 left;
      u32 right;
    };
    u32 data[2];
  };
} node;

#define AOC_T node
#include <aoc/vector.h>

typedef char *string;

static inline bool string_equals(const string *const a, const string *const b) {
  return memcmp(*a, *b, 3) == 0;
}

#define AOC_KEY_T string
#define AOC_VALUE_T u32
#define AOC_KEY_T_EMPTY ((string){"000"})
#define AOC_KEY_T_HASH(x) aoc_string_hash1(*x, 3)
#define AOC_KEY_T_EQUALS string_equals
#define AOC_BASE2_CAPACITY
#include <aoc/map.h>

typedef struct {
  u32 current;
  aoc_map_string_u32 ids;
  aoc_vector_u8 *directions;
  aoc_vector_node *nodes;
} parsing_context;

static u32 resolve_id(char *input, parsing_context *const ctx) {
  const u32 hash = aoc_string_hash1(input, 3);
  u32 id = 0;
  if (!aoc_map_string_u32_contains_pre_hashed(&ctx->ids, input, hash)) {
    id = ctx->current;
    aoc_map_string_u32_insert_pre_hashed(&ctx->ids, input, ctx->current++,
                                         hash);
  } else {
    aoc_map_string_u32_get_pre_hashed(&ctx->ids, input, &id, hash);
  }
  return id;
}

static void parse(char *input, parsing_context *const ctx) {
  while (*input != '\n') {
    aoc_vector_u8_push(ctx->directions, *input == 'L' ? 0 : 1);
    input++;
  }
  input += 2;

  while (*input != '\0') {
    const u32 from = resolve_id(input, ctx);
    const node_type type = input[2] == 'A'   ? NODE_TYPE_START
                           : input[2] == 'Z' ? NODE_TYPE_DESTINATION
                                             : NODE_TYPE_NORMAL;
    const u32 left = resolve_id(input + 7, ctx);
    const u32 right = resolve_id(input + 12, ctx);
    aoc_vector_node_ensure_capacity(ctx->nodes, from + 1);
    node n = {.id = from, .left = left, .right = right, .type = type};
    ctx->nodes->items[from] = n;
    ctx->nodes->length = AOC_MAX(ctx->nodes->length, from + 1);
    input += 17;
  }
}

static u32 solve_part1(const u32 start, const u32 dest,
                       const aoc_vector_u8 *const directions,
                       const aoc_vector_node *const nodes) {
  u32 solution = 0;
  node current = nodes->items[start];
  while (current.id != dest) {
    const u8 dir = directions->items[solution % directions->length];
    current = nodes->items[current.data[dir]];
    solution++;
  }

  return solution;
}

static u64 greatest_common_divisor(u64 a, u64 b) {
  while (b != 0) {
    const u64 t = a % b;
    a = b;
    b = t;
  }
  return a;
}

static u64 least_common_multiple(const u64 n, const u64 arr[const n]) {
  u64 lcm = arr[0];
  for (u64 i = 1; i < n; ++i)
    lcm = (arr[i] * lcm) / greatest_common_divisor(arr[i], lcm);
  return lcm;
}

static u64 solve_part2(const aoc_vector_u8 *const directions,
                       const aoc_vector_node *const nodes,
                       aoc_vector_node *const startingPoints) {
  u64 results[startingPoints->length];

  for (u32 i = 0; i < startingPoints->length; ++i) {
    node current = startingPoints->items[i];
    u32 steps = 0;
    do {
      const u8 dir = directions->items[steps % directions->length];
      current = nodes->items[current.data[dir]];
      steps++;
    } while (current.type != NODE_TYPE_DESTINATION);
    results[i] = steps;
  }

  return least_common_multiple(startingPoints->length, results);
}

int main(void) {
  aoc_bump bump = {0};
  aoc_bump_init(&bump, 38000);
  aoc_allocator allocator = aoc_bump_create_allocator(&bump);
  aoc_set_allocator(&allocator);

  char *input = aoc_file_read_all2("day08/input.txt");
  aoc_vector_u8 directions = {0};
  aoc_vector_node nodes = {0};
  aoc_vector_u8_create(&directions, 300);
  aoc_vector_node_create(&nodes, 700);
  parsing_context ctx = {.directions = &directions, .nodes = &nodes};
  aoc_map_string_u32_create(&ctx.ids, 1 << 10);

  parse(input, &ctx);
  u32 start, dest;
  aoc_map_string_u32_get(&ctx.ids, "AAA", &start);
  aoc_map_string_u32_get(&ctx.ids, "ZZZ", &dest);

  const u32 part1 = solve_part1(start, dest, &directions, &nodes);

  aoc_vector_node startingPoints = {0};
  aoc_vector_node_create(&startingPoints, 10);

  aoc_map_iter_string_u32 iter = {0};
  aoc_map_iter_string_u32_init(&iter, &ctx.ids);
  string s;
  u32 id;
  while (aoc_map_string_u32_iterate(&iter, &s, &id)) {
    if (nodes.items[id].type == NODE_TYPE_START)
      aoc_vector_node_push(&startingPoints, nodes.items[id]);
  }

  const u64 part2 = solve_part2(&directions, &nodes, &startingPoints);

  printf("%u\n%lu\n", part1, part2);

  aoc_bump_destroy(&bump);
}
