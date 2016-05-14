#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int a = _reserved_input_[0];
int b = _reserved_input_[1];
int n = _reserved_input_[2];

int i;
 i = 0;
int cond;

iif_assume((a == 0) && (b == 0) && (n >= 0));
while(i < n)
{
recordi(a, b, n);
if (cond) {a = a + 1; b = b + 2;}
else {a = a + 2; b = b + 1;}
i = i + 1;
}
recordi(a, b, n);
iif_assert(a + b == 3 * n);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/bhmr2007_true-unreach-call.var", loopFunction, "loopFunction", "../tmp/bhmr2007_true-unreach-call.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/bhmr2007_true-unreach-call.cnt", "../tmp/bhmr2007_true-unreach-call");
}
