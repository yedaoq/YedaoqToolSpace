#include <windows.h>
#include <tchar.h>

const wchar_t SHARING_WINDOW_CLASS[] = TEXT("sharing frame");

HWND			indicator_window_ = NULL;

HDC				mem_dc_ = NULL;
HBITMAP			mem_screen_bmp_ = NULL;
HGDIOBJ			mem_screen_bmp_prev_ = NULL;
int				mem_screen_bmp_width_ = 1600;
int				mem_screen_bmp_height_ = 900;

HDC				mem_frame_dc_ = NULL;
HBITMAP			mem_frame_bmp_ = NULL;
HGDIOBJ			mem_frame_bmp_prev_ = NULL;
int				mem_frame_bmp_width_ = 0;
int				mem_frame_bmp_height_ = 0;

SIZE			cursor_size_;
HCURSOR			cursor_handle_ = NULL;


void debug_output_win(__in const WCHAR* szMsg, ...)
{
	va_list vl;
	TCHAR wszMsg[1024] = {0};

	int len;
	va_start(vl, szMsg);
	len = _vstprintf_s(wszMsg, 1024, szMsg, vl);
	_tcscpy(wszMsg+len, TEXT("\r\n"));
	va_end(vl);

	OutputDebugString(wszMsg);
}


bool Init4AS()
{
	if (NULL == mem_frame_dc_)
	{
		HDC screen_dc = GetDC(NULL);
		mem_dc_ = CreateCompatibleDC(screen_dc);
		mem_frame_dc_ = CreateCompatibleDC(screen_dc);

		mem_screen_bmp_ = CreateCompatibleBitmap(screen_dc, mem_screen_bmp_width_, mem_screen_bmp_height_);
		ReleaseDC(NULL, screen_dc);

		mem_screen_bmp_prev_ = SelectObject(mem_dc_, mem_screen_bmp_);
	}
	
	return true;
}

bool FillScreenData(LPRECT lpRect, BYTE *pData, BITMAPINFO *pHeader, HCURSOR hCursor)
{
	TCHAR display_device[33] = TEXT("\\\\.\\DISPLAY1");
	POINT display_device_origin = {0, 0};

	bool is_window = (FALSE != IsWindow(indicator_window_));
	if (!is_window)
	{
		indicator_window_ = FindWindow(SHARING_WINDOW_CLASS, NULL);
		is_window = indicator_window_ && IsWindow(indicator_window_);
	}

	if (is_window)
	{
		if(IsWindowVisible(indicator_window_) && !IsIconic(indicator_window_))
		{
			HMONITOR monitor_handle = MonitorFromWindow(indicator_window_, MONITOR_DEFAULTTOPRIMARY);
			MONITORINFOEX mi;
			mi.cbSize = sizeof(mi);
			if (GetMonitorInfo(monitor_handle, &mi))
			{
				_tcscpy(display_device, mi.szDevice);
				display_device_origin.x = mi.rcMonitor.left;
				display_device_origin.y = mi.rcMonitor.top;
			}
		}
	}

	HDC monitor_dc = CreateDC(display_device, display_device, NULL, NULL);
	if (NULL == monitor_dc)
	{
		return false;
	}

	int monitor_width = GetDeviceCaps(monitor_dc, HORZRES);
	int monitor_height = GetDeviceCaps(monitor_dc, VERTRES);

	if (monitor_width > mem_screen_bmp_width_ || monitor_height > mem_screen_bmp_height_)
	{
		SelectObject(mem_dc_, mem_screen_bmp_prev_);
		DeleteObject(mem_screen_bmp_);

		mem_screen_bmp_width_ = monitor_width;
		mem_screen_bmp_height_ = monitor_height;

		mem_screen_bmp_ = CreateCompatibleBitmap(monitor_dc, mem_screen_bmp_width_, mem_screen_bmp_height_);
		mem_screen_bmp_prev_ = SelectObject(mem_dc_, mem_screen_bmp_);
	}

	int frame_height = pHeader->bmiHeader.biHeight;
	if (frame_height < 0) frame_height = -frame_height;

	if (pHeader->bmiHeader.biWidth != mem_frame_bmp_width_ || frame_height != mem_frame_bmp_height_)
	{
		if (NULL != mem_frame_bmp_)
		{
			SelectObject(mem_frame_dc_, mem_frame_bmp_prev_);
			DeleteObject(mem_frame_bmp_);
		}

		mem_frame_bmp_width_ = pHeader->bmiHeader.biWidth;
		mem_frame_bmp_height_ = frame_height;
		mem_frame_bmp_ = CreateCompatibleBitmap(monitor_dc, mem_frame_bmp_width_, mem_frame_bmp_height_);
		mem_frame_bmp_prev_ = SelectObject(mem_frame_dc_, mem_frame_bmp_);
	}

	StretchBlt(mem_frame_dc_, 0, 0, mem_frame_bmp_width_, mem_frame_bmp_height_, monitor_dc, 0, 0, monitor_width, monitor_height, SRCCOPY | CAPTUREBLT);

}

SIZE GetCursorSize(HCURSOR cursor)
{
	SIZE size = {24, 24};
	ICONINFO icon_info;
	if(!GetIconInfo(cursor, &icon_info))
		return size;

	HBITMAP icon_bmp = icon_info.hbmColor ? icon_info.hbmColor : icon_info.hbmMask;
	BITMAP bitmap;
	if(GetObject(icon_bmp, sizeof(bitmap), &bitmap) > 0)
	{
		size.cx = bitmap.bmWidth;
		size.cy = bitmap.bmHeight > 0 ? bitmap.bmHeight : -bitmap.bmHeight;
	}

	return size;
}

bool FillScreenData_Fast(LPRECT lpRect, BYTE *pData, BITMAPINFO *pHeader)
{
	TCHAR display_device[33] = TEXT("\\\\.\\DISPLAY1");
	POINT display_device_origin = {0, 0};

	bool is_window = IsWindow(indicator_window_);
	if (!is_window)
	{
		indicator_window_ = FindWindow(SHARING_WINDOW_CLASS, NULL);
		is_window = indicator_window_ && IsWindow(indicator_window_);
	}

	if (is_window)
	{
		//if(IsWindowVisible(indicator_window_) && !IsIconic(indicator_window_))
		{
			HMONITOR monitor_handle = MonitorFromWindow(indicator_window_, MONITOR_DEFAULTTOPRIMARY);
			MONITORINFOEX mi;
			mi.cbSize = sizeof(mi);
			if (GetMonitorInfo(monitor_handle, &mi))
			{
				_tcscpy(display_device, mi.szDevice);
				display_device_origin.x = mi.rcMonitor.left;
				display_device_origin.y = mi.rcMonitor.top;
			}
			else
			{
				debug_output_win(TEXT("VCam GetMonitorInfo fail!"));
			}
		}
// 		else
// 		{
// 			debug_output_win(TEXT("VCam window(%08x) is invisible!"));
// 		}
	}

	debug_output_win(TEXT("VCam Device : %s, Window : %08x"), display_device, indicator_window_);

	HDC monitor_dc = CreateDC(display_device, display_device, NULL, NULL);
	if (NULL == monitor_dc)
	{
		debug_output_win(TEXT("VCam CreateDC fail!"));
		return false;
	}

	int monitor_width = GetDeviceCaps(monitor_dc, HORZRES);
	int monitor_height = GetDeviceCaps(monitor_dc, VERTRES);

	int frame_height = pHeader->bmiHeader.biHeight;
	if (frame_height < 0) frame_height = -frame_height;

	if (pHeader->bmiHeader.biWidth != mem_frame_bmp_width_ || frame_height != mem_frame_bmp_height_)
	{
		if (NULL != mem_frame_bmp_)
		{
			DeleteObject(mem_frame_bmp_);
		}

		mem_frame_bmp_width_ = pHeader->bmiHeader.biWidth;
		mem_frame_bmp_height_ = frame_height;
		mem_frame_bmp_ = CreateCompatibleBitmap(monitor_dc, mem_frame_bmp_width_, mem_frame_bmp_height_);
	}

	debug_output_win(TEXT("VCam blt : (%d,%d), (%d, %d)"), monitor_width, monitor_height, mem_frame_bmp_width_, mem_frame_bmp_height_);

	mem_frame_bmp_prev_ = SelectObject(mem_frame_dc_, mem_frame_bmp_);
	StretchBlt(mem_frame_dc_, 0, 0, mem_frame_bmp_width_, mem_frame_bmp_height_, monitor_dc, 0, 0, monitor_width, monitor_height, SRCCOPY | CAPTUREBLT);

	DeleteDC(monitor_dc);

	CURSORINFO cursor_info;
	cursor_info.cbSize = sizeof(cursor_info);
	GetCursorInfo(&cursor_info);
	if (cursor_info.flags & CURSOR_SHOWING)
	{
		POINT pt = cursor_info.ptScreenPos;
		pt.x -= display_device_origin.x;
		pt.y -= display_device_origin.y;

		if (!(pt.x < 0 || pt.y < 0 || pt.x > monitor_width || pt.y > monitor_height))
		{
			if (cursor_handle_ != cursor_info.hCursor)
			{
				cursor_handle_ = cursor_info.hCursor;
				cursor_size_ = GetCursorSize(cursor_info.hCursor);
				debug_output_win(TEXT("VCam cursor size : (%d,%d)"), cursor_size_.cx, cursor_size_.cy);
			}

			if (pt.x + cursor_size_.cx > monitor_width) 
			{
				pt.x = monitor_width - cursor_size_.cx;
			}

			if (pt.y + cursor_size_.cy > monitor_height)
			{
				pt.y = monitor_height - cursor_size_.cy;
			}

			int stretch_x = pt.x * mem_frame_bmp_width_ / monitor_width;
			int stretch_width = cursor_size_.cx  * mem_frame_bmp_width_ / monitor_width;
			int stretch_y = pt.y * mem_frame_bmp_height_ / monitor_height;
			int stretch_height = cursor_size_.cy * mem_frame_bmp_height_ / monitor_height;

			debug_output_win(TEXT("VCam draw cursor : (%d,%d, %d, %d)"), stretch_x, stretch_y, stretch_width, stretch_height);

			DrawIconEx(mem_frame_dc_, stretch_x, stretch_y, cursor_info.hCursor, stretch_width, stretch_height, 0, NULL, DI_NORMAL);
		}
		else
		{
			debug_output_win(TEXT("Vcam : cursor is out of monitor!"));
		}
	}
	else
	{
		debug_output_win(TEXT("Vcam : cursor is hide!"));
	}

	SelectObject(mem_frame_dc_, mem_frame_bmp_prev_);
	GetDIBits(mem_frame_dc_, mem_frame_bmp_, 0, mem_frame_bmp_height_, pData, pHeader, DIB_RGB_COLORS);

	
	return true;
}