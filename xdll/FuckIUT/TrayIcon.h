#pragma once

#include "TrayIcon/SS_TrayIcon.h"

class TrayIcon : public SS_TrayIcon
{
public:
	TrayIcon(HINSTANCE hInstance, INT nMaxIconSets=1, INT nMaxIconsPerSet=1, HWND hWnd=NULL)
		:SS_TrayIcon(hInstance,nMaxIconSets,nMaxIconsPerSet,hWnd)
	{
		
	}
	~TrayIcon(){}
public:
	static LRESULT CALLBACK OnMouseClickTI(WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK OnMouseDblClickTI(WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK OnMouseRightClickTI(WPARAM wParam, LPARAM lParam);
};