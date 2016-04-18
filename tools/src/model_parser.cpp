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

class Model_Var{
	public:
		string name;
		int value;
		friend ostream& operator << (ostream& out, const Model_Var& mv) {
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
		inf >> model[i].name;
	inf.close();
	return true;
}

bool parse_item(string str) {
	int start = 0;
	int end = str.find("[", start);
	char name[32];
	int value;
	while (end <= str.length()) {
		start = end + 1;
		/*end = str.find("=", start);
		string name = str.substr(start, end - start);
		int index = 0;
		while (index < Nv) {
			if (name != model[index].name)
				index++;
			else
				break;
		}
		if (index >= Nv)
			return false;
		start = end + 2;
		model[index].value = stoi(str.substr(start), nullptr, 10);
		*/
		end = str.find(",", start);
		if (end == string::npos)
			end = str.find("]", start);
		if (end == string::npos)
			return false;
			
		sscanf(str.substr(start, end-start).c_str(), "%s = %d", name, &value);
		//cout << name << " = " << value << endl;
		string vname(name);
		int index = 0;
		while (index < Nv) {
			if (name != model[index].name)
				index++;
			else
				break;
		}
		if (index >= Nv)
			return false;
		model[index].value = value;
	}

	return true;
}


int main(int argc, char** argv) {
	assert ((argc < 3) || "# of arguments is less than 3.");
	parse_var_file(argv[1]);
	ifstream modelfile(argv[2]);

	string strline;
	string fun_name;
	getline(modelfile, strline, '\n');
	if (strline.find("unsat") != string::npos) return 0;
	while (getline(modelfile, strline, '\n')) {
		cout << strline << endl;
		parse_item(strline);
	}
	modelfile.close();

	//cout << "-->model:\n";
	if (argc > 3)
		save_to_file(argv[3]);

	//cout << *model << endl;

	return 1;
}
