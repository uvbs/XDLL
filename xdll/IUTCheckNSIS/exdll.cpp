#include <windows.h>
#include <nsis/pluginapi.h> // nsis plugin

#include <ppl/stl/strings.h>
#include <ppl/mswin/file.h>
#include <ppl/mswin/ipc.h>

HINSTANCE g_hInstance;

HWND g_hwndParent;


bool CheckIUT()
{
	bool iutExists = false;
	string iutName = "iresea";
	for ( ProcessWalker walker; walker.HasMore(); walker.MoveNext() )
	{
		const PROCESSENTRY32& info = walker.Current();
		string name = info.szExeFile;
		strings::make_lower( name );
		if ( name.find( iutName) != string::npos )
		{
			// 找到iut进程，还需要检查pplive进程是否已经在运行
			iutExists = true;
			break;
		}
	}
	// pplive没有启动，如果iut存在，则需要启动
	return iutExists;
}


extern "C" void __declspec(dllexport) Check(HWND hwndParent, int string_size, 
                                      char *variables, stack_t **stacktop,
                                      extra_parameters *extra)
{
	  g_hwndParent=hwndParent;

	  EXDLL_INIT();

	  // note if you want parameters from the stack, pop them off in order.
	  // i.e. if you are called via exdll::myFunction file.dat poop.dat
	  // calling popstring() the first time would give you file.dat,
	  // and the second time would give you poop.dat. 
	  // you should empty the stack of your parameters, and ONLY your
	  // parameters.

	  if( CheckIUT() == true)
		  pushint(1);
	  else
		  pushint(0);

	  // do your stuff here
	  //{
	  //  char buf[1024];
	  //  wsprintf(buf,"$0=%s\n",getuservariable(INST_0));
	  //  MessageBox(g_hwndParent,buf,0,MB_OK);
	  //}


}





BOOL WINAPI DllMain(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
	g_hInstance=(HINSTANCE)hInst;
	return TRUE;
}
