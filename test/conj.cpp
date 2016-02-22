#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int a[]) {
int x = a[0];

iif_assume((x>=0) && (x<=50));
int loop_times = 10;
while(loop_times-- > 0)
{
recordi(x);
 if (x == 50) {
			x --;
		} else if (x == 0) {
			x ++;
		} else {
			x += rand() % 3 - 1;
		}
}
recordi(x);
iif_assert((x>=0) && (x<=50));
return 0;
}


int main(int argc, char** argv)
 {
iifContext context("../tmp/conj.var", loopFunction, "loopFunction");
context.addLearner("kernel");
return context.learn("../tmp/conj.inv");
}
