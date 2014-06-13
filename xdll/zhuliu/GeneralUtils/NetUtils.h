#ifndef _NET_UTILS_H_
#define _NET_UTILS_H_

#define WIN32_LEAN_AND_MEAN 

#include <windows.h>
#include <WinInet.h>
#include <string>
#pragma comment(lib, "WinInet.lib")

using namespace std;

#ifndef tstring
#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif
#endif

#define DOWNLOAD_BUFFER_SIZE	1024

#define UM_NOTIFY_FILELENGTH	WM_APP + 1
#define UM_NOTIFY_SCALE_CHANGE	WM_APP + 2

class NetUtils
{
public:
	static string UrlEncoder(string str);
	static string UrlDecoder(string str);

	static string GetHostByName(string strName);

	static BOOL OpenURL(LPCTSTR pszURL, HWND hWndActivate);

	// 如果bUseFile为true, 则strLocalFile有效, buf和dwBufSize被忽略
	// 如果bUseFile为false, 则buf和dwBufSize有效, strLocalFile被忽略
	static bool Upload(tstring strServerIP, tstring strServerPort, tstring strRemoteFile, string& strRet,
		bool bUseFile, tstring strLocalFile, BYTE* buf = NULL, DWORD dwBufSize = 0);

	static tstring FormatSeconds(int seconds);
	static tstring FormatBytes(INT64 bytes);

	static BOOL ParseUrl(const tstring& strUrl, tstring& strServer, tstring& strObject);
	static BOOL DownloadFile_(tstring strUrl, tstring strProxy, tstring strFileFullPath, HWND hWnd = NULL);
	static BOOL DownloadFile(tstring strServer, tstring strObject, tstring strProxy, tstring strFileFullPath, HWND hWnd = NULL);

private:
	static bool UseHttpSendReqEx_Buf(HINTERNET hRequest, BYTE* buffer, DWORD dwBufferSize);
	static bool UseHttpSendReqEx_File(HINTERNET hRequest, DWORD dwPostSize, tstring strLocalFile);
	static unsigned char ToHex(unsigned char x);
	static unsigned char FromHex(unsigned char x); 
	NetUtils(void);
};

#endif
