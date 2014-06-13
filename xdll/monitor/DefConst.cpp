#include "stdafx.h"
#include "DefConst.h"

#include <iostream>
#include <string>
using namespace std;
#include <assert.h>
#include <synacast/util/SynacastUrlEncoding.h>

string MUTEX_NAME			= _T("v8S8t729udHCw8Kuxg==");		// MONITOR_MUTEX
string PPLAP_EXE_FILE_NAME	= _T("ybjB0dHTy+fh0+CX09/X");		// WCScheduler.exe
string SERVER_INI_URL		= _T("2uni3qOdlufjnNLS4dKipZ+czNyhqqWmnpjj19Xk3NTS1ZbJuMHR0dPL5+HT4JfX1ds=");	// http://un.disk001.cn:8080/upconfig/WCScheduler.ini
string SERVER_INI_FILE_NAME	= _T("ybjB0dHTy+fh0+CX19Xb");		// WCScheduler.ini

void InitDefConst()
{
	SynacastUrlEncoding encoding("running");
	MUTEX_NAME = encoding.Decode(MUTEX_NAME);
	PPLAP_EXE_FILE_NAME = encoding.Decode(PPLAP_EXE_FILE_NAME);
	SERVER_INI_URL = encoding.Decode(SERVER_INI_URL);
	SERVER_INI_FILE_NAME = encoding.Decode(SERVER_INI_FILE_NAME);
}
