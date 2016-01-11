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
			x ++;
		} else {
		}
}
recordi(x);
iif_assert((x>=0) && (x<=50));
return 0;
}



int main(int argc, char** argv)
 {
iifContext context("../test/conj.log", loopFunction, "loopFunction");
context.addLearner("linear").addLearner("conjunctive");
return context.learn("../test/conj.inv");
}
