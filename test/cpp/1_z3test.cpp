#include "iif.h"


int main(int argc, char** argv)
{
	//char RET_TRANS[20][5] = {"False", "True", "Could Not Answer", "Timeout", "Unhandled Error"};
	Equation e1(-3.4, 1.44524345);
	Equation e2(-2.0, 1.5626422);
	std::cout << "e1: " << e1 << std::endl;
	std::cout << "e2: " << e2 << std::endl;
	std::cout << "---------------test1-----------------\n";
	std::cout << "[" << e1 << "]\t==>\t[" << e2 << "]? ";
	int ret = e1.imply(e2);
	switch(ret) {
		case 0: std::cout << "False\n"; break;
		case 1: std::cout << "True\n"; break;
		case 2: std::cout << "Could not answer query\n"; break;
		case 3: std::cout << "Timeout.\n"; break;
		default: std::cout << "Unhandled error\n";
	}
	
	std::cout << "---------------test2-----------------\n";
	std::cout << "[" << e2 << "]\t==>\t[" << e1 << "]? ";
	ret = e2.imply(e1);
	switch(ret) {
		case 0: std::cout << "False\n"; break;
		case 1: std::cout << "True\n"; break;
		case 2: std::cout << "Could not answer query\n"; break;
		case 3: std::cout << "Timeout.\n"; break;
		default: std::cout << "Unhandled error\n";
	}
	return 0;
}
