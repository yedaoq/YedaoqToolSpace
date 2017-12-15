#pragma once

bool GetZoomDir(LPTSTR buf, int buf_len);
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


/*BOOL IsUserAdmin();*/