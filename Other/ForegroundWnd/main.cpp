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

// #define WS_EX_DLGMODALFRAME     0x00000001L
// #define WS_EX_NOPARENTNOTIFY    0x00000004L
// #define WS_EX_TOPMOST           0x00000008L
// #define WS_EX_ACCEPTFILES       0x00000010L
// #define WS_EX_TRANSPARENT       0x00000020L
// #if(WINVER >= 0x0400)
// #define WS_EX_MDICHILD          0x00000040L
// #define WS_EX_TOOLWINDOW        0x00000080L
// #define WS_EX_WINDOWEDGE        0x00000100L
// #define WS_EX_CLIENTEDGE        0x00000200L
// #define WS_EX_CONTEXTHELP       0x00000400L
// 
// #endif /* WINVER >= 0x0400 */
// #if(WINVER >= 0x0400)
// 
// #define WS_EX_RIGHT             0x00001000L
// #define WS_EX_LEFT              0x00000000L
// #define WS_EX_RTLREADING        0x00002000L
// #define WS_EX_LTRREADING        0x00000000L
// #define WS_EX_LEFTSCROLLBAR     0x00004000L
// #define WS_EX_RIGHTSCROLLBAR    0x00000000L
// 
// #define WS_EX_CONTROLPARENT     0x00010000L
// #define WS_EX_STATICEDGE        0x00020000L
// #define WS_EX_APPWINDOW         0x00040000L
// 
// 
// #define WS_EX_OVERLAPPEDWINDOW  (WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE)
// #define WS_EX_PALETTEWINDOW     (WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST)
// 
// 
// #if(WINVER >= 0x0500)
// #define WS_EX_NOINHERITLAYOUT   0x00100000L // Disable inheritence of mirroring by children
// #define WS_EX_LAYOUTRTL         0x00400000L // Right to left mirroring
// #endif /* WINVER >= 0x0500 */
// 
// #if(_WIN32_WINNT >= 0x0501)
// #define WS_EX_COMPOSITED        0x02000000L
// #endif /* _WIN32_WINNT >= 0x0501 */
// #if(_WIN32_WINNT >= 0x0500)
// #define WS_EX_NOACTIVATE        0x08000000L

window_style window_ex_styles[] = {
	{WS_EX_LAYERED, "Layer"},
	{WS_EX_TRANSPARENT, "Transparent"},
	{WS_EX_TOPMOST, "Top"},
	{WS_EX_TOOLWINDOW, "Tool"},
	{WS_EX_WINDOWEDGE, "WndEdge"},
	{WS_EX_CLIENTEDGE, "CliEdge"},
	{WS_EX_LEFTSCROLLBAR, "LScroll"},
	{WS_EX_RIGHTSCROLLBAR, "RScroll"},
	{WS_EX_APPWINDOW, "App"},
	{WS_EX_OVERLAPPEDWINDOW, "Overlap"},
	{WS_EX_PALETTEWINDOW, "Palette"},
	{WS_EX_COMPOSITED, "Composition"},
	{WS_EX_NOACTIVATE, "NoActivate"},
};

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
		bool cursor_window = 0xFF00 & GetKeyState(VK_LCONTROL);
		if(cursor_window)
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

			_tprintf(TEXT("%08X (%s) PID : %08X TID : %08X (%d,%d,%d,%d)"), hwnd, (cursor_window ? TEXT("Cursor") : TEXT("Fore")), pid, tid, area.left, area.top, area.right - area.left, area.bottom - area.top);

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