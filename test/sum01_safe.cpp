#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int i = _reserved_input_[0];
int n = _reserved_input_[1];
int sn = _reserved_input_[2];

int a = 2;
iif_assume(i==1 && sn==0);
while(i<=n)
{
recordi(i, n, sn);
sn = sn + a; i++; 
}
recordi(i, n, sn);
iif_assert(sn==n*a || sn==0);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/sum01_safe.var", loopFunction, "loopFunction", "../tmp/sum01_safe.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/sum01_safe.cnt", "../tmp/sum01_safe");
}
