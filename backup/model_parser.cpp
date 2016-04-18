/***************************************************************
 *  @file model_parser.cpp
 *  @brief           
 *             
 *  @author Li Jiaying
 *  @bug no known bugs
 ***************************************************************/
#include <iostream>
#include <fstream>
#include <cstring>
#include <cassert>
using namespace std;

const char* fun_def = "(define-fun ";
const int sz_fun_def = strlen(fun_def);
const char* arr_def = "(_ as-array ";
const int sz_arr_def = strlen(arr_def);
const char* ite_str = "(ite (= ";
const int sz_ite_str = strlen(ite_str);
const char* index_str = "#x0000000";
const int sz_index_str = strlen(index_str);
const char* byte_str = "#x";
const int sz_byte_str = strlen(byte_str);

class Model_Var{
	public:
		string fun_name;
		string arr_name;
		int value;
		friend ostream& operator << (ostream& out, const Model_Var& mv) {
			/*
			 * out << "[fun_name: " << mv.fun_name << ", arr_name: " << mv.arr_name 
				<< ", value: " << hex << mv.value << dec << "]";
				*/
			out << mv.value; 
			return out;
		}

};

Model_Var* model;
int Nv = 0;

bool save_to_file(const char* filename) {
	ofstream ofs(filename);
	for (int i = 0; i < Nv; i++) {
		ofs << model[i].value << "\t";
	}
	ofs << endl;
	ofs.close();
	return true;
}

bool parse_var_file(const char* varfile) {
	ifstream inf(varfile);
	inf >> Nv;
	model = new Model_Var[Nv];
	for (int i = 0; i < Nv; i++)
		inf >> model[i].fun_name;
	inf.close();
}

bool parse_ite_statement(string str, int nv = Nv) {
	int spos = str.find(ite_str);
	if (spos == string::npos) return false;
	spos = str.find(index_str, spos);
	if (spos == string::npos) return false;
	int index = str[spos + sz_index_str] - '0';
	spos = str.find(byte_str, spos + sz_index_str + 2);
	if (spos == string::npos) return false;
	char value = stoi(str.substr(spos + 2), nullptr, 16);
	((char*)(&model[nv].value))[index] = value;
	return true;
}


int main(int argc, char** argv)
{
	assert ((argc < 3) || "# of arguments is less than 3.");
	parse_var_file(argv[1]);
	ifstream modelfile(argv[2]);

	string strline;
	string fun_name;
	int spos, epos;
	int index;
	getline(modelfile, strline, '\n');
	if (strline.find("unsat") != string::npos) return 0;  
	while (getline(modelfile, strline, '\n')) {
		spos = strline.find(fun_def);
		if (spos != string::npos) {
			epos = strline.find(" (", spos + sz_fun_def);
			fun_name = strline.substr(spos + sz_fun_def, epos - spos - sz_fun_def);
			for (index = 0; index < Nv; index++) {
				if (model[index].fun_name.compare(fun_name) == 0) {
					break;
				}
				if (model[index].arr_name.compare(fun_name) == 0) {
					break;
				}
			}

			if (index >= Nv) continue;

			getline(modelfile, strline, '\n');
			spos = strline.find(arr_def);
			if (spos != string::npos) {
				epos = strline.find_first_of(" )", spos + sz_arr_def);
				model[index].arr_name = strline.substr(spos + sz_arr_def, epos - spos - sz_arr_def);
			} else {
				spos = strline.find(ite_str);
				if (spos == string::npos) continue;
				for (int j = 0; j < 4; j++) {
					if (parse_ite_statement(strline, index) == false)
						break;
					getline(modelfile, strline, '\n');
				}
			}
		}
	}
	modelfile.close();

	//cout << "-->model:\n";
	/*for (int i = 0; i < Nv; i++) {
		cout << model[i] << "\t";
	}
	*/
	if (argc > 3)
		save_to_file(argv[3]);

	return 1;
}
