#include "StdAfx.h"
#include "DialogImage.h"

typedef LRESULT(CALLBACK *fWindowProc)(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProc_PicMain(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static fWindowProc		g_win_proc_pic_main = 0;
static CDialogImage*	g_dialog_image = 0;

CDialogImage::CDialogImage(void)
{
	frame_image_height_ = -1;
	frame_image_width_ = -1;
	frame_image_ = NULL;

	memset(region_brushs_, 0, sizeof(region_brushs_));
	memset(region_handles_, 0, sizeof(region_handles_));
	memset(region_show_flags_, 0, sizeof(region_show_flags_));

	memset(dc_memory_, 0,sizeof(dc_memory_));
	frame_image_ = NULL;

	render_image_erase_ = false;
	render_region_fill_ = false;
	render_area_refline_ = false;
	render_show_thumb_ = false;
	render_show_scale_ = false;

	ctl_pic_main_scroll_x_ = ctl_pic_main_scroll_y_ = 0;

	mouse_drag_flag_ = false;
}

CDialogImage::~CDialogImage(void)
{
	for (int i = 0; i < ARRAYSIZE(region_handles_); ++i)
	{
		if (NULL != region_handles_[i])
		{
			DeleteObject(region_handles_[i]);
			region_handles_[i] = NULL;
		}
	}

	if (NULL != frame_image_)
	{
		DeleteObject(frame_image_);
	}

	for (int i = 0; i < ARRAYSIZE(dc_memory_); ++i)
	{
		DeleteDC(dc_memory_[i]);
		dc_memory_[i] = NULL;
	}
}

LRESULT CDialogImage::OnInitDialog( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
{
	bHandled = TRUE;

	g_dialog_image = this;

	hwnd_main_ = GetAncestor(*this, GA_ROOT);

	pen_area_refline_ = CreatePen(PS_SOLID, 2, 0x880088);

	ctl_scroll_h_.Attach(GetDlgItem(IDC_SCROLL_H));
	ctl_scroll_v_.Attach(GetDlgItem(IDC_SCROLL_V));

	ctl_tick_h_ = GetDlgItem(IDC_TICK_H);
	ctl_tick_v_ = GetDlgItem(IDC_TICK_V);

	ctl_pic_main_ = GetDlgItem(IDC_PIC_MAIN);

	region_brushs_[REGION_VISIBLE] = CreateSolidBrush(0xFF);
	region_brushs_[REGION_UPDATE] = CreateSolidBrush(0xFF00);
	region_brushs_[REGION_INVISIBLE] = CreateSolidBrush(0xFF0000);
	region_brushs_[REGION_BACK] = CreateSolidBrush(0x6666);

	dc_memory_[0] = CreateCompatibleDC(NULL);
	dc_memory_[1] = CreateCompatibleDC(NULL);

	SetBkColor(dc_memory_[0], COLOR_BTNFACE);

	InitImageTick();
	InitImageBackgroundTransparent();

	g_win_proc_pic_main = (fWindowProc)::SetWindowLongPtr(ctl_pic_main_, GWLP_WNDPROC, (LONG_PTR)WindowProc_PicMain);

	return 0;
}

LRESULT CDialogImage::OnSize( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;

	if(SIZE_MINIMIZED == wParam)
	{
		return 0;
	}

	RECT rc = {0, 0, LOWORD(lParam), HIWORD(lParam)};

	ctl_pic_main_width_ = rc.right - 36;
	ctl_pic_main_height_ = rc.bottom - 36;
	ctl_pic_main_rgn_ = CreateRectRgn(0, 0, ctl_pic_main_width_, ctl_pic_main_height_);

	::MoveWindow(ctl_pic_main_, 20, 20, ctl_pic_main_width_, ctl_pic_main_height_, TRUE);
	
	::MoveWindow(ctl_tick_v_, 0, 20, 20, rc.bottom - 36, TRUE);
	::MoveWindow(ctl_tick_h_, 20, 0, rc.right - 36, 20, TRUE);

	::MoveWindow(ctl_scroll_v_, rc.right - 16, 20, 16, rc.bottom - 36, FALSE);
	::MoveWindow(ctl_scroll_h_, 20, rc.bottom - 16, rc.right - 36, 16, FALSE);

	int tmp = 4096 - rc.right + 36;
	ctl_scroll_h_.SetScrollRange(0, tmp > 0 ? tmp : 0, TRUE);
	tmp = 2160 - rc.bottom + 36;
	ctl_scroll_v_.SetScrollRange(0, tmp > 0 ? tmp : 0, TRUE);

	return 0;
}

LRESULT CDialogImage::OnDestroy( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
{
	bHandled = TRUE;
	::SetWindowLongPtr(ctl_pic_main_, DWLP_DLGPROC, (LONG_PTR)g_win_proc_pic_main);
	return 0;
}

LRESULT CDialogImage::OnDrawItem( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;

	LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;

	switch(wParam)
	{
	case IDC_PIC_MAIN:
		if (render_show_thumb_)
			DrawMainFrameAsThumb(lpDrawItem);
		else if (render_show_scale_)
			DrawMainCompPicAsThumb(lpDrawItem);
		else
			DrawMainPic(lpDrawItem, NULL);
		break;

	case IDC_TICK_H:
		DrawTickBar(lpDrawItem, image_tick_h_, ctl_pic_main_scroll_x_, 0, frame_capture_rc_.left - ctl_pic_main_scroll_x_, 0, frame_capture_rc_.right - ctl_pic_main_scroll_x_, 4);
		break;

	case IDC_TICK_V:
		DrawTickBar(lpDrawItem, image_tick_v_, 0, ctl_pic_main_scroll_y_, 0, frame_capture_rc_.top - ctl_pic_main_scroll_y_, 4, frame_capture_rc_.bottom - ctl_pic_main_scroll_y_);
		break;
	}
	return 0;
}

LRESULT CDialogImage::OnVScroll( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;
	if((HWND)lParam == ctl_scroll_v_)
	{
		switch(LOWORD(wParam))
		{
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			ctl_pic_main_scroll_y_ = HIWORD(wParam);
			ctl_scroll_v_.SetScrollPos(ctl_pic_main_scroll_y_);
			::InvalidateRect(ctl_tick_v_, NULL, FALSE);
			::InvalidateRect(ctl_pic_main_, NULL, FALSE);
			//Refresh();
			break;
		case SB_ENDSCROLL:
			//ctl_slider_frame_.SetFocus();
			::PostMessage(hwnd_main_, WM_SET_FOCUS_TO_SLIDER_FRAME, 0, 0);
			break;
		}
	}
	return 0;
}

LRESULT CDialogImage::OnHScroll( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;
	if (INVALID_HANDLE_VALUE == img_log_file_handle_)
	{
		return 0;
	}

	if((HWND)lParam == ctl_scroll_h_)
	{
		switch(LOWORD(wParam))
		{
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			ctl_pic_main_scroll_x_ = HIWORD(wParam);
			ctl_scroll_h_.SetScrollPos(ctl_pic_main_scroll_x_);
			::InvalidateRect(ctl_tick_h_, NULL, FALSE);
			::InvalidateRect(ctl_pic_main_, NULL, FALSE);
			//Refresh();
			break;
		case SB_ENDSCROLL:
			//ctl_slider_frame_.SetFocus();
			::PostMessage(hwnd_main_, WM_SET_FOCUS_TO_SLIDER_FRAME, 0, 0);
			break;
		}
	}

	return 0;
}

void CDialogImage::Refresh()
{
	if (log_block_frame_index_ < 0)
	{
		return;
	}

	if (log_block_frame_index_ != current_frame_index_)
	{
		for (int i = 0; i < ARRAYSIZE(region_handles_); ++i)
		{
			if (NULL != region_handles_[i])
			{
				DeleteObject(region_handles_[i]);
				region_handles_[i] = NULL;
			}
		}

		HBITMAP image_handle = LoadFrameImage(log_block_screen_data_);

		frame_capture_rc_.left = log_block_screen_data_->left;
		frame_capture_rc_.top = log_block_screen_data_->top;
		frame_capture_rc_.right = frame_capture_rc_.left + log_block_screen_data_->bi.bmiHeader.biWidth;
		frame_capture_rc_.bottom = frame_capture_rc_.top + abs(log_block_screen_data_->bi.bmiHeader.biHeight);

		ScrollToContent();

		if (frame_capture_rc_.right > frame_image_width_ || frame_capture_rc_.bottom > frame_image_height_)
		{
			ReCreateFrameImageForSize(frame_capture_rc_.right + 1, frame_capture_rc_.bottom + 1);
			ClearFrameImageContent();
		}
		else if (log_block_frame_index_ < current_frame_index_ || render_image_erase_)
		{
			ClearFrameImageContent();
		}

		

		HGLOBAL obj_prev = SelectObject(dc_memory_[0], frame_image_);
		HGLOBAL obj_prev1 = SelectObject(dc_memory_[1], image_handle);
 		BitBlt(dc_memory_[0], frame_capture_rc_.left, frame_capture_rc_.top, frame_capture_rc_.right - frame_capture_rc_.left, frame_capture_rc_.bottom - frame_capture_rc_.top,
 			dc_memory_[1], 0, 0, SRCCOPY);

		// compound with brush
// 		int origin_x = (frame_capture_rc_.right - frame_capture_rc_.left);
// 		origin_x = /*origin_x - */frame_capture_rc_.left % origin_x;
// 		int origin_y = (frame_capture_rc_.bottom - frame_capture_rc_.top);
// 		origin_y = /*origin_y - */frame_capture_rc_.top % origin_y;
// 		HBRUSH	image_brush = CreatePatternBrush(image_handle);
// 		SetBrushOrgEx(dc_memory_[0], origin_x, origin_y, NULL);
// 		FillRect(dc_memory_[0], &frame_capture_rc_, image_brush);
// 		DeleteObject(image_brush);

		SelectObject(dc_memory_[0], obj_prev);
		SelectObject(dc_memory_[1], obj_prev1);	

		DeleteObject(image_handle);

		current_frame_index_ = log_block_frame_index_;
	}

	::InvalidateRect(ctl_pic_main_, NULL, FALSE);
	::InvalidateRect(ctl_tick_h_, NULL, FALSE);
	::InvalidateRect(ctl_tick_v_, NULL, FALSE);
}

void CDialogImage::SetImageErase( bool v )
{
	if(render_image_erase_ != v)
	{
		render_image_erase_ = v;
		Refresh();
	}
}

void CDialogImage::SetRegionFill( bool v )
{
	if (render_region_fill_ != v)
	{
		render_region_fill_ = v;
		Refresh();
	}
	
}

void CDialogImage::SetRegionShow( EnumRegion region, bool show )
{
	if (region_show_flags_[region] != show)
	{
		region_show_flags_[region] = show;
		Refresh();
	}
}

void CDialogImage::InitImageTick()
{
	TCHAR buf[20];

	// 	SetBkColor(dc_memory_, 0);
	// 	SetTextColor(dc_memory_, 0xFFFFFF);
	SetBkMode(dc_memory_[0], TRANSPARENT);

	RECT rc_h = {0, 0, 4096, 20};

	image_tick_h_ = CreateCompatibleBitmap(dc_memory_[0], 4096, 20);
	HGLOBAL prev_obj = SelectObject(dc_memory_[0], image_tick_h_);

	FillRect(dc_memory_[0], &rc_h, (HBRUSH)(COLOR_BTNFACE));

	MoveToEx(dc_memory_[0], 0, 19, NULL);
	LineTo(dc_memory_[0], 4095, 19);

	for (int i = 9; i < 4096; i += 10)
	{
		MoveToEx(dc_memory_[0], i, 15, NULL);
		LineTo(dc_memory_[0], i, 19);
	}

	for (int i = 99; i < 4096; i+= 100)
	{
		MoveToEx(dc_memory_[0], i, 0, NULL);
		LineTo(dc_memory_[0], i, 19);

		_itot_s((i + 1) / 100, buf, 10);
		TextOut(dc_memory_[0], i + 3, 1, buf, _tcslen(buf));
	}

	RECT rc_v = {0, 0, 20, 2160};

	image_tick_v_ = CreateCompatibleBitmap(dc_memory_[0], 20, 2160);
	SelectObject(dc_memory_[0], image_tick_v_);

	FillRect(dc_memory_[0], &rc_v, (HBRUSH)(COLOR_BTNFACE));

	MoveToEx(dc_memory_[0], 19, 0, NULL);
	LineTo(dc_memory_[0], 19, 2159);

	for (int i = 9; i < 2160; i += 10)
	{
		MoveToEx(dc_memory_[0], 15, i, NULL);
		LineTo(dc_memory_[0], 19, i);
	}

	for (int i = 99; i < 4096; i+= 100)
	{
		MoveToEx(dc_memory_[0], 0, i, NULL);
		LineTo(dc_memory_[0], 19, i);

		_itot_s((i + 1) / 100, buf, 10);
		TextOut(dc_memory_[0], 3, i + 3, buf, _tcslen(buf));
	}

// 	image_tick_grid_ = CreateCompatibleBitmap(dc_memory_, 40, 40);
// 	SelectObject(dc_memory_, image_tick_grid_);
// 
// 	RECT rc_grid = {0, 0, 40, 40};
// 
// 	FillRect(dc_memory_, &rc_grid, (HBRUSH)(COLOR_BTNFACE));
// 
// 	for(int i = 0; i < 40; i += 10)
// 	{
// 		MoveToEx(dc_memory_, 0, i, NULL);
// 		LineTo(dc_memory_, 39, i);
// 
// 		MoveToEx(dc_memory_, i, 0, NULL);
// 		LineTo(dc_memory_, i, 39);
// 	}
// 
// 	image_tick_grid_brush_ = CreatePatternBrush(image_tick_grid_);

	SelectObject(dc_memory_[0], prev_obj);
}

void CDialogImage::ReCreateFrameImageForSize( int width, int height )
{
	if (frame_image_height_ < height || frame_image_width_ < width)
	{
		if (NULL != frame_image_)
		{
			DeleteObject(frame_image_);
		}

		frame_image_width_ = width;
		frame_image_height_ = height;

		HDC dc = GetDC();
		frame_image_ = CreateCompatibleBitmap(dc, frame_image_width_, frame_image_height_);
		ReleaseDC(dc);
	}
}

void CDialogImage::DrawMainPic( LPDRAWITEMSTRUCT lpDrawItem, HBITMAP hbp )
{
	//FillRect(lpDrawItem->hDC, &lpDrawItem->rcItem, brush_background_transparent_);

	RECT image_rc = {0, 0, frame_image_width_, frame_image_height_};
	RECT view_rc = {ctl_pic_main_scroll_x_, ctl_pic_main_scroll_y_, ctl_pic_main_scroll_x_ + lpDrawItem->rcItem.right, ctl_pic_main_scroll_y_ + lpDrawItem->rcItem.bottom};

	RECT inter_rc;
	if(!IntersectRect(&inter_rc, &view_rc, &image_rc))
		return;

	HGLOBAL obj_prev = SelectObject(dc_memory_[0], frame_image_);
	BitBlt(lpDrawItem->hDC, inter_rc.left - view_rc.left, inter_rc.top - view_rc.top, inter_rc.right - inter_rc.left, inter_rc.bottom - inter_rc.top, dc_memory_[0], inter_rc.left - image_rc.left, inter_rc.top - image_rc.top, SRCCOPY);
	SelectObject(dc_memory_[0], obj_prev);

	HRGN rgn_tmp = CreateRectRgn(inter_rc.left - view_rc.left, inter_rc.top - view_rc.top, inter_rc.right - view_rc.left, inter_rc.bottom - view_rc.top);
	CombineRgn(rgn_tmp, ctl_pic_main_rgn_, rgn_tmp, RGN_DIFF);
	FillRgn(lpDrawItem->hDC, rgn_tmp, brush_background_transparent_);
	DeleteObject(rgn_tmp);

	if (NULL != current_frame_cursor_image_)
	{
		DrawIcon(lpDrawItem->hDC, current_frame_mouse_position_.x - ctl_pic_main_scroll_x_, current_frame_mouse_position_.y - ctl_pic_main_scroll_y_, current_frame_cursor_image_);
	}	

	DrawImageArea(lpDrawItem, NULL);
	DrawImageRegion(lpDrawItem);
}

void CDialogImage::DrawMainFrameAsThumb( LPDRAWITEMSTRUCT lpDrawItem )
{
	bool scale = false;
	float scale_rate = 1.0;

	int x, y, width, height;

	int frame_capture_width = frame_capture_rc_.right - frame_capture_rc_.left;
	int frame_capture_height = frame_capture_rc_.bottom - frame_capture_rc_.top;

	// 长宽成比例缩放
	if ((float)frame_capture_width / frame_capture_height > (float)lpDrawItem->rcItem.right / lpDrawItem->rcItem.bottom)
	{
		if (lpDrawItem->rcItem.right - 20 < frame_capture_width)
		{
			scale = true;
			scale_rate = (float)(lpDrawItem->rcItem.right - 20) / frame_capture_width;
		}
	}
	else
	{
		if (lpDrawItem->rcItem.bottom - 20 < frame_capture_height)
		{
			scale = true;
			scale_rate = (float)(lpDrawItem->rcItem.bottom - 20) / frame_capture_height;
		}
	}

	if (scale)
	{
		width = (int)(scale_rate * frame_capture_width);
		x = (lpDrawItem->rcItem.right - width) / 2;
		
		height = (int)(scale_rate * frame_capture_height);
		y = (lpDrawItem->rcItem.bottom - height) / 2;
	}
	else
	{
		width = frame_capture_rc_.right - frame_capture_rc_.left;
		x = (lpDrawItem->rcItem.right - width) / 2;
		
		height = frame_capture_rc_.bottom - frame_capture_rc_.top;
		y = (lpDrawItem->rcItem.bottom - height) / 2;
	}

	// 2. 长宽不成比例缩放
// 	if (lpDrawItem->rcItem.right < frame_capture_rc_.right - frame_capture_rc_.left + 20)
// 	{
// 		scale = true;
// 		x = 10;
// 		width = lpDrawItem->rcItem.right - 20;
// 	}
// 	else
// 	{
// 		x = (lpDrawItem->rcItem.right - (frame_capture_rc_.right - frame_capture_rc_.left)) / 2;
// 		width = frame_capture_rc_.right - frame_capture_rc_.left;
// 	}
// 
// 	if (lpDrawItem->rcItem.bottom < frame_capture_rc_.bottom - frame_capture_rc_.top + 20)
// 	{
// 		scale = true;
// 		y = 10;
// 		height = lpDrawItem->rcItem.bottom - 20;
// 	}
// 	else
// 	{
// 		y = (lpDrawItem->rcItem.bottom - (frame_capture_rc_.bottom - frame_capture_rc_.top)) / 2;
// 		height = frame_capture_rc_.bottom - frame_capture_rc_.top;
// 	}

	HGLOBAL obj_prev = SelectObject(dc_memory_[0], frame_image_);

	if (scale)
	{
		SetStretchBltMode(lpDrawItem->hDC,STRETCH_HALFTONE); 
		StretchBlt(lpDrawItem->hDC, x, y, width, height, dc_memory_[0], frame_capture_rc_.left, frame_capture_rc_.top, frame_capture_rc_.right - frame_capture_rc_.left, frame_capture_rc_.bottom - frame_capture_rc_.top, SRCCOPY);
	}
	else
	{
		BitBlt(lpDrawItem->hDC, x, y, width, height, dc_memory_[0], frame_capture_rc_.left, frame_capture_rc_.top, SRCCOPY);
	}

	SelectObject(dc_memory_[0], obj_prev);

	HRGN rgn_tmp = CreateRectRgn(x, y, x + width, y + height);
	CombineRgn(rgn_tmp, ctl_pic_main_rgn_, rgn_tmp, RGN_DIFF);
	FillRgn(lpDrawItem->hDC, rgn_tmp, brush_background_transparent_);
	DeleteObject(rgn_tmp);
}

void CDialogImage::DrawMainCompPicAsThumb( LPDRAWITEMSTRUCT lpDrawItem )
{
	bool scale = false;
	float scale_rate = 1.0;

	int x, y, width, height;

	// 长宽成比例缩放
	if ((float)frame_image_width_ / frame_image_height_ > (float)lpDrawItem->rcItem.right / lpDrawItem->rcItem.bottom)
	{
		if (lpDrawItem->rcItem.right - 20 < frame_image_width_)
		{
			scale = true;
			scale_rate = (float)(lpDrawItem->rcItem.right - 20) / frame_image_width_;
		}
	}
	else
	{
		if (lpDrawItem->rcItem.bottom - 20 < frame_image_height_)
		{
			scale = true;
			scale_rate = (float)(lpDrawItem->rcItem.bottom - 20) / frame_image_height_;
		}
	}

	if (scale)
	{
		width = (int)(scale_rate * frame_image_width_);
		height = (int)(scale_rate * frame_image_height_);
	}
	else
	{
		width = frame_image_width_;
		height = frame_image_height_;
		
	}

	x = (lpDrawItem->rcItem.right - width) / 2;
	y = (lpDrawItem->rcItem.bottom - height) / 2;

	HGLOBAL obj_prev = SelectObject(dc_memory_[0], frame_image_);

	if (scale)
	{
		SetStretchBltMode(lpDrawItem->hDC,STRETCH_HALFTONE); 
		StretchBlt(lpDrawItem->hDC, x, y, width, height, dc_memory_[0], 0, 0, frame_image_width_, frame_image_height_, SRCCOPY);
	}
	else
	{
		BitBlt(lpDrawItem->hDC, x, y, width, height, dc_memory_[0], 0, 0, SRCCOPY);
	}

	SelectObject(dc_memory_[0], obj_prev);

	HRGN rgn_tmp = CreateRectRgn(x, y, x + width, y + height);
	CombineRgn(rgn_tmp, ctl_pic_main_rgn_, rgn_tmp, RGN_DIFF);
	FillRgn(lpDrawItem->hDC, rgn_tmp, brush_background_transparent_);
	DeleteObject(rgn_tmp);
}

void CDialogImage::DrawImageArea( LPDRAWITEMSTRUCT lpDrawItem, RECT* area )
{
	if (!render_area_refline_)
	{
		return;
	}

	HGLOBAL obj_prev = SelectObject(lpDrawItem->hDC, pen_area_refline_);
	//SetTextColor(lpDrawItem->hDC, 0x666600);

	RECT frame_capture_rc = frame_capture_rc_;
	OffsetRect(&frame_capture_rc, -ctl_pic_main_scroll_x_, -ctl_pic_main_scroll_y_);

	if (frame_capture_rc.left > lpDrawItem->rcItem.left && frame_capture_rc.left < lpDrawItem->rcItem.right)
	{
		DrawImageAreaLine(lpDrawItem, frame_capture_rc.left, lpDrawItem->rcItem.top, frame_capture_rc.left, lpDrawItem->rcItem.bottom);
	}

	if (frame_capture_rc.right >= lpDrawItem->rcItem.left && frame_capture_rc.right < lpDrawItem->rcItem.right)
	{
		DrawImageAreaLine(lpDrawItem, frame_capture_rc.right, lpDrawItem->rcItem.top, frame_capture_rc.right, lpDrawItem->rcItem.bottom);
	}

	if (frame_capture_rc.top >= lpDrawItem->rcItem.top && frame_capture_rc.top < lpDrawItem->rcItem.bottom)
	{
		DrawImageAreaLine(lpDrawItem, lpDrawItem->rcItem.left, frame_capture_rc.top, lpDrawItem->rcItem.right,  frame_capture_rc.top);
	}

	if (frame_capture_rc.bottom >= lpDrawItem->rcItem.top && frame_capture_rc.bottom < lpDrawItem->rcItem.bottom)
	{
		DrawImageAreaLine(lpDrawItem, lpDrawItem->rcItem.left, frame_capture_rc.bottom, lpDrawItem->rcItem.right,  frame_capture_rc.bottom);
	}

	SelectObject(lpDrawItem->hDC, obj_prev);
}

void CDialogImage::DrawImageAreaLine( LPDRAWITEMSTRUCT lpDrawItem, int start_x, int start_y, int end_x, int end_y )
{
// 	MoveToEx(lpDrawItem->hDC, start_x, start_y, NULL);
// 	LineTo(lpDrawItem->hDC, end_x,  end_y);

	int width = end_x - start_x;
	if (width <= 0) width = 2;

	int height = end_y - start_y;
	if (height <= 0) height = 2;

	PatBlt(lpDrawItem->hDC, start_x, start_y, width, height, DSTINVERT);
}


void CDialogImage::DrawTickBar( LPDRAWITEMSTRUCT lpDrawItem, HBITMAP bmp, int x, int y, int bold_x, int bold_y, int bold_right, int bold_bottom)
{
	HGLOBAL obj_prev = SelectObject(dc_memory_[0], bmp);
	BitBlt(lpDrawItem->hDC, 0, 0, lpDrawItem->rcItem.right, lpDrawItem->rcItem.bottom, dc_memory_[0], x, y, SRCCOPY);
	RECT rc = {bold_x, bold_y, bold_right, bold_bottom};
	FillRect(lpDrawItem->hDC, &rc, (HBRUSH)COLOR_BTNHIGHLIGHT);
	SelectObject(dc_memory_[0], obj_prev);
}

void CDialogImage::ClearFrameImageContent()
{
	HGLOBAL obj_prev = SelectObject(dc_memory_[0], frame_image_);
	RECT rc = {0, 0, frame_image_width_, frame_image_height_};
	FillRect(dc_memory_[0], &rc, brush_background_transparent_);

	SelectObject(dc_memory_[0], obj_prev);
}

void CDialogImage::ResetScrollRange()
{
	RECT rc;
	::GetClientRect(ctl_pic_main_, &rc);
	int tmp = 4096 - rc.right;
	ctl_scroll_h_.SetScrollRange(0, tmp > 0 ? tmp : 0, TRUE);
	tmp = 2160 - rc.bottom;
	ctl_scroll_v_.SetScrollRange(0, tmp > 0 ? tmp : 0, TRUE);
}

void CDialogImage::DrawImageRegion( LPDRAWITEMSTRUCT lpDrawItem )
{
	int region_sizes[] = {log_block_screen_data_->updated_rgn_data_len, log_block_screen_data_->visible_rgn_data_len, log_block_screen_data_->invisible_rgn_data_len, log_block_screen_data_->back_rgn_data_len };
	LPRGNDATA region_datas[] = { log_block_screen_data_->get_updated_rgn_data(), log_block_screen_data_->get_visible_rgn_data(), log_block_screen_data_->get_invisible_rgn_data(), log_block_screen_data_->get_back_rgn_data() };

	for(int i = 0; i < ARRAYSIZE(region_show_flags_); ++i)
	{
		if (region_show_flags_[i])
		{
			if (NULL == region_handles_[i])
			{
				if (region_sizes[i] > 0)
				{
					region_handles_[i] = ExtCreateRegion(NULL, region_sizes[i], region_datas[i]);
				}
			}

			if (NULL != region_handles_[i])
			{
				DrawImageRegion(lpDrawItem, region_handles_[i], region_brushs_[i]);
			}
		}
	}
}

void CDialogImage::DrawImageRegion( LPDRAWITEMSTRUCT lpDrawItem, HRGN rgn, HBRUSH brush )
{
	OffsetRgn(rgn, -ctl_pic_main_scroll_x_, -ctl_pic_main_scroll_y_);
	if (render_region_fill_)
	{
		FillRgn(lpDrawItem->hDC, rgn, brush);
	}
	else
	{
		FrameRgn(lpDrawItem->hDC, rgn, brush, 3, 3);
	}
	OffsetRgn(rgn, ctl_pic_main_scroll_x_, ctl_pic_main_scroll_y_);
}

void CDialogImage::SetAreaRefLineShow( bool v )
{
	if (render_area_refline_ != v)
	{
		render_area_refline_ = v;
		::InvalidateRect(ctl_pic_main_, NULL, FALSE);
	}
}

void CDialogImage::OnPicMainMouseMove( WPARAM wParam,LPARAM lParam )
{
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);

	if (mouse_drag_flag_)
	{
		ctl_pic_main_scroll_x_ = mouse_drag_scroll_x_ - (x - mouse_drag_mouse_x_);
		ctl_pic_main_scroll_y_ = mouse_drag_scroll_y_ - (y - mouse_drag_mouse_y_);
		if (ctl_pic_main_scroll_x_ < 0) ctl_pic_main_scroll_x_ = 0;
		if (ctl_pic_main_scroll_y_ < 0) ctl_pic_main_scroll_y_ = 0;
		ctl_scroll_h_.SetScrollPos(ctl_pic_main_scroll_x_);
		ctl_scroll_v_.SetScrollPos(ctl_pic_main_scroll_y_);
		::InvalidateRect(ctl_pic_main_, NULL, FALSE);
		::InvalidateRect(ctl_tick_h_, NULL, FALSE);
		::InvalidateRect(ctl_tick_v_, NULL, FALSE);
	}
	else
	{
		x += ctl_pic_main_scroll_x_;
		y += ctl_pic_main_scroll_y_;
		::PostMessage(hwnd_main_, WM_SET_TXT_CURSOR_POS, 0, MAKELPARAM(x, y));
	}

// 	if (render_show_thumb_)
// 	{
// 		render_show_thumb_ = false;
// 		::InvalidateRect(ctl_pic_main_, NULL, FALSE);
// 	}
}

void CDialogImage::OnPicMainLButtonDown( WPARAM wParam,LPARAM lParam )
{
	mouse_drag_flag_ = true;
	mouse_drag_mouse_x_ = LOWORD(lParam);
	mouse_drag_mouse_y_ = HIWORD(lParam);
	mouse_drag_scroll_x_ = ctl_pic_main_scroll_x_;
	mouse_drag_scroll_y_ = ctl_pic_main_scroll_y_;
}

void CDialogImage::OnPicMainLButtonUp( WPARAM wParam,LPARAM lParam )
{
	mouse_drag_flag_ = false;
	::PostMessage(hwnd_main_, WM_SET_FOCUS_TO_SLIDER_FRAME, 0, 0);
}

void CDialogImage::ScrollToContent()
{
	if (frame_capture_rc_.left > ctl_pic_main_width_ + ctl_pic_main_scroll_x_)
	{
		ctl_pic_main_scroll_x_ = frame_capture_rc_.right - ctl_pic_main_width_;
		if (ctl_pic_main_scroll_x_ > frame_capture_rc_.left)
		{
			ctl_pic_main_scroll_x_ = frame_capture_rc_.left;
		}
		ctl_scroll_h_.SetScrollPos(ctl_pic_main_scroll_x_);
	}
	else if (frame_capture_rc_.right < ctl_pic_main_scroll_x_)
	{
		ctl_pic_main_scroll_x_ = frame_capture_rc_.left;
		if (frame_capture_rc_.right > ctl_pic_main_scroll_x_ + ctl_pic_main_width_)
		{
			ctl_pic_main_scroll_x_ = frame_capture_rc_.right - ctl_pic_main_width_;
		}
		ctl_scroll_h_.SetScrollPos(ctl_pic_main_scroll_x_);
	}

	if (frame_capture_rc_.top > ctl_pic_main_height_ + ctl_pic_main_scroll_y_)
	{
		ctl_pic_main_scroll_y_ = frame_capture_rc_.bottom - ctl_pic_main_height_;
		if (ctl_pic_main_scroll_y_ > frame_capture_rc_.top)
		{
			ctl_pic_main_scroll_y_ = frame_capture_rc_.top;
		}
		ctl_scroll_v_.SetScrollPos(ctl_pic_main_scroll_y_);
	}
	else if (frame_capture_rc_.bottom < ctl_pic_main_scroll_y_)
	{
		ctl_pic_main_scroll_y_ = frame_capture_rc_.top;
		if (frame_capture_rc_.bottom > ctl_pic_main_scroll_y_ + ctl_pic_main_height_)
		{
			ctl_pic_main_scroll_y_ = frame_capture_rc_.bottom - ctl_pic_main_height_;
		}
		ctl_scroll_v_.SetScrollPos(ctl_pic_main_scroll_y_);
	}
}

LRESULT CDialogImage::OnMouseMove( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;
	if (LOWORD(lParam) > ctl_pic_main_width_ && HIWORD(lParam) > ctl_pic_main_height_)
	{
		if(!render_show_thumb_)
		{
			render_show_thumb_ = true;
			::InvalidateRect(ctl_pic_main_, NULL, FALSE);
		}
	}
	return 0;
}

void CDialogImage::OnMettingLoaded()
{
	current_frame_index_ = -1;
}

BOOL CDialogImage::PreTranslateMessage( MSG* pMsg )
{
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		if (VK_RETURN == pMsg->wParam)
		{
			if (!render_show_thumb_)
			{
				render_show_thumb_ = true;
				::InvalidateRect(ctl_pic_main_, NULL, FALSE);
			}
		}
		else if(' ' == pMsg->wParam)
		{
			::PostMessage(hwnd_main_, WM_PLAY_SWITCH, 0, 0);
		}
		break;
	case WM_KEYUP:
		if (VK_RETURN == pMsg->wParam)
		{
			if (render_show_thumb_)
			{
				render_show_thumb_ = false;
				::InvalidateRect(ctl_pic_main_, NULL, FALSE);
			}
		}
		break;

	}

	return FALSE;
}

void CDialogImage::InitImageBackgroundTransparent()
{
	HDC dc = GetDC();
	image_background_transparent_ = CreateCompatibleBitmap(dc, 20, 20);
	ReleaseDC(dc);

	HGLOBAL obj_prev = SelectObject(dc_memory_[0], image_background_transparent_);
	HBRUSH brush_white = CreateSolidBrush(0xFFFFFF);
	HBRUSH brush_gray = CreateSolidBrush(0xDDDDDD);

	for (int x = 0; x <= 2; ++x)
	{
		for (int y = 0; y < 2; ++y)
		{
			RECT rc = {x * 10, y * 10, 0, 0};
			rc.right = rc.left + 10;
			rc.bottom = rc.top + 10;

			//DWORD color = ((x + y) % 2) ? COLOR_BTNHIGHLIGHT : COLOR_WINDOW;

			FillRect(dc_memory_[0], &rc, ((x + y) % 2) ? brush_gray : brush_white );
		}
	}
	SelectObject(dc_memory_[0], obj_prev);

	brush_background_transparent_ = CreatePatternBrush(image_background_transparent_);
}

void CDialogImage::SetScale( bool v )
{
	if (render_show_scale_ != v)
	{
		render_show_scale_ = v;
		::InvalidateRect(ctl_pic_main_, NULL, FALSE);
	}
}

void CDialogImage::SetMousePos( const POINT& pt )
{
	mouse_pos_.x = pt.x;
	mouse_pos_.y = pt.y;
}

LRESULT CALLBACK WindowProc_PicMain( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
		g_dialog_image->OnPicMainLButtonDown(wParam, lParam);
		break;

	case WM_LBUTTONUP:
		g_dialog_image->OnPicMainLButtonUp(wParam, lParam);
		break;

	case WM_MOUSEMOVE:
		g_dialog_image->OnPicMainMouseMove(wParam, lParam);
		break;
	}

	return g_win_proc_pic_main(hwnd, uMsg, wParam, lParam);
}
