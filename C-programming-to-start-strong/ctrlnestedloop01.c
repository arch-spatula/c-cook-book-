#include <stdio.h>
#include <stdlib.h>

int main(void) {
  int i = 2, j = 1;

  while (i <= 9) {
    j = 1;
    putchar('\n');
    while (j <= 9) {
      printf("%d * %d = %d\n", i, j, i * j);
      j += 1;
    }
    i += 1;
  }

  return EXIT_SUCCESS;
}

/*2 * 1 = 2*/
/*2 * 2 = 4*/
/*2 * 3 = 6*/
/*2 * 4 = 8*/
/*2 * 5 = 10*/
/*2 * 6 = 12*/
/*2 * 7 = 14*/
/*2 * 8 = 16*/
/*2 * 9 = 18*/

/*3 * 1 = 3*/
/*3 * 2 = 6*/
/*3 * 3 = 9*/
/*3 * 4 = 12*/
/*3 * 5 = 15*/
/*3 * 6 = 18*/
/*3 * 7 = 21*/
/*3 * 8 = 24*/
/*3 * 9 = 27*/

/*4 * 1 = 4*/
/*4 * 2 = 8*/
/*4 * 3 = 12*/
/*4 * 4 = 16*/
/*4 * 5 = 20*/
/*4 * 6 = 24*/
/*4 * 7 = 28*/
/*4 * 8 = 32*/
/*4 * 9 = 36*/

/*5 * 1 = 5*/
/*5 * 2 = 10*/
/*5 * 3 = 15*/
/*5 * 4 = 20*/
/*5 * 5 = 25*/
/*5 * 6 = 30*/
/*5 * 7 = 35*/
/*5 * 8 = 40*/
/*5 * 9 = 45*/

/*6 * 1 = 6*/
/*6 * 2 = 12*/
/*6 * 3 = 18*/
/*6 * 4 = 24*/
/*6 * 5 = 30*/
/*6 * 6 = 36*/
/*6 * 7 = 42*/
/*6 * 8 = 48*/
/*6 * 9 = 54*/

/*7 * 1 = 7*/
/*7 * 2 = 14*/
/*7 * 3 = 21*/
/*7 * 4 = 28*/
/*7 * 5 = 35*/
/*7 * 6 = 42*/
/*7 * 7 = 49*/
/*7 * 8 = 56*/
/*7 * 9 = 63*/

/*8 * 1 = 8*/
/*8 * 2 = 16*/
/*8 * 3 = 24*/
/*8 * 4 = 32*/
/*8 * 5 = 40*/
/*8 * 6 = 48*/
/*8 * 7 = 56*/
/*8 * 8 = 64*/
/*8 * 9 = 72*/

/*9 * 1 = 9*/
/*9 * 2 = 18*/
/*9 * 3 = 27*/
/*9 * 4 = 36*/
/*9 * 5 = 45*/
/*9 * 6 = 54*/
/*9 * 7 = 63*/
/*9 * 8 = 72*/
/*9 * 9 = 81*/
