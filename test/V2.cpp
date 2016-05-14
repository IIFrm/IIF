#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int x = _reserved_input_[0];
int y = _reserved_input_[1];

iif_assume(x==0 && y==0);
while(y>=0)
{
recordi(x, y);
if (x<=50) y++;
	else y--;
	x++;
}
recordi(x, y);
iif_assert(x==101);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/V2.var", loopFunction, "loopFunction", "../tmp/V2.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/V2.cnt", "../tmp/V2");
}
