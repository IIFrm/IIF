#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reverved_input_[]) {
int x = _reverved_input_[0];
int y = _reverved_input_[1];

iif_assume(x<=4*y && x >= 0);
while(x>0)
{
recordi(x, y);
x=x-4; y=y-1;
}
recordi(x, y);
iif_assert(y>=0);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/xy4.var", loopFunction, "loopFunction", "../tmp/xy4.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/xy4.cnt", "../tmp/xy4");
}
