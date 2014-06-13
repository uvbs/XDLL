// SynacastCode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
using namespace std;

#include <assert.h>
#include <synacast/util/SynacastUrlEncoding.h>

int main(int argc, char* argv[])
{
	SynacastUrlEncoding encoding("running");
	while(true)
	{
		char str[1024];
		cin.getline(str, 1024);
		if( str == "exit" )
			break;
		string str_encode = encoding.Encode(str);
		cout << str_encode << endl;
	}
	return 0;
}

