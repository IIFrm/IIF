#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reverved_input_[]) {
int x = _reverved_input_[0];
int y = _reverved_input_[1];

iif_assume( y < 1024 && y > 0);
while(x < 100)
{
recordi(x, y);
x = x + y;
}
recordi(x, y);
iif_assert((y <= 0 || (y > 0 && x >= 100)));
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/terminator_03_true-unreach-call_true-termination.var", loopFunction, "loopFunction", "../tmp/terminator_03_true-unreach-call_true-termination.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/terminator_03_true-unreach-call_true-termination.cnt", "../tmp/terminator_03_true-unreach-call_true-termination");
}
