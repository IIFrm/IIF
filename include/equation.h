#ifndef _EQUATION_H_
#define _EQUATION_H_

//#include "header.h"
#include "config.h"

#include <cmath>
#include <cfloat>
#include <stdarg.h>
#include <cstdlib>
#include <iostream>
#include <iomanip>


extern int maxv;
extern int minv;

//const double UPBOUND = pow(0.1, 3);

inline double _roundoff(double x);


class Solution{
	public:
		Solution();

		Solution(double a0, ...);

		friend std::ostream& operator << (std::ostream& out, const Solution& sol);

		double x[VARS];
};


class Equation{
	public:
		Equation();

		Equation(double a0, ...);

		Equation(const Equation& equ);

		friend std::ostream& operator << (std::ostream& out, const Equation& equ);

		bool imply(const Equation& e2);

		static int linearSolver(const Equation* equ, Solution& sol) {
			if (equ == NULL) {
				for (int i = 0; i < VARS; i++)
					sol.x[i] = rand() % (maxv - minv + 1) + minv;
				return 0;
			}

			int j; // justify whether all the coefficients are zeros...
			for (j = 0; j < VARS; j++) {
				if (equ->theta[j] != 0) break;
			}
			// If all the coefficients are zeros....
			// We just randomly pickup solutions to return
			if (j == VARS) {
				for (int i = 0; i < VARS; i++) {
					sol.x[i] = rand() % (maxv - minv + 1) + minv;
				}
				return 0;
			}


			int pick;
			double reminder;
			int times = 0;
solve:
			pick = rand() % VARS;
			while (equ->theta[pick] == 0)
				pick = (pick + 1) % VARS;
			reminder = - equ->theta0;
			for (int i = 0; i < VARS; i++) {
				sol.x[i] = rand() % (maxv - minv + 1) + minv;
				if (i != pick)
					reminder -= sol.x[i] * equ->theta[i];
			}
			sol.x[pick] = nearbyint(reminder / equ->theta[pick]) + rand() % 3 - 1;
			if (sol.x[pick] > maxv || sol.x[pick] < minv) {
				if (++times > 10) // sometimes we can not get solution between given scope
					// we try 10 times, if still no suitable solution, we pick the last one...
					goto solve;
			}
			//std::cout << "solved the equation to get one solution";
			return 0;
		}

		static double calc(const Equation* equ, double* sol) {
			if (sol == NULL) return -1;
			if (equ == NULL) return -1;
			double res = equ->theta0;
			for (int i = 0; i < VARS; i++) {
				res += equ->theta[i] * sol[i];
			}
			return res;
		}

		int is_similar(const Equation* e, int precision = 4);

		int roundoff(Equation& e);

		double theta0;
		double theta[VARS];
	private:
};

#endif
