// iut.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include <stdlib.h>

//static int padding_data[100 * 1024] = { 'm', 'c', };

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	int timeval = atoi(lpCmdLine);
	if ( timeval <= 0 )
		return 0;
	::Sleep( timeval );
	return 0;
}



