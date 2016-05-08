#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reverved_input_[]) {
int i = _reverved_input_[0];
int k = _reverved_input_[1];

int j;
j = 1;
iif_assume(0 <= k && k <= 1 && i == 1);
while(i < 1024)
{
recordi(i, k);
i = i + 1; j = j + k; k = k - 1;
}
recordi(i, k);
iif_assert(1 <= i + k && i + k <= 2 && i >= 1);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/css2003_true-unreach-call.var", loopFunction, "loopFunction", "../tmp/css2003_true-unreach-call.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/css2003_true-unreach-call.cnt", "../tmp/css2003_true-unreach-call");
}
