#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <aoc/aoc.h>

typedef enum {
  CARD_TYPE_2,
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

static const u8 card_values_with_jack[] = {
    [CARD_TYPE_2] = 2,  [CARD_TYPE_3] = 3,  [CARD_TYPE_4] = 4,
    [CARD_TYPE_5] = 5,  [CARD_TYPE_6] = 6,  [CARD_TYPE_7] = 7,
    [CARD_TYPE_8] = 8,  [CARD_TYPE_9] = 9,  [CARD_TYPE_T] = 10,
    [CARD_TYPE_J] = 11, [CARD_TYPE_Q] = 12, [CARD_TYPE_K] = 13,
    [CARD_TYPE_A] = 14,
};

static const u8 card_values_with_joker[] = {
    [CARD_TYPE_2] = 2,  [CARD_TYPE_3] = 3,  [CARD_TYPE_4] = 4,
    [CARD_TYPE_5] = 5,  [CARD_TYPE_6] = 6,  [CARD_TYPE_7] = 7,
    [CARD_TYPE_8] = 8,  [CARD_TYPE_9] = 9,  [CARD_TYPE_T] = 10,
    [CARD_TYPE_J] = 1,  [CARD_TYPE_Q] = 11, [CARD_TYPE_K] = 12,
    [CARD_TYPE_A] = 13,
};

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

static hand_kind get_hand_kind_with_jacks(const u8 cards[const 5]) {
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

static int compare_counts(const void *a, const void *b) {
  return ((i8) * (u8 *)b) - ((i8) * (u8 *)a);
}

static hand_kind get_hand_kind_with_jokers(const u8 cards[const 5]) {
  u8 counts[CARD_TYPE_COUNT] = {0};
  for (u8 i = 0; i < 5; ++i)
    counts[cards[i]]++;

  u8 jokers = counts[CARD_TYPE_J];
  if (jokers == 5)
    return HAND_KIND_FIVE_OF_A_KIND;
  counts[CARD_TYPE_J] = 0;

  qsort(counts, CARD_TYPE_COUNT, sizeof(u8), compare_counts);

  bool has5 = false, has4 = false, has3 = false;
  u8 pairCount = 0;
  for (u8 i = 0; i < CARD_TYPE_COUNT; ++i) {
    if (counts[i] == 0)
      continue;
    if (counts[i] + jokers == 5) {
      counts[i] += jokers;
      has5 = true;
      jokers = 0;
      break;
    } else if (counts[i] + jokers == 4) {
      counts[i] += jokers;
      has4 = true;
      jokers = 0;
      break;
    } else if (counts[i] + jokers == 3) {
      counts[i] += jokers;
      has3 = true;
      jokers = 0;
      break;
    }
  }

  for (u8 i = 0; i < CARD_TYPE_COUNT; ++i) {
    if (counts[i] + jokers == 2) {
      counts[i] += jokers;
      pairCount++;
      jokers = 0;
    }
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
    default:  hand.cards[i] = line[i] - '0' - 2; break;
    }
    // clang-format on
  }
  hand.bid = strtoul(line + 6, NULL, 10);
  hand.kind = get_hand_kind_with_jacks(hand.cards);
  aoc_vector_card_hand_push(hands, hand);
}

static i32 compare_hand_with_jokers(const card_hand *const a,
                                    const card_hand *const b) {
  i32 diff = (i32)a->kind - (i32)b->kind;
  for (u8 i = 0; i < 5 && diff == 0; ++i) {
    diff = (i32)card_values_with_joker[a->cards[i]] -
           (i32)card_values_with_joker[b->cards[i]];
  }
  return diff;
}

static i32 compare_hand_with_jacks(const card_hand *const a,
                                   const card_hand *const b) {
  i32 diff = (i32)a->kind - (i32)b->kind;
  for (u8 i = 0; i < 5 && diff == 0; ++i) {
    diff = (i32)card_values_with_jack[a->cards[i]] -
           (i32)card_values_with_jack[b->cards[i]];
  }
  return diff;
}

static u32 solve(const aoc_vector_card_hand *const hands) {
  u32 solution = 0;
  for (size_t i = 0; i < hands->length; ++i)
    solution += ((i + 1) * hands->items[i].bid);
  return solution;
}

int main(void) {
  aoc_vector_card_hand hands = {0};
  aoc_vector_card_hand_create(&hands, 1000);

  aoc_file_read_lines1("day07/input.txt", (aoc_line_func)parse, &hands);

  qsort(hands.items, hands.length, sizeof(card_hand),
        (int (*)(const void *, const void *))compare_hand_with_jacks);
  const u32 part1 = solve(&hands);

  for (size_t i = 0; i < hands.length; ++i)
    hands.items[i].kind = get_hand_kind_with_jokers(hands.items[i].cards);
  qsort(hands.items, hands.length, sizeof(card_hand),
        (int (*)(const void *, const void *))compare_hand_with_jokers);
  const u32 part2 = solve(&hands);

  printf("%u\n%u\n", part1, part2);

  aoc_vector_card_hand_destroy(&hands);
}
