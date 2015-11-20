#include "equation.h"

//#include <cstdio>
#include <cstdlib>
#include <vector>
//#include <cstring>
#include <iostream>

#ifdef linux
//#include <stp/c_interface.h>
#include "z3++.h"
using namespace z3;
#endif

const double UPBOUND = pow(0.1, 3);
inline double _roundoff(double x)
{
	int inx = nearbyint(x);
	if ((inx >= x * (1 - UPBOUND) && inx <= x * (1 + UPBOUND))
			|| (inx <= x * (1 - UPBOUND) && inx >= x * (1 + UPBOUND)))
		return double(inx);
	return x;
}


Solution::Solution() {
	x[0] = 0;
	for (int i = 1; i < VARS; i++) {
		x[i] = 0;
	}
}

Solution::Solution(double a0, ...) {
	va_list ap;
	va_start(ap, a0);
	x[0] = a0;

	for (int i = 1; i < VARS; i++) {
		x[i] = va_arg(ap, double);
	}
	va_end(ap);
}

std::ostream& operator << (std::ostream& out, const Solution& sol) {
	out << "(" << sol.x[0];
	for (int j = 1; j < VARS; j++)
		out << "," << sol.x[j];
	out << ")";
	return out;
}

Equation::Equation() {
	theta0 = 0;
	for (int i = 1; i < VARS; i++) {
		theta[i] = 0; 
	}
}

Equation::Equation(double a0, ...)
{
	va_list ap;
	va_start(ap, a0);
	theta0 = a0;

	for (int i = 0; i < VARS; i++) {
		theta[i] = va_arg(ap, double);
	}
	va_end(ap);
}

Equation::Equation(const Equation& equ) {
	theta0 = equ.theta0;
	for (int i = 0; i < VARS; i++)
		theta[i] = equ.theta[i];
}


Equation& Equation::operator=(const Equation& rhs) {
	if (this == &rhs) { return *this; }
	theta0 = rhs.theta0;
	for (int i = 0; i < VARS; i++)
		theta[i] = rhs.theta[i];
	return *this;
}


std::ostream& operator << (std::ostream& out, const Equation& equ) {
	out << std::setprecision(16) << equ.theta[0] << "{0}";
	for (int j = 1; j < VARS; j++)
		out << "  +  " << equ.theta[j] << "{" <<j << "}";
	out << " >= " << -1 * equ.theta0;
	return out;
}


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


bool Equation::imply(const Equation& e2) {
#ifdef linux
	std::cout << "-------------Imply solving-------------\n";
	Equation& e1 = *this;
	z3::config cfg;
	cfg.set("auto_config", true);
	z3::context c(cfg);
	
	std::vector<z3::expr> x;
	std::vector<z3::expr> theta1;
	std::vector<z3::expr> theta2;
	char name[8];
	char real[65];
	for (int i = 0; i < VARS; i++) {
		//sprintf(name, "%c", 'a' + i);
		sprintf(name, "x%d", i);
		z3::expr tmp = c.real_const(name);
		x.push_back(tmp);
		
		snprintf(real, 64, "%2.8f", e1.theta[i]);
		tmp = c.real_val(real);
		theta1.push_back(tmp);

		snprintf(real, 64, "%2.8f", e2.theta[i]);
		tmp = c.real_val(real);
		theta2.push_back(tmp);
	}

	snprintf(real, 64, "%2.8f", e1.theta0);
	z3::expr expr1 = c.real_val(real);
	snprintf(real, 64, "%2.8f", e2.theta0);
	z3::expr expr2 = c.real_val(real);
	//std::cout << "expr1: " << expr1 << std::endl;
	//std::cout << "expr2: " << expr2 << std::endl;

	for (int i = 0; i < VARS; i++) {
		expr1 = expr1 + theta1[i] * x[i];
		expr2 = expr2 + theta2[i] * x[i];
	}

	z3::expr hypo = expr1 >= 0;
	z3::expr conc = expr2 >= 0;
	//std::cout << "hypo: " << hypo << std::endl;
	//std::cout << "conc: " << conc << std::endl;
	
	//z3::expr final_expr = z3::implies((z3::expr)expr1, (z3::expr)expr2);
	z3::expr query = implies(hypo, conc);
	std::cout << "Query : " << query << std::endl; 
	std::cout << "Answer: ";
	
	z3::solver s(c);
	s.add(!query);
	z3::check_result ret = s.check();
	if (ret == unsat) {
		std::cout << "True" << std::endl;
		return true;
	} else {
		std::cout << "False" << std::endl;
		return false;
	}
#endif
	return false;
}


int Equation::is_similar(const Equation* e, int precision) {
	if (e == NULL) return -1;
	if ((theta0 == 0) && (e->theta0 == 0))
		return -1;
	double ratio = theta0 / e->theta0;
	double down, up;
	if (ratio >= 0) {
		down = ratio * (1 - pow(0.1, precision));
		up = ratio * (1 + pow(0.1, precision));
	}
	else {
		up = ratio * (1 - pow(0.1, precision));
		down = ratio * (1 + pow(0.1, precision));
	}
	//std::cout << "[" << down << ", " << ratio << ", " << up << "]" << std::endl;
	for (int i = 0; i < VARS; i++) {
		if ((theta[i] < e->theta[i] * down) || (theta[i] > e->theta[i] * up))
			return -1;
	}
	return 0;
}

int Equation::roundoff(Equation& e) {
	//std::cout << "ROUND OFF " << *this << " --> ";
	//double min = std::abs(theta0);
	double min = DBL_MAX;
	for (int i = 0; i < VARS; i++) {
		if (theta[i] == 0) continue;
		min = (std::abs(theta[i]) < min) ? std::abs(theta[i]) : min;
	}

	if (min == DBL_MAX) min = 1;	// otherwise we will have */0 operation, return inf or nan...
	if (min == 0) min = 1;	// otherwise we will have */0 operation, return inf or nan...

	if ((std::abs(theta0) < min) && (100 * std::abs(theta0) >= min))	
		// 100 * theta0 is to keep {0.999999x1 + 0.9999999x2 >= 1.32E-9} from converting to  {BIGNUM x1 + BIGNUM x1  >= 1}
		min = std::abs(theta0);


	//if (min <= pow(0.1, 5)) min = 1;
	for (int i = 0; i < VARS; i++)
		e.theta[i] = _roundoff(theta[i] / min);
	e.theta0 = _roundoff(theta0 / min);
	//std::cout << e << std::endl;
	return 0;
}

