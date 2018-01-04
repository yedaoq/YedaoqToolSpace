// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "ASToolView.h"
#include "MainFrm.h"
#include "ASLogView.h"
#include "util/util.h"
#include "ASImgView.h"

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;

	return m_view.PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle()
{
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CreateSimpleStatusBar();

	m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	CMenuHandle menuMain = GetMenu();
	m_view.SetWindowMenu(menuMain.GetSubMenu(WINDOW_MENU_POSITION));

	SetProduct(PRODUCT_ZOOM);

	CheckWindowsMenuItem(menuMain, ID_ASIMG_ENABLE, IsToolRecordInstalled());

	view_log_.Create(m_view);
	m_view.AddPage(view_log_.m_hWnd, _T("Live Log"));

	view_as_img_.Create(m_view);
	m_view.AddPage(view_as_img_.m_hWnd, _T("AS Image"));
	view_as_img_.SetStatusBar(m_hWndStatusBar);

	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	bHandled = FALSE;
	return 1;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
//	CASToolView* pView = new CASToolView;
	CASLogView* pView = new CASLogView;
	pView->Create(m_view);
	m_view.AddPage(pView->m_hWnd, _T("Live Log"));

	// TODO: add code to initialize document

	return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainFrame::OnWindowClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int nActivePage = m_view.GetActivePage();
	if(nActivePage != -1)
		m_view.RemovePage(nActivePage);
	else
		::MessageBeep((UINT)-1);

	return 0;
}

LRESULT CMainFrame::OnWindowCloseAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_view.RemoveAllPages();

	return 0;
}

LRESULT CMainFrame::OnWindowActivate(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int nPage = wID - ID_WINDOW_TABFIRST;
	m_view.SetActivePage(nPage);

	return 0;
}

LRESULT CMainFrame::OnASImgEnable( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled )
{
	bHandled = TRUE;

	HMENU menu = GetMenu();
	if(IsWindowsMenuItemChecked(menu, ID_ASIMG_ENABLE, false))
	{
		UninstallToolRecord(true);
	}
	else
	{
		InstallToolRecord(true);
	}

	CheckWindowsMenuItem(menu, ID_ASIMG_ENABLE, IsToolRecordInstalled());

	return 0;
}

LRESULT CMainFrame::OnASImgFilelist( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled )
{
	bHandled = TRUE;
	view_as_img_.InitFileList();
	return 0;
}

LRESULT CMainFrame::OnASRecordStart( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;
	bool succeed = (INVALID_HANDLE_VALUE != (HANDLE)lParam);
	if (succeed)
	{
		MessageBox(TEXT("ÒÑ¿ªÊ¼Â¼Ïñ..."), TEXT("ASÂ¼Ïñ"), MB_OK);
	}
	else
	{
		MessageBox(TEXT("´´½¨Â¼ÏñÎÄ¼þÊ§°Ü£¬ÎÞ·¨Â¼Ïñ¡£"), TEXT("ASÂ¼Ïñ"), MB_OK);
	}

	return 0;
}

LRESULT CMainFrame::OnASRecordStop( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
{
	bHandled = TRUE;
	MessageBox(TEXT("ÒÑÍ£Ö¹Â¼Ïñ¡£"), TEXT("ASÂ¼Ïñ"), MB_OK);
	return 0;
}

LRESULT CMainFrame::OnActivate( UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled )
{
	bHandled = TRUE;

	if (WA_INACTIVE != wParam)
	{
		HMENU menu = GetMenu();
		CheckWindowsMenuItem(menu, ID_ASIMG_ENABLE, IsToolRecordInstalled());
	}

	return 0;
}
