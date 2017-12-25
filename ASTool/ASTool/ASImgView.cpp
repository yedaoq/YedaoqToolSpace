// ASImgView.cpp : implementation of the CASImgView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "ASImgView.h"
#include <math.h>
#include "util/util.h"
#include "asimg/ASRecordPackage_CptHost.h"

enum { TIMER_ID_LOADFRAME = 1001, TIMER_ID_PLAY };

WCHAR				img_log_dir_[MAX_PATH * 2] = TEXT("");
WCHAR				img_log_file_path_[MAX_PATH * 2] = TEXT("");
//HANDLE				img_log_file_handle_ = INVALID_HANDLE_VALUE;
//LARGE_INTEGER		log_file_read_pointer_;

// HICON				current_frame_cursor_image_ = NULL;
// POINT				current_frame_mouse_position_ = {0, 0};

// char*				log_block_buf_ = 0;
// int					log_block_buf_size_ = 0;
// cpt_common*			log_block_data_header_ = 0;
// cpt_screen_data*	log_block_screen_data_ = 0;
// int					log_block_frame_index_ = -1;
// 
// __inline int align_data_32(int width, int bits)
// {
// 	return ((width*bits)+31)/32*4;
// }

void make_export_file_path( int frame_index, TCHAR* buf)
{
	_tcscpy_s(buf, MAX_PATH, img_log_file_path_);
	::PathRemoveExtension(buf);
	_stprintf_s(buf + _tcslen(buf), 100, TEXT("-%d.bmp"), frame_index);
}

void save_to_bmp_file1(WCHAR* file, HBITMAP bmp)
{
	FILE* fh = NULL;
	if(_wfopen_s(&fh, file, TEXT("wb"))!=0) return;

	if(NULL == fh) return;

	BITMAP bm;
	GetObject(bmp, sizeof(bm), &bm);

	char* buf = 0;
	int bytes_count = bm.bmHeight * bm.bmWidth * 4; // bm.bmBitsPixel;

	if (0 == bm.bmBits)
	{
		buf = new char[bytes_count + 20];
		bytes_count = GetBitmapBits(bmp, bytes_count, buf);
		bm.bmBits = (LPVOID)buf;
	}

	BITMAPINFOHEADER bmh;
	bmh.biSize = sizeof(BITMAPINFOHEADER);
	bmh.biCompression = BI_RGB;
	bmh.biWidth = bm.bmWidth;
	bmh.biHeight = -bm.bmHeight;
	bmh.biPlanes = 1;
	bmh.biBitCount = 32;
	bmh.biSizeImage = 0;

	BITMAPFILEHEADER bmfh;
	int nBitsOffset = sizeof(BITMAPFILEHEADER) + bmh.biSize; 
	LONG lFileSize = nBitsOffset + bytes_count;
	bmfh.bfType = 'B'+('M'<<8);
	bmfh.bfOffBits = nBitsOffset;
	bmfh.bfSize = lFileSize;
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;

	fwrite(&bmfh, 1, sizeof(BITMAPFILEHEADER), fh);
	fwrite(&bmh, 1, bmh.biSize, fh);
	fwrite(bm.bmBits, 1, bytes_count, fh);
	fflush(fh);

	if (buf)
	{
		delete[] buf;
	}

	fclose(fh);
}

void save_to_bmp_file(WCHAR* file, BITMAPINFO* pbi, unsigned char* bits)
{
	FILE* fh = NULL;
	if(_wfopen_s(&fh, file, TEXT("wb"))!=0) return;

	if(NULL == fh) return;

	BITMAPINFO& bi = *pbi;
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biCompression = BI_RGB;

	int clrtablesize = 0;
	if(bi.bmiHeader.biBitCount<16)
		clrtablesize = sizeof(RGBQUAD)*(int)(1<<bi.bmiHeader.biBitCount);

	BITMAPFILEHEADER bmfh;
	int nBitsOffset = sizeof(BITMAPFILEHEADER) + bi.bmiHeader.biSize + clrtablesize; 
	LONG lImageSize = bi.bmiHeader.biSizeImage;
	LONG lFileSize = nBitsOffset + lImageSize;
	bmfh.bfType = 'B'+('M'<<8);
	bmfh.bfOffBits = nBitsOffset;
	bmfh.bfSize = lFileSize;
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;

	fwrite(&bmfh, 1, sizeof(BITMAPFILEHEADER), fh);

	fwrite(&bi.bmiHeader, 1, bi.bmiHeader.biSize+clrtablesize, fh);

	fwrite(bits, 1, lImageSize, fh);

	fclose(fh);
}

using namespace NSYedaoqLayout;

BOOL CASImgView::PreTranslateMessage(MSG* pMsg)
{
	if (dlg_image_.PreTranslateMessage(pMsg))
	{
		return TRUE;
	}
	//return CWindow::IsDialogMessage(pMsg);
	return FALSE;
}

BOOL CASImgView::OnIdle()
{
	return FALSE;
}

LRESULT CASImgView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);

	HMODULE module_handle = GetModuleHandle(NULL);
	icon_pause_ = (HICON)LoadImage(module_handle, MAKEINTRESOURCE(IDI_ASIMG_PAUSE), IMAGE_ICON, 16, 16, LR_SHARED); // LoadIcon(module_handle, MAKEINTRESOURCE(IDI_PAUSE));
	icon_play_ = (HICON)LoadImage(module_handle, MAKEINTRESOURCE(IDI_ASIMG_PLAY), IMAGE_ICON, 16, 16, LR_SHARED); // LoadIcon(module_handle, MAKEINTRESOURCE(IDI_PLAY));

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

	DragAcceptFiles(TRUE);

	dc_memory_ = CreateCompatibleDC(NULL);

	ctl_cmb_mettings_.Attach(GetDlgItem(IDC_CMB_FILES));
	ctl_txt_curtime_.Attach(GetDlgItem(IDC_TXT_CURTIME));
	ctl_txt_time_.Attach(GetDlgItem(IDC_TXT_TIME));
	ctl_txt_frame_continue_.Attach(GetDlgItem(IDC_TXT_FRAMECONTINUE));
	ctl_slider_timeline_.Attach(GetDlgItem(IDC_SLIDER_TIME));
	ctl_txt_curframe_.Attach(GetDlgItem(IDC_TXT_CURFRAME));
	ctl_txt_frames_.Attach(GetDlgItem(IDC_TXT_FRAMES));
	ctl_slider_frame_.Attach(GetDlgItem(IDC_SLIDER_FRAME));
	ctl_txt_cursorpos_.Attach(GetDlgItem(IDC_TXT_CURSORPOS));

	ctl_btn_play_.Attach(GetDlgItem(IDC_BTN_PLAY));

	ctl_chk_back_.Attach(GetDlgItem(IDC_CHK_BACK));
	ctl_chk_invisible_.Attach(GetDlgItem(IDC_CHK_INVISIBLE));
	ctl_chk_update_.Attach(GetDlgItem(IDC_CHK_UPDATE));
	ctl_chk_visible_.Attach(GetDlgItem(IDC_CHK_VISIBLE));

	ctl_chk_region_fill_.Attach(GetDlgItem(IDC_CHK_RGNFILL));
	ctl_chk_image_erase_.Attach(GetDlgItem(IDC_CHK_IMAGEERASE));
	ctl_chk_area_refline_.Attach(GetDlgItem(IDC_CHK_AREALINE));
	ctl_chk_scale_.Attach(GetDlgItem(IDC_CHK_SCALE));

	ctl_cmb_playrate_.Attach(GetDlgItem(IDC_CMB_PLAYRATE));

	ctl_tab_panels_.Attach(GetDlgItem(IDC_TAB_PANELS));

	dlg_thumbs_.Create(ctl_tab_panels_);
	//dlg_thumbs_.PostMessage(WM_SET_THUMBS, 0, (LPARAM)nearby_frame_bitmaps_);

	dlg_info_.Create(ctl_tab_panels_);
	//dlg_record_.Create(ctl_tab_panels_);

	dlg_image_.Create(*this);
	dlg_image_.ShowWindow(SW_NORMAL);

	ctl_btn_main_.Attach(GetDlgItem(IDC_PIC_MAIN));

	CFlowLayout* layout_top = layout_main_.AddFlow(NSYedaoqLayout::Direction_Horizon);

	layout_top->AddCtrl(ctl_cmb_mettings_.m_hWnd);
	layout_top->AddZoomBlank();
	layout_top->AddCtrl(GetDlgItem(IDC_BTN_LOGDIR));
	layout_top->AddCtrl(GetDlgItem(IDC_BTN_EXPORTIMAGE));
	layout_top->AddCtrl(GetDlgItem(IDC_BTN_EXPORTFRAME));
	layout_top->AddCtrl(GetDlgItem(IDOK));

	CFlowLayout* layout_second = layout_main_.AddFlow(NSYedaoqLayout::Direction_Horizon);
	layout_second->AddCtrl(ctl_btn_play_);
	layout_second->AddCtrl(ctl_slider_frame_, ResizeInfo::FillInfo);
	layout_second->AddCtrl(ctl_txt_curframe_, ResizeInfo::FixedInfo, ResizeInfo::FixedInfo, AnchorInfo::AnchorCenter, AnchorInfo::AnchorCenter);
	layout_second->AddCtrl(ctl_txt_frames_, ResizeInfo::FixedInfo, ResizeInfo::FixedInfo, AnchorInfo::AnchorCenter, AnchorInfo::AnchorCenter);

	CFlowLayout* layout_option = layout_main_.AddFlow(NSYedaoqLayout::Direction_Horizon);
	layout_option->AddZoomBlank();
	layout_option->AddCtrl(ctl_chk_scale_, ResizeInfo::FixedInfo, ResizeInfo::FixedInfo, AnchorInfo::AnchorCenter, AnchorInfo::AnchorCenter);
	layout_option->AddCtrl(ctl_chk_region_fill_, ResizeInfo::FixedInfo, ResizeInfo::FixedInfo, AnchorInfo::AnchorCenter, AnchorInfo::AnchorCenter);
	layout_option->AddCtrl(ctl_chk_image_erase_, ResizeInfo::FixedInfo, ResizeInfo::FixedInfo, AnchorInfo::AnchorCenter, AnchorInfo::AnchorCenter);
	layout_option->AddCtrl(ctl_cmb_playrate_);
	layout_option->AddCtrl(ctl_chk_area_refline_, ResizeInfo::FixedInfo, ResizeInfo::FixedInfo, AnchorInfo::AnchorCenter, AnchorInfo::AnchorCenter);
	layout_option->AddCtrl(ctl_chk_update_, ResizeInfo::FixedInfo, ResizeInfo::FixedInfo, AnchorInfo::AnchorCenter, AnchorInfo::AnchorCenter);
	layout_option->AddCtrl(ctl_chk_invisible_, ResizeInfo::FixedInfo, ResizeInfo::FixedInfo, AnchorInfo::AnchorCenter, AnchorInfo::AnchorCenter);
	layout_option->AddCtrl(ctl_chk_visible_, ResizeInfo::FixedInfo, ResizeInfo::FixedInfo, AnchorInfo::AnchorCenter, AnchorInfo::AnchorCenter);
	layout_option->AddCtrl(ctl_chk_back_, ResizeInfo::FixedInfo, ResizeInfo::FixedInfo, AnchorInfo::AnchorCenter, AnchorInfo::AnchorCenter);
	layout_option->AddCtrl(ctl_txt_cursorpos_, ResizeInfo::FixedInfo, ResizeInfo::FixedInfo, AnchorInfo::AnchorCenter, AnchorInfo::AnchorCenter);
	layout_option->AddCtrl(ctl_txt_frame_continue_, ResizeInfo::FixedInfo, ResizeInfo::FixedInfo, AnchorInfo::AnchorCenter, AnchorInfo::AnchorCenter);
	layout_option->AddCtrl(ctl_txt_curtime_, ResizeInfo::FixedInfo, ResizeInfo::FixedInfo, AnchorInfo::AnchorCenter, AnchorInfo::AnchorCenter);
	layout_option->AddCtrl(ctl_txt_time_, ResizeInfo::FixedInfo, ResizeInfo::FixedInfo, AnchorInfo::AnchorCenter, AnchorInfo::AnchorCenter);

	CFlowLayout* layout_third = layout_main_.AddFlow(NSYedaoqLayout::Direction_Horizon, ResizeInfo(Resize_Zoomed, 100));

	layout_third->AddCtrl(ctl_tab_panels_, ResizeInfo(Resize_Zoomed, 15), ResizeInfo::FillInfo);
	layout_third->AddCtrl(dlg_image_, ResizeInfo(Resize_Zoomed, 85), ResizeInfo::FillInfo);

	RECT rc;
	GetClientRect(&rc);
	layout_main_.Layout(LayoutPoint::Zero, LayoutSize(rc.right, rc.bottom));

	//InitLogBlockBuffer();
	InitFileList();
	InitTabPanels();

	ctl_cmb_mettings_.SetWindowText(TEXT("select log file..."));

	int play_rates[] = {1, 2, 4, 8, 16};
	for (int i = 0; i < ARRAYSIZE(play_rates); ++i)
	{
		TCHAR buf[30];
		_stprintf_s(buf, ARRAYSIZE(buf) - 1, TEXT("%d倍速"), play_rates[i]);
		ctl_cmb_playrate_.AddString(buf);
	}

	ctl_cmb_playrate_.SetCurSel(0);
	ctl_btn_play_.SetIcon(icon_play_);	

	return TRUE;
}

LRESULT CASImgView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = TRUE;
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	ctl_tab_panels_.DeleteAllItems();
	
	if (NULL != dc_memory_)
	{
		DeleteDC(dc_memory_);
		dc_memory_ = NULL;
	}

	return 0;
}

LRESULT CASImgView::OnTimer( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
{
	bHandled = TRUE;
	return 0;
}

LRESULT CASImgView::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	if (SIZE_MINIMIZED != wParam)
	{
		layout_main_.Layout(NSYedaoqLayout::LayoutPoint(6,6), NSYedaoqLayout::LayoutSize(LOWORD(lParam) - 12, HIWORD(lParam) - 6));
		InvalidateRect(NULL, TRUE);

		RECT rc;
		ctl_tab_panels_.GetClientRect(&rc);
		dlg_thumbs_.MoveWindow(0, 0, rc.right, rc.bottom - 20);
		dlg_info_.MoveWindow(0, 0, rc.right, rc.bottom - 20);
		//dlg_record_.MoveWindow(0, 0, rc.right, rc.bottom - 20);
	}

	return 0;
}

LRESULT CASImgView::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	bHandled = TRUE;
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CASImgView::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled)
{
	// TODO: Add validation code 
	bHandled = TRUE;
	CloseDialog(wID);
	return 0;
}

void CASImgView::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}

CASImgView::CASImgView()
	: layout_main_(NSYedaoqLayout::Direction_Vertical), record_package_(0)
{
	play_flag_ = false;
	play_thread_handle_ = INVALID_HANDLE_VALUE;

	log_file_latest_cursorimage_pos_ = 0;
	log_file_latest_mousepos_.x = 0;
	log_file_latest_mousepos_.y = 0;

}

void CASImgView::InitFileList()
{
	ctl_cmb_mettings_.SetCurSel(-1);
	ctl_cmb_mettings_.ResetContent();

	WCHAR path_buf[MAX_PATH];
	SHGetSpecialFolderPath(NULL, path_buf, CSIDL_APPDATA, FALSE);
	::PathCombine(img_log_dir_, path_buf, TEXT("Zoom\\logs"));
	::PathAppend(path_buf, _T("Zoom\\logs\\cpthost.exe-*-as.imgs"));

	_tcscpy_s(img_log_file_path_, path_buf);

	WIN32_FIND_DATA find_data;
	HANDLE find_handle = FindFirstFile(path_buf, &find_data);

	if (INVALID_HANDLE_VALUE != find_handle)
	{
		do 
		{
			ctl_cmb_mettings_.AddString(find_data.cFileName);

		} while (FindNextFile(find_handle, &find_data));

		FindClose(find_handle);
	}
}

LRESULT CASImgView::OnCmbMettingSelChanged(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled)
{
	bHandled = TRUE;

	WCHAR path_buf[MAX_PATH];
	ctl_cmb_mettings_.GetWindowText(path_buf, ARRAYSIZE(path_buf));
	
	if (0 != _tcsstr(img_log_file_path_, path_buf))
	{
		return 0;
	}

	::PathCombine(img_log_file_path_, img_log_dir_, path_buf);

	CloseASRecording();
	LoadASRecording(img_log_file_path_);

	ctl_slider_frame_.SetFocus();

	return 0;	
}

LRESULT CASImgView::OnCmbPlayrateSelChanged( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled )
{
	bHandled = TRUE;
	ctl_slider_frame_.SetFocus();
	if (play_flag_)
	{
		StopPlay();
		StartPlay();
	}

	return 0;
}

bool CASImgView::LoadASRecording( LPCTSTR file_path )
{
	CloseASRecording();

	record_package_ = new ASRecordFrameData_CptHost();
	if (!record_package_->LoadFrom(file_path))
	{
		ctl_status_bar_.SetTipText(0, TEXT("无法打开录像文件！"));
		return false;
	}
	else
	{
		ctl_status_bar_.SetTipText(0, TEXT("打开录像文件成功"));
	}

	current_frame_res_.SetRecordPackage(record_package_);

	ctl_txt_curtime_.SetWindowText(TEXT("00:00:00:0000"));
	ctl_slider_frame_.SetScrollRange(SB_CTL, 0, record_package_->GetFrameCount() - 1, FALSE);
	ctl_slider_timeline_.SetPos(0);

	dlg_image_.SetFrameRes(&current_frame_res_);
	dlg_info_.SetFrameRes(&current_frame_res_);
	dlg_thumbs_.SetFrameRes(&current_frame_res_);
	//dlg_image_.SetASRecordPackage(record_package_);
	UpdateTimeLine();
	SetCurrentFrame(0, true);

	return true;
}

void CASImgView::CloseASRecording()
{
	KillTimer(TIMER_ID_LOADFRAME);
	if (record_package_)
	{
		delete record_package_;
		record_package_ = 0;
	}
}

bool CASImgView::UpdateTimeLine()
{
	if(!record_package_)
		return false;

	metting_duration_miniseconds_ = record_package_->GetRecordDurationMiniseconds();
	metting_duration_ = time_duration::FromMiniseconds(metting_duration_miniseconds_);

	TCHAR buf[128];
	_stprintf_s(buf, ARRAYSIZE(buf), TEXT("/ %02d:%02d:%02d"), (int)metting_duration_.hour, (int)metting_duration_.minute, (int)metting_duration_.seconds);
	ctl_txt_time_.SetWindowText(buf);

	ctl_slider_timeline_.SetRange(0, (DWORD)(metting_duration_miniseconds_ / 1000));
	ctl_slider_timeline_.SetPageSize(15);
	ctl_slider_timeline_.SetLineSize(1);

	ctl_slider_timeline_.SetPos(20);
	ctl_slider_timeline_.SetTicFreq(15);

	ctl_slider_timeline_.GetToolTips();

	_stprintf_s(buf, ARRAYSIZE(buf), TEXT("/ %06d"), record_package_->GetFrameCount());
	ctl_txt_frames_.SetWindowText(buf);
	ctl_txt_curframe_.SetWindowText(TEXT("000000"));

	ctl_slider_frame_.SetRange(0, record_package_->GetFrameCount() - 1);
	ctl_slider_frame_.SetLineSize(1);
	ctl_slider_frame_.SetPageSize(50);

	return true;
}

LRESULT CASImgView::OnHScroll( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;
	if (!record_package_)
	{
		return 0;
	}

	if ((HWND)lParam == (HWND)ctl_slider_frame_)
	{
		int frame_index = ctl_slider_frame_.GetPos();
		SetCurrentFrame(frame_index, true);
	}
	else if ((HWND)lParam == (HWND)ctl_slider_timeline_)
	{
		int frame_index = record_package_->GetFrameOfTime(ctl_slider_timeline_.GetPos());
		SetCurrentFrame(frame_index, true);
	}

	return 0;
}

LRESULT CASImgView::OnVScroll( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;
	return 0;
}

LRESULT CASImgView::OnDrawItem( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;
	return 0;
}

void CASImgView::SetCurrentFrame( int frame_index, bool refresh_panel )
{
	if (current_frame_res_.GetFrameIndex() == frame_index)
	{
		ctl_status_bar_.SetTipText(1, TEXT("帧索引未变化！"));
		return;
	}
	 
	current_frame_res_.UpdateForFrame(frame_index);
	current_frame_index_ = frame_index;

	ctl_slider_frame_.SetPos(frame_index);

	time_duration frame_time = time_duration::FromMiniseconds(current_frame_res_.FrameTime());
	
	TCHAR buf[128];
	_stprintf_s(buf, ARRAYSIZE(buf), TEXT("%02d:%02d:%02d:%04d"), (int)frame_time.hour, (int)frame_time.minute, (int)frame_time.seconds, (int)frame_time.miniseconds);
	ctl_txt_curtime_.SetWindowText(buf);

	_stprintf_s(buf, ARRAYSIZE(buf), TEXT("%06d"), frame_index);
	ctl_txt_curframe_.SetWindowText(buf);

	if (frame_index < record_package_->GetFrameCount() - 1)
	{
		__int64 next_frame_time = record_package_->GetFrameTime(frame_index + 1);
		time_duration continue_time = time_duration::FromMiniseconds(next_frame_time - current_frame_res_.FrameTime());
		_stprintf_s(buf, ARRAYSIZE(buf), TEXT("%02d:%02d:%04d"), (int)continue_time.minute, (int)continue_time.seconds, (int)continue_time.miniseconds);
		ctl_txt_frame_continue_.SetWindowText(buf);
	}
	else
	{
		ctl_txt_frame_continue_.SetWindowText(TEXT("00:00:0000"));
	}
	
	if (refresh_panel)
	{
		BOOL tmp;
		OnTabPanelSwitched(0, 0, tmp);
	}

	dlg_image_.Refresh();
}

void CASImgView::DisplayImageInStatic( HBITMAP hbp, DWORD ctl_id )
{
	HWND ctl = GetDlgItem(ctl_id);
	RECT rc;
	
	::GetClientRect(ctl, &rc);
	HDC dc = ::GetDC(ctl);

	SelectObject(dc_memory_, hbp);

	BITMAP bitmap;
	GetObject(hbp, sizeof(bitmap), &bitmap);

	SetStretchBltMode(dc,STRETCH_HALFTONE); 
	StretchBlt(dc, 0, 0, rc.right, rc.bottom, dc_memory_, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);

	::ReleaseDC(ctl, dc);

}

LRESULT CASImgView::OnBtnRegion( WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	bHandled = TRUE;

	WTL::CButton btn(hWndCtl);
	bool checked = (0 != btn.GetCheck());

	switch(wID)
	{
	case IDC_CHK_UPDATE:
		dlg_image_.SetRegionShow(REGION_UPDATE, checked);
		break;
	case IDC_CHK_INVISIBLE:
		dlg_image_.SetRegionShow(REGION_INVISIBLE, checked);
		break;
	case IDC_CHK_VISIBLE:
		dlg_image_.SetRegionShow(REGION_VISIBLE, checked);
		break;
	case IDC_CHK_BACK:
		dlg_image_.SetRegionShow(REGION_BACK, checked);
		break;
	case IDC_CHK_RGNFILL:
		dlg_image_.SetRegionFill(checked);
		break;
	}
	ctl_slider_frame_.SetFocus();
	return 0;
}

LRESULT CASImgView::OnBtnImageErase( WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	bHandled = TRUE;
	dlg_image_.SetImageErase(0 != ctl_chk_image_erase_.GetCheck());
 	ctl_slider_frame_.SetFocus();
	return 0;
}

LRESULT CASImgView::OnBtnRegionFill( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled )
{
	bHandled = TRUE;
	dlg_image_.SetRegionFill(0 != ctl_chk_region_fill_.GetCheck());
	ctl_slider_frame_.SetFocus();
	return 0;
}

LRESULT CASImgView::OnBtnAreaRefLine( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled )
{
	bHandled = TRUE;
	dlg_image_.SetAreaRefLineShow(0 != ctl_chk_area_refline_.GetCheck());
	ctl_slider_frame_.SetFocus();
	return 0;
}

LRESULT CASImgView::OnBtnPlay( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled )
{
	bHandled = TRUE;
	if (!play_flag_)
	{
		StartPlay();
	}
	else
	{
		StopPlay();
	}

	ctl_slider_frame_.SetFocus();

	return 0;
}

void CASImgView::StartPlay()
{
	if (record_package_)
	{
		play_flag_ = true;
		
		if (INVALID_HANDLE_VALUE == play_thread_handle_)
		{
			play_thread_handle_ = (HANDLE)_beginthread(ThreadEntry4Play, 0, this);
			SetThreadPriority(play_thread_handle_, THREAD_PRIORITY_ABOVE_NORMAL);
		}

		ctl_btn_play_.SetIcon(icon_pause_);
	}
}

void CASImgView::StopPlay()
{
	play_flag_ = false;
	ctl_btn_play_.SetIcon(icon_play_);
}

void CASImgView::ThreadEntry4Play( void* params)
{
	CASImgView* dlg = (CASImgView*)params;
	dlg->LoopPlay();
}

void CASImgView::LoopPlay()
{
	LARGE_INTEGER counter_frequency;
	QueryPerformanceFrequency(&counter_frequency);

	while(true)
	{
		while(!play_flag_) Sleep(200);

		play_target_frame_index_ = current_frame_index_;
		while (play_flag_ && (unsigned int)play_target_frame_index_ < record_package_->GetFrameCount() - 1)
		{
			__int64 filetime_duration = record_package_->GetFrameTime(play_target_frame_index_ + 1) - record_package_->GetFrameTime(play_target_frame_index_);
			Sleep((DWORD)filetime_duration);

			++play_target_frame_index_;
			PostMessage(WM_PLAY_NEXTFRAME, 0, 0);
		}

		if (play_flag_)
		{
			StopPlay();
		}
	}
}

LRESULT CASImgView::OnPlayNextFrame( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
{
	bHandled = TRUE;
	
	if (current_frame_index_ != play_target_frame_index_)
	{
		SetCurrentFrame(play_target_frame_index_, false);
	}
	else
	{
		OutputDebugStringA("OnPlayNextFrame ignore frame!\r\n");
	}
	return 0;
}

void CASImgView::InitTabPanels()
{
	TCITEM item;
	memset(&item, 0, sizeof(item));
	item.mask = TCIF_TEXT;

	item.pszText = TEXT("thumbs");
	item.cchTextMax = 6;
	ctl_tab_panels_.AddItem(&item);

	item.pszText = TEXT("info");
	item.cchTextMax = 4;
	ctl_tab_panels_.AddItem(&item);

// 	item.pszText = TEXT("record");
// 	item.cchTextMax = _tcslen(item.pszText);
// 	ctl_tab_panels_.AddItem(&item);
}

LRESULT CASImgView::OnTabPanelSwitched( int wParam, LPNMHDR lpnh, BOOL& bHandled )
{
	bHandled = TRUE;

	HWND panels[] = {dlg_thumbs_, dlg_info_/*, dlg_record_*/};
	int cur_sel = ctl_tab_panels_.GetCurSel();
	for (int i = 0; i < ARRAYSIZE(panels); ++i)
	{
		::ShowWindow(panels[i], (cur_sel == i) ? SW_SHOWNORMAL : SW_HIDE);
	}

	if (record_package_)
	{
		::PostMessage(panels[cur_sel], WM_REFRESH_PANEL, 0, 0);
	}
	
	ctl_slider_frame_.SetFocus();

	return 0;
}

LRESULT CASImgView::OnBtnExportFrame( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled )
{
	bHandled = TRUE;
	TCHAR buf[MAX_PATH * 2];
	make_export_file_path(current_frame_index_, buf);
	throw 1;
	//save_to_bmp_file(buf, &log_block_screen_data_->bi, log_block_screen_data_->get_image_bits());
	ctl_slider_frame_.SetFocus();
	return 0;
	
}

LRESULT CASImgView::OnBtnExportImage( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled )
{
	bHandled = TRUE;
	TCHAR buf[MAX_PATH * 2];
	make_export_file_path(current_frame_index_, buf);
	save_to_bmp_file1(buf, dlg_image_.GetImage());
	ctl_slider_frame_.SetFocus();
	return 0;
}

LRESULT CASImgView::OnBtnLogDir( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled )
{
	bHandled = TRUE;
	WCHAR path_buf[MAX_PATH * 2];
	GetProductLogDir(path_buf, ARRAYSIZE(path_buf));
	::ShellExecute(NULL, TEXT("explore"), path_buf, NULL, NULL, SW_SHOWNORMAL);
	ctl_slider_frame_.SetFocus();
	return 0;
}

LRESULT CASImgView::OnSetFocusToSliderFrame( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
{
	bHandled = TRUE;
	ctl_slider_frame_.SetFocus();
	return 0;
}

LRESULT CASImgView::OnSetTxtCursorPos( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled =TRUE;
	TCHAR buf[64];
	_stprintf_s(buf, ARRAYSIZE(buf), TEXT("POS [%d, %d]"), (int)LOWORD(lParam), (int)HIWORD(lParam));
	ctl_txt_cursorpos_.SetWindowText(buf);
	return 0;
}

LRESULT CASImgView::OnBtnScale( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled )
{
	bHandled = TRUE;
	dlg_image_.SetScale(0 != ctl_chk_scale_.GetCheck());
	ctl_slider_frame_.SetFocus();
	return 0;
}

LRESULT CASImgView::OnPlaySwitch( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
{
	OnBtnPlay(0, 0, 0, bHandled);
	return 0;
}

LRESULT CASImgView::OnDropFiles( UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled )
{
	bHandled = TRUE;
	TCHAR file_path[MAX_PATH * 2];
	if(DragQueryFile((HDROP)wParam, 0, file_path, ARRAYSIZE(file_path)) > 0)
	{
		CloseASRecording();
		if(LoadASRecording(file_path))
			_tcscpy(img_log_file_path_, file_path);

		ctl_slider_frame_.SetFocus();
	}
	
	return 0;
}

void CASImgView::SetStatusBar( HWND wnd )
{
	ctl_status_bar_.Attach(wnd);
}


