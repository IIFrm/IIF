#include "iif.h"


int main(int argc, char** argv)
{
	Equation e1[2] = {Equation(-2.0, 1.0, -1.0), Equation(0.0, 0.0, 1.0)};
	Equation e2(0.0, 1.0, 0.0);
	std::cout << "---------------test1-----------------\n";
	std::cout << "[" << e1[0] << " /\\ "<< e1[1] << "]\t==>\t[" << e2 << "]? ";
	int ret = Equation::multi_imply(e1, 2, e2);
	switch(ret) {
		case 0: std::cout << "False\n"; break;
		case 1: std::cout << "True\n"; break;
		case 2: std::cout << "Could not answer query\n"; break;
		case 3: std::cout << "Timeout.\n"; break;
		default: std::cout << "Unhandled error\n";
	}
	
	/*
	Equation e3[2] = {Equation(-2.0, 1.0), Equation(5.0, -1.0)};
	Equation e4(-1.0, 1.0);
	std::cout << "---------------test2-----------------\n";
	std::cout << "[" << e3[0] << " /\\ "<< e3[1] << "]\t==>\t[" << e4 << "]? ";
	ret = Equation::multi_imply(e3, 2, e4);
	switch(ret) {
		case 0: std::cout << "False\n"; break;
		case 1: std::cout << "True\n"; break;
		case 2: std::cout << "Could not answer query\n"; break;
		case 3: std::cout << "Timeout.\n"; break;
		default: std::cout << "Unhandled error\n";
	}


	Equation e5[2] = {Equation(-2.0, 1.0), Equation(5.0, -1.0)};
	Equation e6(-3.0, 1.0);
	std::cout << "---------------test2-----------------\n";
	std::cout << "[" << e5[0] << " /\\ "<< e5[1] << "]\t==>\t[" << e6 << "]? ";
	ret = Equation::multi_imply(e5, 2, e6);
	switch(ret) {
		case 0: std::cout << "False\n"; break;
		case 1: std::cout << "True\n"; break;
		case 2: std::cout << "Could not answer query\n"; break;
		case 3: std::cout << "Timeout.\n"; break;
		default: std::cout << "Unhandled error\n";
	}
	*/
	return 0;
}
