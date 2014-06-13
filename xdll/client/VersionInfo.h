#include <vector>
#include <ppl/stl/strings.h>

class VersionInfo
{
public:
	WORD v0;
	WORD v1;
	WORD v2;
	WORD v3;
	
	VersionInfo() : v0(0), v1(0), v2(0), v3(0) {}
	
	bool operator < (const VersionInfo& v)
	{
		if(v0 != v.v0) 
			return v0 < v.v0;
		if(v1 != v.v1) 
			return v1 < v.v1;
		if(v2 != v.v2) 
			return v2 < v.v2;
		if(v3 != v.v3) 
			return v3 < v.v3;
		return false;
	}
	
	bool operator == (const VersionInfo& v)
	{
		return v0==v.v0 && v1==v.v1 && v2==v.v2 && v3==v.v3;
	}
	
	bool operator <= (const VersionInfo& v)
	{
		return (*this<v) || (*this==v);
	}
	
	bool operator > (const VersionInfo& v)
	{
		return !(*this<=v);
	}
	
	bool Parse(const std::string& line)
	{
		std::vector<string> items;
		strings::split( std::back_inserter( items ), line, '.' );
		if(items.size()>0) {
			v0 = atoi(items[0].c_str());
		} 
		if(items.size()>1) {
			v1 = atoi(items[1].c_str());
		}
		if(items.size()>2) {
			v2 = atoi(items[2].c_str());
		}
		if(items.size()>3) {
			v3 = atoi(items[3].c_str());
		}
		return true;
	}
};