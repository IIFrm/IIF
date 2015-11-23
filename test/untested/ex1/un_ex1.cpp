#include <assert.h>
#include <stdlib.h>
#include <time.h>

int main (int argc, char** argv) {
	int xa = atoi(argv[1]);
	int ya = atoi(argv[2]);

	int x;
	int y;

	srand(time(NULL));
	int loopnum1 = rand() % 20;
	assert (xa + 2 * ya >= 0);
	while (loopnum1--) {
	int loopnum2 = rand() % 10;
		x = xa + 2*ya;
		y = -2*xa + ya;

		x++;
		if (loopnum2--) y = y+x;
		else y = y-x;

		xa = x - 2*y;
		ya = 2*x + y;
		printf("%d %d\n", xa, ya);
	}


	assert (xa + 2*ya >= 0);
	return 0;
}

