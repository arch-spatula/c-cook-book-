#include <stdio.h>
#include <stdlib.h>

int main(void) {
  int input = 0, discount = 100;
  scanf("%d", &input);

  if (input <= 3 || input >= 65)
    discount = 0;
  else if (input <= 13)
    discount = 50;
  else if (input <= 19)
    discount = 75;
  else
    discount = 100;

  printf("최종요금: %d원\n", 1000 * discount / 100);
  return EXIT_SUCCESS;
}
