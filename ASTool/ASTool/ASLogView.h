// ASLogView.h : interface of the CASLogView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Layout.h"
#include "FilterList.h"

struct ReadBuffer;

class CASLogView : public CDialogImpl<CASLogView>, public CUpdateUI<CASLogView>,
	public CMessageFilter, public CIdleHandler
{
public:
	enum { IDD = IDD_ASLOG_FROM };

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	CASLogView();

	BEGIN_UPDATE_UI_MAP(CASLogView)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CASLogView)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_HANDLER(IDC_CMB_FILES, CBN_SELCHANGE, OnCbnSelchangeCmbFiles)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		COMMAND_HANDLER(IDC_BTN_REFRESH, BN_CLICKED, OnBnClickedBtnRefresh)
		COMMAND_HANDLER(IDC_CMB_FILTER, CBN_EDITUPDATE, OnCbnEditupdateCmbFilter)
		COMMAND_HANDLER(IDC_CMB_FILTER, CBN_SELCHANGE, OnCbnSelchangeCmbFilter)
		COMMAND_HANDLER(IDC_BTN_CLEAR, BN_CLICKED, OnBnClickedBtnClear)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnCbnSelchangeCmbFiles(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	void CloseDialog(int nVal);

protected:
	bool InitFileList();
	bool InitFilterCombobox();

	bool InitSciLexerCtrl();

	bool OpenLogFile(LPCTSTR file_name);
	void CloseLogFile();

	void UpdateFilters();

	bool ReadLogAsync();
	//bool ReadLog();
	//bool ParseLogContent();
	bool ParseLogContent(ReadBuffer* buf);

	bool TestFilters(char* log);

	inline void PrintLog(const char* log, int log_len);
	void ClearLog();

protected:
	WTL::CComboBox	ctl_cmb_file_list_;
	WTL::CComboBox	ctl_cmb_file_filter_;
	HWND			ctl_log_;

	NSYedaoqLayout::CFlowLayout*	layout_main_;

	HANDLE			log_file_handle_;
	LARGE_INTEGER	log_file_position_;

	char			log_filters_buf_[4096];
	char*			log_filters_[10];

	UINT_PTR		read_timer_id_;

	int				read_content_parsed_pos_;

	FilterList		filters_serializer_;

public:
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedBtnRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCbnEditupdateCmbFilter(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCbnSelchangeCmbFilter(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCbnSelendokCmbFilter(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedBtnClear(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
