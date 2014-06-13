
#pragma once

#include <ppl/util/macro.h>
#include <boost/noncopyable.hpp>


#ifndef HWND_MESSAGE
#define HWND_MESSAGE     ((HWND)-3)
#endif


class Window : private boost::noncopyable
{
public:
	static void RunLoop()
	{
		MSG          msg ;
		while ( GetMessage( &msg, NULL, 0, 0 ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	Window() : m_hwnd(NULL)
	{
	}
	~Window()
	{
		if ( this->IsValid() )
		{
			assert( this->IsWindow() );
			this->Close();
		}
	}

	HWND GetHandle() const { return m_hwnd; }

	bool IsValid() const
	{
		return m_hwnd != NULL;
	}

	bool IsWindow() const
	{
		return FALSE != ::IsWindow( m_hwnd );
	}

	bool IsValidWindow() const
	{
		return this->IsValid() && this->IsWindow();
	}

	bool Destroy() const
	{
		assert( IsValidWindow() );
		return FALSE != ::DestroyWindow( m_hwnd );
	}

	bool Close()
	{
		if ( false == this->IsValid() )
			return true;
		assert( this->IsWindow() );
		bool ret = ( FALSE != ::CloseWindow( m_hwnd ) );
		this->m_hwnd = NULL;
		return ret;
	}

	bool Create(WNDPROC wndProc, LPCTSTR className, LPCTSTR windowCaption, HWND hwndParent = NULL, HINSTANCE hInstance = NULL)
	{
		HINSTANCE dllInstance = GetModuleHandle(NULL);
		assert( NULL == m_hwnd );
		assert( wndProc != NULL );

		int iCmdShow = SW_HIDE;
		HWND         hwnd ;
		WNDCLASSEX     wndclass ;
		FILL_ZERO(wndclass);
		wndclass.cbSize = sizeof(wndclass);

		wndclass.style         = CS_CLASSDC;
		wndclass.lpfnWndProc   = wndProc;
		wndclass.cbClsExtra    = 0 ;
		wndclass.cbWndExtra    = 0 ;
		wndclass.hInstance     = dllInstance;
		wndclass.hIcon         = NULL;
		wndclass.hCursor       = NULL;
		wndclass.hbrBackground = NULL;
		wndclass.lpszMenuName  = NULL;
		wndclass.lpszClassName = className;

		if (!RegisterClassEx(&wndclass))
		{
			return false;
		}
		hwnd = ::CreateWindow(
			className,      // window class name
			windowCaption,	// window caption
			0,				// window style
			0,              // initial x position
			0,              // initial y position
			0,              // initial x size
			0,              // initial y size
			hwndParent,     // parent window handle
			NULL,           // window menu handle
			dllInstance,      // program instance handle
			NULL) ;         // creation parameters

		assert( hwnd != NULL );

		if ( NULL == hwnd )
			return false;

		//ShowWindow(hwnd, iCmdShow) ;
		//UpdateWindow(hwnd);
		m_hwnd = hwnd;
		return true;
	}

	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		assert( IsValid() && IsWindow() );
		return ::DefWindowProc( m_hwnd, message, wParam, lParam );
	}


private:
	HWND m_hwnd;
};





