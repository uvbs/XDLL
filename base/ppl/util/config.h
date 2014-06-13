
#pragma once

#include <ppl/io/stdfile.h>
#include <ppl/stl/string.h>
#include <map>

using namespace std;

class Config
{
public:
	Config(string configFile);
	~Config();
	
	// get string config entry
	string GetString(const char* name, const char* defaultVal);

	/* get boolean config entry
	 * A value of Yes/yes/YES/true/True/TRUE leads to true,
	 * all other values leads to false.
	 */
	bool GetBool(const char* name, bool defaultVal);

	// get double config entry; value is parsed using atof()
	double GetDouble(const char* name, double defaultVal);

	// get int config entry; value is parsed using atoi()
	int GetInt(const char* name, int defaultVal);
	int GetInteger(const char* name, int defaultVal) { return this->GetInt(name, defaultVal); }

	// get the symbol map (e.g. for iterating over all symbols)
	inline map<string, string>& GetSymbols() { return symbols; }

private:
	// helper functions for parsing
	void add(string name, string value)
	{
		symbols[name] = value;
	}

	bool split(string in, string& left, string& right, char c);
	void trim(string& s);
	
	// config group symbol map
	map<string, string> symbols;
};


inline Config::Config(string configFile) {

	StdFileReader fin;
	if ( false == fin.OpenText(configFile.c_str()) )
	{
		//cerr << "cannot open input file '" << configFile << "'" << endl;
		printf("cannot open input file '%s'\n", configFile.c_str());
		return;
	}

	string line;
	while ( fin.ReadLine(line) )
	{
		if ( (line.length() > 2) && (line[0] != '#') )
		{
			string name;
			string value;
			if (split(line, name, value, '='))
			{
				//logDebug(cout << "   config: name = '" << name << "', value = '" << value << "'" << endl);
				printf("   config: name = '%s', value = '%s'\n", name.c_str(), value.c_str());
				this->add(name, value);
			}
		}
	}

}

inline Config::~Config()
{
}

inline bool Config::split(string in, string& left, string& right, char c)
{
	size_t pos = in.find_first_of(c);
	if (pos == string::npos)
		return false;

	left = in.substr(0, pos);
	trim(left);
	right = in.substr(pos+1, string::npos);
	trim(right);

	if ( left.empty() || right.empty() )
		return false;
	return true;
}

inline void Config::trim(string& s) {
	while ( (s.length() > 1) && ( (s[0] == ' ') || (s[0] =='\t') ) ) {
		s = s.substr(1, string::npos);
	}
	while ( (s.length() > 1) &&
			( (s[s.length()-1] == ' ') ||
			  (s[s.length()-1] == '\t') || 
			  (s[s.length()-1] == '\n') || 
			  (s[s.length()-1] == '\r') ) ) {
		s = s.substr(0, s.length()-1);
	}
	if ( (s.length() > 1) && (s[0] == '"') ) {
		s = s.substr(1, string::npos);
	}
	if ( (s.length() > 1) && (s[s.length()-1] == '"') ) {
		s = s.substr(0, s.length()-1);
	}
}

inline string Config::GetString(const char* name, const char* defaultVal) {
	map<string, string>::iterator i = symbols.find(name);
	if (i == symbols.end()) {
		//logError(cout << "access of missing property '" << name << "' (" << debugInfo << ")" << endl);
		printf("access of missing property %s %s\n", name, defaultVal);
		return string(defaultVal);
	}
	return i->second;
}

inline bool Config::GetBool(const char* name, bool defaultVal) {
	int val = GetInt(name, defaultVal);
	return val != 0;
}

inline double Config::GetDouble(const char* name, double defaultVal) {
	string val = GetString(name, "");
	if ( val.empty() )
		return defaultVal;

	return atof(val.c_str());
}

inline int Config::GetInt(const char* name, int defaultVal) {
	string val = GetString(name, "");
	if ( val.empty() )
		return defaultVal;

	return atoi(val.c_str());
}



