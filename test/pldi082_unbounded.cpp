#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int x = _reserved_input_[0];
int y = _reserved_input_[1];
int N = _reserved_input_[2];

iif_assume(N>=0);
while(y >= 0)
{
recordi(x, y, N);
if (x <= N) y++;
if (x >= N + 1) y--;
x++;
}
recordi(x, y, N);
iif_assert((y!=-1 || x<2*N + 3));
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/pldi082_unbounded.var", loopFunction, "loopFunction", "../tmp/pldi082_unbounded.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/pldi082_unbounded.cnt", "../tmp/pldi082_unbounded");
}
