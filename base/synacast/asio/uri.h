#pragma once

#include <string>

const int NOPOS = -1;
class Uri
{
public:
	Uri(const string &url);

	Uri(const char *url);

	bool replacepath(string file_name);

	bool replacefile(string path_name);

	string getfileurl();

	string getrequest();

	string geturl()
	{
		return url_;
	}

	string getprotocol();

	string getdomain();


	string getport();

	string getpath();

	string getfile();

	string getparameter(const string& key);

	string getparameter();

	string gethost();

	string getreferer();

private:
	void parseurl();

private:
	string url_;
	size_t protcalpos_;
	size_t host_start_pos_;
	size_t host_end_pos_;
	size_t domain_end_pos_;
	size_t port_start_pos_;
	size_t port_end_pos_;
	size_t path_start_pos_;
	size_t path_end_pos_;
	size_t file_start_pos_;
	size_t file_end_pos_;
	size_t param_start_pos_;
	size_t param_end_pos_;
	size_t request_start_pos_;
	size_t request_end_pos_;
};
