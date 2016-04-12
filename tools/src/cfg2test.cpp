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
		FileHelper(const char* cfgfilename, const char* cppfilename, const char* varfilename, const char* invfileprefix, const char* testcasefilename, const char* oldtracefilename) {
			this->cfgfilename = cfgfilename;
			this->cppfilename = cppfilename;
			this->varfilename = varfilename;
			this->invfileprefix = invfileprefix;
			this->testcasefilename = testcasefilename;
			this->oldtracefilename = oldtracefilename;
			confignum = 7;
			cs = new Config[confignum];
			cs[0].key = "names";
			cs[1].key = "beforeloop";
			cs[2].key = "precondition";
			cs[3].key = "loopcondition";
			cs[4].key = "loop";
			cs[5].key = "postcondition";
			cs[6].key = "afterloop";
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
			//cppFile << endl;
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

	private:
		inline bool writeRecordi(ofstream& cppFile) {
			cppFile << "recordi(" << variables[0];
			for (int i = 1; i < vnum; i++)
				cppFile << ", " << variables[i];
			cppFile << ");";
			return true;
		}

		inline bool writeCppHeader(ofstream& cppFile) {
			cppFile << "#include \"iif.h\"\n"
				<< "#include <iostream>\n" 
				<< "using namespace iif;\n\n";
			return true;
		}

		bool writeCppLoopFunction(ofstream& cppFile) {
			cppFile <<"int loopFunction(int a[]) {\n";
			for (int i = 0; i < vnum; i++) {
				cppFile << "int " << variables[i] << " = a[" << i << "];\n";
			}
			cppFile << "\n";
			for (int i = 0; i < confignum; i++) {
				if (cs[i].key == "loop") { 
					cppFile << "{\n"; 
					writeRecordi(cppFile); 
				}
				if (cs[i].cppstatement.compare("") != 0)
					cppFile << cs[i].cppstatement << endl;
				if (cs[i].key == "loop") { 
					cppFile << "}\n"; 
					writeRecordi(cppFile); 
					cppFile << "\n"; 
				}
			}
			return true;
		}

		inline bool writeCppMain(ofstream& cppFile) {
			cppFile << "\nint main(int argc, char** argv)\n {\n";
			if (oldtracefilename)
				cppFile << "iifContext context(\"../" << varfilename 
					<<"\", loopFunction, \"loopFunction\", \"../" << oldtracefilename << "\");\n";
			else
				cppFile << "iifContext context(\"../" << varfilename <<"\", loopFunction, \"loopFunction\");\n";

			if (testcasefilename) {
				cppFile << "context.addLearner(\"linear\", \"../" << testcasefilename << "\");\n";
				cppFile << "context.addLearner(\"conjunctive\", \"../" << testcasefilename << "\");\n";
			} else {
				cppFile << "context.addLearner(\"linear\");\n";
				cppFile << "context.addLearner(\"conjunctive\");\n";
			}

			cppFile << "return context.learn(\"../" << invfileprefix << "\");\n}" << endl;
			return true;

			/*cppFile << "int main(int argc, char** argv)\n {\n" 
				<< "iifContext context(\"../" << varfilename <<"\", loopFunction, \"loopFunction\");\n"
				<< "context.addLearner(\"poly\");\n"
				<< "context.addLearner(\"rbf\");\n"
				<< "context.addLearner(\"linear\").addLearner(\"rbf\");\n"
				<< "context.addLearner(\"linear\").addLearner(\"conjunctive\");\n"
				<< "return context.learn(\"../" << invfileprefix << "\");\n}" << endl;
				*/
		}

	private:
		const char* cfgfilename;
		const char* cppfilename;
		const char* varfilename;
		const char* invfileprefix;
		const char* testcasefilename;
		const char* oldtracefilename;
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
	const char* invfileprefix = "inputcfg";
	const char* testcasefilename = NULL;
	const char* oldtracefilename = NULL;
	if (argc >= 2) cfgfilename = argv[1];
	if (argc >= 3) cppfilename = argv[2];
	if (argc >= 4) varfilename = argv[3];
	if (argc >= 5) invfileprefix = argv[4];
	if (argc >= 6) testcasefilename = argv[5];
	if (argc >= 7) oldtracefilename = argv[6];
	//cout << "!!!" << testcasefilename << " !!!" << oldtracefilename << endl;

	FileHelper fh(cfgfilename, cppfilename, varfilename, invfileprefix, testcasefilename, oldtracefilename);
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
