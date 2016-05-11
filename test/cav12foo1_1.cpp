#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int x = _reserved_input_[0];
int y = _reserved_input_[1];

iif_assume(x == 0 && y == 0 );
while(rand() % 16)
{
recordi(x, y);
x++;y++;
}
recordi(x, y);
iif_assert(x == y);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/cav12foo1_1.var", loopFunction, "loopFunction", "../tmp/cav12foo1_1.ds");
context.addLearner("linear");
context.addLearner("conjunctive");
return context.learn("../tmp/cav12foo1_1.cnt", "../tmp/cav12foo1_1");
}
