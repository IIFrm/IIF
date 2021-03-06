#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int a[]) {
int x = a[0];
int y = a[1];

iif_assume((x == -50));
while(x<0)
{
recordi(x, y);x=x+y;
y++;
}
recordi(x, y);
iif_assert(y>0);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/PV1.var", loopFunction, "loopFunction", "../tmp/PV1.ds");
context.addLearner("linear", "../tmp/PV1.cnt");
context.addLearner("conjunctive", "../tmp/PV1.cnt");
return context.learn("../tmp/PV1");
}
