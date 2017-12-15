#pragma once

#include "..\resource.h"
#include "common.h"


class CDialogInfo : public CDialogImpl<CDialogInfo>, public CUpdateUI<CDialogInfo>
{
public:
	enum { IDD = IDD_ASIMG_FRAMEINFO };

public:
	CDialogInfo(void);
	~CDialogInfo(void);

	BEGIN_UPDATE_UI_MAP(CDialogInfo)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CDialogInfo)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)

		//MESSAGE_HANDLER(WM_SET_THUMBS, OnSetThumbs)
		MESSAGE_HANDLER(WM_REFRESH_PANEL, OnRefreshPanel)

	END_MSG_MAP()

protected:
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	
	//LRESULT OnSetThumbs(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnRefreshPanel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

protected:

	void OutputRegion(LPRGNDATA rgn, LPCTSTR desc);

protected:
	WTL::CRichEditCtrl		ctl_edt_info_;
	int						frame_index_;
};
