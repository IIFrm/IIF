/** @file equation.h
 *  @brief Defines the linear equation format and its solution format.
 *
 *  @author Li Jiaying
 *  @bug No known bugs. 
 */

#include "equation.h"


bool Equation::factorNv1Times2(double *B) { //, double B, double C) {
#if (linux || __MACH__)
	z3::config cfg;
	cfg.set("auto_config", true);
	z3::context ctx(cfg);
	std::cout << GREEN << B[0] << " * x^2 + " << B[1] << " * x + " << B[2];
	std::cout << BLUE << " = " << YELLOW << "(a0x + b0) (a1x + b1)\t" << WHITE;

	// A * x^2 + B * x + C = 0
	z3::expr a0 = ctx.int_const("a0");
	z3::expr a1 = ctx.int_const("a1");
	z3::expr b0 = ctx.int_const("b0");
	z3::expr b1 = ctx.int_const("b1");

	char real[65];
	std::vector<z3::expr> b;
	for (int i = 0; i < 3; i++) {
		snprintf(real, 64, "%2.8f", B[i]);
		b.push_back(ctx.real_val(real));
	}

	z3::solver s(ctx);
	s.add(a0 * a1 == b[0]);
	s.add(a0 * b1 + a1 * b0 == b[1]);
	s.add(b0 * b1 == b[2]);
	s.add(a0 <= a1);
	s.add(a1 > 0);

	//std::cout << s << std::endl;
	z3::check_result retu = s.check();
	if (retu == unsat) {
		std::cout << RED << "unSAT" << WHITE << std::endl;
		return -1;
	}
	//std::cout << "SAT" << std::endl;
	z3::model z3m = s.get_model();
	//std::cout << "Z3 MODEL: "<< RED << z3m << WHITE << "\n";
	for (unsigned i = 0; i < z3m.size(); i++) {
		z3::func_decl v = z3m[i];
		std::cout << v.name() << " = " << z3m.get_const_interp(v) << "\n";
	}
#endif
	return 0;
}


bool Equation::factorNv1Times3(double *B) { //, double B, double C, double D) {
#if (linux || __MACH__)
	z3::config cfg;
	cfg.set("auto_config", true);
	z3::context ctx(cfg);
	std::cout << GREEN << B[0] << " * x^3 + " << B[1] << " * x^2 + " << B[2] << " * x + " << B[3]; 
	std::cout << BLUE << " = " << YELLOW << "(a0x + b0) (a1x + b1) (a2x + b2)\t" << WHITE;

	z3::expr a0 = ctx.int_const("a0");
	z3::expr a1 = ctx.int_const("a1");
	z3::expr a2 = ctx.int_const("a2");
	z3::expr b0 = ctx.int_const("b0");
	z3::expr b1 = ctx.int_const("b1");
	z3::expr b2 = ctx.int_const("b2");

	char real[65];
	std::vector<z3::expr> b;
	for (int i = 0; i < 4; i++) {
		snprintf(real, 64, "%2.8f", B[i]);
		b.push_back(ctx.real_val(real));
	}

	z3::solver s(ctx);
	s.add(a0 * a1 * a2 == b[0]);
	s.add(a0 * a1 * b2 + a0 * b1 * a2 + b0 * a1 * a2 == b[1]);
	s.add(a0 * b1 * b2 + b0 * a1 * b2 + b0 * b1 * a2 == b[2]);
	s.add(b0 * b1 * b2 == b[3]);
	s.add(a0 <= a1);
	s.add(a1 <= a2);
	s.add(a1 > 0);
	//std::cout << s << std::endl;
	z3::check_result retu = s.check();
	if (retu == unsat) {
		std::cout << RED << "unSAT" << WHITE << std::endl;
		return false;
	}
	//std::cout << "SAT" << std::endl;
	z3::model z3m = s.get_model();
	//std::cout << "Z3 MODEL: "<< RED << z3m << WHITE << "\n";
	for (unsigned i = 0; i < z3m.size(); i++) {
		z3::func_decl v = z3m[i];
		std::cout << v.name() << " = " << z3m.get_const_interp(v) << "\n";
	}
#endif
	return true;
}


bool Equation::factorNv2Times2(double *B) {
#if (linux || __MACH__)
	z3::config cfg;
	cfg.set("auto_config", true);
	z3::context ctx(cfg);

	// Ax^2 + By^2 + Cxy + Dx + Ey + F = 0
	std::cout << GREEN << B[0] << " * x^2 + " << B[1] << " * y^2 + " << B[2] << " * xy + " << B[3] << " * x + " << B[4] << " * y + " << B[5];
	std::cout << BLUE << " = " << YELLOW << "(a0x + b0y + c0) (a1x + b1y + c1)\t" << WHITE; 

	z3::expr a0 = ctx.int_const("a0");
	z3::expr a1 = ctx.int_const("a1");
	z3::expr a2 = ctx.int_const("a2");
	z3::expr b0 = ctx.int_const("b0");
	z3::expr b1 = ctx.int_const("b1");
	z3::expr b2 = ctx.int_const("b2");
	z3::expr c0 = ctx.int_const("c0");
	z3::expr c1 = ctx.int_const("c1");
	z3::expr c2 = ctx.int_const("c2");

	char real[65];
	std::vector<z3::expr> b;
	for (int i = 0; i < 6; i++) {
		snprintf(real, 64, "%2.8f", B[i]);
		b.push_back(ctx.real_val(real));
	}

	z3::solver s(ctx);
	s.add(a0 * a1 == b[0]);
	s.add(b0 * b1 == b[1]);
	s.add(a0 * b1 + b0 * a1 == b[2]);
	s.add(a0 * c1 + c0 * a1 == b[3]);
	s.add(b0 * c1 + c0 * b1 == b[4]);
	s.add(c0 * c1 == b[5]);
	s.add(a0 <= a1);
	s.add(a1 > 0);
	//std::cout << s << std::endl;
	z3::check_result retu = s.check();
	if (retu == unsat) {
		std::cout << RED << "unSAT" << WHITE << std::endl;
		return false;
	}
	//std::cout << "SAT" << std::endl;
	z3::model z3m = s.get_model();
	//std::cout << "Z3 MODEL: \n"<< GREEN << z3m << WHITE << "\n";
	for (unsigned i = 0; i < z3m.size(); i++) {
		z3::func_decl v = z3m[i];
		std::cout << v.name() << " = " << z3m.get_const_interp(v) << "\n";
	}
#endif
	return true;
}


bool Equation::factorNv2Times3(double* B) {
#if (linux || __MACH__)
	z3::config cfg;
	cfg.set("auto_config", true);
	z3::context ctx(cfg);

	std::cout << GREEN << B[0] << " * x^3 + " << B[1] << " * y^3 + " << B[2] << " * x^2y + " << B[3] << " * xy^2 + " 
		<< B[4] << " * x^2 + " << B[5] << " * y^2 + " << B[6] << " * xy + " << B[7] << " * x + " << B[8] << " * y + " << B[9];
	std::cout << BLUE << " = " << YELLOW << "(a0x + b0y + c0) (a1x + b1y + c1) (a2x + b2y + c2)\t" << WHITE; 

	z3::expr a0 = ctx.int_const("a0");
	z3::expr a1 = ctx.int_const("a1");
	z3::expr a2 = ctx.int_const("a2");
	z3::expr b0 = ctx.int_const("b0");
	z3::expr b1 = ctx.int_const("b1");
	z3::expr b2 = ctx.int_const("b2");
	z3::expr c0 = ctx.int_const("c0");
	z3::expr c1 = ctx.int_const("c1");
	z3::expr c2 = ctx.int_const("c2");

	char real[65];
	std::vector<z3::expr> b;
	for (int i = 0; i < 10; i++) {
		snprintf(real, 64, "%2.8f", B[i]);
		b.push_back(ctx.real_val(real));
	}

	z3::solver s(ctx);
	s.add(a0 * a1 * a2 == b[0]); // x^3
	s.add(b0 * b1 * b2 == b[1]); // y^3
	s.add(a0 * a1 * b2 + a0 * b1 * a2 + b0 * a1 * a2 == b[2]); // x^2y
	s.add(a0 * b1 * b2 + b0 * a1 * b2 + b0 * b1 * a2 == b[3]); // xy^2

	s.add(a0 * a1 * c2 + a0 * c1 * a2 + c0 * a1 * a2 == b[4]); // x^2
	s.add(c0 * b1 * b2 + b0 * c1 * b2 + b0 * b1 * c2 == b[5]); // y^1
	s.add(a0 * b1 * c2 + a0 * c1 * b2 + b0 * a1 * c2 + c0 * a1 * b2 + b0 * c1 * a2 + c0 * b1 * a2 == b[6]); // xy

	s.add(a0 * c1 * c2 + c0 * a1 * c2 + c0 * c1 * a2 == b[7]); // x
	s.add(b0 * c1 * c2 + c0 * b1 * c2 + c0 * c1 * b2 == b[8]); // y

	s.add(c0 * c1 * c2 == b[9]); // 1

	s.add(a0 <= a1);
	s.add(a1 <= a2);
	s.add(a1 > 0);

	//std::cout << s << std::endl;
	z3::check_result retu = s.check();
	if (retu == unsat) {
		std::cout << RED << "unSAT" << WHITE << std::endl;
		return false;
	}
	//std::cout << "SAT" << std::endl;
	z3::model z3m = s.get_model();
	//std::cout << "Z3 MODEL: \n"<< GREEN << z3m << WHITE << "\n";
	for (unsigned s = 0; s < z3m.size(); s++) {
		z3::func_decl v = z3m[s];
		std::cout << v.name() << " = " << z3m.get_const_interp(v) << "\n";
	}
#endif
	return true;
}


bool Equation::factorNv3Times2(double *B) { //, double B, double C, double D, double E, 
#if (linux || __MACH__)
	z3::config cfg;
	cfg.set("auto_config", true);
	z3::context ctx(cfg);

	// Ax^2 + By^2 + Cz^2 + Dxy + Exz + Fyz + Gx + Hy + Iz + J = 0
	std::cout << GREEN << B[0] << " * x^2 + " << B[1] << " * y^2 + " << B[2] << " * z^2 + " << B[3] << " * xy + " 
		<< B[4] << " * xz + " << B[5] << " * yz + " << B[6] << " * x + " << B[7] << " * y + " << B[8] << " * z + " << B[9];
	std::cout << BLUE << " = " << YELLOW << "(a0x + b0y + c0z + d0) (a1x + b1y + c1z + d1)\t" << WHITE;

	z3::expr a0 = ctx.int_const("a0");
	z3::expr a1 = ctx.int_const("a1");
	z3::expr b0 = ctx.int_const("b0");
	z3::expr b1 = ctx.int_const("b1");
	z3::expr c0 = ctx.int_const("c0");
	z3::expr c1 = ctx.int_const("c1");
	z3::expr d0 = ctx.int_const("d0");
	z3::expr d1 = ctx.int_const("d1");

	char real[65];
	std::vector<z3::expr> b;
	for (int i = 0; i < 10; i++) {
		snprintf(real, 64, "%2.8f", B[i]);
		b.push_back(ctx.real_val(real));
	}

	z3::solver s(ctx);
	s.add(a0 * a1 == b[0]); // x^2
	s.add(b0 * b1 == b[1]); // y^2
	s.add(c0 * c1 == b[2]); // z^2
	s.add(a0 * b1 + b0 * a1 == b[3]); // xy
	s.add(a0 * c1 + c0 * a1 == b[4]); // xz
	s.add(b0 * c1 + c0 * b1 == b[5]); // yz
	s.add(a0 * d1 + d0 * a1 == b[6]); // x
	s.add(b0 * d1 + d0 * b1 == b[7]); // y
	s.add(c0 * d1 + d0 * c1 == b[8]); // z
	s.add(d0 * d1 == b[9]); // z^2
	s.add(a0 <= a1);
	s.add(a1 > 0);

	//std::cout << s << std::endl;
	z3::check_result retu = s.check();
	if (retu == unsat) {
		std::cout << RED << "unSAT" << WHITE << std::endl;
		return false;
	}
	//std::cout << "SAT" << std::endl;
	z3::model z3m = s.get_model();
	//std::cout << "Z3 MODEL: \n"<< GREEN << z3m << WHITE << "\n";
	for (unsigned s = 0; s < z3m.size(); s++) {
		z3::func_decl v = z3m[s];
		std::cout << v.name() << " = " << z3m.get_const_interp(v) << "\n";
	}
#endif
	return true;
}

bool Equation::toStandardForm(const Equation& e, double* coefs/*, int et*/) {
	int et = e.getEtimes();
	if (et > 3) return false;
	if (Nv > 3) return false;

	if (et == 1) {
		for (int i = 0; i < Nv - 1; i++)
			coefs[i] = e.getTheta(i+1);
		coefs[Nv-1] = e.getTheta(0);
		return true;
	}
	if (et == 2) {
		if (Nv == 1) {
			coefs[0] = e.getTheta(2);
			coefs[1] = e.getTheta(1);
			coefs[2] = e.getTheta(0);
			return true;
		}
		if (Nv == 2) {
			coefs[0] = e.getTheta(3);
			coefs[1] = e.getTheta(5);
			coefs[2] = e.getTheta(4);
			coefs[3] = e.getTheta(1);
			coefs[4] = e.getTheta(2);
			coefs[5] = e.getTheta(0);
			return true;
		}
		if (Nv == 3) {
			coefs[0] = e.getTheta(4);
			coefs[1] = e.getTheta(7);
			coefs[2] = e.getTheta(9);
			coefs[3] = e.getTheta(5);
			coefs[4] = e.getTheta(6);
			coefs[5] = e.getTheta(8);
			coefs[6] = e.getTheta(1);
			coefs[7] = e.getTheta(2);
			coefs[8] = e.getTheta(3);
			coefs[9] = e.getTheta(0);
			return true;
		}
	}
	if (et == 3) {
		if (Nv == 1) {
			coefs[0] = e.getTheta(3);
			coefs[1] = e.getTheta(2);
			coefs[2] = e.getTheta(1);
			coefs[3] = e.getTheta(0);
			return true;
		}
		if (Nv == 2) {
			coefs[0] = e.getTheta(6);
			coefs[1] = e.getTheta(9);
			coefs[2] = e.getTheta(7);
			coefs[3] = e.getTheta(8);
			coefs[4] = e.getTheta(3);
			coefs[5] = e.getTheta(5);
			coefs[6] = e.getTheta(4);
			coefs[7] = e.getTheta(1);
			coefs[8] = e.getTheta(2);
			coefs[9] = e.getTheta(0);
			return true;
		}
		if (Nv == 3) {
			return false;
		}
	}
	return false;
}

#if (linux || __MACH__)
z3::expr Equation::toZ3expr(char** name, z3::context& c) const
{
	char** pname = name;
	if (pname == NULL) {
		pname = new char*[Cv1to4];
		for (int i = 0; i < Cv1to4; i++) {
			pname[i] = new char[8];
			sprintf(pname[i], "x%d", i);
		}
	}

	const Equation& e = *this;
	std::vector<z3::expr> x;
	std::vector<z3::expr> theta;

	char real[65];
	snprintf(real, 64, "%2.8f", e.theta[0]);
	z3::expr expr = c.real_val(real);
	theta.push_back(expr);

	for (int i = 1; i < dims; i++) {
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
		for (int i = 0; i < dims; i++) {
			delete[]pname[i];
		}
		delete[]pname;
	}
	return hypo;
}
#endif
bool Equation::imply(const Equation& e2) {
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

bool Equation::multiImply(const Equation* e1, int e1_num, const Equation& e2) {
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
	}
	else {
#ifdef __PRT_QUERY
		std::cout << "False" << std::endl;
#endif
		return false;
	}
#endif
	return false;
}
