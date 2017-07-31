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
	{WS_MINIMIZE, "Min"},
	{WS_MAXIMIZE, "Max"},
	{WS_BORDER, "Border"},
	{WS_CAPTION, "Caption"},
	{WS_VISIBLE, "Visible"},
};

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

HDC			mem_dc_ = NULL;
HBITMAP		mem_bmp_ = NULL;
int			mem_bmp_width_ = 1000;
int			mem_bmp_height_ = 800;

const int   max_enum_count = 10;
HWND		enum_wnds[max_enum_count + 1] = {NULL};
HWND		enum_wnds_origin[max_enum_count + 1] = {NULL};

struct hwnd_info
{
	DWORD style;
	DWORD style_ex;
	RECT  rect;
};

static HBITMAP acquire_mem_bmp(int width, int height)
{
	static HBITMAP bmp_prev = NULL;
	if (width > mem_bmp_width_ || height > mem_bmp_height_)
	{
		if (mem_bmp_)
		{
			::SelectObject(mem_dc_, bmp_prev);
			DeleteObject(mem_bmp_);
			mem_bmp_ = NULL;
		}
		
		mem_bmp_width_ = width;
		mem_bmp_height_ = height;
	}

	if (NULL == mem_bmp_)
	{
		HDC dc = GetDC(NULL);
		mem_bmp_ = CreateCompatibleBitmap(dc, mem_bmp_width_, mem_bmp_height_);
		ReleaseDC(NULL, dc);
	}

	return mem_bmp_;
}

static BOOL CALLBACK enum_windows_proc(__in HWND hwnd, __in  LPARAM lParam)
{
	TCHAR buf[256];
	GetClassName(hwnd, buf, ARRAYSIZE(buf));

	if (!IsWindowVisible(hwnd) || IsIconic(hwnd)
		 //|| 0 == _tcscmp(buf, TEXT("tooltips_class32"))
		 //|| 0 != _tcscmp(buf, TEXT("SysShadow"))
		)
	{
		return TRUE;
	}

	int* enum_count = (int*)lParam;
	enum_wnds[(*enum_count)++] = hwnd;
	return (*enum_count < max_enum_count);
}

static bool is_enum_wnds_changed(HWND* list_new, HWND* list_old)
{
	for (int i = 0; i < max_enum_count; ++i)
	{
		if (list_old[i] != list_new[i])
		{
			return true;
		}

// 		if (NULL == list_new[i])
// 		{
// 			return false;
// 		}
	}

	return false;
}

void revert_frame(RECT& rc, HDC dc)
{
	BitBlt(dc, rc.left, rc.top, rc.right - rc.left, 4, NULL, 0, 0, DSTINVERT);
	BitBlt(dc, rc.left, rc.bottom - 4, rc.right - rc.left, 4, NULL, 0, 0, DSTINVERT);

	BitBlt(dc, rc.left, rc.top + 4, 4, rc.bottom - rc.top - 8, NULL, 0, 0, DSTINVERT);
	BitBlt(dc, rc.right - 4, rc.top + 4, 4, rc.bottom - rc.top - 8, NULL, 0, 0, DSTINVERT);
}

void highlight_enum_wnds(HWND* wnds)
{
	HDC dc = GetDC(NULL);
	RECT rc;
	while(NULL != *wnds)
	{
// 		if (!IsWindowVisible(*wnds) || IsIconic(*wnds))
// 		{
// 			continue;
// 		}
		
		GetWindowRect(*wnds, &rc);

		revert_frame(rc, dc);
		Sleep(200);
		revert_frame(rc, dc);

// 		HRGN rgn = CreateRectRgnIndirect(&rc);
// 
// 		InflateRect(&rc, -8, -8);
// 		OffsetRect(&rc, 4, 4);
// 
// 		CombineRgn()

		++wnds;
	}

	ReleaseDC(NULL, dc);
}

void print_wnds_info( HWND* wnds ) 
{
	static int idx = 0;
	printf("=====================================================\r\n   Window List : %d \r\n=====================================================\r\n", ++idx);

	while(NULL != *wnds)
	{
		HWND hwnd = *wnds;

		DWORD pid = NULL;
		DWORD tid = GetWindowThreadProcessId(hwnd, &pid);

		RECT area;
		GetWindowRect(hwnd, &area);

		_tprintf(TEXT("%08X (%s) PID : %08X TID : %08X (%d,%d,%d,%d)"), hwnd, TEXT("Top"), pid, tid, area.left, area.top, area.right - area.left, area.bottom - area.top);

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

		++wnds;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	mem_dc_ = CreateCompatibleDC(NULL);

	while(true)
	{
		int enum_count = 0;
		memset(enum_wnds, 0, sizeof(enum_wnds));
		EnumWindows(enum_windows_proc, (LPARAM)&enum_count);

		//if (is_enum_wnds_changed(enum_wnds, enum_wnds_origin))
		{
			memcpy(enum_wnds_origin, enum_wnds, sizeof(enum_wnds_origin));

			print_wnds_info(enum_wnds);
			//highlight_enum_wnds(enum_wnds);
		}

		Sleep(500);
	}

	return 0;
}