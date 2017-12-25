#include "StdAfx.h"
#include "..\resource.h"
#include "..\..\cpt_data.h"
#include "DialogInfo.h"
#include "ASFrameRes.h"

CDialogInfo::CDialogInfo( void )
{
	frame_index_ = -1;
}

CDialogInfo::~CDialogInfo(void)
{
}

LRESULT CDialogInfo::OnInitDialog( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
{
	bHandled =TRUE;
	ctl_edt_info_.Attach(GetDlgItem(IDC_RESULT));
	return 0;
}

LRESULT CDialogInfo::OnDestroy( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
{
	EndDialog(0);
	return 0;
}

LRESULT CDialogInfo::OnSize( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;

	if(! (SIZE_MAXIMIZED == wParam || SIZE_RESTORED == wParam || 0 == wParam))
	{
		return 0;
	}

	RECT rc = {0, 0, LOWORD(lParam), HIWORD(lParam)};
	ctl_edt_info_.MoveWindow(0, 0, rc.right, rc.bottom, FALSE);
	return 0;
}

LRESULT CDialogInfo::OnRefreshPanel( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;

	if (frame_res_->GetFrameIndex() == frame_index_)
	{
		return 0;
	}

	//ctl_edt_info_.Clear();
	ctl_edt_info_.SetWindowText(TEXT(""));

	frame_index_ = frame_res_->GetFrameIndex();

	TCHAR buf[256];
	_stprintf_s(buf, ARRAYSIZE(buf) - 1, TEXT("frame index : %d\r\n\r\n"), frame_index_);
	ctl_edt_info_.AppendText(buf);

	const RECT& screen_rc = frame_res_->ScreenRect();
	_stprintf_s(buf, ARRAYSIZE(buf) - 1, TEXT("image pos : (%d, %d)\r\n      size: (%d, %d)\r\n"), screen_rc.left, screen_rc.top, screen_rc.right - screen_rc.left, screen_rc.bottom - screen_rc.top); 
	ctl_edt_info_.AppendText(buf);

	const HRGN* rgns = frame_res_->ScreenRegions();

	char rgn_buf[1024];


	if (rgns[REGION_VISIBLE])
	{
		GetRegionData(rgns[REGION_VISIBLE], ARRAYSIZE(rgn_buf), (LPRGNDATA)rgn_buf);
		OutputRegion((LPRGNDATA)rgn_buf,	TEXT("\r\nvisible   region : "));
	}

	if (rgns[REGION_INVISIBLE])
	{
		GetRegionData(rgns[REGION_INVISIBLE], ARRAYSIZE(rgn_buf), (LPRGNDATA)rgn_buf);
		OutputRegion((LPRGNDATA)rgn_buf,	TEXT("\r\ninvisible region : "));
	}

	if (rgns[REGION_UPDATE])
	{
		GetRegionData(rgns[REGION_UPDATE], ARRAYSIZE(rgn_buf), (LPRGNDATA)rgn_buf);
		OutputRegion((LPRGNDATA)rgn_buf,	TEXT("\r\nupdate    region : "));
	}

	if (rgns[REGION_BACK])
	{
		GetRegionData(rgns[REGION_BACK], ARRAYSIZE(rgn_buf), (LPRGNDATA)rgn_buf);
		OutputRegion((LPRGNDATA)rgn_buf,	TEXT("\r\nback      region : "));
	}

	LPWSTR system_cursors[] = {IDC_APPSTARTING, IDC_ARROW, IDC_CROSS, IDC_HAND, IDC_HELP, IDC_IBEAM, IDC_ICON, IDC_NO, IDC_SIZE};
	LPCTSTR system_cursor_names[] = {TEXT("IDC_APPSTARTING"), TEXT("IDC_ARROW"), TEXT("IDC_CROSS"), TEXT("IDC_HAND"), TEXT("IDC_HELP"), TEXT("IDC_IBEAM"), TEXT("IDC_ICON"), TEXT("IDC_NO"), TEXT("IDC_SIZE")};

	for (int i = 0; i < ARRAYSIZE(system_cursors); ++i)
	{
		_stprintf_s(buf, ARRAYSIZE(buf), TEXT("%s : 0x%p\r\n"), system_cursor_names[i], LoadCursor(NULL, system_cursors[i]));
		ctl_edt_info_.AppendText(buf);
	}

	_stprintf_s(buf, ARRAYSIZE(buf), TEXT("Current Cursor : 0x%p"), GetCursor());
	ctl_edt_info_.AppendText(buf);

	return 0;
}

// LRESULT CDialogInfo::OnRefreshPanel1( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled )
// {
// 	bHandled = TRUE;
// 
// 	cpt_screen_data* screen_data = (cpt_screen_data*)lParam;
// 
// 	if (!screen_data || screen_data->index == frame_index_)
// 	{
// 		return 0;
// 	}
// 
// 	//ctl_edt_info_.Clear();
// 	ctl_edt_info_.SetWindowText(TEXT(""));
// 
// 	frame_index_ = screen_data->index;
// 
// 	TCHAR buf[256];
// 	_stprintf_s(buf, ARRAYSIZE(buf) - 1, TEXT("frame index : %d\r\n\r\n"), screen_data->index);
// 	ctl_edt_info_.AppendText(buf);
// 
// 	_stprintf_s(buf, ARRAYSIZE(buf) - 1, TEXT("image pos : (%d, %d)\r\n      size: (%d, %d)\r\n"), screen_data->left, screen_data->top, screen_data->bi.bmiHeader.biWidth, screen_data->bi.bmiHeader.biHeight); 
// 	ctl_edt_info_.AppendText(buf);
// 
// 	if (screen_data->visible_rgn_data_len > 0)
// 	{
// 		OutputRegion(screen_data->get_visible_rgn_data(), TEXT("\r\nvisible region : "));
// 	}
// 	
// 	if (screen_data->invisible_rgn_data_len > 0)
// 	{
// 		OutputRegion(screen_data->get_invisible_rgn_data(), TEXT("\r\ninvisible region : "));
// 	}
// 	
// 	if (screen_data->updated_rgn_data_len > 0)
// 	{
// 		OutputRegion(screen_data->get_updated_rgn_data(), TEXT("\r\nupdate region : "));
// 	}
// 	
// 	if (screen_data->back_rgn_data_len > 0)
// 	{
// 		OutputRegion(screen_data->get_back_rgn_data(), TEXT("\r\nback region : "));
// 	}
// 
// 	LPWSTR system_cursors[] = {IDC_APPSTARTING, IDC_ARROW, IDC_CROSS, IDC_HAND, IDC_HELP, IDC_IBEAM, IDC_ICON, IDC_NO, IDC_SIZE};
// 	LPCTSTR system_cursor_names[] = {TEXT("IDC_APPSTARTING"), TEXT("IDC_ARROW"), TEXT("IDC_CROSS"), TEXT("IDC_HAND"), TEXT("IDC_HELP"), TEXT("IDC_IBEAM"), TEXT("IDC_ICON"), TEXT("IDC_NO"), TEXT("IDC_SIZE")};
// 
// 	for (int i = 0; i < ARRAYSIZE(system_cursors); ++i)
// 	{
// 		_stprintf_s(buf, ARRAYSIZE(buf), TEXT("%s : 0x%p\r\n"), system_cursor_names[i], LoadCursor(NULL, system_cursors[i]));
// 		ctl_edt_info_.AppendText(buf);
// 	}
// 
// 	_stprintf_s(buf, ARRAYSIZE(buf), TEXT("Current Cursor : 0x%p"), GetCursor());
// 	ctl_edt_info_.AppendText(buf);
// 	
// 	return 0;
// }

void CDialogInfo::OutputRegion( LPRGNDATA rgn_data, LPCTSTR desc )
{
	ctl_edt_info_.AppendText(desc);
	TCHAR buf[128];
	_stprintf_s(buf, ARRAYSIZE(buf), TEXT("\r\n    (%d, %d, %d, %d)\r\n"), rgn_data->rdh.rcBound.left, rgn_data->rdh.rcBound.top, rgn_data->rdh.rcBound.right, rgn_data->rdh.rcBound.bottom);
	ctl_edt_info_.AppendText(buf);

	RECT* rc = (RECT*)((char*)rgn_data + rgn_data->rdh.dwSize);
	for (unsigned int i = 0; i < rgn_data->rdh.nCount; ++i)
	{
		_stprintf_s(buf, ARRAYSIZE(buf), TEXT("    (%d, %d, %d, %d)\r\n"), rc->left, rc->top, rc->right, rc->bottom);
		ctl_edt_info_.AppendText(buf);
		++rc;
	}
}

void CDialogInfo::SetFrameRes( ASFrameRes* res )
{
	frame_res_ = res;
}

