#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows XP.
#define _WIN32_WINNT 0x0501     // Change this to the appropriate value to target other versions of Windows.
#endif

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

struct window_style
{
	DWORD	val;
	LPCSTR str;
};

window_style window_styles[] = {
	{WS_POPUP, "Pop"},
	{WS_CHILD, "Child"},
	{WS_DISABLED, "Disable"},
	{WS_MINIMIZE, "Mini"},
	{WS_MAXIMIZE, "Max"},
	{WS_BORDER, "Border"},
	{WS_CAPTION, "Caption"},
};

#define WS_OVERLAPPED       0x00000000L
#define WS_POPUP            0x80000000L
#define WS_CHILD            0x40000000L
#define WS_MINIMIZE         0x20000000L
#define WS_VISIBLE          0x10000000L
#define WS_DISABLED         0x08000000L
#define WS_CLIPSIBLINGS     0x04000000L
#define WS_CLIPCHILDREN     0x02000000L
#define WS_MAXIMIZE         0x01000000L
#define WS_CAPTION          0x00C00000L     /* WS_BORDER | WS_DLGFRAME  */
#define WS_BORDER           0x00800000L
#define WS_DLGFRAME         0x00400000L
#define WS_VSCROLL          0x00200000L
#define WS_HSCROLL          0x00100000L
#define WS_SYSMENU          0x00080000L
#define WS_THICKFRAME       0x00040000L
#define WS_GROUP            0x00020000L
#define WS_TABSTOP          0x00010000L



static BOOL CALLBACK enum_windows_region_proc(__in  HWND hwnd, __in  LPARAM lParam)
{
	TCHAR title_buf[256];
	GetWindowText(hwnd, title_buf, ARRAYSIZE(title_buf));
	if (_tcscmp(title_buf, TEXT("Default IME")) && _tcscmp(title_buf, TEXT("MSCTFIME UI")) && 0 != *title_buf)
	{
		_tprintf(TEXT("%p %s\r\n"), hwnd, title_buf);
	}
	return TRUE;
}

int _tmain(int argc, _TCHAR* argv[])
{
	HWND foreground_wnd = NULL;
	while(true)
	{
		HWND hwnd = NULL;
		if(0xFF00 & GetKeyState(VK_LCONTROL))
		{
			POINT pt;
			GetCursorPos(&pt);
			hwnd = WindowFromPoint(pt);
		}
		else
		{
			hwnd = GetForegroundWindow();
		}

		if (foreground_wnd != hwnd)
		{
			foreground_wnd = hwnd;

			DWORD pid = NULL;
			DWORD tid = GetWindowThreadProcessId(hwnd, &pid);

			RECT area;
			GetWindowRect(hwnd, &area);

			_tprintf(TEXT("%08X PID : %08X TID : %08X (%d,%d,%d,%d)"), hwnd, pid, tid, area.left, area.top, area.right - area.left, area.bottom - area.top);

			LONG_PTR win_style = GetWindowLongPtr(hwnd, GWL_STYLE);

			for (int i = 0; i < ARRAYSIZE(window_styles); ++i)
			{
				if (win_style & window_styles[i].val)
				{
					printf(" %s", window_styles[i].str);
				}
			}

			if(win_style & WS_CHILD)
			{
				_tprintf(TEXT("\r\n\tParent :"));
				HWND parent_wnd = hwnd;
				while(parent_wnd = GetParent(parent_wnd))
				{
					_tprintf(TEXT(" %08X"), parent_wnd);
				}
			}
			else
			{
				HWND parent_wnd = GetParent(hwnd);
				_tprintf(TEXT("\r\n\tOwner : %08X"), parent_wnd);
			}

			TCHAR title_buf[256];
			TCHAR class_buf[256];

			GetClassName(hwnd, class_buf, ARRAYSIZE(class_buf));
			GetWindowText(hwnd, title_buf, ARRAYSIZE(title_buf));

			_tprintf(TEXT("\r\n\tClass : %s\r\n\tTitle : %s\r\n\r\n"), class_buf, title_buf);

		}
		
		Sleep(300);
	}

	return 0;
}