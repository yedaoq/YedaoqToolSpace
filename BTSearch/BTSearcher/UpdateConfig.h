/********************************************************************
	Created :	2012-11-6   15:23
	FileName:	UpdateConfig.h
	Author:		Yedaoquan
	Purpose:	
*********************************************************************/

#ifndef _UpdateConfig_h__
#define _UpdateConfig_h__

#include "UpdateLib/UpdateConfigImpl.h"
#include "ProductInfo.h"

class UpdateConfig : public UpdateConfigImpl
{
public:
	UpdateConfig(void);
	virtual ~UpdateConfig(void);

	virtual bool Initialize(bool is_running);

	virtual LPCTSTR GetUpdateUrl() { return TEXT("http://u") PRODUCT_DOMAIN TEXT("u.jpg"); }
	virtual LPCTSTR GetReportUrl() { return TEXT("http://st") PRODUCT_DOMAIN TEXT("p.jpg"); }
	virtual LPCTSTR GetProductID() { return PRODUCT_ID; }
	virtual LPCTSTR GetDataFolderName() { return PRODUCT_APPDATA_FOLDER; } //APP_DATA下的子文件夹名称

	LPCTSTR GetTongJiUrl(){ return TEXT("http://tj") PRODUCT_DOMAIN TEXT("tj.gif"); }

	virtual LPCTSTR GetUninstBindUrl() { return TEXT("http://st") PRODUCT_DOMAIN TEXT("b.gif"); }
	virtual LPCTSTR GetUninstBindFileName() { return TEXT("friend.dat"); }

	virtual LPCTSTR GetConfigFileName() { return PRODUCT_CONFIG; }
	virtual LPCTSTR GetAutorunKey() { return keyname_autorun_; }
	virtual LPCTSTR GetAutorunParam() { return TEXT("hide"); }

	int				GetConfigInt(LPCTSTR key, int default_val);
	void			SetConfigInt(LPCTSTR key, int val);

	virtual bool IsAutoRun();

	virtual LPCTSTR GetAutorunCommond();

protected:
	CString keyname_autorun_;
	CString autorun_commond_;
};

#endif // _UpdateConfig_h__
