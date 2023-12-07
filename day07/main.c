#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <aoc/aoc.h>

typedef enum {
  CARD_TYPE_2 = 2,
  CARD_TYPE_3,
  CARD_TYPE_4,
  CARD_TYPE_5,
  CARD_TYPE_6,
  CARD_TYPE_7,
  CARD_TYPE_8,
  CARD_TYPE_9,
  CARD_TYPE_T,
  CARD_TYPE_J,
  CARD_TYPE_Q,
  CARD_TYPE_K,
  CARD_TYPE_A,
  CARD_TYPE_COUNT,
} card_type;

typedef enum {
  HAND_KIND_HIGH_CARD,
  HAND_KIND_ONE_PAIR,
  HAND_KIND_TWO_PAIR,
  HAND_KIND_THREE_OF_A_KIND,
  HAND_KIND_FULL_HOUSE,
  HAND_KIND_FOUR_OF_A_KIND,
  HAND_KIND_FIVE_OF_A_KIND,
} hand_kind;

typedef struct __attribute__((packed)) {
  u8 cards[5];
  u16 bid;
  hand_kind kind : 8;
} card_hand;

#define AOC_T card_hand
#include <aoc/vector.h>

static hand_kind get_hand_kind(const u8 cards[const 5]) {
  u8 counts[CARD_TYPE_COUNT] = {0};
  for (u8 i = 0; i < 5; ++i)
    counts[cards[i]]++;

  bool has5 = false, has4 = false, has3 = false;
  u8 pairCount = 0;
  for (u8 i = 0; i < CARD_TYPE_COUNT; ++i) {
    if (counts[i] == 5) {
      has5 = true;
      break;
    } else if (counts[i] == 4) {
      has4 = true;
      break;
    }
    if (counts[i] == 3)
      has3 = true;
    if (counts[i] == 2)
      pairCount++;
  }

  if (has5) {
    return HAND_KIND_FIVE_OF_A_KIND;
  } else if (has4) {
    return HAND_KIND_FOUR_OF_A_KIND;
  } else if (has3 && pairCount == 1) {
    return HAND_KIND_FULL_HOUSE;
  } else if (has3) {
    return HAND_KIND_THREE_OF_A_KIND;
  } else if (pairCount == 2) {
    return HAND_KIND_TWO_PAIR;
  } else if (pairCount == 1) {
    return HAND_KIND_ONE_PAIR;
  } else {
    return HAND_KIND_HIGH_CARD;
  }
}

static void parse(char *line, aoc_vector_card_hand *const hands) {
  card_hand hand = {0};
  for (u8 i = 0; i < 5; ++i) {
    // clang-format off
    switch (line[i]) {
    case 'T': hand.cards[i] = CARD_TYPE_T;   break;
    case 'J': hand.cards[i] = CARD_TYPE_J;   break;
    case 'Q': hand.cards[i] = CARD_TYPE_Q;   break;
    case 'K': hand.cards[i] = CARD_TYPE_K;   break;
    case 'A': hand.cards[i] = CARD_TYPE_A;   break;
    default:  hand.cards[i] = line[i] - '0'; break;
    }
    // clang-format on
  }
  hand.bid = strtoul(line + 6, NULL, 10);
  hand.kind = get_hand_kind(hand.cards);
  aoc_vector_card_hand_push(hands, hand);
}

static i32 compare_hand(const card_hand *const a, const card_hand *const b) {
  i32 diff = (i32)a->kind - (i32)b->kind;
  for (u8 i = 0; i < 5 && diff == 0; ++i) {
    diff = (i32)a->cards[i] - (i32)b->cards[i];
  }
  return diff;
}

static u32 solve_part1(const aoc_vector_card_hand *const hands) {
  u32 solution = 0;
  qsort(hands->items, hands->length, sizeof(card_hand),
        (int (*)(const void *, const void *))compare_hand);
  for (size_t i = 0; i < hands->length; ++i)
    solution += ((i + 1) * hands->items[i].bid);
  return solution;
}

int main(void) {
  aoc_vector_card_hand hands = {0};
  aoc_vector_card_hand_create(&hands, 1000);

  aoc_file_read_lines1("day07/input.txt", (aoc_line_func)parse, &hands);

  const u32 part1 = solve_part1(&hands);

  printf("%u\n", part1);

  aoc_vector_card_hand_destroy(&hands);
}
