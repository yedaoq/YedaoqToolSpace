// toolRecord.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "toolRecord.h"
#include <windows.h>
#include <shlwapi.h>
#include <tchar.h>
#include "..\cpt_data.h"

#pragma comment(lib, "shlwapi.lib")

static TCHAR	img_log_file_path_[MAX_PATH * 2] = TEXT("");
static HANDLE	img_log_file_handle_ = INVALID_HANDLE_VALUE;

extern "C" __declspec(dllexport) bool log_init(LPCTSTR log_file_path)
{
	if (!log_file_path || !*log_file_path)
	{
		return false;
	}

	OutputDebugString(log_file_path);

	_tcscpy_s(img_log_file_path_, ARRAYSIZE(img_log_file_path_) - 1, log_file_path);
	::PathRenameExtension(img_log_file_path_, TEXT(".imgs"));

	img_log_file_handle_ = CreateFile(img_log_file_path_, FILE_WRITE_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
	return INVALID_HANDLE_VALUE != img_log_file_handle_;
}

extern "C" __declspec(dllexport) void log_uninit()
{
	if (INVALID_HANDLE_VALUE != img_log_file_handle_)
	{
		FlushFileBuffers(img_log_file_handle_);
		CloseHandle(img_log_file_handle_);
		img_log_file_handle_ = INVALID_HANDLE_VALUE;
	}

	*img_log_file_path_ = 0;
}

extern "C" __declspec(dllexport) void log_capture_data(cpt_common* data)
{
	OutputDebugString(img_log_file_path_);

	DWORD bytes_written = 0;

	FILETIME ft;
	CoFileTimeNow(&ft);

	WriteFile(img_log_file_handle_, &ft, sizeof(ft), &bytes_written, NULL);
	WriteFile(img_log_file_handle_, data, data->size, &bytes_written, NULL);
	FlushFileBuffers(img_log_file_handle_);
}