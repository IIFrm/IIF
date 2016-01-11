/***************************************************************
 *  @file cfgReader.cpp
 *  @brief           
 *             
 *  @author Li Jiaying
 *  @bug no known bugs
 ***************************************************************/
/*
 *  parameter: cfgfilepath 文件的绝对路径名如: /user/home/my.cfg
 *  key         文本中的变量名
 *  value       对应变量的值，用于保存
 *  
 */
#include <string>
#include <sstream>

namespace patch
{
	template < typename T > std::string to_string( const T& n )
	{
		std::ostringstream stm ;
		stm << n ;
		return stm.str() ;
	}
}

#include <iostream>
#include <string>
#include <fstream>
using namespace std;
using namespace patch;


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
		FileHelper(const char* cfgfilename, const char* cppfilename, const char* logfilename) {
			this->cfgfilename = cfgfilename;
			this->cppfilename = cppfilename;
			this->logfilename = logfilename;
			confignum = 6;
			cs = new Config[confignum];
			cs[0].key = "num";
			cs[1].key = "names";
			cs[2].key = "precondition";
			cs[3].key = "loopcondition";
			cs[4].key = "loop";
			cs[5].key = "postcondition";
			variables = NULL;
			vnum = 0;
		}

		~FileHelper() {
			if (cs != NULL)
				delete []cs;
			if (variables != NULL)
				delete []variables;
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

			//cs[0].value >> vnum;
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

		bool writeLogFile() {
			ofstream logFile(logfilename);
			if( !logFile.is_open()) {
				cout<<"can not open cpp file!"<<endl;
				return false;
			}
			logFile << vnum << endl;
			for (int i = 0; i< vnum; i++)
				logFile << variables[i] << endl;
			logFile.close();
			return true;
		}

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
			for (int i = 0; i < vnum; i++) 
				cppFile << "int " + variables[i] + " = a[" + to_string(i) + "];\n";
			for (int i = 0; i < confignum; i++) {
				if (cs[i].key == "loop") { cppFile << "{\n"; writeRecordi(cppFile); }
				cppFile << cs[i].cppstatement << endl;
				if (cs[i].key == "loop") { cppFile << "}\n"; writeRecordi(cppFile); }
			}
			return true;
		}

		inline bool writeCppMain(ofstream& cppFile) {
			cppFile << "int main(int argc, char** argv)\n {\n" 
				<< "iifContext context(\"../" << logfilename <<"\", loopFunction, \"loopFunction\");\n"
				<< "context.addLearner(\"linear\").addLearner(\"conjunctive\");\n"
				<< "return context.learn();\n}" << endl;
			return true;
		}

	private:
		const char* cfgfilename;
		const char* cppfilename;
		const char* logfilename;
		Config* cs;
		int confignum;
		string* variables;
		int vnum;
};


int main(int argc, char** argv) 
{
	const char* cfgfilename = "inputcfg.cfg";
	const char* cppfilename = "inputcfg.cpp";
	const char* logfilename = "inputcfg.log";
	if (argc >= 2) cfgfilename = argv[1];
	if (argc >= 3) cppfilename = argv[2];
	if (argc >= 4) logfilename = argv[3];
	FileHelper fh(cfgfilename, cppfilename, logfilename);
	fh.readConfigFile();
	fh.writeCppFile();
	fh.writeLogFile();
	return fh.getVnum();
}
