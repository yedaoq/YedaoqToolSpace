// ASImgView.cpp : implementation of the CASImgView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "ASImgView.h"
#include <math.h>
//#include "cpt_util.h"

enum { TIMER_ID_LOADFRAME = 1001, TIMER_ID_PLAY };

WCHAR				img_log_dir_[MAX_PATH * 2] = TEXT("");
WCHAR				img_log_file_path_[MAX_PATH * 2] = TEXT("");
HANDLE				img_log_file_handle_ = INVALID_HANDLE_VALUE;
LARGE_INTEGER		log_file_read_pointer_;

HICON				current_frame_cursor_image_ = NULL;
POINT				current_frame_mouse_position_ = {0, 0};

char*				log_block_buf_ = 0;
int					log_block_buf_size_ = 0;
cpt_common*			log_block_data_header_ = 0;
cpt_screen_data*	log_block_screen_data_ = 0;
int					log_block_frame_index_ = -1;

__inline int align_data_32(int width, int bits)
{
	return ((width*bits)+31)/32*4;
}

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


HBITMAP	LoadFrameImage(cpt_screen_data* data)
{
	BITMAPINFOHEADER bih = data->bi.bmiHeader;
	HBITMAP hbp = CreateBitmap(bih.biWidth, bih.biHeight < 0 ? -bih.biHeight : bih.biHeight, bih.biPlanes, bih.biBitCount, data->get_image_bits());
	return hbp;
}

time_duration time_duration::FromMiniseconds( LONGLONG miniseconds )
{
	LONGLONG seconds = miniseconds / 1000;
	time_duration result = FromSeconds((LONG)seconds);
	result.miniseconds = miniseconds % 1000;
	return result;
}

time_duration time_duration::FromSeconds( LONG seconds )
{
	time_duration result;
	result.miniseconds = 0;
	result.seconds = seconds % 60;
	LONG minutes = seconds / 60;
	result.minute = minutes % 60;
	result.hour = (unsigned short)(minutes / 60);
	return result;
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
	icon_pause_ = (HICON)LoadImage(module_handle, MAKEINTRESOURCE(IDI_ASIMG_PLAY), IMAGE_ICON, 16, 16, LR_SHARED); // LoadIcon(module_handle, MAKEINTRESOURCE(IDI_PAUSE));
	icon_play_ = (HICON)LoadImage(module_handle, MAKEINTRESOURCE(IDI_ASIMG_PLAY), IMAGE_ICON, 16, 16, LR_SHARED); // LoadIcon(module_handle, MAKEINTRESOURCE(IDI_PLAY));

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

	DragAcceptFiles(TRUE);

	img_frames_.reserve(10000);

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

	ctl_scroll_h_.Attach(GetDlgItem(IDC_SCROLL_H));
	ctl_scroll_v_.Attach(GetDlgItem(IDC_SCROLL_V));

	dlg_thumbs_.Create(ctl_tab_panels_);
	dlg_thumbs_.PostMessage(WM_SET_THUMBS, 0, (LPARAM)nearby_frame_bitmaps_);

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

	InitLogBlockBuffer();
	InitMeetingList();
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

	free(log_block_buf_);
	log_block_buf_ = NULL;

	if (INVALID_HANDLE_VALUE != img_log_file_handle_)
	{
		CloseHandle(img_log_file_handle_);
		img_log_file_handle_ = INVALID_HANDLE_VALUE;
	}
	
	if (NULL != dc_memory_)
	{
		DeleteDC(dc_memory_);
		dc_memory_ = NULL;
	}

	return 0;
}

// LRESULT CASImgView::OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
// {
// 	bHandled = TRUE;
// 
// 	HWND	hwnd_focus = GetFocus();
// 	TCHAR	hwnd_class[128];
// 	if (NULL != hwnd_focus)
// 	{
// 		GetClassName(hwnd_focus, hwnd_class, ARRAYSIZE(hwnd_class));
// 	}
// 
// 	bHandled = TRUE;
// 	switch(wParam)
// 	{
// 	case VK_SPACE:
// 	case VK_LEFT:
// 	case VK_RIGHT:
// 		if (_tcscmp(hwnd_class, TEXT("")))
// 		{
// 			ctl_slider_frame_.SetFocus();
// 			return 1;
// 		}
// 	}
// 
// 	return 1;
// }

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

// 		ctl_pic_main_.GetClientRect(&rc);
// 		int tmp = 4096 - rc.right;
// 		ctl_scroll_h_.SetScrollRange(0, tmp > 0 ? tmp : 0, TRUE);
// 		tmp = 2160 - rc.bottom;
// 		ctl_scroll_v_.SetScrollRange(0, tmp > 0 ? tmp : 0, TRUE);

		//ctl_scroll_v_.SetScrollPos(300, TRUE);
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
	: layout_main_(NSYedaoqLayout::Direction_Vertical)
{
	memset(nearby_frame_bitmaps_, 0, sizeof(nearby_frame_bitmaps_));
	play_flag_ = false;
	play_thread_handle_ = INVALID_HANDLE_VALUE;
	current_frame_cursor_image_pos_ = 0;
	current_frame_cursor_image_ = 0;

	log_file_latest_cursorimage_pos_ = 0;
	log_file_latest_mousepos_.x = 0;
	log_file_latest_mousepos_.y = 0;

	nearby_frame_index_base_ = -100;
}

void CASImgView::InitMeetingList()
{
	ctl_cmb_mettings_.SetCurSel(-1);
	ctl_cmb_mettings_.Clear();

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

	CloseImgLogFile();
	LoadImgLogFile(img_log_file_path_);

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

bool CASImgView::LoadImgLogFile( LPCTSTR file_path )
{
	if (INVALID_HANDLE_VALUE != img_log_file_handle_)
	{
		CloseImgLogFile();
	}

	bool alive_metting_log_flag = false;
	img_log_file_handle_ = CreateFile(file_path, FILE_READ_DATA, 0, NULL, OPEN_ALWAYS, NULL, NULL);
	if (INVALID_HANDLE_VALUE == img_log_file_handle_)
	{
		alive_metting_log_flag = true;
		img_log_file_handle_ = CreateFile(file_path, FILE_READ_DATA, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, NULL, NULL);
	}

	log_file_read_pointer_.QuadPart = 0;
	current_frame_index_ = -1;
	img_frames_.clear();

	ctl_txt_curtime_.SetWindowText(TEXT("00:00:00:0000"));
	ctl_slider_frame_.SetScrollRange(SB_CTL, 0, img_frames_.size() - 1, FALSE);
	ctl_slider_timeline_.SetPos(0);

	if (!LoadImgLogContent())
	{
		return false;
	}

	if (alive_metting_log_flag)
	{
		SetTimer(TIMER_ID_LOADFRAME, 500, NULL);
	}

	dlg_image_.OnMettingLoaded();
	SetCurrentFrame(0, true);

	return true;
}

void CASImgView::CloseImgLogFile()
{
	KillTimer(TIMER_ID_LOADFRAME);

	if (INVALID_HANDLE_VALUE != img_log_file_handle_)
	{
		CloseHandle(img_log_file_handle_);
		img_log_file_handle_ = INVALID_HANDLE_VALUE;

		log_block_frame_index_ = -1;
	}
}

bool CASImgView::LoadImgLogContent()
{
	img_frame_info info;
	file_frame_header header;

	SetFilePointerEx(img_log_file_handle_, log_file_read_pointer_, NULL, FILE_BEGIN);

	DWORD	byte_readed = 0;
	while(true)
	{
		ReadFile(img_log_file_handle_, &header, sizeof(header), &byte_readed, NULL);
		if (byte_readed < sizeof(header)/* || header.common.size > 100000000*/)
		{
			break;
		}

		switch(header.common.data_type)
		{
		case cpt_dt_display_device:
			break;
		case cpt_dt_screen_capture:
			info.frame_data_pos.QuadPart = log_file_read_pointer_.QuadPart + sizeof(FILETIME);
			info.frame_time = header.frame_time;
			info.cursur_image_pos.QuadPart = log_file_latest_cursorimage_pos_;
			info.mouse_pos = log_file_latest_mousepos_;
			img_frames_.push_back(info);
			break;
		case cpt_dt_mouse_pointer:
			log_file_latest_cursorimage_pos_ = log_file_read_pointer_.QuadPart + sizeof(FILETIME);
			break;
		case cpt_dt_mouse_pos:
			log_file_latest_mousepos_ = ReadMousePos(log_file_read_pointer_.QuadPart + sizeof(FILETIME));
			break;
		default:
			int x= 0;
			break;
		}

		LONG block_size = header.common.size + sizeof(FILETIME);
		log_file_read_pointer_.QuadPart += block_size;

		if (!SetFilePointerEx(img_log_file_handle_, log_file_read_pointer_, NULL, FILE_BEGIN))
		{
			log_file_read_pointer_.QuadPart -= block_size;
			break;
		}
	}

	if (img_frames_.size() <= 0)
	{
		return false;
	}

	UpdateTimeLine();
	return true;
}

bool CASImgView::UpdateTimeLine()
{
	if(img_frames_.size() < 0)
		return false;

	metting_duration_miniseconds_ = (((LARGE_INTEGER&)img_frames_.rbegin()->frame_time).QuadPart - ((LARGE_INTEGER&)img_frames_.begin()->frame_time).QuadPart) / 10000;
	metting_duration_ = time_duration::FromMiniseconds(metting_duration_miniseconds_);

	TCHAR buf[128];
	_stprintf_s(buf, ARRAYSIZE(buf), TEXT("/ %02d:%02d:%02d"), (int)metting_duration_.hour, (int)metting_duration_.minute, (int)metting_duration_.seconds);
	ctl_txt_time_.SetWindowText(buf);

	int range_lowbound, range_upperbound;
	ctl_slider_timeline_.GetRange(range_lowbound, range_upperbound);

	ctl_slider_timeline_.SetRange(0, (DWORD)(metting_duration_miniseconds_ / 1000));
	ctl_slider_timeline_.SetPageSize(15);
	ctl_slider_timeline_.SetLineSize(1);

	ctl_slider_timeline_.SetPos(20);
	ctl_slider_timeline_.SetTicFreq(15);

	ctl_slider_timeline_.GetToolTips();

	_stprintf_s(buf, ARRAYSIZE(buf), TEXT("/ %06d"), img_frames_.size());
	ctl_txt_frames_.SetWindowText(buf);
	ctl_txt_curframe_.SetWindowText(TEXT("000000"));

	ctl_slider_frame_.SetRange(0, img_frames_.size() - 1);
	ctl_slider_frame_.SetLineSize(1);
	ctl_slider_frame_.SetPageSize(50);

	return true;
}

int CASImgView::GetImageFrameIndexOfSliderPos( int pos )
{
	__int64 frame_time = img_frames_.begin()->frmae_time_i64 + pos * 10000 * 1000;

	unsigned int index = (unsigned int)((__int64)img_frames_.size() * (pos * 1000) / metting_duration_miniseconds_);
	if (index >= img_frames_.size())
	{
		index = img_frames_.size() - 1;
	}

	if (img_frames_[index].frmae_time_i64 > frame_time)
	{
		while(img_frames_[index].frmae_time_i64 > frame_time && index >= 0)
		{
			--index;
		}
	}
	else
	{
		for (++index; img_frames_[index].frmae_time_i64 <= frame_time && index < img_frames_.size(); ++index);
		--index;
	}

	return (int)index;
}

LRESULT CASImgView::OnHScroll( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;
	if (INVALID_HANDLE_VALUE == img_log_file_handle_)
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
		int frame_index = GetImageFrameIndexOfSliderPos(ctl_slider_timeline_.GetPos());
		SetCurrentFrame(frame_index, true);
	}
// 	else if((HWND)lParam == ctl_scroll_h_)
// 	{
// 		switch(LOWORD(wParam))
// 		{
// 		case SB_THUMBTRACK:
// 		case SB_THUMBPOSITION:
// 			image_main_scroll_x_ = HIWORD(wParam);
// 			ctl_scroll_h_.SetScrollPos(image_main_scroll_x_);
// 			ctl_pic_main_.Invalidate(TRUE);
// 			break;
// 		case SB_ENDSCROLL:
// 			ctl_slider_frame_.SetFocus();
// 			break;
// 		}
// 	}

	return 0;
}

LRESULT CASImgView::OnVScroll( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;
// 	if((HWND)lParam == ctl_scroll_v_)
// 	{
// 		switch(LOWORD(wParam))
// 		{
// 		case SB_THUMBTRACK:
// 		case SB_THUMBPOSITION:
// 			image_main_scroll_y_ = HIWORD(wParam);
// 			ctl_scroll_v_.SetScrollPos(image_main_scroll_y_);
// 			ctl_pic_main_.Invalidate(TRUE);
// 			break;
// 		case SB_ENDSCROLL:
// 			ctl_slider_frame_.SetFocus();
// 			break;
// 		}
// 	}
	return 0;
}

LRESULT CASImgView::OnDrawItem( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;

//	LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;

// 	switch(wParam)
// 	{
// 	case IDC_PIC_MAIN:
// 		DrawMainPic(lpDrawItem, image_main_);
// 		break;
// 	}
	return 0;
}

void CASImgView::SetCurrentFrame( int frame_index, bool refresh_panel )
{
	if (frame_index < 0 || (unsigned int)frame_index >= img_frames_.size() || frame_index == current_frame_index_)
	{
		return;
	}

	current_frame_index_ = frame_index;
	log_block_frame_index_ = frame_index;
	ctl_slider_frame_.SetPos(frame_index);

	if (refresh_panel)
	{
		BuildNearbyImages(current_frame_index_ - 3);
	}

	time_duration frame_time = time_duration::FromMiniseconds((img_frames_[frame_index].frmae_time_i64 - img_frames_.begin()->frmae_time_i64) / 10000);
	
	TCHAR buf[128];
	_stprintf_s(buf, ARRAYSIZE(buf), TEXT("%02d:%02d:%02d:%04d"), (int)frame_time.hour, (int)frame_time.minute, (int)frame_time.seconds, (int)frame_time.miniseconds);
	ctl_txt_curtime_.SetWindowText(buf);

	_stprintf_s(buf, ARRAYSIZE(buf), TEXT("%06d"), frame_index);
	ctl_txt_curframe_.SetWindowText(buf);

	if ((unsigned int)frame_index < img_frames_.size() - 1)
	{
		time_duration continue_time = time_duration::FromMiniseconds((img_frames_[frame_index + 1].frmae_time_i64 - img_frames_[frame_index].frmae_time_i64) / 10000);
		_stprintf_s(buf, ARRAYSIZE(buf), TEXT("%02d:%02d:%04d"), (int)continue_time.minute, (int)continue_time.seconds, (int)continue_time.miniseconds);
		ctl_txt_frame_continue_.SetWindowText(buf);
	}
	else
	{
		ctl_txt_frame_continue_.SetWindowText(TEXT("00:00:0000"));
	}

	// 加载鼠标数据
	current_frame_mouse_position_ = img_frames_[frame_index].mouse_pos;
	if (current_frame_cursor_image_pos_ != img_frames_[frame_index].cursur_image_pos.QuadPart)
	{
		ReadCursorImage(img_frames_[frame_index].cursur_image_pos.QuadPart);
	}
	
	// 重新加载当前帧
	if (!LoadFrameData(frame_index))
	{
		MessageBox(TEXT("无法获取此帧数据！"), TEXT("提示"), MB_OK);
		return;
	}
	
	if (refresh_panel)
	{
		BOOL tmp;
		OnTabPanelSwitched(0, 0, tmp);
	}

	//dlg_image_.SetMousePos(&img_frames_[frame_index].mouse_pos);
	dlg_image_.Refresh();
}

HBITMAP CASImgView::LoadFrameImage( int frame_index )
{
	if (LoadFrameData(frame_index))
	{
		cpt_screen_data* screen_data = (cpt_screen_data*)log_block_buf_;
		BITMAPINFOHEADER bih = screen_data->bi.bmiHeader;
		HBITMAP hbp = CreateBitmap(bih.biWidth, bih.biHeight < 0 ? -bih.biHeight : bih.biHeight, bih.biPlanes, bih.biBitCount, screen_data->get_image_bits());
		return hbp;
	}

	return NULL;
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

// void CASImgView::RefreshThumbs()
// {
// 	DWORD ctl_thumb_ids[] = {IDC_PIC_THUMB1, IDC_PIC_THUMB2, IDC_PIC_THUMB3, IDC_PIC_THUMB4, IDC_PIC_THUMB5, IDC_PIC_THUMB6, IDC_PIC_THUMB7 };
// 	for (int i = 0; i < ARRAYSIZE(nearby_frame_bitmaps_); ++i)	{
// 		DisplayImageInStatic(nearby_frame_bitmaps_[i], ctl_thumb_ids[i]);
// 		//::SendMessage(GetDlgItem(ctl_thumb_ids[i]), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)nearby_frame_bitmaps_[i]);
// 	}
// }

// void CASImgView::DrawOwnerDrawCtl( LPDRAWITEMSTRUCT lpDrawItem, HBITMAP hbp )
// {
// 	if (NULL == hbp)
// 	{
// 		return;
// 	}
// 
// 	HGLOBAL prev_obj = SelectObject(dc_memory_, hbp);
// 
// 	BITMAP bitmap;
// 	GetObject(hbp, sizeof(bitmap), &bitmap);
// 
// 	SetStretchBltMode(lpDrawItem->hDC, STRETCH_HALFTONE); 
// 	StretchBlt(lpDrawItem->hDC, lpDrawItem->rcItem.left, lpDrawItem->rcItem.top, lpDrawItem->rcItem.right, lpDrawItem->rcItem.bottom, dc_memory_, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
// 	SelectObject(prev_obj);
// }

bool CASImgView::LoadFrameData( int frame_index )
{
	LARGE_INTEGER frame_pos = img_frames_[frame_index].frame_data_pos;

	SetFilePointerEx(img_log_file_handle_, frame_pos, NULL, FILE_BEGIN);

	DWORD bytes_readed = 0;
	ReadFile(img_log_file_handle_, log_block_buf_, sizeof(cpt_common), &bytes_readed, NULL);
	if (bytes_readed < sizeof(cpt_common))
	{
		return false;
	}

	if (cpt_dt_screen_capture == log_block_data_header_->data_type)
	{
		SetFilePointerEx(img_log_file_handle_, frame_pos, NULL, FILE_BEGIN);
		ReadFile(img_log_file_handle_, log_block_buf_, log_block_data_header_->size, &bytes_readed, NULL);
		if (bytes_readed < log_block_data_header_->size)
		{
			return false;
		}
	}

	return true;
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
// 	render_image_erase_ = ctl_chk_image_erase_.GetCheck();
 	ctl_slider_frame_.SetFocus();
	return 0;
}

LRESULT CASImgView::OnBtnRegionFill( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled )
{
	bHandled = TRUE;
	dlg_image_.SetRegionFill(0 != ctl_chk_region_fill_.GetCheck());
	//render_region_fill_ = (0 != ctl_chk_region_fill_.GetCheck());
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

void CASImgView::BuildNearbyImages( int first_frame_index )
{
	int prev_nearby_frame_base_index = nearby_frame_index_base_;
	HBITMAP	prev_nearby_bitmaps[7];
	memcpy(prev_nearby_bitmaps, nearby_frame_bitmaps_, sizeof(prev_nearby_bitmaps));

	nearby_frame_index_base_ = first_frame_index;

	int reload_range_lbound = 0;
	int reload_range_ubound = ARRAYSIZE(nearby_frame_bitmaps_);

	for (int idx = 0; idx < ARRAYSIZE(prev_nearby_bitmaps); ++idx)
	{
		int nearby_frame_index = nearby_frame_index_base_ + idx;
		if (nearby_frame_index >= 0 && (unsigned int)nearby_frame_index < img_frames_.size())
		{
			if(nearby_frame_index >= prev_nearby_frame_base_index && nearby_frame_index < (prev_nearby_frame_base_index + 7/* ARRAYSIZE(prev_nearby_bitmaps)*/) 
				&& NULL != prev_nearby_bitmaps[nearby_frame_index - prev_nearby_frame_base_index])
			{
				nearby_frame_bitmaps_[idx] = prev_nearby_bitmaps[nearby_frame_index - prev_nearby_frame_base_index];
				prev_nearby_bitmaps[nearby_frame_index - prev_nearby_frame_base_index] = NULL;
			}
			else
			{
				nearby_frame_bitmaps_[idx] = LoadFrameImage(nearby_frame_index);
			}
		}
		else
		{
			nearby_frame_bitmaps_[idx] = NULL;
		}
	}

	for (int i = 0; i < ARRAYSIZE(prev_nearby_bitmaps); ++i)
	{
		if (NULL != prev_nearby_bitmaps[i])
		{
			DeleteObject(prev_nearby_bitmaps[i]);
		}
	}
}

void CASImgView::StartPlay()
{
	if (INVALID_HANDLE_VALUE != img_log_file_handle_)
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

// 		LARGE_INTEGER counter_begin;
// 		LARGE_INTEGER counter_now;
// 		QueryPerformanceCounter(&counter_begin);
// 
// 		__int64 filetime_first_frame = img_frames_.begin()->frmae_time_i64;

		play_target_frame_index_ = current_frame_index_;
		while (play_flag_ && (unsigned int)play_target_frame_index_ < img_frames_.size() - 1)
		{
// 			__int64 filetime_next_frame = img_frames_[play_target_frame_index_ + 1].frmae_time_i64;
// 			__int64 frame_duration_miniseconds = (filetime_next_frame - filetime_first_frame) / 10000;
// 			
// 			QueryPerformanceCounter(&counter_now);
// 			__int64 sleep_miniseconds = frame_duration_miniseconds - (counter_now.QuadPart - counter_begin.QuadPart) * 1000 / counter_frequency.QuadPart;
// 			if (sleep_miniseconds > 2)
// 			{
// 				Sleep(sleep_miniseconds);
// 			}

			__int64 filetime_duration = (img_frames_[play_target_frame_index_ + 1].frmae_time_i64 - img_frames_[play_target_frame_index_].frmae_time_i64) / 10000;
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

	item.pszText = TEXT("record");
	item.cchTextMax = _tcslen(item.pszText);
	ctl_tab_panels_.AddItem(&item);
}

// LRESULT CASImgView::OnTabPanelSwitched( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled )
// {
// 	bHandled = TRUE;
// 	HWND panels[] = {dlg_thumbs_, dlg_info_};
// 	int cur_sel = ctl_tab_panels_.GetCurSel();
// 	for (int i = 0; i < ARRAYSIZE(panels); ++i)
// 	{
// 		::ShowWindow(panels[i], (cur_sel == i) ? SW_SHOWMINIMIZED : SW_HIDE);
// 	}
// 
// 	return 0;
// }

LRESULT CASImgView::OnTabPanelSwitched( int wParam, LPNMHDR lpnh, BOOL& bHandled )
{
	bHandled = TRUE;

	HWND panels[] = {dlg_thumbs_, dlg_info_/*, dlg_record_*/};
	int cur_sel = ctl_tab_panels_.GetCurSel();
	for (int i = 0; i < ARRAYSIZE(panels); ++i)
	{
		::ShowWindow(panels[i], (cur_sel == i) ? SW_SHOWNORMAL : SW_HIDE);
	}

	if (INVALID_HANDLE_VALUE != img_log_file_handle_)
	{
		::PostMessage(panels[cur_sel], WM_REFRESH_PANEL, 0, (LPARAM)log_block_data_header_);
	}
	
	ctl_slider_frame_.SetFocus();

	return 0;
}

// void CASImgView::DrawImageArea( LPDRAWITEMSTRUCT lpDrawItem, RECT* image_area )
// {
// 	if (image_area->left > 0)
// 	{
// 		RECT rc = {image_area->left + 20, 20, image_area->left + 21, lpDrawItem->rcItem.bottom};
// 		InvertRect(lpDrawItem->hDC, &rc);
// 	}
// 
// 	if (image_area->right > 0)
// 	{
// 		RECT rc = {image_area->right + 20, 20, image_area->right + 21, lpDrawItem->rcItem.bottom};
// 		InvertRect(lpDrawItem->hDC, &rc);
// 	}
// 
// 	if (image_area->top > 0)
// 	{
// 		RECT rc = {20, image_area->top + 20, lpDrawItem->rcItem.right, image_area->top + 21};
// 		InvertRect(lpDrawItem->hDC, &rc);
// 	}
// 
// 	if (image_area->bottom > 0)
// 	{
// 		RECT rc = {20, image_area->bottom + 20, lpDrawItem->rcItem.right, image_area->bottom + 21};
// 		InvertRect(lpDrawItem->hDC, &rc);
// 	}
// }

void CASImgView::InitLogBlockBuffer()
{
	log_block_buf_size_ = sizeof(cpt_screen_data) + 3 * (sizeof(RGNDATA) + 4096) + 4096 * 2160 * 4 + 1000;
	log_block_buf_ = (char*)malloc(log_block_buf_size_);
	log_block_data_header_ = (cpt_common*)log_block_buf_;
	log_block_screen_data_ = (cpt_screen_data*)log_block_buf_;
}

LRESULT CASImgView::OnBtnExportFrame( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled )
{
	bHandled = TRUE;
	TCHAR buf[MAX_PATH * 2];
	make_export_file_path(current_frame_index_, buf);
	save_to_bmp_file(buf, &log_block_screen_data_->bi, log_block_screen_data_->get_image_bits());
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
	WCHAR path_buf[MAX_PATH];
	SHGetSpecialFolderPath(NULL, path_buf, CSIDL_APPDATA, FALSE);
	::PathAppend(path_buf, _T("Zoom\\logs\\"));
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

POINT CASImgView::ReadMousePos(LONGLONG file_pos )
{	
	cpt_mouse_position_data* data = (cpt_mouse_position_data*)log_block_buf_;
	DWORD bytes_readed = 0;
	POINT pt = {0, 0};
	SetFilePointerEx(img_log_file_handle_, (LARGE_INTEGER&)file_pos, NULL, FILE_BEGIN);

	ReadFile(img_log_file_handle_, log_block_buf_, sizeof(cpt_mouse_position_data), &bytes_readed, NULL);
	if (bytes_readed >= sizeof(cpt_mouse_position_data))
	{
		pt = data->pos;
	}

	return pt;
}

void CASImgView::ReadCursorImage( LONGLONG file_pos )
{
	if(current_frame_cursor_image_) DestroyIcon(current_frame_cursor_image_);
	current_frame_cursor_image_ = NULL;

	if (0 == file_pos)
	{
		return;
	}

	cpt_mouse_pointer_data* data = (cpt_mouse_pointer_data*)log_block_buf_;
	SetFilePointerEx(img_log_file_handle_, (LARGE_INTEGER&)file_pos, NULL, FILE_BEGIN);

	DWORD bytes_readed = 0;
	ReadFile(img_log_file_handle_, log_block_buf_, sizeof(cpt_common), &bytes_readed, NULL);
	if (bytes_readed < sizeof(cpt_common))
	{
		return;
	}

	bytes_readed = 0;
	ReadFile(img_log_file_handle_, log_block_buf_ + sizeof(cpt_common), log_block_data_header_->size - sizeof(cpt_common), &bytes_readed, NULL);
	if (bytes_readed < log_block_data_header_->size - sizeof(cpt_common))
	{
		return;
	}

	LPBYTE color_bits = data->pointer_bits + align_data_32(data->pointer_shape.cx, 1)*data->pointer_shape.cy;

	if(data->pointer_shape.BitsPixel == 32)
	{
		HBITMAP bmp_mask = CreateBitmap(data->pointer_shape.cx, data->pointer_shape.cy, 1, 1, data->pointer_bits);

		HDC hdc = ::GetDC(NULL);

		BYTE bis[256];
		BITMAPV5HEADER& bi = *(BITMAPV5HEADER*)bis;

		ZeroMemory(&bi,sizeof(BITMAPV5HEADER));
		bi.bV5Size           = sizeof(BITMAPV5HEADER);
		bi.bV5Width           = data->pointer_shape.cx;
		bi.bV5Height          = -data->pointer_shape.cy;
		bi.bV5Planes = 1;
		bi.bV5BitCount = 32;
		bi.bV5Compression = BI_BITFIELDS;
		bi.bV5RedMask   =  0x00FF0000;
		bi.bV5GreenMask =  0x0000FF00;
		bi.bV5BlueMask  =  0x000000FF;
		bi.bV5AlphaMask =  0xFF000000; 
		bi.bV5SizeImage = data->pointer_shape.cbWidth * data->pointer_shape.cy;

		LPBYTE bmp_bits;
		HBITMAP bmp_cursor = CreateDIBSection(hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (LPVOID*)&bmp_bits, NULL, 0);
		memcpy(bmp_bits, color_bits, data->pointer_shape.cbWidth * data->pointer_shape.cy);
	/*
			LPDWORD lpbits = (LPDWORD)bmp_bits;
			for(int i = 0; i< cs.cx*cs.cy; i++)
		{
				if((lpbits[i]&0xFF000000)== 0xFF000000)
					lpbits[i] &= 0x6fffffff;
		}
	*/
		::ReleaseDC(NULL, hdc);

		ICONINFO icn_info;
		icn_info.fIcon = FALSE;
		icn_info.xHotspot = data->pointer_shape.xHotSpot;
		icn_info.yHotspot = data->pointer_shape.yHotSpot;
		icn_info.hbmMask = bmp_mask;
		icn_info.hbmColor = bmp_cursor;

		current_frame_cursor_image_ = CreateIconIndirect(&icn_info);

		DeleteObject(bmp_mask);
		DeleteObject(bmp_cursor);
	}
	else
	{
		current_frame_cursor_image_ = CreateIcon(GetModuleHandle(NULL), data->pointer_shape.cx, data->pointer_shape.cy, data->pointer_shape.Planes, data->pointer_shape.BitsPixel,  data->pointer_bits, color_bits);
	}
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
	if(DragQueryFile((HDROP)wParam, 0, file_path, ARRAYSIZE(file_path)) <= 0)
		return 0;
	
	CloseImgLogFile();
	if(LoadImgLogFile(file_path))
		_tcscpy(img_log_file_path_, file_path);

	ctl_slider_frame_.SetFocus();
}


