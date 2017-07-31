// Time.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Time.h"

#pragma comment(lib, "Msimg32.lib")

#define MAX_LOADSTRING 100
#define WND_WIDTH 720
#define WND_HEIGHT 240

#define TIMER_LAYERED 1

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

HDC			dc_mem_ = NULL;
HBITMAP		bmp_mem_ = NULL;
HBRUSH		brush_white_ = NULL;
HFONT		font_text_ = NULL;

HDC			dc_mem_alpha_ = NULL;
HBITMAP		bmp_mem_alpha_ = NULL;

bool		mouse_hover_ = false;
DWORD*		bmp_buf_ = NULL;

static SYSTEMTIME time_;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	time_.wSecond = 61;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TIME, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TIME));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
#if TIMER_LAYERED
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TIME_LAYERED));
#else
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TIME));
#endif
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL; //MAKEINTRESOURCE(IDC_TIME);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_POPUPWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, WND_WIDTH, WND_HEIGHT, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void RePaint(HWND hwnd)
{
	RECT rc = {0, 0, WND_WIDTH, WND_HEIGHT};
	DWORD back_color = COLOR_WINDOW;

#if TIMER_LAYERED
	back_color = COLOR_WINDOWTEXT;
#endif

	FillRect(dc_mem_, &rc, GetSysColorBrush(back_color));

	BITMAPINFO bitmapinfo;
	bitmapinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapinfo.bmiHeader.biBitCount = 32;
	bitmapinfo.bmiHeader.biHeight = WND_HEIGHT;
	bitmapinfo.bmiHeader.biWidth = WND_WIDTH;
	bitmapinfo.bmiHeader.biPlanes = 1;
	bitmapinfo.bmiHeader.biCompression = BI_RGB;
	bitmapinfo.bmiHeader.biXPelsPerMeter = 0;
	bitmapinfo.bmiHeader.biYPelsPerMeter = 0;
	bitmapinfo.bmiHeader.biClrUsed = 0;
	bitmapinfo.bmiHeader.biClrImportant = 0;
	bitmapinfo.bmiHeader.biSizeImage = bitmapinfo.bmiHeader.biWidth * bitmapinfo.bmiHeader.biHeight * bitmapinfo.bmiHeader.biBitCount / 8;

	GetDIBits(dc_mem_, bmp_mem_, 0, WND_HEIGHT, bmp_buf_, &bitmapinfo, DIB_RGB_COLORS);

	DWORD* bmp_buf_end = bmp_buf_ + bitmapinfo.bmiHeader.biHeight * bitmapinfo.bmiHeader.biWidth;
	for (DWORD* tmp = bmp_buf_; tmp < bmp_buf_end; ++tmp)
	{
		*tmp |= 0x80000000;
	}

	SetDIBits(dc_mem_, bmp_mem_, 0, WND_WIDTH, bmp_buf_, &bitmapinfo, DIB_RGB_COLORS);

	TCHAR buf[256];
	_stprintf(buf, TEXT("%02d:%02d:%02d"), time_.wHour, time_.wMinute, time_.wSecond);

	BOOL ret = FALSE;
	ret = TextOut(dc_mem_, 30, 30, buf, _tcslen(buf));

	GetDIBits(dc_mem_, bmp_mem_, 0, WND_HEIGHT, bmp_buf_, &bitmapinfo, DIB_RGB_COLORS);

	for (DWORD* tmp = bmp_buf_; tmp < bmp_buf_end; ++tmp)
	{
		if ((*tmp & 0xFF000000))
		{
			if (!mouse_hover_)
			{
				*tmp &= 0x00FFFFFF;
			}
		}
		else
		{
			*tmp |= 0xFF000000;
		}
	}

	SetDIBits(dc_mem_, bmp_mem_, 0, WND_WIDTH, bmp_buf_, &bitmapinfo, DIB_RGB_COLORS);

	SIZE sz = {WND_WIDTH, WND_HEIGHT};
	POINT pt = {0, 0};

	BLENDFUNCTION blendFunc32bpp;
	blendFunc32bpp.AlphaFormat = AC_SRC_ALPHA; //AC_SRC_OVER; // AC_SRC_ALPHA;
	blendFunc32bpp.BlendFlags = 0;
	blendFunc32bpp.BlendOp = AC_SRC_OVER;
	blendFunc32bpp.SourceConstantAlpha = 255;

#if TIMER_LAYERED
	HDC dc_win = GetDC(hwnd);
	ret = UpdateLayeredWindow(hwnd, /*dc_screen*/ /*dc_win*/ NULL, NULL, &sz, dc_mem_, &pt, 0, &blendFunc32bpp, ULW_ALPHA);
	ReleaseDC(hwnd, dc_win);
#else 
	ret = InvalidateRect(hwnd, NULL, FALSE);
#endif

	if (FALSE == ret)
	{
		ret = FALSE;
	}
}

void OnTimer(HWND hwnd)
{
	SYSTEMTIME t;
	GetLocalTime(&t);

	if (t.wSecond == time_.wSecond)	return;

	time_ = t;

	RePaint(hwnd);
}

void OnPaint(HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC dc = BeginPaint(hwnd, &ps);
	BitBlt(dc, 0, 0, WND_WIDTH, WND_HEIGHT, dc_mem_, 0, 0, SRCCOPY);
	EndPaint(hwnd, &ps);
}

bool InitGDIObjects()
{
	HDC dc_screen = GetDC(NULL);
	dc_mem_ = CreateCompatibleDC(dc_screen);
	bmp_mem_ = CreateCompatibleBitmap(dc_screen, WND_WIDTH, WND_HEIGHT);

	SelectObject(dc_mem_, bmp_mem_);
	SetBkMode(dc_mem_, TRANSPARENT);

	font_text_ = CreateFont(160, 0, 0, 0, FW_DEMIBOLD, 0, 0, 0, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FF_DONTCARE | DEFAULT_PITCH, TEXT("Consolas"));

	SelectObject(dc_mem_, font_text_);

	dc_mem_alpha_ = CreateCompatibleDC(dc_screen);
	bmp_mem_alpha_ = CreateCompatibleBitmap(dc_screen, 20, 20);

	RECT rc = {0, 0, 20, 20};
	SelectObject(dc_mem_alpha_, bmp_mem_alpha_);
	FillRect(dc_mem_alpha_, &rc, (HBRUSH)COLOR_GRAYTEXT);

	ReleaseDC(NULL, dc_screen);

	bmp_buf_ = (DWORD*)malloc(WND_WIDTH * WND_HEIGHT * 4);

	return true;
}

bool TrackMouseLeave(HWND hwnd)
{
	TRACKMOUSEEVENT tme = {0};
	tme.hwndTrack = hwnd;
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE | TME_NONCLIENT;

	BOOL ret = TrackMouseEvent(&tme);
	DWORD err = GetLastError();

	return FALSE != ret;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_ERASEBKGND:
		OutputDebugStringA("Layered Windows : WM_ERASEBKGND\r\n");
		break;
	case WM_PAINT:
		OnPaint(hWnd);
		break;
	case WM_TIMER:
		OnTimer(hWnd);
		break;
	case WM_NCHITTEST:
		return HTCAPTION;
		break;
	case WM_NCMOUSEMOVE:
		OutputDebugStringA("Layered Windows : WM_NCMOUSEMOVE\r\n");
		if (!mouse_hover_)
		{
			TrackMouseLeave(hWnd);
			mouse_hover_ = true;
			RePaint(hWnd);
		}
		break;

	case WM_NCMOUSELEAVE:
		OutputDebugStringA("Layered Windows : WM_NCMOUSELEAVE\r\n");
		mouse_hover_ = false;
		RePaint(hWnd);
		break;
	case WM_CREATE:
		{
			InitGDIObjects();

#if TIMER_LAYERED
			SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED /*| WS_EX_TRANSPARENT*/);	
			SetWindowText(hWnd, TEXT("Layered Clock"));
#else
			SetWindowText(hWnd, TEXT("Normal Clock"));
#endif

			SetTimer(hWnd, 233, 10, NULL);	
			TrackMouseLeave(hWnd);
		}

		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
