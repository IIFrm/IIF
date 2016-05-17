#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int a = _reserved_input_[0];
int b = _reserved_input_[1];
int i = _reserved_input_[2];
int n = _reserved_input_[3];

int cond = rand()%2;
iif_assume((a == 0) && (b == 0) && (i==0) && (n >= 0));
while(i < n)
{
recordi(a, b, i, n);
if (cond) {a = a + 1; b = b + 2;}
else {a = a + 2; b = b + 1;}
i = i + 1;
cond = rand()%2;
}
recordi(a, b, i, n);
iif_assert(a + b == 3 * n);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/bhmr2007_true-unreach-call.var", loopFunction, "loopFunction", "../tmp/bhmr2007_true-unreach-call.ds");
context.addLearner("conjunctive");
return context.learn("../tmp/bhmr2007_true-unreach-call.cnt", "../tmp/bhmr2007_true-unreach-call");
}
