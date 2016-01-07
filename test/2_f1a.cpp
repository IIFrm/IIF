#include "iif.h"

int f1a(int* a) {
	int x = a[0];
	int y = a[1];

	iif_assume(x + y >= 0);
	while (x >= 0) {
		recordi(x, y);
		x--;
		y++;
	}
	recordi(x, y);
	iif_assert(y > 0);
	return 0;
}

int main(int argc, char** argv)
{
	States* gsets = initSystem(f1a, "f1a");

	std::cout << "TRY SVM method ...\n";
	LinearLearn ll(gsets);
	if (ll.learn() == 0) 
		return shutdownSystem(gsets); 

	std::cout << "TRY SVM-I method ...\n";
	ConjunctiveLearn cl(gsets);
	if (cl.learn() == 0)
		return shutdownSystem(gsets); 

	//std::cout << "TRY other method again...\n";
	shutdownSystem(gsets);
	return -1;
}
