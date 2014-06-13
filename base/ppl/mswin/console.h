
#pragma once




class Console : private noncopyable
{
public:
	static inline bool InitConsole()
	{
		TCHAR buf[257];
		if (GetConsoleTitle(buf, 256) > 0)
			return true;
		if (!AllocConsole())
			return false;
		freopen("CONOUT$", "w+t", stdout);
		freopen("CONOUT$", "w+t", stderr);
		//freopen("CONIN$", "r+t", stdin);
		return true;
	}


	Console()
	{
		InitConsole();
	}
	~Console()
	{
		::FreeConsole();
	}
};




inline BOOL __stdcall AntiAbortingCtrlHandler(DWORD fdwCtrlType)
{
	BOOL res = FALSE;
	switch (fdwCtrlType)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
		if (IDOK == MessageBox(NULL, _T("Do you want to close me ? I refuse! \n Please use \'x\' to exit."),
			_T("ALERT"), MB_OKCANCEL | MB_ICONWARNING | MB_SETFOREGROUND))
		{
			res = TRUE;
		}
		break;
	}
	return res;
}

inline bool InstallAntiAbortingCtrlHandler()
{
	return FALSE != ::SetConsoleCtrlHandler(AntiAbortingCtrlHandler, TRUE);
}

