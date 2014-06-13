
#pragma once

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>


class ShellFolders
{
public:
	static tstring CurrentUserHome()
	{
		struct passwd* userInfo = getpwuid(getuid());
		if ( NULL == userInfo )
			return tstring();
		return tstring( userInfo->pw_dir );
	}

};


