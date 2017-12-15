#include "StdAfx.h"
#include "DialogThumbs.h"
#include "..\resource.h"


CDialogThumbs::CDialogThumbs( void )
{
	thumb_handls_ = 0;
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

	LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;

	switch(wParam)
	{
	case IDC_BTN_THUMB1:
		DrawOwnerDrawCtl(lpDrawItem, thumb_handls_[0]);
		break;
	case IDC_BTN_THUMB2:
		DrawOwnerDrawCtl(lpDrawItem, thumb_handls_[1]);
		break;
	case IDC_BTN_THUMB3:
		DrawOwnerDrawCtl(lpDrawItem, thumb_handls_[2]);
		break;
	case IDC_BTN_THUMB4:
		DrawOwnerDrawCtl(lpDrawItem, thumb_handls_[3]);
		break;
	case IDC_BTN_THUMB5:
		DrawOwnerDrawCtl(lpDrawItem, thumb_handls_[4]);
		break;
	case IDC_BTN_THUMB6:
		DrawOwnerDrawCtl(lpDrawItem, thumb_handls_[5]);
		break;
	case IDC_BTN_THUMB7:
		DrawOwnerDrawCtl(lpDrawItem, thumb_handls_[6]);
		break;
	}
	return 0;
}

LRESULT CDialogThumbs::OnDestroy( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
{
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
	thumb_handls_ = (HBITMAP*)lParam;
	return 0;
}

LRESULT CDialogThumbs::OnRefreshPanel( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
{
	bHandled = TRUE;
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

	SetStretchBltMode(lpDrawItem->hDC, STRETCH_HALFTONE); 
	StretchBlt(lpDrawItem->hDC, lpDrawItem->rcItem.left, lpDrawItem->rcItem.top, lpDrawItem->rcItem.right, lpDrawItem->rcItem.bottom, dc_memory_, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
}

LRESULT CDialogThumbs::OnDlgColor( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
{
	bHandled = TRUE;
	return (LRESULT)brush_dlg_background_;
}

