#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int x = _reserved_input_[0];

int loop_times;
loop_times = 10;
iif_assume((x>=0) && (x<=50));
while(loop_times-->0)
{
recordi(x);
 if (x>50) x++;
		if (x == 0) {
			x ++;
		} else x--;
}
recordi(x);
iif_assert((x>=0) && (x<=50));
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/conj.var", loopFunction, "loopFunction", "../tmp/conj.ds");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/conj.cnt", "../tmp/conj");
}
