#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int a[]) {
int x = a[0];
int y = a[1];

x=0; y=0;
iif_assume(y >= 0);
while(true)
{
recordi(x, y);
if (x <= 50) 
	y++;
	else
	y--;
	if (y < 0)
	break;
	x++;
}
recordi(x, y);
iif_assert(x==102);
return 0;
}


int main(int argc, char** argv)
 {
iifContext context("../tmp/V2.var", loopFunction, "loopFunction");
context.addLearner("linear");
return context.learn("../tmp/V2");
}
