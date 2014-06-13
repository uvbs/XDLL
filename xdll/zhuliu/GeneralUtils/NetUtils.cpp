// 根据工程设置决定是否包含"stdafx.h"
#include "stdafx.h"
#include "NetUtils.h"
#include "StrUtils.h"
#include "RegAndProfile.h"
#include "MyLog.h"
#include <Winsock2.h>

#pragma comment(lib, "Ws2_32.lib")


unsigned char NetUtils::ToHex(unsigned char x) 
{ 
	return  x > 9 ? x + 55 : x + 48; 
}

unsigned char NetUtils::FromHex(unsigned char x) 
{ 
	unsigned char y;
	if( x >= 'A' && x <= 'Z' ) y = x - 'A' + 10;
	else if( x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if( x >= '0' && x <= '9') y = x - '0';
	else assert( 0 );
	return y;
}

string NetUtils::UrlEncoder(string str)
{
	string strTemp = "";
	size_t length = str.length();
	for( size_t i = 0; i < length; i ++ )
	{
		if( (str[i] >= '0' && str[i] <= '9') || (str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z') )
			strTemp += str[i];
		else if( str[i] == ' ' ) strTemp += "%20";
		else if( str[i] == '.') strTemp += '.';
		else if( str[i] == '/') strTemp += '/';
		else if( str[i] == '-') strTemp += '-';
		else if( str[i] == ':') strTemp += ':';
		else
		{
			strTemp += '%';
			strTemp += ToHex( (unsigned char)str[i] >> 4 );
			strTemp += ToHex( (unsigned char)str[i] % 16 );
		}
	}
	return strTemp;
}

string NetUtils::UrlDecoder(string str)
{
	string strTemp = "";
	size_t length = str.length();
	for( size_t i = 0; i < length; i ++ )
	{
		if( str[i] == '+') strTemp += ' ';
		else if( str[i] == '%')
		{
			assert( i + 2 < length );
			unsigned char hign = FromHex( (unsigned char)str[++i] );
			unsigned char low = FromHex( (unsigned char)str[++i] );
			strTemp += hign*16 + low;
		} else strTemp += str[i];
	}
	return strTemp;
}

bool NetUtils::UseHttpSendReqEx_Buf(HINTERNET hRequest, BYTE* buffer, DWORD dwBufferSize)
{
	INTERNET_BUFFERS BufferIn;
	DWORD dwBytesWritten = 0;
	BOOL bRet = TRUE;
	BufferIn.dwStructSize = sizeof(INTERNET_BUFFERS); // Must be set or error will occur
	BufferIn.Next = NULL; 
	BufferIn.lpcszHeader = NULL;
	BufferIn.dwHeadersLength = 0;
	BufferIn.dwHeadersTotal = 0;
	BufferIn.lpvBuffer = NULL;                
	BufferIn.dwBufferLength = 0;
	BufferIn.dwBufferTotal = dwBufferSize; // This is the only member used other than dwStructSize
	BufferIn.dwOffsetLow = 0;
	BufferIn.dwOffsetHigh = 0;

	if (!HttpSendRequestEx(hRequest, &BufferIn, NULL, 0, 0))
	{
		return false;
	}

	bRet = InternetWriteFile(hRequest, buffer, dwBufferSize, &dwBytesWritten);
	if (!bRet)
	{
		return false;
	}
	if (!HttpEndRequest(hRequest, NULL, 0, 0))
	{
		return false;
	}

	return true;
}

bool NetUtils::UseHttpSendReqEx_File(HINTERNET hRequest, DWORD dwPostSize, tstring strLocalFile)
{
	DWORD dwRead;
	BYTE* buffer;
	FILE* fLocal;
	
#ifdef _UNICODE
	string strLocalFile1 = StrUtils::WStrToStr(strLocalFile);
#else
	string strLocalFile1 = strLocalFile;
#endif

	if (!(fLocal=fopen(strLocalFile1.c_str(), "rb")))
	{
		return false;
	}
	fseek(fLocal, 0L, SEEK_END);
	dwRead = ftell(fLocal);
	rewind(fLocal);
	buffer = (BYTE*)malloc(dwRead);
	if (!buffer)
	{
		fclose(fLocal);
		free(buffer);
		return false;
	}
	dwRead = (DWORD)fread(buffer, 1, dwRead, fLocal);
	dwPostSize = dwRead;

	bool bRet = UseHttpSendReqEx_Buf(hRequest, buffer, dwPostSize);
	fclose(fLocal);
	free(buffer);
	return bRet;
}


// strServerIP    ->  _T("ppvacall.synacast.com")
// strServerPort  ->  _T("80")
// strRemoteFile  ->  _T("post/info_0.do")

// string& strRet ->  返回信息

// bUseFile       ->  文件还是buffer
// strLocalFile   ->  _T("c:\\log.txt")
// buf            ->  buffer地址
// dwBufSize      ->  buffer长度
bool NetUtils::Upload(tstring strServerIP, tstring strServerPort, tstring strRemoteFile, string& strRet,
					  bool bUseFile, tstring strLocalFile,  BYTE* buf, DWORD dwBufSize)
{
	DWORD dwPostSize = 0;
	int intServerPort = (int)StrUtils::GetINT64ByStr(strServerPort);
	HINTERNET hSession = InternetOpen(_T("HttpSendRequestEx"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (!hSession)
	{
		return false;
	}

	HINTERNET hConnect = InternetConnect(hSession, strServerIP.c_str(), intServerPort,
		NULL, NULL, INTERNET_SERVICE_HTTP, NULL, NULL);
	if (!hConnect)
	{
		return false;
	}
	else
	{
		HINTERNET hRequest = HttpOpenRequest(hConnect, _T("POST"), strRemoteFile.c_str(), 
			NULL, NULL, NULL, INTERNET_FLAG_NO_CACHE_WRITE, 0);
		if (!hRequest)
		{
			printf("Failed to open request handle\n");
			return false;
		}
		else
		{
			bool bSuccess = true;
			if (bUseFile)
			{
				bSuccess = UseHttpSendReqEx_File(hRequest, dwPostSize, strLocalFile);
			} 
			else
			{
				bSuccess = UseHttpSendReqEx_Buf(hRequest, buf, dwBufSize);
			}
			if (bSuccess)
			{    
				char retBuf[1024] = { 0 };
				DWORD dwBytesRead = 0;
				
				printf("\nThe following was returned by the server:\n");
				do
				{    
					dwBytesRead = 0;
					memset(retBuf, 0, 1024);
					if (InternetReadFile(hRequest, retBuf, 1024-1, &dwBytesRead))
					{
						strRet += string(retBuf);
						printf("%s", retBuf);
					}
					else
					{
						printf("\nInternetReadFile failed");
						break;
					}
				}
				while (dwBytesRead > 0);
				printf("\n");
			}
			else
			{
				return false;
			}
			if (!InternetCloseHandle(hRequest))
				printf("Failed to close Request handle\n");
		}
		if (!InternetCloseHandle(hConnect))
			printf("Failed to close Connect handle\n");
	}
	if (InternetCloseHandle(hSession) == FALSE)
	{
		printf("Failed to close Session handle\n");
	}

	return true;
}

BOOL NetUtils::OpenURL(LPCTSTR pszURL, HWND hWndActivate)
{
	tstring strProcess;
	while (true)
	{
		//尝试获取IE地址
		if (RegAndProfile::GetRegStr(_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\IEXPLORE.EXE"), _T(""), strProcess, 260, HKEY_LOCAL_MACHINE))
		{
			break;
		}
		//获取缺省浏览器
		if (RegAndProfile::GetRegStr(_T("http\\shell\\open\\command"), _T(""), strProcess, 260, HKEY_CLASSES_ROOT))
		{
			break;
		}

		tstring strKey;
		if (RegAndProfile::GetRegStr(_T(".htm"), _T(""), strProcess, 260, HKEY_CLASSES_ROOT))
		{
			strKey += _T("\\shell\\open\\command");
			if (RegAndProfile::GetRegStr(strKey, _T(""), strProcess, 260, HKEY_CLASSES_ROOT))
			{
				break;
			}
		}
		return FALSE;
	}

	BOOL dwOldStyleEx = 0, dwNewStyleEx = 0;
	if (hWndActivate != NULL)
	{
		hWndActivate = ::GetAncestor(hWndActivate, GA_ROOT);
	}
	if (hWndActivate != NULL)
	{
		dwOldStyleEx = ((DWORD)::GetWindowLong(hWndActivate, GWL_EXSTYLE));
		dwNewStyleEx = dwOldStyleEx;
		if (!(dwOldStyleEx&WS_EX_TOPMOST))
		{
			dwNewStyleEx |= WS_EX_TOPMOST;
			::SetWindowPos(hWndActivate, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
		}
	}
	if (strProcess.size() > 0)
	{
		if (strProcess.find(_T("%1")) == -1)
		{
			strProcess += _T(" ");
			strProcess += pszURL;
		}
		else
		{
			StrUtils::Replace(strProcess, _T("%1"), pszURL);
		}

		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		si.dwFlags     = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOW; 
		ZeroMemory(&pi, sizeof(pi));
		if (::CreateProcess(NULL, (LPTSTR)strProcess.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		{
			::WaitForInputIdle(pi.hProcess, 1000);
			if (hWndActivate != NULL && dwOldStyleEx != dwNewStyleEx)
			{
				::Sleep(1500);
				::SetWindowPos(hWndActivate, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
			}
			//			if (hWndActivate != NULL)
			//			{
			//				MySetForgroundWindow(hWndActivate);
			//			}
			::CloseHandle(pi.hProcess);
			::CloseHandle(pi.hThread);
			return TRUE;
		}
	}
	return FALSE;
}

string NetUtils::GetHostByName(string strName)
{
	WORD wVersionRequested;   
	WSADATA wsaData;   
	wVersionRequested = MAKEWORD(2, 0);

	string strRet = "";

	if (WSAStartup(wVersionRequested, &wsaData) == 0)   
	{   
		hostent* pHost;   

		pHost = gethostbyname(strName.c_str());     
		for (int i = 0; pHost != NULL && pHost->h_addr_list[i] != NULL; i++)   
		{   
			char str[100];   
			str[0] = '\0';   
			LPCSTR psz = inet_ntoa(*(in_addr*)pHost->h_addr_list[i]);   
			strRet = string(psz);
			break;
		}
		WSACleanup();
	}
	return strRet;
}

tstring NetUtils::FormatSeconds(int seconds)
{
	TCHAR szBuff[256] = { 0 };
	if (seconds < 0)
	{
		return tstring(TEXT("未知"));
	}
	else if (seconds < 60)
	{
		_stprintf(szBuff, TEXT("%-d秒"), seconds);
		return tstring(szBuff);
	}
	else if (seconds < 3600)
	{
		_stprintf(szBuff, TEXT("%-d分%d秒"), seconds / 60, seconds % 60);
		return tstring(szBuff);
	}
	else
	{
		_stprintf(szBuff, TEXT("%-d小时%d分"), seconds / 3600, (seconds % 3600) / 60);
		return tstring(szBuff);
	}
}

tstring NetUtils::FormatBytes(INT64 bytes)
{
	TCHAR szBuff[256] = { 0 };
	if (bytes < (1U<<10))
	{
		_stprintf(szBuff, TEXT("%-dB"), bytes);
		return tstring(szBuff);
	}
	else if (bytes < (1U<<20))
	{
		_stprintf(szBuff, TEXT("%3.1fKB"), bytes / ((1U<<10) + 0.0));
		return tstring(szBuff);
	}
	else if (bytes < (1U<<30))
	{
		_stprintf(szBuff, TEXT("%3.1fMB"), bytes / ((1U<<20) + 0.0));
		return tstring(szBuff);
	}
	else
	{
		_stprintf(szBuff, TEXT("%3.1fGB"), bytes / ((1U<<30) + 0.0));
		return tstring(szBuff);
	}
}

BOOL NetUtils::ParseUrl(const tstring& strUrl, tstring& strServer, tstring& strObject)
{
	if (strUrl.size() < 4)
	{
		return FALSE;
	}

	tstring url = strUrl;

	int n = StrUtils::IndexOfFirst(url, _T("://"));
	if (n > 0 && (int)url.size() > n + 3)
	{
		if ((int)url.size() > n + 3)
		{
			url = url.substr(n + 3);
		}
		else
		{
			return FALSE;
		}
	}

	int n2 = StrUtils::IndexOfFirst(url,_T("/"));
	if (n2 < 0)
	{
		strServer = url;
		strObject = _T("");
	}
	else if (n2 == 0)
	{
		return FALSE;
	}
	else if (n2 > 0)
	{
		if ((int)url.size() > n2 + 1)
		{
			strServer = url.substr(0, n2);
			strObject = url.substr(n2 + 1);
		}
		else
		{
			// url.size() == n2 + 1
			strServer = url.substr(0, n2);
			strObject = _T("");
		}
	}

	return TRUE;
}

BOOL NetUtils::DownloadFile_(tstring strUrl, tstring strProxy, tstring strFileFullPath, HWND hWnd)
{
	tstring strServer(_T(""));
	tstring strObject(_T(""));
	if (ParseUrl(strUrl, strServer, strObject))
	{
		if (DownloadFile(strServer, strObject, strProxy, strFileFullPath, hWnd))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		//LOG__(_T("Failed to ParseUrl. strUrl: %s"), strUrl.c_str());
		return false;
	}
}


//======================================================================================
// Function:   DownloadFile
// Date:       2008/11/26
// Purpose:    Download file to strFileFullPath.
// Parameters:
//		strServer -> "www.baidu.com:80"
//		strObject -> "/home.html"
//		strProxy  -> "127.0.0.1:9000"(or "")
//		strFile   -> "c:\\a.html"
// Return Value:	Returns TRUE if successful, or FALSE otherwise.
//======================================================================================
BOOL NetUtils::DownloadFile(tstring strServer, tstring strObject, tstring strProxy, tstring strFileFullPath, HWND hWnd)
{
	BOOL bSuccess = TRUE;

	HINTERNET hOpen, hConnect, hReq;
	DWORD  dwSize, dwCode;
	DWORD  dwFlags = 
		INTERNET_FLAG_RELOAD |
		INTERNET_FLAG_NO_CACHE_WRITE |
		INTERNET_FLAG_KEEP_CONNECTION;

	CHAR szData[DOWNLOAD_BUFFER_SIZE] = { 0 };

	HANDLE hFile = NULL;

	// specified by dwService
	INTERNET_PORT dwPort = INTERNET_INVALID_PORT_NUMBER;

	TCHAR szAccept[] = TEXT("*/*");

	LPTSTR AcceptTypes[2] = { 0 };
	AcceptTypes[0] = szAccept;

	try
	{
		if (strProxy.size() == 0)
		{
			// No proxy was specified
			if (!(hOpen = InternetOpen(TEXT("PPVAUPDATE"), LOCAL_INTERNET_ACCESS, NULL, 0, 0)))
			{
				throw(StrUtils::Format(_T("Failed to InternetOpen. GetLastError()==%u"), GetLastError()));
			}
		}
		else
		{
			// Use proxy server
			if (!(hOpen = InternetOpen(TEXT("PPVAUPDATE"),  CERN_PROXY_INTERNET_ACCESS, strProxy.c_str(), NULL, 0)))
			{
				throw(StrUtils::Format(_T("Failed to InternetOpen. GetLastError()==%u"), GetLastError()));
			}
		}

		if (!(hConnect = InternetConnect(hOpen, strServer.c_str(), dwPort, TEXT(""),  TEXT(""), INTERNET_SERVICE_HTTP, 0, 0)))
		{
			throw(StrUtils::Format(_T("Failed to InternetConnect. GetLastError()==%u"), GetLastError()));
		}

		if (!(hReq = HttpOpenRequest(hConnect, TEXT("GET"), strObject.c_str(), HTTP_VERSION, TEXT(""),
			(LPCTSTR*) AcceptTypes, dwFlags ,0)))
		{
			throw(StrUtils::Format(_T("Failed to HttpOpenRequest. GetLastError()==%u"), GetLastError()));
		}

		if (!HttpSendRequest(hReq, NULL, 0, NULL, 0))
		{
			throw(StrUtils::Format(_T("Failed to HttpSendRequest. GetLastError()==%u"), GetLastError()));
		}

		// head
		dwSize = sizeof(DWORD);
		if (!HttpQueryInfo(hReq, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwCode, &dwSize, NULL))
		{
			throw(StrUtils::Format(_T("Failed to HttpQueryInfo. GetLastError()==%u"), GetLastError()));
		}

		if (dwCode < 200 || dwCode > 299)
		{
			throw(StrUtils::Format(_T("HttpQueryInfo's dwCode==%u"), dwCode));
		}

		// content
		char QueryBuf[16] = { 0 };
		DWORD dwQueryBufLen = sizeof(QueryBuf);
		if (!HttpQueryInfo(hReq, HTTP_QUERY_CONTENT_LENGTH, QueryBuf, &dwQueryBufLen, NULL))
		{
			throw(StrUtils::Format(_T("Failed to HttpQueryInfo. GetLastError()==%u"), GetLastError()));
		} 
		DWORD dwContentLength = (DWORD)StrUtils::GetINT64ByStr(tstring((TCHAR*)QueryBuf));
		if (dwContentLength == 0) // error
		{
			throw(StrUtils::Format(_T("dwContentLength==%u\n"), dwContentLength));
		}

		if (::IsWindow(hWnd))
			::SendMessage(hWnd, UM_NOTIFY_FILELENGTH, (WPARAM)dwContentLength, NULL);

		// Open local file
		hFile = CreateFile(
			strFileFullPath.c_str(),
			GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			throw(StrUtils::Format(_T("Can't open file: %s. GetLastError()==%u"), strFileFullPath, GetLastError()));
		}


		DWORD dwLenToRead = 0;
		if (dwContentLength >= DOWNLOAD_BUFFER_SIZE - 1)
		{
			dwLenToRead = DOWNLOAD_BUFFER_SIZE - 1;
		}
		else
		{
			dwLenToRead = dwContentLength;
		}

		// download
		do
		{
			if (!InternetReadFile(hReq, (LPVOID)szData, dwLenToRead, &dwSize))
			{
				throw(StrUtils::Format(_T("Failed to InternetReadFile. GetLastError()==%u"), GetLastError()));
			}
			if (dwSize != 0)
			{
				DWORD dwWritten = 0;
				if (!WriteFile(hFile, szData, dwSize, &dwWritten, NULL))
				{
					throw(StrUtils::Format(_T("Failed to WriteFile. GetLastError()==%u"), GetLastError()));
				}

				if (::IsWindow(hWnd))
					::SendMessage(hWnd, UM_NOTIFY_SCALE_CHANGE, (WPARAM)dwSize, NULL);

				dwContentLength -= dwSize;

				if (dwContentLength == 0) break;

				if (dwContentLength >= DOWNLOAD_BUFFER_SIZE - 1)
				{
					dwLenToRead = DOWNLOAD_BUFFER_SIZE - 1;
				}
				else
				{
					dwLenToRead = dwContentLength;
				}
			}
		}
		while (dwSize);



		if (!InternetCloseHandle(hReq))
		{
			throw(StrUtils::Format(_T("Failed to CloseHandle on hReq. GetLastError()==%u"), GetLastError()));
		}
		if (!InternetCloseHandle(hConnect))
		{
			throw(StrUtils::Format(_T("Failed to CloseHandle on hConnect. GetLastError()==%u"), GetLastError()));
		}
		if (!InternetCloseHandle(hOpen))
		{
			throw(StrUtils::Format(_T("Failed to CloseHandle on hOpen. GetLastError()==%u"), GetLastError()));
		}
	}
	catch (tstring& s)
	{
//		LOG__(_T("%s\n"), e.GetInfo().c_str());
		bSuccess = FALSE;
	}

	if (hFile)
	{
		FlushFileBuffers(hFile);
		CloseHandle(hFile);
	}
	return bSuccess;
}