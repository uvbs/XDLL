
#include "stdafx.h"

#include <synacast/asio/uri.h>



Uri::Uri( const string &url ) : url_(url),
protcalpos_(NOPOS),
host_start_pos_(NOPOS),
host_end_pos_(NOPOS),
port_start_pos_(NOPOS),
path_start_pos_(NOPOS),
file_start_pos_(NOPOS),
param_start_pos_(NOPOS),
param_end_pos_(NOPOS),
domain_end_pos_(NOPOS),
port_end_pos_(NOPOS),
file_end_pos_(NOPOS),
path_end_pos_(NOPOS),
request_start_pos_(NOPOS),
request_end_pos_(NOPOS)
{
	parseurl();
}

Uri::Uri( const char *url ) : url_(url),
protcalpos_(NOPOS),
host_start_pos_(NOPOS),
host_end_pos_(NOPOS),
port_start_pos_(NOPOS),
path_start_pos_(NOPOS),
file_start_pos_(NOPOS),
param_start_pos_(NOPOS),
param_end_pos_(NOPOS),
domain_end_pos_(NOPOS),
port_end_pos_(NOPOS),
file_end_pos_(NOPOS),
path_end_pos_(NOPOS),
request_start_pos_(NOPOS),
request_end_pos_(NOPOS)
{
	parseurl();
}

bool Uri::replacepath( string file_name )
{
	url_ = getreferer() + file_name;
	return true;
}

bool Uri::replacefile( string path_name )
{
	if (file_start_pos_ != NOPOS)
	{
		url_ = url_.substr(0, file_start_pos_) + path_name;

		return true;
	}
	else
		return false;
}

string Uri::getfileurl()
{
	if (file_end_pos_ != NOPOS)
		return url_.substr(0, file_end_pos_);
	else
		return "";
}

string Uri::getrequest()
{
	if (request_start_pos_ != NOPOS)
		return url_.substr(request_start_pos_, request_end_pos_ - request_start_pos_);
	else
		return "";
}

string Uri::getprotocol()
{
	if(protcalpos_ != NOPOS)
		return url_.substr(0, protcalpos_);
	else
		return "http";
}

string Uri::getport()
{
	if(port_start_pos_ != NOPOS)
		return url_.substr(port_start_pos_, port_end_pos_ - port_start_pos_);
	else
		return "80";
}

string Uri::getpath()
{
	if(path_start_pos_ != NOPOS)
		return url_.substr(path_start_pos_, path_end_pos_ - path_start_pos_);
	else
		return "/";
}

string Uri::getfile()
{
	if(file_start_pos_ != NOPOS)
		return url_.substr(file_start_pos_, file_end_pos_ - file_start_pos_);
	else
		return "";
}

string Uri::getparameter( const string& key )
{
	string para = getparameter();
	size_t startp_, endp_;
	if((startp_ = para.find(key)) == string::npos)
		return "";
	else
	{
		startp_ = para.find('=', startp_) + 1;
		endp_ = para.find('&', startp_);
		if (endp_ == string::npos)
			endp_ = para.length();
		return para.substr(startp_, endp_ - startp_);
	}
}

string Uri::getparameter()
{
	if(param_start_pos_ != NOPOS)
		return url_.substr(param_start_pos_, param_end_pos_ - param_start_pos_);
	else
		return "";
}

string Uri::gethost()
{
	if (host_start_pos_ != NOPOS)
		return url_.substr(host_start_pos_, host_end_pos_ - host_start_pos_);
	else
		return "";
}

string Uri::getreferer()
{
	if (host_end_pos_ != NOPOS)
		return url_.substr(0, host_end_pos_);
	else
		return "";
}

void Uri::parseurl()
{
	string::size_type beginpos = 0, endpos = 0;

	beginpos = url_.find(':', 0);
	endpos = url_.find('/', 0);
	if(beginpos != string::npos && beginpos < endpos && beginpos < url_.find('.', 0))
	{
		protcalpos_ = beginpos;
		beginpos = url_.find('/', endpos + 1) + 1;
		host_start_pos_ = beginpos;
	}
	else
	{
		host_start_pos_ = 0;
		beginpos = host_start_pos_;
	}

	endpos = url_.find(':', beginpos);
	if(endpos != string::npos)
	{
		port_start_pos_ = endpos + 1;
		beginpos = endpos;
		domain_end_pos_ = endpos;
	}

	endpos = url_.find('/', beginpos);
	if(endpos != string::npos)
	{
		if (domain_end_pos_ == NOPOS)
			domain_end_pos_ = endpos;
		if (port_start_pos_ != NOPOS)
			port_end_pos_ = endpos;
		path_start_pos_ = endpos;
		request_start_pos_ = endpos;
		host_end_pos_ = endpos;
	}
	else
	{
		url_ = url_ + '/';
		endpos = url_.find('/', beginpos);
		path_start_pos_ = endpos;
		if (domain_end_pos_ == NOPOS)
			domain_end_pos_ = url_.length() - 1;
		if (port_start_pos_ != NOPOS)
			port_end_pos_ = endpos;
		request_start_pos_ = endpos;
		host_end_pos_ = endpos;
	}

	beginpos = endpos;

	//endpos = url_.find('.', beginpos);
	//if(endpos != string::npos)
	//{
	//	beginpos = url_.rfind('/', endpos) + 1;
	//	file_start_pos_ = beginpos;
	//}

	beginpos = url_.find('?', beginpos);
	if(beginpos != string::npos)
	{
		param_start_pos_ = beginpos + 1;
		file_end_pos_ = beginpos;
		path_end_pos_ = beginpos;

		file_start_pos_= url_.rfind('/', beginpos) + 1;

	} else
	{
		file_end_pos_ = url_.length();
		path_end_pos_ = url_.length();

		file_start_pos_= url_.rfind('/', url_.length()) + 1;
	}

	beginpos = url_.rfind('#', url_.length() - 1);
	if(beginpos != string::npos)
	{
		request_end_pos_ = beginpos;
		param_end_pos_ = beginpos;
	} else
	{
		request_end_pos_ = url_.length();
		param_end_pos_ = url_.length();
	}
}

string Uri::getdomain()
{
	if(host_start_pos_!= NOPOS)
		return url_.substr(host_start_pos_, domain_end_pos_- host_start_pos_);
	else
		return "";
}