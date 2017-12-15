#include "StdAfx.h"
#include "Data.h"
#include <tlhelp32.h>
#include <tchar.h>

ProcessData::ProcessData(void)
{
	main_process_name_[0] = 0;
	main_process_id_ = 0;
	metting_process_id_ = 0;
	cpthost_process_id_ = 0;

	product_name_[0] = 0;
	log_file_count_ = 0;
	for (int i = 0; i < ARRAYSIZE(log_file_names_); ++i)
	{
		log_file_names_[i][0] = 0;
	}
}

bool ProcessData::InitRunningMettingInfo()
{
	// 通过CptHost往上找父进程的方式来确定产品名称的方式，在通过服务启动CptHost的情况下会有问题
	PROCESSENTRY32 pe32 = {sizeof(pe32)} ;  

	HANDLE hProcessShot = CreateToolhelp32Snapshot ( TH32CS_SNAPPROCESS, 0 ) ;  
	if ( hProcessShot == INVALID_HANDLE_VALUE )  
		return FALSE ;  

	LPCTSTR szTargets[] = {TEXT("CptHost.exe"), TEXT("Zoom.exe"), };
	int		szPids[20] = {0};
	int		szPidCount = 0;

	for( BOOL r = Process32First ( hProcessShot, &pe32); r; r = Process32Next(hProcessShot, &pe32))  
	{  
		if (_tcsstr(pe32.szExeFile, TEXT("CptHost.exe")))
		{
			this->cpthost_process_id_ = pe32.th32ProcessID;
			this->metting_process_id_ = pe32.th32ParentProcessID;
			break;
		}
	}

	if (!metting_process_id_)
	{
		return false;
	}
	
	for( BOOL r = Process32First ( hProcessShot, &pe32); r; r = Process32Next(hProcessShot, &pe32))  
	{  
		if (pe32.th32ProcessID == metting_process_id_)
		{
			_tcscpy(main_process_name_, pe32.szExeFile);
			main_process_id_ = pe32.th32ParentProcessID;
			break;
		}
	}

	if (0 == _tcscmp(main_process_name_, TEXT("CptService.exe")))
	{
		for( BOOL r = Process32First ( hProcessShot, &pe32); r; r = Process32Next(hProcessShot, &pe32))  
		{  
			if (0 == _tcscmp(pe32.szExeFile, TEXT("Zoom.exe")))
			{
				_tcscpy(main_process_name_, pe32.szExeFile);
				main_process_id_ = pe32.th32ProcessID;
				break;
			}
		}
	}

	CloseHandle ( hProcessShot );  

	_tcscpy(product_name_, main_process_name_);
	::PathRemoveExtension(product_name_);

	return true;
}

bool ProcessData::InitRunningMettingLog()
{
	TCHAR buf[MAX_PATH * 2];
	SHGetSpecialFolderPath(NULL, buf, CSIDL_APPDATA, FALSE);
	::PathAppend(buf, this->product_name_);
	::PathAppend(buf, TEXT("logs\\*.log"));

	TCHAR pids[3][10];
	_itot_s(main_process_id_, pids[0], 10);
	_itot_s(metting_process_id_, pids[1], 10);
	_itot_s(cpthost_process_id_, pids[2], 10);

	log_file_count_ = 0;

	WIN32_FIND_DATA wfd;
	HANDLE find_handle = ::FindFirstFile(buf, &wfd);
	if (INVALID_HANDLE_VALUE != find_handle)
	{
		for (BOOL ret = (INVALID_HANDLE_VALUE != find_handle); ret; ret = ::FindNextFile(find_handle, &wfd))
		{
			//if (_tcsstr(wfd.cFileName, TEXT(".log")))
			{
				for (int i = 0; i < ARRAYSIZE(pids); ++i)
				{
					if (_tcsstr(wfd.cFileName, pids[i]))
					{
						_tcscpy(log_file_names_[log_file_count_++], wfd.cFileName);
						break;
					}
				}
			}

		}

		::FindClose(find_handle);
	}

	return true;
}
