/***************************************************************
 *  @file demo_conj.cpp
 *  @brief           
 *             
 *  @author Li Jiaying
 *  @bug no known bugs
 ***************************************************************/
#include <cassert>
#include <iostream>
#include <cstdlib>
#include <assert.h>
#define assume(x) assert(x)

int x = 0;
int z = 0;
int func1()
{


	//x, z : int
	assume((x >= 0) && (x <= 10000));
	z = x + 1;
	assert(z > 0);


}

int func2()
{


	//x, z : int
	assume((x >= 0) && (x <= 10000));
	if (x % 2)
		z = x + 1;
	else 
		z = x;
	assert(z > 0);


}

int func3()
{


	//x, z : int
	assume((x >= 0) && (z == 0));
	while (x >= 0) {
		z = z + 1;
		x = x - 1;
	}
	assert(z > 0);


}

int func4()
{
	int P1 = 0;
	int B = 0;
	int S1 = 0;


	assume(P1);		// Precondition:   P1		P1 => Inv
	while (B) {		// Loop Condition: B		(Inv /\ B) ~~Si~~> Inv
		S1;			// Loop Body: 	   Si...			 
		S2;
		...	
	}			
	assert(z > 0);	// Postcondition:  P2		(Inv /\ !B) => P2


}


int func4()
{


	//x, y : int
	assume(x + y > 0);
	while (x >= 0) {
		x = x - 1;
		y = y + 1;
	}
	assert(y > 0);


}

int func5()
{


	//x, y : int
	x = rand(), y = rand();
	assume_print(x + y > 0);
	while (x >= 0) {
		print(x, y);
		x = x - 1;
		y = y + 1;
	}
	print(x, y);
	assert_print(y > 0);


}

int func10()
{

	//x, z : int
	assume((x>=0) && (x<=50));
	while(rand()) {
		if (x <= 0) x++;
		if (x >= 50) x--;
		if ((x>0) && (x<50)) x += rand()%3 - 1;
	}
	assert((x>=0) && (x<=50))

}
