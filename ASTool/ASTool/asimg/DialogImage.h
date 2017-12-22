#pragma once

#include "..\resource.h"
#include "common.h"

class ASFrameRes;

class CDialogImage : public CDialogImpl<CDialogImage>, public CUpdateUI<CDialogImage>
{
public:
	enum { IDD = IDD_ASIMG_FRAMEIMG };

	CDialogImage(void);
	~CDialogImage(void);

	BEGIN_UPDATE_UI_MAP(CDialogImage)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CDialogImage)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
		MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)

		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		//MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)

	END_MSG_MAP()

protected:
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnHScroll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnVScroll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

public:
	void SetRegionShow(EnumRegion region, bool show);
	void SetRegionFill(bool v);
	void SetImageErase(bool v);
	void SetAreaRefLineShow(bool v);
	void SetScale(bool v);
	void SetMousePos(const POINT& pt);

	void Refresh();

	//void SetASRecordPackage(IASRecordPackage* pkg);
	void SetFrameRes(ASFrameRes* res);

	HBITMAP GetImage() { return composit_frame_image_; }

public:
	void OnPicMainLButtonDown(WPARAM wParam,LPARAM lParam);
	void OnPicMainLButtonUp(WPARAM wParam,LPARAM lParam);
	void OnPicMainMouseMove(WPARAM wParam,LPARAM lParam);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	void InitImageTick();
	HBRUSH InitImageBackgroundBrush();
	void ReCreateFrameImageForSize(int width, int height);
	void ClearFrameImageContent();

	void ResetScrollRange();
	void ScrollViewToArea(const RECT& area);

	void DrawMainPic(LPDRAWITEMSTRUCT lpDrawItem, HBITMAP hbp);
	void DrawMainFrameAsThumb(LPDRAWITEMSTRUCT lpDrawItem);
	void DrawMainCompPicAsThumb(LPDRAWITEMSTRUCT lpDrawItem);
	void DrawImageArea(LPDRAWITEMSTRUCT lpDrawItem, const RECT* area);
	void DrawImageAreaLine(LPDRAWITEMSTRUCT lpDrawItem, int start_x, int start_y, int end_x, int end_y);
	void DrawImageRegion(LPDRAWITEMSTRUCT lpDrawItem);
	void DrawImageRegion(LPDRAWITEMSTRUCT lpDrawItem, HRGN rgn, HBRUSH brush);
	void DrawTickBar(LPDRAWITEMSTRUCT lpDrawItem, HBITMAP bmp, int x, int y, int bold_x, int bold_y, int bold_right, int bold_bottom);

protected:
	//IASRecordPackage*	record_package_;
	//const ASRecordFrameData*	record_current_frame_;
	ASFrameRes*			frame_res_;

	HWND				hwnd_main_;

	WTL::CScrollBar		ctl_scroll_v_;
	WTL::CScrollBar		ctl_scroll_h_;

	HWND				ctl_tick_v_;
	HWND				ctl_tick_h_;

	HWND				ctl_pic_main_;
	int					ctl_pic_main_width_;
	int					ctl_pic_main_height_;

	int					ctl_pic_main_scroll_x_;
	int					ctl_pic_main_scroll_y_;
	HRGN				ctl_pic_main_rgn_;

protected:
	bool				render_image_erase_;
	bool				render_region_fill_;
	bool				render_area_refline_;
	bool				render_show_thumb_;
	bool				render_show_scale_;

	HBITMAP				image_tick_v_;
	HBITMAP				image_tick_h_;

	HBRUSH				brush_background_transparent_;

	HPEN				pen_area_refline_;

protected:
	HDC					dc_memory_[2];
	HBITMAP				composit_frame_image_;
	int					composit_frame_image_width_;
	int					composit_frame_image_height_;

	RECT				frame_capture_rc_;

	POINT				mouse_pos_;

protected:
	bool				region_show_flags_[REGIONCOUNT];
	HRGN				region_handles_[REGIONCOUNT];
	HBRUSH				region_brushs_[REGIONCOUNT];

	int					current_frame_index_;

protected:
	bool				mouse_drag_flag_;
	int					mouse_drag_mouse_x_;
	int					mouse_drag_mouse_y_;
	int					mouse_drag_scroll_x_;
	int					mouse_drag_scroll_y_;
};
