#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int x = _reserved_input_[0];
int y = _reserved_input_[1];

iif_assume( x == 0 && y >= 0);
while( x < 99)
{
recordi(x, y);
 if (y%2 == 0) x+=2; else x++;
}
recordi(x, y);
iif_assert( x%2 == y%2);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/diamond_true-unreach-call1.var", loopFunction, "loopFunction", "../tmp/diamond_true-unreach-call1.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/diamond_true-unreach-call1.cnt", "../tmp/diamond_true-unreach-call1");
}
