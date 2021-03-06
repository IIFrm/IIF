/** @file polynomail.h
 *  @brief Defines the linear polynomail format and its solution format.
 *
 *  @author Li Jiaying
 *  @bug No known bugs.
 */

#ifndef _POLYNOMIAL_H_
#define _POLYNOMIAL_H_

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
#include <gsl/gsl_poly.h>
#include "color.h"
#include "solution.h"
#include "candidates.h"
#if (linux || __MACH__)
#include "z3++.h"
using namespace z3;
#endif

extern int maxv;
extern int minv;
extern std::string* variables;
extern VariablePowerArray* vparray;
extern int vnum;
class Candidates;


/** \class Polynomial
 *  @brief This class defines an polynomail by storing all its coefficiencies.
 *		   An polynomail is regarded a hyperplane in math.
 *
 *  theta[0] * 1 + theta[1] * x_1 + theta[1] * x_2 + ... + theta[Cv1to4] * x_{Cv0to4} >= 0
 */
class Polynomial {
	private:
		int dims;
		int etimes;


	public:
		/** @brief Default constructor.
		 *		   Set all its elements to value 0
		 */
		Polynomial() {
			setEtimes(1);
			for (int i = 1; i < Cv0to4; i++) {
				theta[i] = 0;
			}
		}

		bool set(double* values) {
			for (int i = 0; i < dims; i++) {
				theta[i] = values[i];
			}
			return true;
		}

		bool set(int* values) {
			for (int i = 0; i < dims; i++) {
				theta[i] = values[i];
			}
			return true;
		}

		/** @brief Most useful constructor
		 *		   Set its elements to the given values, order keeps
		 *		   The first element is Theta0
		 */
		Polynomial(double a0, ...) {
			setEtimes(1);
			va_list ap;
			va_start(ap, a0);
			theta[0] = a0;

			for (int i = 1; i < dims; i++) {
				theta[i] = va_arg(ap, double);
			}
			va_end(ap);
		}

		/** @brief Copy constructor.
		 *
		 * @param poly The polynomail to be copied.
		 */
		Polynomial(Polynomial& poly) {
			setEtimes(poly.getEtimes());
			for (int i = 0; i < poly.getDims(); i++)
				theta[i] = poly.theta[i];
		}

		/** @brief Overwrite = operator
		 *
		 *	This is needed when we want to delete a polynomail in an polynomail list
		 *	We copy the next polynomail to the current one, and repeat this process until tails
		 *
		 *	@param rhs The right-hand-side polynomail of assignment
		 */
		Polynomial& operator=(Polynomial& rhs);

		std::string toString() const;

		/** @brief Output the polynomail in a readable format
		 *
		 *	Example:
		 *  5 + 2{0} + 3{1} >= 0
		 *
		 *  @param poly the polynomail to be ouput
		 */
		friend std::ostream& operator<< (std::ostream& out, const Polynomial& poly);


		/** @brief This method converts *this polynomail object to z3 expr object.
		 *
		 *  Introducing this method help to simplify imlementation of uniImply method.
		 *
		 *  @param name contains each variants names.
		 *				If NULL, the name would be "x_1", "x_2" form.
		 *	@param c is z3::context, defines which context the return expr will be used.
		 *	@return z3::expr
		 */
#if (linux || __MACH__)
		z3::expr toZ3expr(char** name, z3::context& c) const;
#endif

		/** @brief This uniImply method checks whether this polynomail object can uniImply another one or not
		 *		   That is to say:  *this ==> e2 ??
		 *		   *this is default polynomail left side
		 *
		 *  Currently, it is based on Z3 prover.
		 *  And the default precision is set to E-8 (2.8f), which is changeable if need
		 *
		 *  @param e2 is the polynomail right side
		 *  @return bool true if yes, false if no.
		 */
		bool uniImply(const Polynomial& e2);
		static bool multiImply(const Polynomial* e1, int e1_num, const Polynomial& e2);

		static int solve_univariate_polynomial(_in_ double* coefs, _in_ int power, _out_ double* results) {
			double* solutions = new double[power * 2];
			gsl_poly_complex_workspace* w = gsl_poly_complex_workspace_alloc (power + 1);
			gsl_poly_complex_solve (coefs, power + 1, w, solutions);
			gsl_poly_complex_workspace_free (w);
			int num_rational_solution = 0;
			for(int i = 0; i < power; i++) {
				if (solutions[2*i+1] == 0)
					results[num_rational_solution++] = solutions[2 * i];
			}
			delete []solutions;
			return num_rational_solution;
		}

		static int solve_univariate_polynomial(_in_ Polynomial& poly, _out_ double* results) {
			assert(Nv == 1);
			return solve_univariate_polynomial(poly.theta, poly.getEtimes(), results);
		}

		inline int solve_univariate_polynomial(double* results) {
			return solve_univariate_polynomial(*this, results);
		}

		double evaluateItem(int index, const double* given_values) {
			assert ((index >= 0) && (index < dims));
			double result = theta[index];
			for (int i = 0; i < Nv; i++)
				result *= pow(given_values[i], vparray[index][i]);
			return result;
		}

		double evaluateCoef(int x, int power, double* given_values) {
			double result = 0;
			given_values[x] = 1;
			for (int i = 0; i < dims; i++) {
				if (vparray[i][x] == power)
					result += evaluateItem(i, given_values);
			}
			return result;
		}

		int evaluatePolynomial(int x, _in_ _out_ double* given_values) {
			double* uni_coefs = new double [etimes];
			double* results = new double [etimes * 2];
			for (int power = 0; power < etimes; power++)
				uni_coefs[power] = evaluateCoef(x, power, given_values);
			int num_rational_solution = solve_univariate_polynomial(uni_coefs, etimes, results);
			if (num_rational_solution > 0)
				given_values[x] = results[0];
			delete []results;
			delete []uni_coefs;
			return num_rational_solution;
		}


		static int solve_multivariate_polynomial(Polynomial& poly, double* results, int x) {
			assert(Nv > 1);
			assert(poly.dims <= Cv0to4);
			return poly.evaluatePolynomial(x, results);
		}

		inline int solve_multivariate_polynomial(double* results, int x = 0) {
			return solve_multivariate_polynomial(*this, results, x);
		}

		static bool factorNv1Times2(double *B);
		static bool factorNv1Times3(double *B);
		static bool factorNv2Times2(double *B);
		static bool factorNv2Times3(double *B);
		static bool factorNv3Times2(double *B);

		static bool toStandardForm(const Polynomial& e, double* coefs);

		bool toStandardForm(double* coefs) {
			return Polynomial::toStandardForm(*this, coefs);
		}

		bool factor() {
			//std::cout << "\t  <<Factoring Polynomial>> \n";
			if (getEtimes() == 1) return true;
			double coefs[Cv0to4];
			this->toStandardForm(coefs);
			switch(Nv) {
				case 1:
					//solve_univariate_polynomial()
					if (getEtimes() == 2) return factorNv1Times2(coefs);
					if (getEtimes() == 3) return factorNv1Times3(coefs);
					return false;
				case 2:
					if (getEtimes() == 2) return factorNv2Times2(coefs);
					if (getEtimes() == 3) return factorNv2Times3(coefs);
					return false;
				case 3:
					if (getEtimes() == 2) return factorNv3Times2(coefs);
					return false;
				case 4:
					return true;
			}
			return false;
		}


		/** @brief A shell on linear_solver(poly, sol)
		 *
		 * @param sol set by callee as a solution to given object
		 * @return int 0 if no error.
		 */
		int solver(Solution& sol) {
			return solver(this, sol);
		}

		/** @brief The solver for an Polynomial.
		 *
		 * This method calcuate the most informative points in space
		 * It return a points really on the margin or next to the margin
		 *
		 * @param sol is set by callee as a solution to given object
		 *			  contains the solution, integer format
		 * @return int 0 if no error.
		 */
		static int solver(/*const*/ Polynomial* poly, Solution& sol) {
			if (poly == NULL) {
				/**
				 * poly == NULL means no polynomail is specified
				 * So we randomly generate points in given scope [minv, maxv]
				 */
				for (int i = 0; i < Nv; i++)
					sol[i] = rand() % (maxv - minv + 1) + minv;
				return 0;
			}

			if (Nv == 1) { 
				double* results = new double[poly->getEtimes()];
				std::cout << "*";
				int num_rational_solution = poly->solve_univariate_polynomial(results);
				if (num_rational_solution >= 1)
					sol[0] = results[rand() % num_rational_solution] + rand() % 3 - 1;
				else
					sol[0] = rand() % (maxv - minv + 1) + minv;
				std::cout << sol[0] << "*";
				delete []results;
				return 0;
			}

			int j;
			//assert(poly->dims == Cv0to1);
			///< a flag to justify whether all the coefficients are zeros...
			for (j = 0; j < poly->dims; j++) {
				if (poly->theta[j] != 0) break;
			}

			/// If all the coefficients are zeros....
			/// We just randomly pickup solutions to return
			if (j == poly->dims) {
				for (int i = 0; i < poly->dims; i++) {
					sol[i] = rand() % (maxv - minv + 1) + minv;
				}
				return 0;
			}

			int pick;
			double reminder;
			int times = 0;

solve_again:
			pick = rand() % poly->dims;
			///< pick store the dimension that should not generate randomly
			///< The algo is we generate numbers randomly, unless the picked dimension
			///< The picked dimension should be calcuate based on polynomail and other dimensions
			while (poly->theta[pick + 1] == 0)
				pick = (pick + 1) % poly->dims;
			reminder = -poly->theta[0];
			for (int i = 0; i < Cv1; i++) {
				sol[i] = rand() % (maxv - minv + 1) + minv;
				if (i != pick)
					reminder -= sol[i] * poly->theta[i + 1];
			}
			sol[pick] = int(reminder / poly->theta[pick + 1]) + rand() % 3 - 1;
			if (sol[pick] > maxv || sol[pick] < minv) {
				if (++times > 10)
					/** sometimes we can not get solution between given scope
					 *	we try 10 times, if still no suitable solution, we pick the last one...
					 */
					goto solve_again;
			}
			//std::cout << "solved the polynomail to get one solution";
			return 0;
		}

		/** @brief This static method is used to get the position info for the given point against given polynomail
		 *
		 *	It just substitutes variants with the given point.
		 *
		 *	@param poly is the given polynomail, should not be NULL
		 *	@param sol is the tested solution, should not be NULL
		 *	@return The distance/value of the solution to the given polynomail
		 */
		static double calc(Polynomial& poly, double* sol) {
			if (sol == NULL) return -1;
			//if (&poly == NULL) return -1;
			double res = poly.theta[0];
			for (int i = 0; i < Nv; i++) {
				res += poly.theta[i+1] * sol[i];
			}
			return res;
		}

		/** @brief This method is used to check whether *this polynomail is similar to given polynomail e or not.
		 *		   *this ~= e ???
		 *
		 * @param precision defines how much variance we can bare.
		 *		  The default is 4, which means we can bare 0.0001 difference.
		 *		  In this case 1 ~=1.00001, but 1!~=1.000011
		 */
		int is_similar(Polynomial& e2, int precision = PRECISION) {
			if (dims != e2.getDims()) return -1;
			double ratio = 0;
			int i;
			for (i = 0; i < dims; i++) {
				if ((theta[i] != 0) && (e2.theta[i] != 0)) {
					ratio = theta[i] / e2.theta[i];
					break;
				}
			}
			if (i >= dims)
				return -1;
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
			for (int i = 0; i < dims; i++) {
				if (e2.theta[i] >= 0) {
					if ((theta[i] < e2.theta[i] * down) || (theta[i] > e2.theta[i] * up))
						return -1;
				}
				else {
					if ((theta[i] < e2.theta[i] * up) || (theta[i] > e2.theta[i] * down))
						return -1;
				}
			}
			return 0;
		}

		/** @brief Do roundoff job for an polynomail
		 *
		 *	Sometimes the polynomail has ugly coefficiencies
		 *	we want to make it elegent, which is the purpose of involing this method
		 *	Currently we have not done much work on this
		 *	We have not even use gcd function to adjust the coefficients.
		 *
		 *	For example.
		 *	1.2345 x1 >= 2.4690    ==>		x1 >= 2
		 *	2 x1 >= 5.000001	   ==>		x1 >= 2.5
		 *
		 *	@param e Contains the polynomail that has already rounded off
		 *	@return int 0 if no error.
		 */
		int roundoff(Polynomial& e);

		Polynomial* roundoff();

		int toCandidates(Candidates* cs);

		inline double getTheta(int i) const {
			assert((i < dims) || "parameter for getTheta is out of boundary.");
			return theta[i];
		}
		inline bool setTheta(int i, double value) {
			assert((i < dims) || "parameter for getTheta is out of boundary.");
			theta[i] = value;
			return true;
		}
		inline int setTheta(double* values) {
			for (int i = 1; i < dims; i++)
				theta[i] = values[i];
			return true;
		}
		inline double getTheta0() const { return theta[0]; }
		inline bool setTheta0(double value) { theta[0] = value; return true; }

		inline int getEtimes() const {
			return etimes;
		}

		bool setEtimes(int et) {
			etimes = et;
			if (et == 1) { dims = Cv0to1; return true; }
			if (et == 2) { dims = Cv0to2; return true; }
			if (et == 3) { dims = Cv0to3; return true; }
			if (et == 4) { dims = Cv0to4; return true; }
			return false;
		}

		bool setDims(int dim) {
			if (dim == Cv0to1) { dims = dim; etimes = 1; return true; }
			if (dim == Cv0to2) { dims = dim; etimes = 2; return true; }
			if (dim == Cv0to3) { dims = dim; etimes = 3; return true; }
			if (dim == Cv0to4) { dims = dim; etimes = 4; return true; }
			return false;
		}

		inline int getDims() {
			return dims;
		}

		//protected:
	public:
		double theta[Cv0to4];
};

#endif

