/********************************************************************
	Created :	2013-10-15   15:00
	FileName:	App.h
	Author:		Yedaoquan
	Purpose:	
*********************************************************************/
#ifndef _App_h__
#define _App_h__

#include "UpdateLib/AppBase.h"
#include "UpdateConfig.h"

#define UPDATE_PROCESS_PARAM TEXT("/rcnnn")

enum
{
	PATH_DATA_DIR = 300,
	PATH_CONFIG_FILE,
	PATH_DB,				// Êý¾Ý¿â
	PATH_SKIN,				// Æ¤·ô¿âÄ¿Â¼
	PATH_DIRTEMPLETE,		// Ä£°åÄ¿Â¼
	PATH_DIRCACHE,			// »º´æÄ¿Â¼
};

class CommandLineParams;
class WndConfig;
class WndMain;

class App : public AppBase
{
public:
	//WndConfig*			wnd_config;
	WndMain*			wnd_main;

	DWORD				ui_thread_id;

	bool				flag_quit;

public:
	//void ShowInstallError();

	virtual IUpdateConfig* CreateUpdateConfig();
	virtual IDUISkinProvider* CreateSkinProvider();
	virtual bool InitPath();

	virtual bool OnBeforeInit();
	virtual bool OnBeforeRun();

	virtual bool OnBeforeQuit();
	virtual void OnAfterDestroy();

	virtual	DWORD	UpdateDelaySeconds() { return 0; }
	virtual	bool	NeedDirectUI() { return false; }

	bool HasAutorunParam();

};

extern App*		app_context;

#endif // _App_h__