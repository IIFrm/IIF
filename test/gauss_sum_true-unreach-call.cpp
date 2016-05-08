#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reverved_input_[]) {
int n = _reverved_input_[0];
int sum = _reverved_input_[1];

int i;
i = 1;
iif_assume(sum == 0 && 1 <= n && n <= 1000);
while(i <= n)
{
recordi(n, sum);
sum = sum + i;
}
recordi(n, sum);
iif_assert(2 * sum == n * (n + 1));
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/gauss_sum_true-unreach-call.var", loopFunction, "loopFunction", "../tmp/gauss_sum_true-unreach-call.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/gauss_sum_true-unreach-call.cnt", "../tmp/gauss_sum_true-unreach-call");
}
