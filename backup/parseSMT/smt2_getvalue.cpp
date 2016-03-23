#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
using namespace std;

int Nv;
vector<string> variables;
vector<string> get_variable_statements;

static bool readVar(const char* ifname)
{
	ifstream inf(ifname);
	inf >> Nv;
	string str;
	while (inf >> str)
		variables.push_back(str);
	inf.close();
	return true;
}

static bool prepare_get_variable_values() {
	ostringstream stm;
	for (int i = 0; i < Nv; i++) {
		for (int j = 0; j < 4; j++) {
			stm.str("");
			stm << "(get-value ((select " << variables[i] <<" (_ bv" << j << " 32))))";
			get_variable_statements.push_back(stm.str());
		}
	}
	return true;
}

static bool add_statements(const char* ifname, const char* outfname) {
	ifstream inf(ifname);
	ofstream outf(outfname);
	string str;
	while (getline(inf, str, '\n')) {
		if (str.compare("(exit)") != 0) {
			outf << str << endl;
			continue;
		}
		for (int i = 0; i < Nv * 4; i++) {
			outf << get_variable_statements[i] << endl;
		}
		outf << str << endl;
	}
	inf.close();
	outf.close();
	return true;
}


int main(int argc, char** argv) 
{
	if (argc < 4) return -1;
	readVar(argv[1]);
	prepare_get_variable_values();
	add_statements(argv[2], argv[3]);
	return 0;
}
