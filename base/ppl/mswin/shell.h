
#pragma once

#include <shlobj.h>
#pragma comment(lib, "shell32.lib")


class ShellFolders
{
public:
	static tstring CommonAppDataDirectory()
	{
		TCHAR szPath[MAX_PATH * 4 + 1] = { 0 };
		if(SUCCEEDED(SHGetFolderPath(NULL, 
			CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE, 
			NULL, 
			0, 
			szPath))) 
		{
		}

		return tstring( szPath );
	}

};


