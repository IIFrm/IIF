/** @file equation.h
 *  @brief Defines the linear equation format and its solution format.
 *
 *  @author Li Jiaying
 *  @bug No known bugs. 
 */

#ifndef _EQUATION_H_
#define _EQUATION_H_

#include "config.h"
#include <cmath>
#include <cfloat>
#include <stdarg.h>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <string>
#include <sstream>
#include "color.h"
#include "solution.h"
#if (linux || __MACH__)
#include "z3++.h"
using namespace z3;
#endif


extern int maxv;
extern int minv;
extern std::string* variables;
extern int vnum;
//extern char variable_name[VARS][8];

const double UPBOUND = pow(0.1, PRECISION);
static double _roundoff(double x)
{
	int inx = nearbyint(x);
	if ((inx >= x * (1 - UPBOUND) && inx <= x * (1 + UPBOUND))
			|| (inx <= x * (1 - UPBOUND) && inx >= x * (1 + UPBOUND)))
		return double(inx);
	if (std::abs(x) <= UPBOUND)
		return 0;
	return double(inx);
	return x;
}


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
		Equation() {
			theta0 = 0;
			for (int i = 1; i < VARS; i++) {
				theta[i] = 0; 
			}
		}

		bool reset(int* values) {
			for (int i = 0; i < VARS; i++) {
				theta[i] = values[i]; 
			}
			theta0 = values[VARS];
			return true;
		}

		/** @brief Most useful constructor
		 *		   Set its elements to the given values, order keeps
		 *		   The first element is Theta0
		 */
		Equation(double a0, ...) {
			va_list ap;
			va_start(ap, a0);
			theta0 = a0;

			for (int i = 0; i < VARS; i++) {
				theta[i] = va_arg(ap, double);
			}
			va_end(ap);
		}

		/** @brief Copy constructor.
		 *
		 * @param equ The equation to be copied.
		 */
		Equation(const Equation& equ) {
			theta0 = equ.theta0;
			for (int i = 0; i < VARS; i++)
				theta[i] = equ.theta[i];
		}

		/** @brief Overwrite = operator
		 *	
		 *	This is needed when we want to delete a equation in an equation list
		 *	We copy the next equation to the current one, and repeat this process until tails
		 *
		 *	@param rhs The right-hand-side equation of assignment
		 */
		Equation& operator=(const Equation& rhs) {
			if (this == &rhs) { return *this; }
			theta0 = rhs.theta0;
			for (int i = 0; i < VARS; i++)
				theta[i] = rhs.theta[i];
			return *this;
		}

		std::string toString() {
			std::ostringstream stm;
			stm << theta[0];
			stm << " * " << variables[0];
			for (int j = 1; j < VARS; j++) {
				stm << "  +  ";
				stm << theta[j] << " * " << variables[j];
			}
			if (theta0 == 0)
				stm << " >= 0";
			else 
				stm << " >= " << -1 * theta0;
			return stm.str();
		}

		/** @brief Output the equation in a readable format
		 * 
		 *	Example:
		 *  2{0} + 3{1} >= 5
		 *  
		 *  @param equ the equation to be ouput
		 */
		friend std::ostream& operator << (std::ostream& out, const Equation& equ) {
			out << std::setprecision(16) << equ.theta[0];
			//out << "{0}";
			out << " * " << variables[0];
			for (int j = 1; j < VARS; j++) {
				out << "  +  " << equ.theta[j]; 
				//out << "{" <<j << "}";
				out << " * " << variables[j];
			}
			if (equ.theta0 == 0)
				out << " >= 0";
			else 
				out << " >= " << -1 * equ.theta0;
			return out;
		}

		/** @brief This method converts *this equation object to z3 expr object.
		 *
		 *  Introducing this method help to simplify imlementation of imply method.
		 *  
		 *  @param name contains each variants names. 
		 *				If NULL, the name would be "x_1", "x_2" form.
		 *	@param c is z3::context, defines which context the return expr will be used.
		 *	@return z3::expr
		 */
//#ifdef linux
#if (linux || __MACH__)
		z3::expr toZ3expr (char** name, z3::context& c) const
		{
			char** pname = name;
			if (pname == NULL) {
				pname = new char*[VARS];
				for (int i = 0; i < VARS; i++) {
					pname[i] = new char[8];
					sprintf(pname[i], "x%d", i);
				}
			}

			const Equation& e = *this;
			std::vector<z3::expr> x;
			std::vector<z3::expr> theta;

			char real[65];
			snprintf(real, 64, "%2.8f", e.theta0);
			z3::expr expr = c.real_val(real);

			for (int i = 0; i < VARS; i++) {
				z3::expr tmp = c.real_const(pname[i]);
				x.push_back(tmp);

				snprintf(real, 64, "%2.8f", e.theta[i]);
				tmp = c.real_val(real);
				theta.push_back(tmp);

				expr = expr + theta[i] * x[i];
			}

			//std::cout << "expr1: " << expr1 << std::endl;
			//std::cout << "expr2: " << expr2 << std::endl;

			z3::expr hypo = expr >= 0;
			if (name == NULL) {
				for (int i = 0; i < VARS; i++) {
					delete []pname[i];
				}
				delete []pname;
			}
			return hypo;
		}
#endif

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
		bool imply(const Equation& e2) {
//#ifdef linux
#if (linux || __MACH__)
#ifdef __PRT_QUERY
			std::cout << "-------------Imply solving-------------\n";
#endif
			Equation& e1 = *this;
			z3::config cfg;
			cfg.set("auto_config", true);
			z3::context c(cfg);

			/*char** name = new char* [1];
			  name[0] = "asdfg";
			  z3::expr hypo = e1.to_z3expr(name, c);
			  z3::expr conc = e2.to_z3expr(name, c);
			  */
			z3::expr hypo = e1.toZ3expr(NULL, c);
			z3::expr conc = e2.toZ3expr(NULL, c);
#ifdef __PRT_QUERY
			std::cout << "hypo: " << hypo << std::endl;
			std::cout << "conc: " << conc << std::endl;
#endif

			z3::expr query = implies(hypo, conc);
#ifdef __PRT_QUERY
			std::cout << "Query : " << query << std::endl; 
			std::cout << "Answer: ";
#endif

			z3::solver s(c);
			s.add(!query);
			z3::check_result ret = s.check();
			if (ret == unsat) {
				return true;
			}
#endif
			return false;
		}

		static bool multiImply(const Equation* e1, int e1_num, const Equation& e2) {
//#ifdef linux
#if (linux || __MACH__)
#ifdef __PRT_QUERY
			std::cout << "-------------Multi-Imply solving-------------\n";
#endif

			z3::config cfg;
			cfg.set("auto_config", true);
			z3::context c(cfg);


			z3::expr hypo = e1[0].toZ3expr(NULL, c);
			for (int i = 1; i < e1_num; i++) {
				hypo = hypo && e1[i].toZ3expr(NULL, c);;
			}

			z3::expr conc = e2.toZ3expr(NULL, c);

			//std::cout << "hypo: " << hypo << std::endl;
			//std::cout << "conc: " << conc << std::endl;

			z3::expr query = implies(hypo, conc);
#ifdef __PRT_QUERY
			std::cout << "Query : " << query << std::endl; 
			std::cout << "Answer: ";
#endif

			z3::solver s(c);
			s.add(!query);
			z3::check_result ret = s.check();


			if (ret == unsat) {
#ifdef __PRT_QUERY
				std::cout << "True" << std::endl;
#endif
				return true;
			} else {
#ifdef __PRT_QUERY
				std::cout << "False" << std::endl;
#endif
				return false;
			}
#endif
			return false;
		}

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
					sol.setVal(i, rand() % (maxv - minv + 1) + minv);
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
					sol.setVal(i, rand() % (maxv - minv + 1) + minv);
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
				sol.setVal(i, rand() % (maxv - minv + 1) + minv);
				if (i != pick)
					reminder -= sol.getVal(i) * equ->theta[i];
			}
			sol.setVal(pick, int(reminder / equ->theta[pick]) + rand() % 2);
			if (sol.getVal(pick) > maxv || sol.getVal(pick) < minv) {
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
			//if (&equ == NULL) return -1;
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
		int is_similar(const Equation& e2, int precision = PRECISION) {
			Equation& e1 = *this;
			//if (&e2 == NULL) return -1;
			double ratio = 0;
			if ((e1.theta0 != 0) && (e2.theta0 != 0)) {
				ratio = e1.theta0 / e2.theta0;
			} else {
				int i;
				for(i = 0; i < VARS; i++) {
					if ((e1.theta[i] != 0) && (e2.theta[i] != 0)) {
						ratio = e1.theta[i] / e2.theta[i];
						break;
					}
				}
				if (i >= VARS)
					return -1;
			}
			//std::cout << "[ratio=" << ratio <<"]";
			double down, up;
			if (ratio >= 0) {
				down = ratio * (1 - pow(0.1, precision));
				up = ratio * (1 + pow(0.1, precision));
			}
			else {
				up = ratio * (1 - pow(0.1, precision));
				down = ratio * (1 + pow(0.1, precision));
			}
			//std::cout << "[" << down << ", " << ratio << ", " << up << "]";
			for (int i = 0; i < VARS; i++) {
				if (e2.theta[i] >= 0) {
					if ((e1.theta[i] < e2.theta[i] * down) || (e1.theta[i] > e2.theta[i] * up))
						return -1;
				}else {
					if ((e1.theta[i] < e2.theta[i] * up) || (e1.theta[i] > e2.theta[i] * down))
						return -1;
				}
			}
			return 0;
		}

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
		int roundoff(Equation& e) {
			//std::cout << "ROUND OFF " << *this << " --> ";
			//double min = std::abs(theta0);
			double min = DBL_MAX;
			double second_min = min;
			for (int i = 0; i < VARS; i++) {
				if (theta[i] == 0) continue;
				if (std::abs(theta[i]) < min) {
					second_min = min;
					min = std::abs(theta[i]);
				}
			}

			if (min == DBL_MAX) min = 1;	// otherwise we will have */0 operation, return inf or nan...
			if (min == 0) min = 1;	// otherwise we will have */0 operation, return inf or nan...
			if (second_min == DBL_MAX) second_min = 1;	// otherwise we will have */0 operation, return inf or nan...
			if (second_min == 0) second_min = 1;	// otherwise we will have */0 operation, return inf or nan...

#ifdef __PRT_EQUATION
			setColor(std::cout, GREEN);
			std::cout << "Before roundoff: " << *this;
			//std::cout << "  [min=" << min << "]"; 
			//std::cout << "  [min=" << min << "]"; 
#endif
			if (min / second_min <= UPBOUND) 
				min = second_min;

			if ((std::abs(theta0) < min) && (1000 * std::abs(theta0) >= min))	
				// 100 * theta0 is to keep {0.999999x1 + 0.9999999x2 >= 1.32E-9} from converting to  {BIGNUM x1 + BIGNUM x1  >= 1}
				//if ((std::abs(theta0) < min) && (std::abs(theta0) > 1.0E-4))	
				min = std::abs(theta0);


			//if (min <= pow(0.1, 5)) min = 1;
			for (int i = 0; i < VARS; i++)
				e.theta[i] = _roundoff(theta[i] / min);
			e.theta0 = _roundoff(theta0 / min);
#ifdef __PRT_EQUATION
			std::cout << "\tAfter roundoff: " << e << std::endl;
			setColor(std::cout);
#endif
			//std::cout << e << std::endl;
			return 0;
		}

		int roundoff() {
			return roundoff(*this);
		}

		inline double getTheta0() { return theta0;}
		inline int setTheta0(double value) { theta0 = value; return 0;}
		inline double getTheta(int i) { 
			assert((i < VARS) || "parameter for getTheta is out of boundary.");
			return theta[i];
		}
		inline int setTheta0(int i, double value) { 
			assert((i < VARS) || "parameter for getTheta is out of boundary.");
			theta[i] = value; 
			return 0;
		}
		inline int setTheta(double* values) {
			for (int i = 0; i < VARS; i++)
				theta[i] = values[i];
			return 0;
		}

	private:
		double theta0;
		double theta[VARS];
};

#endif

// Using STP solver
/*
   int handleQuery(VC handle, Expr queryExpr)
   {
   vc_push(handle);
   int result = vc_query(handle, queryExpr);
   vc_pop(handle);
   switch(result)
   {
   case 0: printf("False\n");
   printf("Counter example:\n");
   vc_printCounterExample(handle);
   break;
   case 1: printf("True\n"); break;
   case 2: printf("Could not answer query\n");	break;
   case 3: printf("Timeout.\n"); break;
   default: printf("Unhandled error\n");
   }
   return result;
   }


//#define _STRING(x) #x
//#define STRING(x) _STRING(x)
int Equation::imply(const Equation& e2) {
Equation& e1 = *this;
int width = 32;
VC handle = vc_createValidityChecker();

Expr zero = vc_bvConstExprFromInt(handle, width, 0);

Expr x[VARS], theta1[VARS], theta2[VARS], xtheta1[VARS], xtheta2[VARS], expr1, expr2;
expr1= vc_bvConstExprFromInt(handle, width, (int)e1.theta0);
expr2 = vc_bvConstExprFromInt(handle, width, (int)e2.theta0);

char name[8];
for (int i = 0; i < VARS; i++) {
sprintf(name, "x_%d", i);
x[i] = vc_varExpr(handle, name, vc_bvType(handle, width));

theta1[i] = vc_bvConstExprFromInt(handle, width, (int)e1.theta[i]);
xtheta1[i] = vc_bvMultExpr(handle, width, theta1[i], x[i]);
expr1 = vc_bvPlusExpr(handle, width, expr1, xtheta1[i]);

theta2[i] = vc_bvConstExprFromInt(handle, width, (int)e2.theta[i]);
xtheta2[i] = vc_bvMultExpr(handle, width, theta2[i], x[i]);
expr2 = vc_bvPlusExpr(handle, width, expr2, xtheta2[i]);
}

//expr1 = vc_bvGeExpr(handle, expr1, zero);
//expr2 = vc_bvGeExpr(handle, expr2, zero);

//	Expr imply_expr = vc_impliesExpr(handle, expr1, expr2);
Expr imply_expr = vc_bvLeExpr(handle, expr1, expr2);
std::cout << "imply expr: "; // << expr1 << std::endl;
vc_printExprCCode(handle, imply_expr);
std::cout << "\n\n";
int ret = handleQuery(handle, imply_expr);
vc_Destroy(handle);
return ret;
}
*/


