#include "StdAfx.h"
#include "WndMain.h"
#include "App.h"

WndMain::WndMain(void)
	: wnd_(0)
{
}

WndMain::~WndMain(void)
{
}

void WndMain::Show()
{
	wnd_ = CreateDialog(NULL, MAKEINTRESOURCE(IDD_MAIN), NULL, MyDlgProc);
	ShowWindow(wnd_, SW_SHOWNORMAL);
}

INT_PTR CALLBACK WndMain::MyDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_COMMAND:
		app_context->wnd_main->OnCommand(wParam, lParam);
		break;

	case WM_CLOSE:

		break;
	}

	return FALSE;
}

void WndMain::OnCommand( WPARAM wParam, LPARAM lParam )
{
	switch(HIWORD(wParam))
	{
	case BN_CLICKED:
		switch(LOWORD(wParam))
		{
		case IDOK:
			OnOK();
			break;
		case IDCANCEL:
			OnCancel();
			break;
		}
	}
}

void WndMain::OnOK()
{
	
}

void WndMain::OnCancel()
{

}
