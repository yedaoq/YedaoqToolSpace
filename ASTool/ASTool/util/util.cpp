#include "StdAfx.h"
#include "util.h"
#include <windows.h>
#include <Strsafe.h>
#include "..\resource.h"

#define TOOLRECORD_DLL_NAME TEXT("toolRecord.dll")

TCHAR zoom_bin_path_[MAX_PATH * 2] = TEXT("");
TCHAR zoom_log_path_[MAX_PATH * 2] = TEXT("");
TCHAR zoom_exe_name_[MAX_PATH] = TEXT("Zoom.exe");
TCHAR tool_err_[1024] = TEXT("");

bool GetProductBinDir(LPTSTR buf, int buf_len)
{
	StringCchCopy(buf, buf_len, zoom_bin_path_);
	return true;
}

bool GetProductLogDir(LPTSTR buf, int buf_len)
{
	StringCchCopy(buf, buf_len, zoom_log_path_);
	return true;
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

bool IsRunAsAdmin()
{
	BOOL                     bIsAdmin              = FALSE;  
	SID_IDENTIFIER_AUTHORITY NtAuthority           = SECURITY_NT_AUTHORITY;  
	PSID                     AdministratorsGroup;   

	if(AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,  
		0, 0, 0, 0, 0, 0, &AdministratorsGroup)) 
	{  
		if (!CheckTokenMembership( NULL, AdministratorsGroup, &bIsAdmin))   
		{  
			bIsAdmin = FALSE;  
		}   
		FreeSid(AdministratorsGroup);   
	}  

	return FALSE != bIsAdmin;  
}

bool RemoveFile( LPCTSTR path )
{
	if (DeleteFile(path))
		return true;

	TCHAR buf[MAX_PATH * 2];
	if (S_OK != StringCchCopy(buf, ARRAYSIZE(buf), path))
		return false;

	int path_len = _tcslen(buf);

	for (int i = 1; i < 10000; ++i)
	{
		_stprintf_s(buf + path_len, ARRAYSIZE(buf) - path_len, TEXT("_%d"), i);
		if (INVALID_FILE_ATTRIBUTES == ::GetFileAttributes(buf))
		{
			return FALSE != ::MoveFile(path, buf);
		}
	}

	return false;
}

void SetToolErrorMsg( LPCTSTR msg )
{
	int len = 0;
	if (msg)
	{
		len = StringCchCopy(tool_err_, ARRAYSIZE(tool_err_) - 1, msg);
	}
	tool_err_[len] = 0;
}

LPCTSTR GetToolErrorMsg()
{
	return tool_err_;
}

bool ExtractResource( HMODULE res_module, DWORD res_id, LPCTSTR res_type, ExtractResourceCallback_t callback, LPVOID callback_param )
{
	bool ret = false;
	HRSRC resource_handle = FindResource(res_module, MAKEINTRESOURCE(res_id), TEXT("DLL"));
	if (NULL != resource_handle)
	{
		LPVOID res_buf = NULL;
		DWORD  res_size = SizeofResource(res_module, resource_handle);
		if (res_size > 0)
		{
			HGLOBAL res_global = LoadResource(res_module, resource_handle);
			if (NULL != res_global)
			{
				res_buf = LockResource(res_global);
				if (NULL != res_buf)
				{
					ret = callback(res_buf, res_size, callback_param);
				}
			}
		}
	}

	return ret;
}

bool ExtractResourceCallback_SaveToFile( LPVOID buf, DWORD buf_len, LPVOID callback_param )
{
	LPCTSTR file_path = (LPCTSTR)callback_param;
	HANDLE file_handle = CreateFile(file_path, FILE_WRITE_ACCESS, 0, NULL, CREATE_ALWAYS, NULL, NULL);
	if (INVALID_HANDLE_VALUE == file_handle)
	{
		SetToolErrorMsg(TEXT("error : can not create target file!\r\n"));
		return false;
	}

	DWORD bytes_written = 0;
	WriteFile(file_handle, buf, buf_len, &bytes_written, NULL);
	FlushFileBuffers(file_handle);
	CloseHandle(file_handle);

	return bytes_written == buf_len;
}

bool LaunchUacApp( LPCTSTR app, LPCTSTR params, bool wait )
{
	SHELLEXECUTEINFO info = {0};
	info.fMask = SEE_MASK_NOCLOSEPROCESS;
	info.cbSize = sizeof(info);
	info.lpFile = app;
	info.lpParameters = params;
	info.lpVerb = TEXT("runas");

	if (!ShellExecuteEx(&info) || NULL == info.hProcess)
	{
		SetToolErrorMsg(TEXT("无法以管理员权限启动程序！"));
		return false;
	}
	else
	{
		WaitForSingleObject(info.hProcess, INFINITE);
		CloseHandle(info.hProcess);
		info.hProcess = NULL;
		return true;
	}
}

bool IsToolRecordInstalled()
{
	TCHAR path_dst[MAX_PATH * 2];
	if (!GetProductBinDir(path_dst, ARRAYSIZE(path_dst)))
		return false;

	::PathAppend(path_dst, TOOLRECORD_DLL_NAME);
	DWORD file_attr = ::GetFileAttributes(path_dst);
	return INVALID_FILE_ATTRIBUTES != file_attr;
}

bool InstallToolRecord( bool adjust_uac )
{
	TCHAR path_dst[MAX_PATH * 2];
	if (!GetProductBinDir(path_dst, ARRAYSIZE(path_dst)))
	{
		SetToolErrorMsg(TEXT("无法定位Zoom安装目录！"));
		return false;
	}
	else
	{
		::PathAppend(path_dst, TOOLRECORD_DLL_NAME);
	}

	HMODULE res_module = GetModuleHandle(NULL);
	if (!ExtractResource(res_module, IDR_DLL_RECORD, TEXT("DLL"), ExtractResourceCallback_SaveToFile, path_dst))
	{
		SetToolErrorMsg(TEXT("解压") TOOLRECORD_DLL_NAME TEXT("文件失败！"));

		if (adjust_uac)
		{
			if (!IsRunAsAdmin())
			{
				TCHAR path_exe[MAX_PATH * 2];
				::GetModuleFileName(NULL, path_exe, ARRAYSIZE(path_exe));
				LaunchUacApp(path_exe, TEXT("inst_tool_record"), true);				
			}
		}
	}

	DWORD file_attr = GetFileAttributes(path_dst);
	return INVALID_FILE_ATTRIBUTES != file_attr;
}

bool UninstallToolRecord( bool adjust_uac )
{
	TCHAR path_dst[MAX_PATH * 2];
	if (!GetProductBinDir(path_dst, ARRAYSIZE(path_dst)))
	{
		return false;
	}

	::PathAppend(path_dst, TEXT("toolRecord.dll"));
	if (INVALID_FILE_ATTRIBUTES == ::GetFileAttributes(path_dst))
	{
		return true;
	}

	if (!RemoveFile(path_dst))
	{
		if (adjust_uac)
		{
			if (!IsRunAsAdmin())
			{
				TCHAR path_exe[MAX_PATH * 2];
				::GetModuleFileName(NULL, path_exe, ARRAYSIZE(path_exe));
				LaunchUacApp(path_exe, TEXT("unst_tool_record"), true);
			}
		}
	}

	DWORD file_attr = GetFileAttributes(path_dst);
	return INVALID_FILE_ATTRIBUTES == file_attr;
}

bool IsAppLaunchForInstallToolRecord(LPCTSTR cmd_line)
{
	if (_tcsstr(cmd_line, TEXT("inst_tool_record")))
	{
		InstallToolRecord(false);
		return true;
	}
	
	if (_tcsstr(cmd_line, TEXT("unst_tool_record")))
	{
		UninstallToolRecord(false);
		return true;
	}

	return false;
}

bool CheckWindowsMenuItem( HMENU menu, DWORD item_id, bool check )
{
	DWORD check_flag = check ? MF_CHECKED : MF_UNCHECKED;
	DWORD ret = CheckMenuItem(menu, ID_ASIMG_ENABLE, MF_BYCOMMAND | check_flag);
	return -1 != ret;
}

bool IsWindowsMenuItemChecked( HMENU menu, DWORD item_id, bool default )
{
	MENUITEMINFO mii = {0};
	mii.cbSize = sizeof(mii);
	mii.fMask = MIIM_STATE;
	if (GetMenuItemInfo(menu, item_id, FALSE, &mii))
	{
		return 0 != (mii.fState & MFS_CHECKED);
	}

	return default;
}

bool SetProduct( EnumZoomProduct product )
{
	switch(product)
	{
	case PRODUCT_ZOOM:
		{
			DWORD dwType;
			TCHAR szVal[MAX_PATH * 2];
			DWORD dwSize = sizeof(szVal);
			LSTATUS result = SHGetValue(HKEY_CLASSES_ROOT, TEXT("ZoomRecording\\DefaultIcon"), TEXT(""), &dwType, szVal, &dwSize);
			if (ERROR_SUCCESS == result)
			{
				LPTSTR tail = _tcsstr(szVal + 1, TEXT("Zoom.exe"));
				if (tail)
				{
					*tail = 0;
					_tcscpy_s(zoom_bin_path_, ARRAYSIZE(zoom_bin_path_), szVal + 1);
				}
			}

			SHGetSpecialFolderPath(NULL, zoom_log_path_, CSIDL_APPDATA, FALSE);
			::PathAppend(zoom_log_path_, TEXT("zoom"));

			if (!*zoom_bin_path_)
			{
				StringCchCopy(zoom_bin_path_, ARRAYSIZE(zoom_bin_path_), zoom_log_path_);
				::PathAppend(zoom_bin_path_, TEXT("bin"));
			}

			::PathAppend(zoom_log_path_, TEXT("logs"));

			
			return true;
		}

	}

	return true;
}

__int64 GetTimeDeltaMiniseconds( const FILETIME& t1, const FILETIME& t2 )
{
	return ((__int64&)t1 - (__int64&)t2) / 10000;
}
