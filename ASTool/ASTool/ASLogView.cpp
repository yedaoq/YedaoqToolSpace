// ASLogView.cpp : implementation of the CASLogView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "ASLogView.h"

#include "Data.h"
#include <stdio.h>
#include "Scintilla/Scintilla.h"
#include "FilterList.h"
#include "util/util.h"

const int		read_buffer_size_ = 0x1000;

struct ReadBuffer
{
	char		buffer_[read_buffer_size_ + 1];
	char*		surplus_;

	ReadBuffer*	prev_;
	ReadBuffer* next_;

} read_buffers_[3];

ReadBuffer*		buffer_to_read_ = read_buffers_;

HANDLE			read_event_ = INVALID_HANDLE_VALUE;
HANDLE			log_console_handle_ = INVALID_HANDLE_VALUE;

void InitReadBuffers()
{
	for (int i = 0; i < ARRAYSIZE(read_buffers_); ++i)
	{
		read_buffers_[i].buffer_[read_buffer_size_] = read_buffers_[i].buffer_[0] = 0;
		read_buffers_[i].surplus_ = 0;

		int prev_buf_idx = i - 1;
		if (prev_buf_idx < 0) prev_buf_idx = (ARRAYSIZE(read_buffers_) - 1);
		read_buffers_[i].prev_ = read_buffers_ + prev_buf_idx;

		int next_buf_idx = i + 1;
		if (next_buf_idx >= ARRAYSIZE(read_buffers_)) next_buf_idx = 0;
		read_buffers_[i].next_ = read_buffers_ + next_buf_idx;
	}

	read_event_ = CreateEvent(NULL, FALSE, FALSE, NULL);
}

ProcessData data;

CASLogView::CASLogView()
{
	layout_main_ = new NSYedaoqLayout::CFlowLayout(NSYedaoqLayout::Direction_Vertical);
	ctl_log_ = NULL;
}

BOOL CASLogView::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CASLogView::OnIdle()
{
	return FALSE;
}

LRESULT CASLogView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

	InitSciLexerCtrl();

	log_filters_buf_[0] = 0;
	memset(log_filters_, 0, sizeof(log_filters_));

	log_file_handle_ = INVALID_HANDLE_VALUE;
	read_timer_id_ = SetTimer(1, 50, NULL);

	InitReadBuffers();
	filters_serializer_.LoadFilters();

	ctl_cmb_file_list_.Attach(GetDlgItem(IDC_CMB_FILES));
	ctl_cmb_file_filter_.Attach(GetDlgItem(IDC_CMB_FILTER));

	NSYedaoqLayout::CFlowLayout* layout_top = layout_main_->AddFlow(NSYedaoqLayout::Direction_Horizon);
	layout_top->AddCtrl(GetDlgItem(IDC_CMB_FILES));
	layout_top->AddCtrl(GetDlgItem(IDC_BTN_REFRESH));
	layout_top->AddCtrl(GetDlgItem(IDC_STATIC1), NSYedaoqLayout::ResizeInfo::FixedInfo, NSYedaoqLayout::ResizeInfo::FixedInfo, NSYedaoqLayout::AnchorInfo::AnchorCenter, NSYedaoqLayout::AnchorInfo::AnchorCenter);
	layout_top->AddCtrl(GetDlgItem(IDC_CMB_FILTER), NSYedaoqLayout::ResizeInfo::FillInfo, NSYedaoqLayout::ResizeInfo::FixedInfo, NSYedaoqLayout::AnchorInfo::AnchorFront, NSYedaoqLayout::AnchorInfo::AnchorCenter);
	layout_top->AddCtrl(GetDlgItem(IDOK));
	layout_top->AddCtrl(GetDlgItem(IDC_BTN_CLEAR));

	layout_main_->AddCtrl(ctl_log_, NSYedaoqLayout::ResizeInfo::FillInfo, NSYedaoqLayout::ResizeInfo::FillInfo);

	//AllocConsole();
	//log_console_handle_ = GetStdHandle(STD_OUTPUT_HANDLE);

	InitFilterCombobox();

	BOOL tmp;
	OnBnClickedBtnRefresh(0, 0, 0, tmp);

	return TRUE;
}

LRESULT CASLogView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	return 0;
}

LRESULT CASLogView::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	char buf[1024] = "";
	::GetWindowTextA(ctl_cmb_file_filter_.m_hWnd, buf, ARRAYSIZE(buf) - 1);
	if (*buf)
	{
		filters_serializer_.AddFilter(buf);
		InitFilterCombobox();
	}

	return 0;
}

LRESULT CASLogView::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	filters_serializer_.SaveFilters();
	CloseDialog(wID);
	return 0;
}

void CASLogView::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}

bool CASLogView::InitFileList()
{
	ctl_cmb_file_list_.SetCurSel(-1);
	ctl_cmb_file_list_.ResetContent();

	for (DWORD i = 0; i < data.log_file_count_; ++i)
	{
		ctl_cmb_file_list_.AddString(data.log_file_names_[i]);
	}
	return TRUE;  
}

LRESULT CASLogView::OnCbnSelchangeCmbFiles(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	int cur_file_idx = ctl_cmb_file_list_.GetCurSel();
	if (cur_file_idx >= 0)
	{
		if (!OpenLogFile(data.log_file_names_[cur_file_idx]))
		{
			SetWindowText(TEXT("无法打开文件"));
			return 0;
		}

		::SendMessage(ctl_log_, SCI_CLEARALL, 0, 0);
	}
	return 0;
}

bool CASLogView::OpenLogFile( LPCTSTR file_name )
{
	if (!file_name || !*file_name)
	{
		return false;
	}

	CloseLogFile();

	TCHAR buf[1024];
	::SHGetSpecialFolderPath(NULL, buf, CSIDL_APPDATA, FALSE);
	::PathAppend(buf, data.product_name_);
	::PathAppend(buf, TEXT("logs"));
	::PathAppend(buf, file_name);

	log_file_handle_ = ::CreateFile(buf, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, /*0*/ FILE_FLAG_OVERLAPPED, NULL);
	if (INVALID_HANDLE_VALUE == log_file_handle_)
	{
		return false;
	}

	GetFileSizeEx(log_file_handle_, &log_file_position_);
	//SetFilePointer(log_file_handle_, 0, 0, FILE_END);

	return true;
}

void CASLogView::CloseLogFile()
{
	if (INVALID_HANDLE_VALUE != log_file_handle_)
	{
		CloseHandle(log_file_handle_);
		log_file_handle_ = INVALID_HANDLE_VALUE;
		//read_content_begin_ = read_content_end_ = 0;
	}
}

LRESULT CASLogView::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: Add your message handler code here and/or call default
	if (read_timer_id_ == (UINT_PTR)wParam )
	{
		if (INVALID_HANDLE_VALUE != log_file_handle_)
		{
			ReadLogAsync();
			OutputDebugStringA("OnTimer\r\n");
		}
	}

	return 0;
}

bool CASLogView::ReadLogAsync()
{
	OVERLAPPED gOverlapped = {0};
	gOverlapped.hEvent = read_event_; 

	for (int i = 0; i < 100; ++i)
	{
		gOverlapped.Offset		= log_file_position_.LowPart; 
		gOverlapped.OffsetHigh	= log_file_position_.HighPart; 

		DWORD bytes_readed		= 0;

		buffer_to_read_ = buffer_to_read_->next_;

		BOOL bResult = ReadFile(log_file_handle_, buffer_to_read_->buffer_, read_buffer_size_, &bytes_readed, &gOverlapped); 
		DWORD dwError = GetLastError();

		if (*log_filters_buf_)
		{
			ParseLogContent(buffer_to_read_->prev_);
		}

		if (bResult) 
		{
			throw 0;
		}

		switch (dwError) 
		{ 
		case ERROR_HANDLE_EOF: 
			::PostMessage(ctl_log_, SCI_SCROLLTOEND, 0, 0);
			return true;
		case ERROR_IO_PENDING: 
			bResult = GetOverlappedResult(log_file_handle_, &gOverlapped, &bytes_readed, TRUE); 

			log_file_position_.QuadPart += bytes_readed;
			buffer_to_read_->buffer_[bytes_readed] = 0;

			if (!bResult) 
			{ 
				switch (dwError = GetLastError()) 
				{ 
				case ERROR_HANDLE_EOF: 
					{ 
						if (bytes_readed > 0)
						{
							throw 0;
						}
						return true;
					} 
					break;
				default:
					throw 0;
					break;
				}
			} 

			if (bytes_readed > 0)
			{
				if (!*log_filters_buf_)
				{
					PrintLog(buffer_to_read_->buffer_, bytes_readed);
					::SendMessage(ctl_log_, SCI_SCROLLTOEND, 0, 0);
				}
			}
			else
			{
				return true;
			}
			break;
		default:
			throw 0;
		} 
	}

	::PostMessage(ctl_log_, SCI_SCROLLTOEND, 0, 0);

	return true;
}

// bool CASLogView::ReadLog()
// {
// 	DWORD bytes_readed = 0;
// 	do 
// 	{
// 		BOOL ret = ReadFile(log_file_handle_, read_buffer_ + read_content_end_, read_buffer_size_ - read_content_end_, &bytes_readed, NULL);
// 		if (!ret)
// 			break;
// 
// 		OutputDebugStringA("Read block!\r\n");
// 
// 		read_content_end_ += bytes_readed;
// 		read_buffer_[read_content_end_] = 0;
// 
// 		ParseLogContent();
// 	
// 	} while (bytes_readed > 0);
// 	
// 	return true;
// }

bool CASLogView::ParseLogContent(ReadBuffer* buf)
{
	static char msg_line_long[] = "log line too long to hold!\n";
	static char joint_buf[4096];
	static int	joint_buf_len = 0;

	// 处理第一条数据，需要和上次读取的剩余数据合并
	char*	line_begin = buf->buffer_;
	char*	line_end = strchr(line_begin, '\n');
	int		line_size = 0;

	if (!line_end)
	{
		if (*line_begin)	// 空数据
		{
			int line_size = strlen(line_begin);
			if (line_size > ARRAYSIZE(joint_buf) - joint_buf_len)
			{
				PrintLog(msg_line_long, ARRAYSIZE(msg_line_long) - 1);
			}
			else
			{
				memcpy(joint_buf + joint_buf_len, line_begin, line_size);
				joint_buf_len += line_size;
			}
		}
		return true;
	}

	line_size = line_end - line_begin;

	if (joint_buf_len > 0)
	{
		memcpy(joint_buf + joint_buf_len, line_begin, line_size);
		joint_buf_len += line_size;
		joint_buf[joint_buf_len] = 0;

		if (TestFilters(joint_buf))
		{
			joint_buf[joint_buf_len] = '\n';
			joint_buf[joint_buf_len + 1] = 0;
			PrintLog(joint_buf, joint_buf_len + 1);
		}

		joint_buf_len = 0;
		line_begin = line_end + 1;
	}	

	for (line_end = strchr(line_begin, '\n'); line_end; line_end = strchr(line_begin, '\n'))
	{
		*line_end = 0;
		if (TestFilters(line_begin))
		{
			*line_end = '\n';
			PrintLog(line_begin, line_end - line_begin + 1);
		}

		line_begin = line_end + 1;
	}

	if (*line_begin)
	{
		joint_buf_len = strlen(line_begin);
		memcpy(joint_buf, line_begin, joint_buf_len);
	}

	::PostMessage(ctl_log_, SCI_SCROLLTOEND, 0, 0);

	return true;
}

void CASLogView::PrintLog( const char* log, int log_len )
{
	::SendMessage(ctl_log_, SCI_APPENDTEXT /*SCI_ADDTEXT*/, log_len, (LPARAM)log);
	//WriteConsoleA(log_console_handle_, log, log_len, NULL, NULL );
	//puts(log);
}

// bool CASLogView::ParseLogContent()
// {
// 	static TCHAR wide_buf[4096] = TEXT("123");
// 
// 	char* content_begin = read_buffer_ + read_content_begin_;
// 	char* content_end = read_buffer_ + read_content_end_;
// 	for (char* content_ptr = strchr(content_begin, '\n'); content_ptr; content_ptr = strchr(content_begin, '\n'))
// 	{
// 		*content_ptr = 0;
// 		if (content_ptr > content_begin)
// 		{
// 			int ret = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, content_begin, content_ptr - content_begin + 1, wide_buf, ARRAYSIZE(wide_buf));
// 			if (ret > 0)
// 			{
// 				ctl_lst_log_.AddString(wide_buf);
// 			}
// 		}
// 
// 		content_begin = content_ptr + 1;
// 		read_content_begin_ = content_begin - read_buffer_;
// 	}
// 
// 	if (read_content_end_ == read_content_begin_)
// 	{
// 		read_content_begin_ = read_content_end_ = 0;
// 	}
// 	else if(read_content_end_ >= read_buffer_size_)
// 	{
// 		read_content_end_ = read_content_end_ - read_content_begin_;
// 		memmove(read_buffer_, read_buffer_ + read_content_begin_, read_content_end_);
// 		read_content_begin_	= 0;
// 	}
// 
// 	return true;
// }

LRESULT CASLogView::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	// TODO: Add your message handler code here and/or call default
	if (SIZE_MINIMIZED != wParam)
	{
		this->layout_main_->Layout(NSYedaoqLayout::LayoutPoint(3,3), NSYedaoqLayout::LayoutSize(LOWORD(lParam) - 6, HIWORD(lParam) - 6));
		InvalidateRect(NULL, TRUE);
	}
	return 0;
}

LRESULT CASLogView::OnBnClickedBtnRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	// TODO: Add your control notification handler code here
	bHandled = TRUE;
	if(data.InitRunningMettingInfo())
	{
		data.InitRunningMettingLog();
		InitFileList();

		SetWindowText(TEXT("toolLogView"));
	}
	else
	{
		SetWindowText(TEXT("找不到CptHost.exe进程，请确认会议已开始或是否通过服务启动AS，然后重新启动"));
	}

	return 0;
}

LRESULT CASLogView::OnCbnEditupdateCmbFilter(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	::GetWindowTextA(ctl_cmb_file_filter_.m_hWnd, log_filters_buf_, ARRAYSIZE(log_filters_buf_) - 1);
	UpdateFilters();
	return 0;
}


LRESULT CASLogView::OnCbnSelchangeCmbFilter(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	// TODO: Add your control notification handler code here
	int idx = ctl_cmb_file_filter_.GetCurSel();
	if (idx >= 0)
	{
		::SendMessageA(ctl_cmb_file_filter_, CB_GETLBTEXT, idx, (LPARAM)log_filters_buf_);
		UpdateFilters();
	}
	return 0;
}


bool CASLogView::TestFilters( char* log )
{
	for (char** filter_ptr = log_filters_; *filter_ptr; ++filter_ptr)
	{
		if (strstr(log, *filter_ptr))
		{
			return true;
		}
	}

	return false;
}

bool CASLogView::InitFilterCombobox()
{
	ctl_cmb_file_filter_.Clear();

	TCHAR buf[1024];

	FilterList::Item* first_item = filters_serializer_.First();
	FilterList::Item* item = first_item;
	while(*item->buf)
	{
		MultiByteToWideChar(CP_UTF8, 0, item->buf, -1, buf, ARRAYSIZE(buf));
		ctl_cmb_file_filter_.AddString(buf);
		item = item->next;
		if (item == first_item)
		{
			break;
		}
	}

	return true;
}

void CASLogView::UpdateFilters()
{
	int log_filter_count = 0;
	char* tmp = log_filters_buf_;
	if (*log_filters_buf_)
	{
		while(log_filter_count < ARRAYSIZE(log_filters_) - 1)
		{
			while(0 != *tmp && '|' == *tmp) ++tmp;
			if (!*tmp)
				break;

			log_filters_[log_filter_count++] = tmp;

			tmp = strchr(tmp, '|');
			if (tmp)
				*tmp++ = 0;
			else
				break;
		}
	}

	log_filters_[log_filter_count] = 0;

	::SendMessage(ctl_log_, SCI_CLEARALL, 0, 0);
}


LRESULT CASLogView::OnBnClickedBtnClear(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	ClearLog();
	return 0;
}

void CASLogView::ClearLog()
{
	::SendMessage(ctl_log_, SCI_CLEARALL, 0, 0);
}

bool CASLogView::InitSciLexerCtrl()
{
	HMODULE sciLexer_module = ::LoadLibrary(TEXT("SciLexer.dll"));
	if (!sciLexer_module)
	{
		TCHAR sciLexer_path[MAX_PATH * 2] = TEXT("");
		HMODULE app_module = GetModuleHandle(NULL);
		GetModuleFileName(app_module, sciLexer_path, ARRAYSIZE(sciLexer_path));
		::PathRemoveFileSpec(sciLexer_path);
		::PathAppend(sciLexer_path, TEXT("SciLexer.dll"));

		if(ExtractResource(app_module, IDR_DLL_SCILEXER, TEXT("DLL"), ExtractResourceCallback_SaveToFile, sciLexer_path))
			sciLexer_module = ::LoadLibrary(TEXT("SciLexer.dll"));
	}

	if (sciLexer_module)
	{
		ctl_log_ = CreateWindowEx(0, TEXT("Scintilla"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPCHILDREN, 6, 40, 500, 400, *this, NULL, NULL, NULL);
	}
	else
	{
		MessageBox(TEXT("缺乏SciLexer.dll，无法启动！"), TEXT("Error"), MB_OK);
	}

	return NULL != ctl_log_;
}
