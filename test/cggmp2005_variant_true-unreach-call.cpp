#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reverved_input_[]) {
int low = _reverved_input_[0];
int mid = _reverved_input_[1];
int high = _reverved_input_[2];

iif_assume(low == 0 && mid >= 1 && high == 2*mid);
while(mid > 0)
{
recordi(low, mid, high);
low = low + 1; high = high - 1; mid = mid - 1;
}
recordi(low, mid, high);
iif_assert(low == high);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/cggmp2005_variant_true-unreach-call.var", loopFunction, "loopFunction", "../tmp/cggmp2005_variant_true-unreach-call.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/cggmp2005_variant_true-unreach-call.cnt", "../tmp/cggmp2005_variant_true-unreach-call");
}
