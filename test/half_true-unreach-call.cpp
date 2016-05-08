#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reverved_input_[]) {
int n = _reverved_input_[0];
int k = _reverved_input_[1];

 int i;
 i = 0;
iif_assume(k <= 1024 && k >= -1024);
while(i < 2 * k)
{
recordi(n, k);
if (i % 2 == 0) n++;i++;
}
recordi(n, k);
iif_assert(k < 0 || n == k);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/half_true-unreach-call.var", loopFunction, "loopFunction", "../tmp/half_true-unreach-call.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/half_true-unreach-call.cnt", "../tmp/half_true-unreach-call");
}
