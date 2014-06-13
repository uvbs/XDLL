#include <string>
using namespace std;

class UrlEditor
{
public:
	static string UrlEdit(const string& str);

private:
	static string GetCityCode();
	static string GetRndRamdom(const string& v);
};