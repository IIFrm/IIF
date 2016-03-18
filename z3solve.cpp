#include "z3++.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

using namespace std;
using namespace z3;

int vno;
class record{
	public:
		string varname;
		string funcname;
		int value;
		friend ostream& operator << (ostream& out, const record& r) {
			out << r.varname << " --> " << r.funcname << " --> " << r.value;
			return out;
		}
} *recs;

const char* str_as_array = " as-array ";

bool parse_var_filename(const char* var_filename) {
	ifstream ifs(var_filename);
	ifs >> vno;
	recs = new record[vno];
	int i = 0;
	while (ifs >> recs[i].varname) {
		i++;
	}
	assert(i == vno);
	return true;
}

bool save_result_to_file(const char* out_filename) {
	ofstream ofs(out_filename);
	for (int i = 0; i < vno; i++)
		ofs << recs[i].value << "\t";
	ofs.close();
	return true;
}


int main(int argc, char** argv)
{
	if (argc <= 3) return -1;
	try {
		const char* smt_filename = argv[1];
		const char* var_filename = argv[2];
		const char* out_filename = argv[3];
		parse_var_filename(var_filename);
		context c;
		Z3_ast f;
		f = Z3_parse_smtlib2_file(c, smt_filename, 0, 0, 0, 0, 0, 0);
		expr r = to_expr(c, f);
		solver s(c);
		s.add(r);
		//cout << s << "\n";
		check_result ret = s.check();
		if (ret == unsat) {
			//cout << "unSAT" << endl;
			return -1;
		}
		//cout << "SAT" << endl;
		model m = s.get_model();
		//cout << "-->model:\n" << m << "\n";

		//cout << "-->variables & values:\n";
		//for (unsigned i = 0; i < m.size(); i++) {
		//	cout << m[i].name() << " = " << m.get_func_interp(m[i]) << endl;
		//}
		ostringstream stm;
		for (unsigned i = 0; i < m.num_consts(); i++) {
			func_decl f = m.get_const_decl(i);
			//cout << f.name() << " = " << m.get_const_interp(f) << endl;

			stm.str("");
			stm << f.name();
			for (int i = 0; i < vno; i++) {
				if (stm.str() == recs[i].varname) {
					stm.str("");
					stm << m.get_const_interp(f);
					int start_pos = stm.str().find_first_of(str_as_array);
					start_pos += strlen(str_as_array);
					int end_pos = stm.str().find_first_of(")");
					recs[i].funcname = stm.str().substr(start_pos, end_pos - start_pos);
					break;
				}
			}
		}
		//cout << hex;
		for (unsigned i = 0; i < m.num_funcs(); i++) {
			func_decl f = m.get_func_decl(i);
			func_interp fi = m.get_func_interp(f);

			//cout << f.name() << " = " << fi << endl;
			stm.str("");
			stm << f.name();
			for (int i = 0; i < vno; i++) {
				if (stm.str() == recs[i].funcname) {
					stm.str("");
					stm << fi;
					recs[i].value = std::stoi(stm.str(), nullptr, 0);
					//cout << "@@ == " << fi << " == " << stm.str() << " == " << recs[i].value << endl;
					break;
				}
			}
		}

		//cout << "--------------------------------------------------------------------\n";
		/*for (int i = 0; i < vno; i++) {
			cout << i << " --> " << recs[i] << endl;
		}
		cout << dec;
		*/
		save_result_to_file(out_filename);
		delete []recs;
		return 0;
	}
	catch (z3::exception ex) {
		cout << ex.msg() << endl;
		return 1;
	}
}
