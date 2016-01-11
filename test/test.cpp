#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int a[]) {
int xa = a[0];
int ya = a[1];


iif_assume(xa + ya > 0);
while(xa > 0)
{
recordi(xa, ya);
xa--; 
ya++;
}
recordi(xa, ya);
iif_assert(ya > 0);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../test/test.log", loopFunction, "loopFunction");
context.addLearner("linear").addLearner("conjunctive");
return context.learn();
}
