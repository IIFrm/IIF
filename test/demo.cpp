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
int y = 0;
int z = 0;
int xa = 0;
int ya = 0;

int func1()
{


	//x, z : int
	assume((x >= 0) 
		&& (x <= 10000));
	z = x + 1;
	assert(z > 0);


}

int func2()
{


	//x, z : int
	assume((x >= 0) 
		&& (x <= 10000));
	if (x == 0)
		z = x + 1;
	else 
		z = x;
	assert(z > 0);


}

int func3()
{


	//x, z : int
	assume((x >= 0) 
		&& (z == 0));
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
	assert(P2);		// Postcondition:  P2		(Inv /\ !B) => P2


}


int func5()
{


	//x, y : int
	assume(x + y > 0);
	while (x >= 0) {
		x = x - 1;
		y = y + 1;
	}
	assert(y > 0);


}

int func6()
{


	//x, y : int
	x = rand(), y = rand();
	assume(x + y > 0);
	print(x + y > 0);
	while (x >= 0) {
		print(x, y);
		x = x - 1;
		y = y + 1;
	}
	print(x, y);
	assert(y > 0);
	print(y > 0);


}


int looptimes = cin.get();
int branch = 1;

int func7()
{


	//x, y : int
	//xa, ya : int
	assume(xa + 2 * ya >= 0);
	while (looptimes--) {
		x = xa + 2 * ya;
		y = -2 * xa + ya;

		x++;
		if (branch) 
			y = y + x;
		else y = y - x;
		branch = 1 - branch;

		xa = x - 2 * y;
		ya = 2 * x + y;
	}
	assert(xa + 2 * ya >= 0);


}

int func8()
{


	//x, y : int
	//xa, ya : int
	xa = rand(), ya = rand();
	assume(xa + 2 * ya >= 0);
	print(xa + 2 * ya >= 0);
	while (looptimes--) {
		print(xa, ya);
		x = xa + 2 * ya;
		y = -2 * xa + ya;

		x++;
		if (branch) 
			y = y + x;
		else y = y - x;
		branch = 1 - branch;

		xa = x - 2 * y;
		ya = 2 * x + y;
	}
	print(xa, ya);
	assert(xa + 2 * ya >= 0);
	print(xa + 2 * ya >= 0);



}


int func9()
{


	//x, y : int
	//xa, ya : int
	assume((xa + 2 * ya >= 0) 
		&& (xa + 2 * ya <= 10000));
	while (nondet() 
		&& (xa + 2 * ya <= 10000)) {
		x = xa + 2 * ya;
		y = -2 * xa + ya;

		x++;
		if (nondet()) y = y + x;
		else y = y - x;

		xa = x - 2 * y;
		ya = 2 * x + y;
	}
	assert(xa + 2 * ya >= 0);


}


int func10()
{

	//x : int
	//loop_times : uint
	assume((x>=0) && (x<=50));
	while(loop_times-- > 0) {
		if (x <= 0) 
			x++;
		else 
			x--;
	}
	assert((x>=0) && (x<=50))

}

int func11()
{

	//x : int
	//loop_times : uint
	x = rand();
	loop_times = rand();
	assume((x>=0) && (x<=50));
	print((x>=0) && (x<=50));
	while(loop_times-- > 0) {
		print(x, loop_times);
		if (x <= 0) 
			x++;
		else 
			x--;
	}
	print(x, loop_times);
	assert((x>=0) && (x<=50))
	print((x>=0) && (x<=50));

}
