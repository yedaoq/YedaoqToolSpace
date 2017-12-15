#pragma once

#include "..\resource.h"
#include "common.h"


class CDialogThumbs : public CDialogImpl<CDialogThumbs>, public CUpdateUI<CDialogThumbs>
{
public:
	enum { IDD = IDD_ASIMG_THUMB };

public:
	CDialogThumbs(void);
	~CDialogThumbs(void);

	BEGIN_UPDATE_UI_MAP(CDialogInfo)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CDialogInfo)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_CTLCOLORDLG, OnDlgColor)

		MESSAGE_HANDLER(WM_SET_THUMBS, OnSetThumbs)
		MESSAGE_HANDLER(WM_REFRESH_PANEL, OnRefreshPanel)

	END_MSG_MAP()

protected:
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnDlgColor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	
	LRESULT OnSetThumbs(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnRefreshPanel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

protected:
	void DrawOwnerDrawCtl( LPDRAWITEMSTRUCT lpDrawItem, HBITMAP hbp );

protected:
	HBITMAP*	thumb_handls_;
	HDC			dc_memory_;
	HBRUSH		brush_dlg_background_;
};
