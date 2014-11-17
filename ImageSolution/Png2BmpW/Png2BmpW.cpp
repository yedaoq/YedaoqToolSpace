// Png2BmpW.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Png2BmpW.h"
#include <shellapi.h>
#include <Strsafe.h>
#include <shlwapi.h>
#include <atlimage.h>

#pragma comment(lib, "Msimg32.lib")

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

TCHAR pathOut[1024] = TEXT("");					// 最后一个输出文件路径

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void OnDropFiles(HDROP hdrop);
bool Png2AlphaBitmap( LPCTSTR path_png, LPCTSTR path_bmp );
void PaintImage(HDC dc);
void PaintImageEx( HDC dc );

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PNG2BMPW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PNG2BMPW));

	// 主消息循环:
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



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释:
//
//    仅当希望
//    此代码与添加到 Windows 95 中的“RegisterClassEx”
//    函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//    这样应用程序就可以获得关联的
//    “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PNG2BMPW));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_BACKGROUND);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_PNG2BMPW);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   DragAcceptFiles(hWnd, TRUE);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_DROPFILES:
		OnDropFiles((HDROP)wParam);
		InvalidateRect(hWnd, 0, TRUE);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		
		PaintImageEx(hdc);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void OnDropFiles(HDROP hdrop)
{
	if(!hdrop)
		return;

	int filesDropped = DragQueryFile(hdrop, 0xffffffff, NULL, 0);	//获取拖拽文件的个数

	TCHAR pathDropped[1024];

	for(int i = 0; i < filesDropped; ++i)
	{
		ZeroMemory(pathDropped, sizeof(pathDropped));
		DragQueryFile(hdrop, i, pathDropped, 1024);

		if (0 == _tcsicmp(::PathFindExtension(pathDropped), TEXT(".bmp")))
		{
			_tcscpy(pathOut, pathDropped);
		}
		else
		{
			::StringCbCopy(pathOut, sizeof(pathOut), pathDropped);
			::PathRemoveExtension(pathOut);
			::PathAddExtension(pathOut, TEXT(".bmp"));

			Png2AlphaBitmap(pathDropped, pathOut);
		}
	}

	DragFinish(hdrop);
}

void PaintImage( HDC dc )
{
	if (*pathOut)
	{
		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 255;
		bf.AlphaFormat = AC_SRC_ALPHA;

// 		ATL::CImage img;
// 		img.Load(pathOut);
// 		int pix = img.GetBPP();
	//	img.Draw(dc, 100, 100, img.GetWidth(), img.GetHeight(), 0, 0, img.GetWidth(), img.GetHeight());
	//	return;
		
// 		if(!AlphaBlend(dc, 100, 100, img.GetWidth(), img.GetHeight(), img.GetDC(), 0, 0, img.GetWidth(), img.GetHeight(), bf))
// 		{
// 			MessageBox(NULL, TEXT("null"), TEXT("alpha"), MB_OK);
// 		}
// 
// 		img.ReleaseDC();
// 		return;

		HBITMAP hbmp = (HBITMAP)LoadImage(NULL, pathOut, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_LOADTRANSPARENT);
		HDC		mem_dc = CreateCompatibleDC(/*NULL */ dc);
		SelectObject(mem_dc, hbmp);

		int bitsPixel = GetDeviceCaps(dc, BITSPIXEL);
		int logPixelx = GetDeviceCaps(dc, LOGPIXELSX);
		int logPixely = GetDeviceCaps(dc, LOGPIXELSY);

		BITMAP bmp;
		GetObject(hbmp, sizeof(bmp), &bmp);

		if(!AlphaBlend(dc, 100, 100, bmp.bmWidth, bmp.bmHeight, mem_dc, 0, 0, bmp.bmWidth, bmp.bmHeight, bf))
		{
			MessageBox(NULL, TEXT("null"), TEXT("alpha"), MB_OK);
		}

		DeleteObject(hbmp);
		DeleteDC(mem_dc);
	}
}

void PaintImageEx( HDC dc )
{
	if(!*pathOut) return;

	HBITMAP hbmp = (HBITMAP)LoadImage(NULL, pathOut, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_LOADTRANSPARENT);
	BITMAP bmp;
	GetObject(hbmp, sizeof(bmp), &bmp);

	HDC hMemDC = CreateCompatibleDC(dc);
	BITMAPINFOHEADER * bmp_header = new BITMAPINFOHEADER;
	memset(bmp_header, 0, sizeof(BITMAPINFOHEADER));   
	bmp_header->biSize = sizeof (BITMAPINFOHEADER);
	bmp_header->biWidth = bmp.bmWidth;   
	bmp_header->biHeight = bmp.bmHeight;   
	bmp_header->biPlanes = 1 ;   
	bmp_header->biBitCount = 32;    
	bmp_header->biCompression = BI_RGB;   
	bmp_header->biSizeImage = 0;   
	bmp_header->biXPelsPerMeter = 0;   
	bmp_header->biYPelsPerMeter = 0;   
	bmp_header->biClrUsed = 0;   
	bmp_header->biClrImportant = 0;   
	void* dib = NULL;

	HBITMAP bitmap = ::CreateDIBSection(hMemDC, (BITMAPINFO *)bmp_header, 0, &dib, NULL, 0) ;
	delete bmp_header;
	HBITMAP oldBitmap = (HBITMAP)SelectObject(hMemDC, bitmap);

	BLENDFUNCTION bn;
	bn.AlphaFormat = AC_SRC_ALPHA;
	bn.BlendFlags = 0;
	bn.BlendOp = AC_SRC_OVER;
	bn.SourceConstantAlpha = 255; //透明度设置，0为不透明；255为完全透明

	HDC hMemDCSrc = CreateCompatibleDC(dc);
	SelectObject(hMemDCSrc, hbmp);

	BitBlt(hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, dc, 100, 100, SRCCOPY);
	AlphaBlend(hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, hMemDCSrc, 0, 0, bmp.bmWidth, bmp.bmHeight, bn);
	BitBlt(dc, 100, 100, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);

	//AlphaBlend(dc, 100, 100, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, bn);

	DeleteObject(bitmap);
	DeleteObject(hbmp);
	DeleteDC(hMemDC);
	DeleteDC(hMemDCSrc);
}