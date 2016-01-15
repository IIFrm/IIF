#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
using namespace std;


class Config {
	public:
		string key;
		string value;
		string cppstatement;

		friend std::ostream& operator << (std::ostream& out, const Config& c) {
			out << c.key << " = " << c.value;
			return out;
		}

		bool toCppStatement() {
			//cout << "processing <" + key + ", " + value + ">......\n";
			if (key == "precondition") { cppstatement = "iif_assume(" + value + ");";
			} else if (key == "beforeloop") { cppstatement = value;
			} else if (key == "loop") { cppstatement = value;
			} else if (key == "loopcondition") { cppstatement = "while(" + value + ")";
			} else if (key == "loop") { cppstatement = value;
			} else if (key == "postcondition") { 
				cppstatement = "iif_assert(" + value + ");\n"; 
				cppstatement += "return 0;\n}";
			}
			return true;
		}
};


class FileHelper {
	public:
		FileHelper(const char* cfgfilename, const char* cppfilename, const char* varfilename, const char* invfilename) {
			this->cfgfilename = cfgfilename;
			this->cppfilename = cppfilename;
			this->varfilename = varfilename;
			this->invfilename = invfilename;
			confignum = 7;
			cs = new Config[confignum];
			cs[0].key = "names";
			cs[1].key = "precondition";
			cs[2].key = "beforeloop";
			cs[3].key = "loopcondition";
			cs[4].key = "loop";
			cs[5].key = "postcondition";
			cs[6].key = "afterloop";
			/*
			cs[0].key = "num";
			cs[1].key = "names";
			cs[2].key = "precondition";
			cs[3].key = "beforeloop";
			cs[4].key = "loopcondition";
			cs[5].key = "loop";
			cs[6].key = "postcondition";
			cs[7].key = "afterloop";
			*/
			//variables = NULL;
			vnum = 0;
		}

		~FileHelper() {
			if (cs != NULL)
				delete []cs;
			variables.clear();
		}

		bool readConfigFile() {
			ifstream cfgFile(cfgfilename);
			if(!cfgFile.is_open()) {
				cout<<"can not open cfg file!"<<endl;
				return false;
			}
			string line;
			int lastidx = -1;
			while(getline(cfgFile, line)) {
				size_t pos = line.find('=');
				/*if(pos == string::npos) {
					if (lastidx >= 0) 
						cs[lastidx].value += "\n" + line;
					continue;
				}
				*/
				string key = line.substr(0,pos);
				bool get_record = false;
				for (int i = 0; i < confignum; i++) {
					if(cs[i].key == key) {
						get_record = true;
						cs[i].value += line.substr(pos+1);
						lastidx = i;
						break;
					}
				}
				if (get_record == true)
					continue;
				else 
					cs[lastidx].value += "\n" + line;
			}

			cfgFile.close();

			//cout << cs[0] << endl;
			//cs[0].value >> vnum;
			size_t start = 0;
			size_t end = cs[0].value.find(' ');
			while (end == start) {
				end = cs[0].value.find(' ', end+1);
				start++;
			}
			for (int i = 0; end != std::string::npos; i++) {
				variables.push_back(cs[0].value.substr(start, end-start));
				start = end + 1;
				end = cs[0].value.find(' ', start);
			}
			variables.push_back(cs[0].value.substr(start, end-start));
			vnum = variables.size();


			/*for (int i = 0; i < vnum; i++)
				cout << "var[" << i << "] = " << variables[i] << endl;
			
			std::istringstream ss(cs[0].value);
			ss >> vnum;
			variables = new string[vnum];
			size_t start = 0;
			size_t end = cs[1].value.find(' ');
			while (end == start) {
				end = cs[1].value.find(' ', end+1);
				start++;
			}
			for (int i = 0; i < vnum; i++) {
				variables[i] = cs[1].value.substr(start, end-start);
				start = end + 1;
				end = cs[1].value.find(' ', start);
			}
			*/
			return true;
		}

		bool writeCppFile() {
			for (int i = 0; i < confignum; i++)
				cs[i].toCppStatement();
			ofstream cppFile(cppfilename);
			if( !cppFile.is_open()) {
				cout<<"can not open cpp file!"<<endl;
				return false;
			}
			writeCppHeader(cppFile);
			writeCppLoopFunction(cppFile);
			cppFile << endl;
			writeCppMain(cppFile);
			cppFile.close();
			return true;
		}

		inline int getVnum() {
			return vnum;
		}

		bool writeVarFile() {
			ofstream varFile(varfilename);
			if(!varFile.is_open()) {
				cout<<"can not open log file!"<<endl;
				return false;
			}
			varFile << vnum << endl;
			for (int i = 0; i< vnum; i++)
				varFile << variables[i] << endl;
			varFile.close();
			return true;
		}

		/*
		bool writeInvFile() {
			ofstream invFile(invfilename);
			if(!invFile.is_open()) {
				cout<<"can not open cpp file!"<<endl;
				return false;
			}
			invFile << vnum << endl;
			for (int i = 0; i< vnum; i++)
				invFile << variables[i] << endl;
			invFile.close();
			return true;
		}*/

	private:
		inline bool writeRecordi(ofstream& cppFile) {
			cppFile << "recordi(" << variables[0];
			for (int i = 1; i < vnum; i++)
				cppFile << ", " << variables[i];
			cppFile << ");\n";
			return true;
		}

		inline bool writeCppHeader(ofstream& cppFile) {
			cppFile << "#include \"iif.h\"\n"
				<< "#include <iostream>\n" 
				<< "using namespace iif;\n"<< endl;
			return true;
		}

		bool writeCppLoopFunction(ofstream& cppFile) {
			cppFile <<"int loopFunction(int a[]) {\n";
			for (int i = 0; i < vnum; i++) {
				cppFile << "int " << variables[i] << " = a[" << i << "];\n";
				//cppFile << "int " + variables[i] + " = a[" + to_string(i) + "];\n";
			}
			for (int i = 0; i < confignum; i++) {
				if (cs[i].key == "loop") { cppFile << "{\n"; writeRecordi(cppFile); }
				cppFile << cs[i].cppstatement << endl;
				if (cs[i].key == "loop") { cppFile << "}\n"; writeRecordi(cppFile); }
			}
			return true;
		}

		inline bool writeCppMain(ofstream& cppFile) {
			cppFile << "int main(int argc, char** argv)\n {\n" 
				<< "iifContext context(\"../" << varfilename <<"\", loopFunction, \"loopFunction\");\n"
				<< "context.addLearner(\"linear\").addLearner(\"conjunctive\");\n"
				<< "return context.learn(\"../" << invfilename << "\");\n}" << endl;
			return true;
		}

	private:
		const char* cfgfilename;
		const char* cppfilename;
		const char* varfilename;
		const char* invfilename;
		Config* cs;
		int confignum;
		vector<string> variables;
		int vnum;
};


int main(int argc, char** argv) 
{
	const char* cfgfilename = "inputcfg.cfg";
	const char* cppfilename = "inputcfg.cpp";
	const char* varfilename = "inputcfg.var";
	const char* invfilename = "inputcfg.inv";
	if (argc >= 2) cfgfilename = argv[1];
	if (argc >= 3) cppfilename = argv[2];
	if (argc >= 4) varfilename = argv[3];
	if (argc >= 5) invfilename = argv[4];
	FileHelper fh(cfgfilename, cppfilename, varfilename, invfilename);
	//cout << "after construct...\n";
	fh.readConfigFile();
	//cout << "after read config file...\n";
	fh.writeCppFile();
	//cout << "after write cpp file...\n";
	fh.writeVarFile();
	//cout << "after write var file...\n";
	//fh.writeInvFile();
	return fh.getVnum();
}
