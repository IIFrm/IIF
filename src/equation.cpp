/** @file equation.h
 *  @brief Defines the linear equation format and its solution format.
 *
 *  @author Li Jiaying
 *  @bug No known bugs. 
 */

#include "equation.h"

//extern char variable_name[Cv1to4][8];

bool Equation::factorNv1Times2(double A, double B, double C) {
#if (linux || __MACH__)
	z3::config cfg;
	cfg.set("auto_config", true);
	z3::context ctx(cfg);
	std::cout << GREEN << A << " * x^2 + " << B << " * x + " << C;
	std::cout << BLUE << " = " << YELLOW << "(a0x + b0) (a1x + b1)" << WHITE << std::endl; 

	// A * x^2 + B * x + C = 0
	z3::expr a0 = ctx.int_const("a0");
	z3::expr a1 = ctx.int_const("a1");
	z3::expr b0 = ctx.int_const("b0");
	z3::expr b1 = ctx.int_const("b1");
	//z3::expr m0 = ctx.int_const("m0");
	char real[65];
	snprintf(real, 64, "%2.8f", A);
	z3::expr a = ctx.int_val(real);
	snprintf(real, 64, "%2.8f", B);
	z3::expr b = ctx.int_val(real);
	snprintf(real, 64, "%2.8f", C);
	z3::expr c = ctx.int_val(real);

	z3::solver s(ctx);
	s.add(a0 * a1 == a);
	s.add(a0 * b1 + a1 * b0 == b);
	s.add(b0 * b1 == c);
	s.add(a0 <= a1);
	s.add(a1 > 0);

	//std::cout << s << std::endl;
	z3::check_result retu = s.check();
	if (retu == unsat) {
		std::cout << "unSAT" << std::endl;
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


bool Equation::factorNv1Times3(double A, double B, double C, double D) {
#if (linux || __MACH__)
	z3::config cfg;
	cfg.set("auto_config", true);
	z3::context ctx(cfg);
	std::cout << GREEN << A << " * x^3 + " << B << " * x^2 + " << C << " * x + " << D; 
	std::cout << BLUE << " = " << YELLOW << "(a0x + b0) (a1x + b1) (a2x + b2)" << WHITE << std::endl; 

	z3::expr a0 = ctx.int_const("a0");
	z3::expr a1 = ctx.int_const("a1");
	z3::expr a2 = ctx.int_const("a2");
	z3::expr b0 = ctx.int_const("b0");
	z3::expr b1 = ctx.int_const("b1");
	z3::expr b2 = ctx.int_const("b2");
	char real[65];
	snprintf(real, 64, "%2.8f", A);
	z3::expr a = ctx.int_val(real);
	snprintf(real, 64, "%2.8f", B);
	z3::expr b = ctx.int_val(real);
	snprintf(real, 64, "%2.8f", C);
	z3::expr c = ctx.int_val(real);
	snprintf(real, 64, "%2.8f", D);
	z3::expr d = ctx.int_val(real);


	z3::solver s(ctx);
	s.add(a0 * a1 * a2 == a);
	s.add(a0 * a1 * b2 + a0 * b1 * a2 + b0 * a1 * a2 == b);
	s.add(a0 * b1 * b2 + b0 * a1 * b2 + b0 * b1 * a2 == c);
	s.add(b0 * b1 * b2 == d);
	s.add(a0 <= a1);
	s.add(a1 <= a2);
	s.add(a1 > 0);
	//std::cout << s << std::endl;
	z3::check_result retu = s.check();
	if (retu == unsat) {
		std::cout << "unSAT" << std::endl;
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


bool Equation::factorNv2Times2(double A, double B, double C, double D, double E, double F) {
#if (linux || __MACH__)
	z3::config cfg;
	cfg.set("auto_config", true);
	z3::context ctx(cfg);

	// Ax^2 + By^2 + Cxy + Dx + Ey + F = 0
	std::cout << GREEN << A << " * x^2 + " << B << " * y^2 + " << C << " * xy + " << D << " * x + " << E << " * y + " << F;
	std::cout << BLUE << " = " << YELLOW << "(a0x + b0y + c0) (a1x + b1y + c1)" << WHITE << std::endl; 

	z3::expr a0 = ctx.int_const("a0");
	z3::expr a1 = ctx.int_const("a1");
	z3::expr b0 = ctx.int_const("b0");
	z3::expr b1 = ctx.int_const("b1");
	z3::expr c0 = ctx.int_const("c0");
	z3::expr c1 = ctx.int_const("c1");
	char real[65];
	snprintf(real, 64, "%2.8f", A);
	z3::expr a = ctx.real_val(real);
	snprintf(real, 64, "%2.8f", B);
	z3::expr b = ctx.real_val(real);
	snprintf(real, 64, "%2.8f", C);
	z3::expr c = ctx.real_val(real);
	snprintf(real, 64, "%2.8f", D);
	z3::expr d = ctx.real_val(real);
	snprintf(real, 64, "%2.8f", E);
	z3::expr e = ctx.real_val(real);
	snprintf(real, 64, "%2.8f", F);
	z3::expr f = ctx.real_val(real);

	z3::solver s(ctx);
	s.add(a0 * a1 == a);
	s.add(b0 * b1 == b);
	s.add(a0 * b1 + b0 * a1 == c);
	s.add(a0 * c1 + c0 * a1 == d);
	s.add(b0 * c1 + c0 * b1 == e);
	s.add(c0 * c1 == f);
	s.add(a0 <= a1);
	s.add(a1 > 0);
	//std::cout << s << std::endl;
	z3::check_result retu = s.check();
	if (retu == unsat) {
		std::cout << "unSAT" << std::endl;
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


bool Equation::factorNv2Times3(double A, double B, double C, double D, double E, 
		double F, double G, double H, double I, double J) {
#if (linux || __MACH__)
	z3::config cfg;
	cfg.set("auto_config", true);
	z3::context ctx(cfg);

	std::cout << GREEN << A << " * x^3 + " << B << " * y^3 + " << C << " * x^2y + " << D << " * xy^2 + " 
		<< E << " * x^2 + " << F << " * y^2 + " << G << " * xy + " << H << " * x + " << I << " * y + " << J;
	std::cout << BLUE << " = " << YELLOW << "(a0x + b0y + c0) (a1x + b1y + c1) (a2x + b2y + c2)" << WHITE << std::endl; 

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
	snprintf(real, 64, "%2.8f", A);
	z3::expr a = ctx.real_val(real);
	snprintf(real, 64, "%2.8f", B);
	z3::expr b = ctx.real_val(real);
	snprintf(real, 64, "%2.8f", C);
	z3::expr c = ctx.real_val(real);
	snprintf(real, 64, "%2.8f", D);
	z3::expr d = ctx.real_val(real);
	snprintf(real, 64, "%2.8f", E);
	z3::expr e = ctx.real_val(real);
	snprintf(real, 64, "%2.8f", F);
	z3::expr f = ctx.real_val(real);
	snprintf(real, 64, "%2.8f", G);
	z3::expr g = ctx.real_val(real);
	snprintf(real, 64, "%2.8f", H);
	z3::expr h = ctx.real_val(real);
	snprintf(real, 64, "%2.8f", I);
	z3::expr i = ctx.real_val(real);
	snprintf(real, 64, "%2.8f", J);
	z3::expr j = ctx.real_val(real);

	z3::solver s(ctx);
	s.add(a0 * a1 * a2 == a); // x^3
	s.add(b0 * b1 * b2 == b); // y^3
	s.add(a0 * a1 * b2 + a0 * b1 * a2 + b0 * a1 * a2 == c); // x^2y
	s.add(a0 * b1 * b2 + b0 * a1 * b2 + b0 * b1 * a2 == d); // xy^2

	s.add(a0 * a1 * c2 + a0 * c1 * a2 + c0 * a1 * a2 == e); // x^2
	s.add(c0 * b1 * b2 + b0 * c1 * b2 + b0 * b1 * c2 == f); // y^1
	s.add(a0 * b1 * c2 + a0 * c1 * b2 + b0 * a1 * c2 + c0 * a1 * b2 + b0 * c1 * a2 + c0 * b1 * a2 == g); // xy

	s.add(a0 * c1 * c2 + c0 * a1 * c2 + c0 * c1 * a2 == h); // x
	s.add(b0 * c1 * c2 + c0 * b1 * c2 + c0 * c1 * b2 == i); // y

	s.add(c0 * c1 * c2 == j); // 1

	s.add(a0 <= a1);
	s.add(a1 <= a2);
	s.add(a1 > 0);

	//std::cout << s << std::endl;
	z3::check_result retu = s.check();
	if (retu == unsat) {
		std::cout << "unSAT" << std::endl;
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


bool Equation::factorNv3Times2(double A, double B, double C, double D, double E, 
		double F, double G, double H, double I, double J) {
#if (linux || __MACH__)
	z3::config cfg;
	cfg.set("auto_config", true);
	z3::context ctx(cfg);

	// Ax^2 + By^2 + Cz^2 + Dxy + Exz + Fyz + Gx + Hy + Iz + J = 0
	std::cout << GREEN << A << " * x^2 + " << B << " * y^2 + " << C << " * z^2 + " << D << " * xy + " 
		<< E << " * xz + " << F << " * yz + " << G << " * x + " << H << " * y + " << I << " * z + " << J;
	std::cout << BLUE << " = " << YELLOW << "(a0x + b0y + c0z + d0) (a1x + b1y + c1z + d1)" << WHITE << std::endl; 

	z3::expr a0 = ctx.int_const("a0");
	z3::expr a1 = ctx.int_const("a1");
	z3::expr b0 = ctx.int_const("b0");
	z3::expr b1 = ctx.int_const("b1");
	z3::expr c0 = ctx.int_const("c0");
	z3::expr c1 = ctx.int_const("c1");
	z3::expr d0 = ctx.int_const("d0");
	z3::expr d1 = ctx.int_const("d1");

	char real[65];
	snprintf(real, 64, "%2.8f", A);
	z3::expr a = ctx.real_val(real);
	snprintf(real, 64, "%2.8f", B);
	z3::expr b = ctx.real_val(real);
	snprintf(real, 64, "%2.8f", C);
	z3::expr c = ctx.real_val(real);
	snprintf(real, 64, "%2.8f", D);
	z3::expr d = ctx.real_val(real);
	snprintf(real, 64, "%2.8f", E);
	z3::expr e = ctx.real_val(real);
	snprintf(real, 64, "%2.8f", F);
	z3::expr f = ctx.real_val(real);
	snprintf(real, 64, "%2.8f", G);
	z3::expr g = ctx.real_val(real);
	snprintf(real, 64, "%2.8f", H);
	z3::expr h = ctx.real_val(real);
	snprintf(real, 64, "%2.8f", I);
	z3::expr i = ctx.real_val(real);
	snprintf(real, 64, "%2.8f", J);
	z3::expr j = ctx.real_val(real);

	z3::solver s(ctx);
	s.add(a0 * a1 == a); // x^2
	s.add(b0 * b1 == b); // y^2
	s.add(c0 * c1 == c); // z^2
	s.add(a0 * b1 + b0 * a1 == d); // xy
	s.add(a0 * c1 + c0 * a1 == e); // xz
	s.add(b0 * c1 + c0 * b1 == f); // yz
	s.add(a0 * d1 + d0 * a1 == g); // x
	s.add(b0 * d1 + d0 * b1 == h); // y
	s.add(c0 * d1 + d0 * c1 == i); // z
	s.add(d0 * d1 == j); // z^2
	s.add(a0 <= a1);
	s.add(a1 > 0);

	//std::cout << s << std::endl;
	z3::check_result retu = s.check();
	if (retu == unsat) {
		std::cout << "unSAT" << std::endl;
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

bool Equation::toStandardForm(const Equation& e, double* coefs, int et) {
	et = e.getEtimes();
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
}


