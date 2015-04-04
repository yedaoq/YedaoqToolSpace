#include "StdAfx.h"
#include "UpdateConfig.h"
#include "cpl\SystemUtil.h"

UpdateConfig::UpdateConfig(void)
{
}

UpdateConfig::~UpdateConfig(void)
{
}

bool UpdateConfig::Initialize( bool is_running )
{
	bool ret = __super::Initialize(is_running);
	keyname_autorun_ = CString(GetProductID()) + '_' + CString(GetUID()).Right(6);
	return ret;
}

int UpdateConfig::GetConfigInt( LPCTSTR key, int default_val )
{
	return ::GetPrivateProfileInt(GetProductSectionName(), key, default_val, GetConfigFilePath());
}

bool UpdateConfig::IsAutoRun()
{
	return cpl::IsAppAutoStart(GetAutorunKey(), NULL);
}

LPCTSTR UpdateConfig::GetAutorunCommond()
{
	if (!autorun_commond_.IsEmpty())
		return autorun_commond_;

	TCHAR path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	autorun_commond_.Format(TEXT("\"%s\" /%s"), path, this->GetAutorunParam());
	return autorun_commond_;
}

