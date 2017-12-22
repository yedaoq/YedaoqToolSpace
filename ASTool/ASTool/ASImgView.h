// ASImgView.h : interface of the CASImgView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include "Layout.h"
#include "..\cpt_data.h"
#include "util\util.h"

#include "asimg\DialogThumbs.h"
#include "asimg\DialogInfo.h"
#include "asimg\DialogImage.h"
#include "asimg\ASRecordPackage.h"
#include "asimg\ASFrameRes.h"

enum { WM_PLAY_NEXTFRAME = WM_USER + 1021 };

class CASImgView : public CDialogImpl<CASImgView>, public CUpdateUI<CASImgView>,
		public CMessageFilter, public CIdleHandler
{
public:
	enum { IDD = IDD_ASIMG_FORM };

	CASImgView();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CASImgView)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CASImgView)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
		MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)
		MESSAGE_HANDLER(WM_DROPFILES, OnDropFiles)

		//MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_PLAY_NEXTFRAME, OnPlayNextFrame)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnOK)

		COMMAND_ID_HANDLER(IDC_BTN_EXPORTIMAGE, OnBtnExportImage)
		COMMAND_ID_HANDLER(IDC_BTN_EXPORTFRAME, OnBtnExportFrame)
		COMMAND_ID_HANDLER(IDC_BTN_LOGDIR, OnBtnLogDir)

		COMMAND_ID_HANDLER(IDC_BTN_PLAY, OnBtnPlay)

		COMMAND_ID_HANDLER(IDC_CHK_VISIBLE, OnBtnRegion)
		COMMAND_ID_HANDLER(IDC_CHK_INVISIBLE, OnBtnRegion)
		COMMAND_ID_HANDLER(IDC_CHK_UPDATE, OnBtnRegion)
		COMMAND_ID_HANDLER(IDC_CHK_BACK, OnBtnRegion)

		COMMAND_ID_HANDLER(IDC_CHK_IMAGEERASE, OnBtnImageErase)
		COMMAND_ID_HANDLER(IDC_CHK_RGNFILL, OnBtnRegion)
		COMMAND_ID_HANDLER(IDC_CHK_AREALINE, OnBtnAreaRefLine)
		COMMAND_ID_HANDLER(IDC_CHK_SCALE, OnBtnScale)

		MESSAGE_HANDLER(WM_SET_FOCUS_TO_SLIDER_FRAME, OnSetFocusToSliderFrame)
		MESSAGE_HANDLER(WM_SET_TXT_CURSOR_POS, OnSetTxtCursorPos)
		MESSAGE_HANDLER(WM_PLAY_SWITCH, OnPlaySwitch)

		COMMAND_HANDLER(IDC_CMB_FILES, CBN_SELCHANGE, OnCmbMettingSelChanged)
		COMMAND_HANDLER(IDC_CMB_PLAYRATE, CBN_SELCHANGE, OnCmbPlayrateSelChanged)
		//COMMAND_HANDLER(IDC_TAB_PANELS, TCN_SELCHANGE, OnTabPanelSwitched)
		NOTIFY_HANDLER(IDC_TAB_PANELS, TCN_SELCHANGE, OnTabPanelSwitched)
		
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnHScroll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnVScroll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDropFiles(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	//LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnBtnExportFrame(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBtnExportImage(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBtnLogDir(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	
	LRESULT OnBtnRegion(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBtnAreaRefLine(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBtnImageErase(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBtnRegionFill(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBtnScale(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBtnPlay(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPlayNextFrame(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnCmbMettingSelChanged(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCmbPlayrateSelChanged(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSliderTimeLineMouseMoved(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTabPanelSwitched(int wParam, LPNMHDR lpnh, BOOL& bHandled);

	LRESULT OnSetFocusToSliderFrame(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSetTxtCursorPos(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPlaySwitch(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	void CloseDialog(int nVal);

public:
	void InitFileList();
	void SetStatusBar(HWND wnd);

protected:
	void InitImageTick();
	void InitLogBlockBuffer();

	bool LoadASRecording(LPCTSTR file_path);
	void CloseASRecording();

	bool UpdateTimeLine();

	int	 GetFrameIndexOfTime(__int64 time);

	void SetCurrentFrame(int frame_index, bool show_nearby);

	void DisplayImageInStatic(HBITMAP hbp, DWORD ctl_id);

	void RefreshThumbs();

	//void BuildNearbyImages(int first_frame_index);
	
	static void ThreadEntry4Play(void*);
	void LoopPlay();
	void StartPlay();
	void StopPlay();

	void InitTabPanels();

protected:
	IASRecordPackage*	record_package_;
	ASFrameRes			current_frame_res_;

protected:
	WTL::CComboBox		ctl_cmb_mettings_;
	WTL::CStatic		ctl_txt_curtime_;
	WTL::CStatic		ctl_txt_time_;
	WTL::CStatic		ctl_txt_frame_continue_;
	WTL::CStatic		ctl_txt_curframe_;
	WTL::CStatic		ctl_txt_frames_;
	WTL::CStatic		ctl_txt_cursorpos_;
	WTL::CTrackBarCtrl	ctl_slider_timeline_;
	WTL::CTrackBarCtrl	ctl_slider_frame_;

	WTL::CButton		ctl_chk_update_;
	WTL::CButton		ctl_chk_invisible_;
	WTL::CButton		ctl_chk_visible_;
	WTL::CButton		ctl_chk_back_;

	WTL::CButton		ctl_btn_play_;

	WTL::CButton		ctl_chk_image_erase_;
	WTL::CButton		ctl_chk_region_fill_;
	WTL::CButton		ctl_chk_area_refline_;
	WTL::CButton		ctl_chk_scale_;

	WTL::CTabCtrl		ctl_tab_panels_;

	WTL::CButton		ctl_btn_main_;

	WTL::CComboBox		ctl_cmb_playrate_;

	WTL::CStatusBarCtrl	ctl_status_bar_;

	CDialogInfo			dlg_info_;
	CDialogThumbs		dlg_thumbs_;
	CDialogImage		dlg_image_;

	NSYedaoqLayout::CFlowLayout	layout_main_;

	HICON				icon_play_;
	HICON				icon_pause_;

protected:

	ULONG_PTR			read_timer_id_;

protected:
	unsigned int				current_frame_index_;
	LONGLONG					metting_duration_miniseconds_;
	time_duration				metting_duration_;

	HDC							dc_memory_;

// 	HBITMAP						nearby_frame_bitmaps_[7];
// 	int							nearby_frame_index_base_;

protected:
	POINT						log_file_latest_mousepos_;
	LONGLONG					log_file_latest_cursorimage_pos_;

protected:
	HANDLE						play_thread_handle_;
	bool						play_flag_;
	int							play_target_frame_index_;
};
