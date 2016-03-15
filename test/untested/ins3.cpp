#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<iostream>
using namespace std;


bool passP = false;
bool passQ = false;

int main (int argc, char** argv) {
	if (argc < 4) return -1;
	int x = atoi(argv[1]);
	int y = atoi(argv[2]);
	int z = atoi(argv[3]);

	//Precondition: x + y >= 0
	//assume (x + y >= 0)
	if (x + y + z >= 0)
		passP = true;
/*		if (passP) 
			cout << "? 1 1:" << x << " 2:" << y << " 3:" << z << endl;
		else
			cout << "? -1 1:" << x << " 2:" << y << " 3:" << z << endl;
*/

	while (z > 0) {
	if (passP) 
		cout << "? 1 1:" << x << " 2:" << y << " 3:" << z << endl;
	else
		cout << "? -1 1:" << x << " 2:" << y << " 3:" << z << endl;
		x++;
		y++;
		z-=2;
	}

	// Postcondition: y > 0
	// assert (y > 0);
	if (x + y >= 0)
		passQ = true;
	if (passP) { // pass Precondition, Q should be passed!
		if (passQ) cout << "+ 1 1:" << x << " 2:" << y << " 3:" << z << endl;
		else cout << "x 1 1:" << x << " 2:" << y <<  " 3:" << z << endl;	 //	cerr << "!BUG! input:" << atoi(argv[1]) << " " << atoi(argv[2]) << endl;
	} else { // not pass Precondition, if not pass Postcondition, all are negative samples.
		if (passQ) cout << "- -1 1:" << x << " 2:" << y << " 3:" << z << endl;
		else cout << "+ -1 1:" << x << " 2:" << y << " 3:" << z << endl;
	}

	return 0;
}

