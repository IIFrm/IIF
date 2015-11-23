/** @file equation.h
 *  @brief Defines the linear equation format and its solution format.
 *
 *  @author Li Jiaying
 *  @bug No known bugs. 
 */

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


//inline double _roundoff(double x);


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
		Solution();

		/** @brief Most useful constructor
		 *		   Set its elements to the given values, order keeps
		 *		   
		 *	@param a0... each element values for a solution
		 */
		Solution(double a0, ...);

		/** @brief support << operator
		 *		   simply output its elements as a tuple
		 *
		 *	@param sol The solution object to be output
		 */
		friend std::ostream& operator << (std::ostream& out, const Solution& sol);

		/// The data field of Solution, stores all the values as a solution to an Equation 
		double x[VARS];
};


/** \class Equation
 *  @brief This class defines an equation by storing all its coefficiencies.
 *		   An equation is regarded a hyperplane in math.
 * 
 *  theta0 + theta[0] * x_0 + theta[1] * x_1 + ... + theta[VARS] * x_{VARS} >= 0
 */
class Equation{
	public:
		/** @brief Default constructor.
		 *		   Set all its elements to value 0
		 */
		Equation();

		/** @brief Most useful constructor
		 *		   Set its elements to the given values, order keeps
		 *		   The first element is Theta0
		 */
		Equation(double a0, ...);

		/** @brief Copy constructor.
		 *
		 * @param equ The equation to be copied.
		 */
		Equation(const Equation& equ);
		
		/** @brief Overwrite = operator
		 *	
		 *	This is needed when we want to delete a equation in an equation list
		 *	We copy the next equation to the current one, and repeat this process until tails
		 *
		 *	@param rhs The right-hand-side equation of assignment
		 */
		Equation& operator = (const Equation& rhs);

		/** @brief Output the equation in a readable format
		 * 
		 *	Example:
		 *  2{0} + 3{1} >= 5
		 *  
		 *  @param equ the equation to be ouput
		 */
		friend std::ostream& operator << (std::ostream& out, const Equation& equ);

		/** @brief This imply method checks whether this equation object can imply another one or not
		 *		   That is to say:  *this ==> e2 ??
		 *		   *this is default equation left side
		 *
		 *  Currently, it is based on Z3 prover.
		 *  And the default precision is set to E-8 (2.8f), which is changeable if need 
		 *  
		 *  @param e2 is the equation right side
		 *  @return bool true if yes, false if no.
		 */
		bool imply(const Equation& e2);

		/** @brief A shell on linear_solver(equ, sol)
		 * 
		 * @param sol set by callee as a solution to given object
		 * @return int 0 if no error.
		 */
		int linear_solver(Solution& sol) {
			return linear_solver(this, sol);
		}

		/** @brief The solver for an Equation.
		 * 
		 * This method calcuate the most informative points in space
		 * It return a points really on the margin or next to the margin
		 *
		 * @param sol is set by callee as a solution to given object
		 *			  contains the solution, integer format
		 * @return int 0 if no error.
		 */
		static int linear_solver(const Equation* equ, Solution& sol) {
			if (equ == NULL) {
				/**
				 * equ == NULL means no equation is specified
				 * So we randomly generate points in given scope [minv, maxv]
				 */
				for (int i = 0; i < VARS; i++)
					sol.x[i] = rand() % (maxv - minv + 1) + minv;
				return 0;
			}

			int j; 
			///< a flag to justify whether all the coefficients are zeros...
			for (j = 0; j < VARS; j++) {
				if (equ->theta[j] != 0) break;
			}
			
			/// If all the coefficients are zeros....
			/// We just randomly pickup solutions to return
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
			///< pick store the dimension that should not generate randomly
			///< The algo is we generate numbers randomly, unless the picked dimension
			///< The picked dimension should be calcuate based on equation and other dimensions
			while (equ->theta[pick] == 0)
				pick = (pick + 1) % VARS;
			reminder = - equ->theta0;
			for (int i = 0; i < VARS; i++) {
				sol.x[i] = rand() % (maxv - minv + 1) + minv;
				if (i != pick)
					reminder -= sol.x[i] * equ->theta[i];
			}
			sol.x[pick] = int(reminder / equ->theta[pick]) + rand() % 2;
			if (sol.x[pick] > maxv || sol.x[pick] < minv) {
				if (++times > 10) 
					/** sometimes we can not get solution between given scope
					 *	we try 10 times, if still no suitable solution, we pick the last one...
					 */
					goto solve;
			}
			//std::cout << "solved the equation to get one solution";
			return 0;
		}

		/** @brief This static method is used to get the position info for the given point against given equation
		 * 
		 *	It just substitutes variants with the given point.
		 *	
		 *	@param equ is the given equation, should not be NULL
		 *	@param sol is the tested solution, should not be NULL
		 *	@return The distance/value of the solution to the given equation
		 */
		static double calc(Equation& equ, double* sol) {
			if (sol == NULL) return -1;
			if (&equ == NULL) return -1;
			double res = equ.theta0;
			for (int i = 0; i < VARS; i++) {
				res += equ.theta[i] * sol[i];
			}
			return res;
		}

		/** @brief This method is used to check whether *this equation is similar to given equation e or not.    
		 *		   *this ~= e ???      
		 * 
		 * @param precision defines how much variance we can bare.
		 *		  The default is 4, which means we can bare 0.0001 difference.
		 *		  In this case 1 ~=1.00001, but 1!~=1.000011
		 */
		int is_similar(const Equation& e, int precision = PRECISION);

		/** @brief Do roundoff job for an equation
		 *
		 *	Sometimes the equation has ugly coefficiencies
		 *	we want to make it elegent, which is the purpose of involing this method
		 *	Currently we have not done much work on this
		 *	We have not even use gcd function to adjust the coefficients.
		 *	
		 *	For example.
		 *	1.2345 x1 >= 2.4690    ==>		x1 >= 2
		 *	2 x1 >= 5.000001	   ==>		x1 >= 2.5	
		 *
		 *	@param e Contains the equation that has already rounded off
		 *	@return int 0 if no error.
		 */
		int roundoff(Equation& e);

		double theta0;
		double theta[VARS];
	private:
};

#endif
