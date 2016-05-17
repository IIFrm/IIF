#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int i = _reserved_input_[0];
int n = _reserved_input_[1];
int sn = _reserved_input_[2];

iif_assume(i==1 && sn==0);
while(i<=n)
{
recordi(i, n, sn);
sn = sn + i; i++; 
}
recordi(i, n, sn);
iif_assert(sn==n*(n+1)/2 || sn==0);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/sum02_safe.var", loopFunction, "loopFunction", "../tmp/sum02_safe.ds");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/sum02_safe.cnt", "../tmp/sum02_safe");
}
