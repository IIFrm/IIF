/*
 *  parameter: cfgfilepath 文件的绝对路径名如: /user/home/my.cfg
 *  key         文本中的变量名
 *  value       对应变量的值，用于保存
 *  
 */
#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <vector>
using namespace std;

enum category {NAME=0, BEFL, PREC, LOOPC, LOOP, POSTC, AFTL, INV};

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
			} else if (key == "afterloop") { cppstatement = value;
			}
			return true;
		}
};


class FileHelper {
	public:
		FileHelper(const char* cfgfname, const char* cppfname) {
			this->cfgfilename = cfgfname;

			int len = strlen(cppfname);
			cppfilename = new char[len + 10];
			strcpy(cppfilename, cppfname);
			cppfilename[len-2] = '\0';
			strcat(cppfilename, "_klee0.c");

			confignum = 8;
			cs = new Config[confignum];
			cs[0].key = "names";
			cs[1].key = "beforeloop";
			cs[2].key = "precondition";
			cs[3].key = "loopcondition";
			cs[4].key = "loop";
			cs[5].key = "postcondition";
			cs[6].key = "afterloop";
			cs[7].key = "invariant";
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
				if(pos == string::npos) {
					if (lastidx >= 0) 
						cs[lastidx].value += "\n" + line;
					continue;
				}
				string key = line.substr(0,pos);
				for (int i = 0; i < confignum; i++) {
					if(cs[i].key == key) {
						cs[i].value += line.substr(pos+1);
						lastidx = i;
						continue;
					}
				}
			}

			cfgFile.close();

			size_t start = 0;
			size_t end = cs[NAME].value.find(' ');
			while (end == start) {
				end = cs[NAME].value.find(' ', end+1);
				start++;
			}
			for (int i = 0; end != std::string::npos; i++) {
				variables.push_back(cs[NAME].value.substr(start, end-start));
				start = end + 1;
				end = cs[NAME].value.find(' ', start);
			}
			variables.push_back(cs[NAME].value.substr(start, end-start));
			vnum = variables.size();

			return true;
		}

		bool writeCFile() {

			//for (int i = 0; i < confignum; i++)
			//	std::cout << cs[i] << endl;
			int len = strlen(cppfilename);
			cppfilename[len-3] = '0';

			for (int choice = 1; choice <= 3; choice++) {
				cppfilename[len-3] += 1;
				//std::cout << "choice" << choice << " : " << cppfilename << std::endl;
				std::ofstream cppFile(cppfilename);
				if(!cppFile.is_open()) {
					cout<<"can not open cpp file!"<<endl;
					return false;
				}
				writeHeader(cppFile);
				writeMain(cppFile, choice);
				cppFile.close();
			}
			return true;
		}

		inline int getVnum() {
			return vnum;
		}


	private:
		inline bool writeHeader(ofstream& cppFile) {
			cppFile << "#include <klee/klee.h>\n" << endl;
			return true;
		}

		inline bool writeMain(ofstream& cppFile, int choice) {
			cppFile << "int main()\n {\n"; 
			for (int i = 0; i < vnum; i++) 
				cppFile << "int " + variables[i] + ";\n";
			for (int i = 0; i < vnum; i++) 
				cppFile << "klee_make_symbolic(&" << variables[i] <<", sizeof(int), \"" << variables[i] << "\");\n";

			// before loop statements;
			cppFile << cs[BEFL].value << std::endl;

			switch (choice) {
				case 1:
					cppFile << "klee_assume(" << cs[PREC].value <<");\n"; 
					cppFile << "klee_Massert(" << cs[INV].value <<");\n"; 
					break;
				case 2:
					cppFile << "klee_assume(" << cs[LOOPC].value <<");\n"; 
					cppFile << "klee_assume(" << cs[INV].value <<");\n"; 
					cppFile << cs[LOOP].value << "\n";
					cppFile << "klee_Massert(" << cs[INV].value <<");\n"; 
					break;
				case 3:
					cppFile << "klee_assume(!(" << cs[LOOPC].value <<"));\n"; 
					cppFile << "klee_assume(" << cs[INV].value <<");\n"; 
					cppFile << "klee_Massert(" << cs[POSTC].value <<");\n"; 
					break;
			}
			cppFile << "return 0;\n}" << endl;
			return true;
		}

	private:
		const char* cfgfilename;
		char* cppfilename;
		Config* cs;
		int confignum;
		vector<string> variables;
		int vnum;
};


int main(int argc, char** argv) 
{
	const char* cfgfilename = "inputcfg.cfg";
	const char* cppfilename = "inputcfg.c";
	if (argc >= 2) cfgfilename = argv[1];
	if (argc >= 3) cppfilename = argv[2];
	FileHelper fh(cfgfilename, cppfilename);
	fh.readConfigFile();
	fh.writeCFile();
	return fh.getVnum();
}
