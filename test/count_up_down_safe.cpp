#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int x = _reserved_input_[0];
int y = _reserved_input_[1];
int n = _reserved_input_[2];

iif_assume(n>=0 && x==n && y==0);
while(x>0)
{
recordi(x, y, n);
x--;y++;
}
recordi(x, y, n);
iif_assert(y==n);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/count_up_down_safe.var", loopFunction, "loopFunction", "../tmp/count_up_down_safe.ds");
context.addLearner("conjunctive");
return context.learn("../tmp/count_up_down_safe.cnt", "../tmp/count_up_down_safe");
}
