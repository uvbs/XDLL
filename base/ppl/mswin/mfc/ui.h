
#pragma once

#include <assert.h>

#include <afxres.h>
#include <ppl/mswin/mfc/string.h>


#ifndef LVS_EX_LABELTIP
#define LVS_EX_LABELTIP	0x00004000
#endif






class ListCtrl
{
public:
	static void SetItemState(CListCtrl& listctrl, int index, DWORD state)
	{
		listctrl.SetItemState(index, state, state);
	}
	static void SelectItem(CListCtrl& listctrl, int index)
	{
		SetItemState(listctrl, index, LVIS_SELECTED | LVIS_FOCUSED);
	}

	static void ModifyExtendedStyle(CListCtrl& listctrl, DWORD add, DWORD remove)
	{
		DWORD style = listctrl.GetExtendedStyle();
		style |= add;
		style &= ~remove;
		listctrl.SetExtendedStyle(style);
	}
	static void EnableLabelTip(CListCtrl& listctrl)
	{
		ModifyExtendedStyle(listctrl, LVS_EX_LABELTIP, 0);
	}
	static void EnableFullRowSelect(CListCtrl& listctrl)
	{
		ModifyExtendedStyle(listctrl, LVS_EX_FULLROWSELECT, 0);
	}
};



class WindowUpdating
{
public:
	explicit WindowUpdating(CWnd& wnd) : m_wnd(wnd)
	{
		m_wnd.SetRedraw(FALSE);
	}
	~WindowUpdating()
	{
		m_wnd.SetRedraw(TRUE);
		m_wnd.Invalidate();
		m_wnd.UpdateWindow();
	}
private:
	CWnd& m_wnd;
};





namespace ppl { namespace detail {


template <typename BaseT = CListCtrl>
class ListView : public BaseT
{
public:
	BOOL ModifyExtendedStyle(DWORD remove = 0, DWORD add = 0)
	{
		DWORD style = GetExtendedStyle() | add;
		SetExtendedStyle(style & (~remove));
		return TRUE;
	}
	void EnableFullRowSelect()
	{
		ModifyExtendedStyle(0, LVS_EX_FULLROWSELECT);
	}
	void EnableLabelTip()
	{
		ModifyExtendedStyle(0, LVS_EX_LABELTIP);
	}
	void EnableNormal()
	{
		ModifyExtendedStyle(0, LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
	}
	int AddColumn(LPCTSTR caption, int width, int format = LVCFMT_LEFT)
	{
		return InsertColumn(GetHeaderCtrl()->GetItemCount(), caption, format, width);
	}
	int AddItem(LPCTSTR text)
	{
		return InsertItem(GetItemCount(), text);
	}
	void SelectItem(int item, bool select = true)
	{
		UINT state = LVIS_SELECTED | LVIS_FOCUSED;
		SetItemState(item, select ? state : 0, state);
	}

	void DeleteAllColumns()
	{
		while (this->DeleteColumn(0))
		{
		}
	}

	CString GetColumnName(int columnIndex)
	{
		CString name;
		LVCOLUMN col = { 0 };
		col.mask = LVCF_TEXT;
		TCHAR buf[1024+1] = { 0 };
		col.pszText = buf;
		col.cchTextMax = 1024;
		if (GetColumn(columnIndex, &col))
		{
			name = buf;
		}
		return name;
	}
};

}
}




template <typename TextBoxT>
inline void AppendText(TextBoxT& textBox, LPCTSTR msg, bool lineFeed = true)
{
	int len = textBox.GetWindowTextLength();
	if (len >= 64 * 1024)
	{
		textBox.SetWindowText(NULL);
		len = 0;
	}
	textBox.SetSel(len, len);
	textBox.ReplaceSel(msg);
	len = textBox.GetWindowTextLength();
	textBox.SetSel(len, len);
	textBox.ReplaceSel(TEXT("\r\n"));
	len = textBox.GetWindowTextLength();
	textBox.SetSel(len, len);
}


#define PPL_IMPL_SHOWMSG(txtMsg)	\
	void ShowMsg(LPCTSTR msg, bool lineFeed = true)\
	{\
		if (::IsWindow(txtMsg.m_hWnd)) {\
			AppendText(txtMsg, msg, lineFeed);\
		}\
	}




inline bool CopyToClipboard(LPCTSTR str, HWND hwnd)
{
	assert(str != NULL);
	size_t len = ::lstrlen(str);
	assert(len < USHRT_MAX);
	//put your text in source
	if (!::OpenClipboard(hwnd))
		return false;
	::EmptyClipboard();
	HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE | GMEM_ZEROINIT, len+1);
	if (clipbuffer == NULL)
	{
		assert(false);
		return false;
	}
	void* buffer = GlobalLock(clipbuffer);
	if (buffer == NULL)
	{
		::GlobalFree(clipbuffer);
		assert(false);
		return false;
	}
	memcpy(buffer, str, len);
	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_TEXT, clipbuffer);
	CloseClipboard();
	return true;
}




inline CString ShowOpenFileDialog(LPCTSTR filter = NULL)
{
	CString filename;
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);

	TCHAR szFile[MAX_PATH] = {0};
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;

	ofn.lpstrInitialDir = NULL;

	ofn.hwndOwner = NULL;

	// Sets the filter string replacing pipes with nulls
	TCHAR szFilter[MAX_PATH];
	lstrcpy(szFilter, filter);
	LPTSTR psz = szFilter;
	int i=0;
	while(psz[i] != 0) {
		if (psz[i]=='|')
			psz[i]=0;
		i++;
	}
	ofn.lpstrFilter = szFilter;

	if (!::GetOpenFileName(&ofn))
		return filename;

	filename = ofn.lpstrFile;
	return filename;
}





inline void DDX_Text(CDataExchange* pDX, int nIDC, UINT64& value)
{
	if (pDX->m_bSaveAndValidate)
	{
		CString str;
		DDX_Text(pDX, nIDC, str);
		_tscanf(str, _T("%I64u"), &value);
	}
	else
	{
		CString str = FormatUINT64(value);
		DDX_Text(pDX, nIDC, str);
	}
}

inline void DDX_Text(CDataExchange* pDX, int nIDC, UINT16& value)
{
	if (pDX->m_bSaveAndValidate)
	{
		CString str;
		DDX_Text(pDX, nIDC, str);
		_tscanf(str, _T("%hu"), &value);
	}
	else
	{
		CString str = FormatInteger(value, _T("%hu"));
		DDX_Text(pDX, nIDC, str);
	}
}


