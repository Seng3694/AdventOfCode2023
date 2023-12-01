#include <ctype.h>
#include <stdio.h>
#include <aoc/aoc.h>
#include <string.h>

static void parse(char *s, void *ud) {
  const int len = (int)strlen(s) - 1;
  uint8_t firstDigit = 0;

  for (int i = 0; i < len; ++i) {
    if (isdigit(s[i])) {
      firstDigit = s[i] - '0';
      break;
    }
  }

  uint8_t secondDigit = 0;
  for (int i = len - 1; i >= 0; --i) {
    if (isdigit(s[i])) {
      secondDigit = s[i] - '0';
      break;
    }
  }

  int *v = ud;
  *v += firstDigit * 10 + secondDigit;
}

int main(void) {
  int part1 = 0;
  aoc_file_read_lines1("day01/input.txt", parse, &part1);
  printf("%d\n", part1);
}
