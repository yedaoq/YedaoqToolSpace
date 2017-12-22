#pragma once

enum EnumZoomProduct
{
	PRODUCT_ZOOM,
};

struct time_duration
{
	unsigned short hour;
	unsigned char  minute;
	unsigned char  seconds;
	unsigned short miniseconds;

	static time_duration FromMiniseconds(LONGLONG miniseconds);
	static time_duration FromSeconds(LONG seconds);
};

bool SetProduct(EnumZoomProduct product);

bool GetProductBinDir(LPTSTR buf, int buf_len);
bool GetProductLogDir(LPTSTR buf, int buf_len);
bool IsRunAsAdmin();

void SetToolErrorMsg(LPCTSTR msg);
LPCTSTR GetToolErrorMsg();

bool RemoveFile(LPCTSTR path);
bool LaunchUacApp(LPCTSTR path, LPCTSTR params, bool wait);

// tool record
bool IsToolRecordInstalled();
bool IsAppLaunchForInstallToolRecord(LPCTSTR cmd_line);
bool InstallToolRecord(bool adjust_uac);
bool UninstallToolRecord(bool adjust_uac);

// extract resource
typedef bool (*ExtractResourceCallback_t)(LPVOID buf, DWORD buf_len, LPVOID callback_param);
bool ExtractResource(HMODULE res_module, DWORD res_id, LPCTSTR res_type, ExtractResourceCallback_t callback, LPVOID callback_param);
bool ExtractResourceCallback_SaveToFile(LPVOID buf, DWORD buf_len, LPVOID callback_param);

bool CheckWindowsMenuItem(HMENU menu, DWORD item_id, bool check);
bool IsWindowsMenuItemChecked(HMENU menu, DWORD item_id, bool default);

__int64 GetTimeDeltaMiniseconds(const FILETIME& t1, const FILETIME& t2);


/*BOOL IsUserAdmin();*/