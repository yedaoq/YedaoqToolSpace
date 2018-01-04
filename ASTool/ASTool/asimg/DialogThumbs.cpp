#include "StdAfx.h"
#include "DialogThumbs.h"
#include "..\resource.h"
#include "ASFrameRes.h"


CDialogThumbs::CDialogThumbs( void )
{
	thumb_handles_ = 0;
	brush_dlg_background_ = CreateSolidBrush(0xDDDDDD);
}

CDialogThumbs::~CDialogThumbs(void)
{
}

LRESULT CDialogThumbs::OnInitDialog( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
{
	dc_memory_ = CreateCompatibleDC(NULL);
	WTL::CButton ctl(GetDlgItem(IDC_BTN_THUMB4));
	ctl.SetIcon(LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ASIMG_RIGHT)));
	return 0;
}

LRESULT CDialogThumbs::OnDrawItem( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;

	if (!thumb_handles_)
	{
		return 0;
	}

	LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;

	switch(wParam)
	{
	case IDC_BTN_THUMB1:
		DrawOwnerDrawCtl(lpDrawItem, thumb_handles_[0]);
		break;
	case IDC_BTN_THUMB2:
		DrawOwnerDrawCtl(lpDrawItem, thumb_handles_[1]);
		break;
	case IDC_BTN_THUMB3:
		DrawOwnerDrawCtl(lpDrawItem, thumb_handles_[2]);
		break;
	case IDC_BTN_THUMB4:
		DrawOwnerDrawCtl(lpDrawItem, thumb_handles_[3]);
		break;
	case IDC_BTN_THUMB5:
		DrawOwnerDrawCtl(lpDrawItem, thumb_handles_[4]);
		break;
	case IDC_BTN_THUMB6:
		DrawOwnerDrawCtl(lpDrawItem, thumb_handles_[5]);
		break;
	case IDC_BTN_THUMB7:
		DrawOwnerDrawCtl(lpDrawItem, thumb_handles_[6]);
		break;
	}
	return 0;
}

LRESULT CDialogThumbs::OnDestroy( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
{
	EndDialog(0);
	return 0;
}

LRESULT CDialogThumbs::OnSize( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;

	if(! (SIZE_MAXIMIZED == wParam || SIZE_RESTORED == wParam || 0 == wParam))
	{
		return 0;
	}

	RECT rc = {0, 0, LOWORD(lParam), HIWORD(lParam)};
	GetClientRect(&rc);

	DWORD thumb_ids[] = {IDC_BTN_THUMB1, IDC_BTN_THUMB2, IDC_BTN_THUMB3, IDC_BTN_THUMB4, IDC_BTN_THUMB5, IDC_BTN_THUMB6, IDC_BTN_THUMB7, };
	
	int item_height = rc.bottom / ARRAYSIZE(thumb_ids);
	for (int i = 0; i < ARRAYSIZE(thumb_ids); ++i)
	{
		HWND ctl_thumb = GetDlgItem(thumb_ids[i]);
		
		::MoveWindow(ctl_thumb, 0, item_height * i + 2, rc.right - 2, item_height - 4, TRUE);
	}
	return 0;
}

LRESULT CDialogThumbs::OnSetThumbs( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;
	thumb_handles_ = (HBITMAP*)lParam;
	return 0;
}

LRESULT CDialogThumbs::OnRefreshPanel( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
{
	bHandled = TRUE;
	thumb_handles_ = frame_res_->NearbyImages();
	InvalidateRect(NULL, FALSE);
	return 0;
}

void CDialogThumbs::DrawOwnerDrawCtl( LPDRAWITEMSTRUCT lpDrawItem, HBITMAP hbp )
{
	if (NULL == hbp)
	{
		return;
	}

	SelectObject(dc_memory_, hbp);

	BITMAP bitmap;
	GetObject(hbp, sizeof(bitmap), &bitmap);

	float rate_x = (float)(lpDrawItem->rcItem.right - lpDrawItem->rcItem.left) / bitmap.bmWidth;
	float rate_y = (float)(lpDrawItem->rcItem.bottom - lpDrawItem->rcItem.top) / abs(bitmap.bmHeight);

	RECT rc;
	if (rate_x > rate_y)
	{
		rc.top = lpDrawItem->rcItem.top;
		rc.bottom = lpDrawItem->rcItem.bottom;

		int width = rate_y * bitmap.bmWidth;
		rc.left = 0;
		rc.right = width;
	}
	else
	{
		rc.left = lpDrawItem->rcItem.left;
		rc.right = lpDrawItem->rcItem.right;

		int height = rate_x * abs(bitmap.bmHeight);

		rc.top = 0;
		rc.bottom = height;
	}

	SetStretchBltMode(lpDrawItem->hDC, STRETCH_HALFTONE); 
	StretchBlt(lpDrawItem->hDC, rc.left, rc.top, rc.right, rc.bottom, dc_memory_, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
}

LRESULT CDialogThumbs::OnDlgColor( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
{
	bHandled = TRUE;
	return (LRESULT)brush_dlg_background_;
}

