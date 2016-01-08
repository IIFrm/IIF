/** @file solution.h
 *  @brief Defines the linear equation format and its solution format.
 *
 *  @author Li Jiaying
 *  @bug No known bugs. 
 */

#ifndef _SOLUTION_H_
#define _SOLUTION_H_

//#include "header.h"
#include "config.h"
//#include "z3++.h"
#include <cmath>
#include <cfloat>
#include <stdarg.h>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <string.h>
#include <assert.h>
#include "color.h"
//#ifdef linux
//#include <stp/c_interface.h>
//#include "z3++.h"
//using namespace z3;
//#endif


extern int maxv;
extern int minv;
//extern char variable_name[VARS][8];
/*
const double UPBOUND = pow(0.1, PRECISION);
inline double _roundoff(double x)
{
	int inx = nearbyint(x);
	if ((inx >= x * (1 - UPBOUND) && inx <= x * (1 + UPBOUND))
			|| (inx <= x * (1 - UPBOUND) && inx >= x * (1 + UPBOUND)))
		return double(inx);
	if (std::abs(x) <= UPBOUND)
		return 0;
	return x;
}
*/


/** \class Solution
 *  @brief This class defines the format of a valid solution to an equation.
 *
 *  It contains values to each variants in an equation
 */
class Solution{
	public:
		/** @brief Default constructor.
		 *		   Set all its elements to value 0
		 */
		Solution() {
			val[0] = 0;
			for (int i = 1; i < VARS; i++) {
				val[i] = 0;
			}
		}

		/** @brief Most useful constructor
		 *		   Set its elements to the given values, order keeps
		 *		   
		 *	@param a0... each element values for a solution
		 */
		Solution(double a0, ...) {
			va_list ap;
			va_start(ap, a0);
			val[0] = a0;

			for (int i = 1; i < VARS; i++) {
				val[i] = va_arg(ap, double);
			}
			va_end(ap);
		}

		/** @brief support << operator
		 *		   simply output its elements as a tuple
		 *
		 *	@param sol The solution object to be output
		 */
		friend std::ostream& operator << (std::ostream& out, const Solution& sol) {
			out << "(" << sol.val[0];
			for (int j = 1; j < VARS; j++)
				out << "," << sol.val[j];
			out << ")";
			return out;
		}

		double getVal(int i)
		{
			assert ((i < VARS) || "index out of bound.");
			return val[i];
		}

		int setVal(int i, double value)
		{
			assert ((i < VARS) || "index out of bound.");
			val[i] = value;
			return 0;
		}

	private:
		/// The data field of Solution, stores all the values as a solution to an Equation 
		double val[VARS];
};


#endif

