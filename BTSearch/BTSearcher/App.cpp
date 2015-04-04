// TXPlayer.cpp : 定义应用程序的入口点。
//
#include "stdafx.h"
#include "App.h"

#include "base/file_util.h"
#include "base/path_service.h"

#include "include/DirectUI.h"
#include "include/DUIHostWindow.h"

#include "UpdateLib/AliveReport.h"
#include "UpdateLib/UpdateThread.h"
#include "UpdateLib/LogReport.h"
#include "UpdateConfig.h"

#include "cpl/SystemUtil.h"
#include "cpl/monitor_util.h"
#include "cpl\CommandLineParams.h"

#include <shellapi.h>
#include "UpdateLib\UninstBindUpdate.h"
#include "cpl/IniFile.h"
//#include "Jobot.h"
#include "HttpThread/HttpTaskThread.h"
#include "cpl/StringUtil.h"
#include <Strsafe.h>
#include <shlobj.h>

#include "WndMain.h"

bool InitPath_Release();
bool InitPath_Developer();

int RunAbiWord(HINSTANCE hInstance, int iCmdShow);
IDirectUI* CreateDirectUI(HINSTANCE ) { return NULL; }

#pragma comment(lib, "base.lib")
#pragma comment(lib, "base_static.lib")
#pragma comment(lib, "dynamic_annotations.lib")
#pragma comment(lib, "breakpad_common.lib")
#pragma comment(lib, "crash_generation_client.lib")

#pragma comment(lib, "cpl.lib")
#pragma comment(lib, "pugixml.lib")
#pragma comment(lib, "jsoncpp.lib")
#pragma comment(lib, "updatelib.lib")
#pragma comment(lib, "HttpThread.lib")
#pragma comment(lib, "SecureSQLite.lib")
#pragma comment(lib, "exception_handler.lib")
#pragma comment(lib, "7z_simple.lib")
#pragma comment(lib, "SecureSQLite.lib")
#pragma comment(lib, "exception_handler.lib")
#pragma comment(lib, "detours.lib")

//#define			_DEVELOPER

//IEMsgForwarder	iemsg_forwarder;
ATL::CComModule	atl_module;
App*			app_context = new App;
extern int				_debug;

IUpdateConfig* App::CreateUpdateConfig()
{
	return new UpdateConfig();
}

IDUISkinProvider* App::CreateSkinProvider()
{
	return 0;
}

bool App::OnBeforeRun()
{
	ui_thread_id = GetCurrentThreadId();

	wnd_main = new WndMain();
	wnd_main->Show();

	return true;
}

bool App::OnBeforeInit()
{
	if(app_startup_params.named_end() != app_startup_params.named(PROGRAMPARAM_SHELL_REPORT))
	{
		//OutputDebugString(TEXT("JobotReport!\n"));
		AliveReport(update_config, LogType::runnormal, 7000);
		return false;
	}

// 	if(app_startup_params.named_end() != app_startup_params.named(PROGRAMPARAM_START_JOBOT))
// 	{
// 		//OutputDebugString(TEXT("StartJobot!\n"));
// 		RunJobot(app_context->app_instance);
// 		return false;
// 	}

	CommandLineParams::NamedParamRef cmd = app_startup_params.named(TEXT("inst"));
	if(app_startup_params.named_end() != cmd)
	{
		bool slient = false;
		tstring cid = cmd->val();
		if(app_startup_params.named_end() != app_startup_params.named(TEXT("s")))
			slient = true;

		update_config->UpdateInfo(cid.c_str(), slient);

		AliveReport(update_config, LogType::install, 7000, true);
		return false;
	}

	if(app_startup_params.named_end() != app_startup_params.named(TEXT("unst")))
	{
		AliveReport(update_config, LogType::uninstall, 7000, true);
		return false;
	}

	if(app_startup_params.count_unnamed() > 1)
	{
		LPCTSTR file_path = app_startup_params[1]->c_str();
		if(!PathFileExists(file_path))
			return false;

		HWND hwnd = FindWindowEx(NULL, NULL, PRODUCT_WND_CLASS, NULL);
		if(hwnd != NULL)
		{
#if defined(UNICODE) || defined(_UNICODE)
			COPYDATASTRUCT cds = {COPYDATE_U_STR, (lstrlenW(file_path) + 1) * sizeof(WCHAR), (void*)file_path };
#else
			COPYDATASTRUCT cds = {COPYDATE_A_STR, lstrlenA(file_path) + 1, (void*)file_path };
#endif
			SendMessage(hwnd, WM_COPYDATA, NULL, reinterpret_cast<LPARAM>(&cds));
			return false;
		}
	}

	return true;
}

void App::OnAfterDestroy()
{
}

bool App::OnBeforeQuit()
{
	return true;
}

bool App::InitPath()
{
#ifdef _DEVELOPER
	return InitPath_Developer();
#else
	return InitPath_Release();
#endif
}

// void App::ShowInstallError()
// {
// 	MessageBox(NULL, PRODUCT_NAME TEXT(" 未正确安装，请重新安装此软件！"), TEXT("启动失败"), MB_OK);
// }

bool App::HasAutorunParam()
{
	return app_startup_params.named_end() != app_startup_params.named(update_config->GetAutorunParam());
}

bool InitPath_Release()
{
	FilePath path;
	PathService::Get(base::DIR_APP_DATA, &path);
	path = path.Append(PRODUCT_APPDATA_FOLDER);
	PathService::OverrideAndCreateIfNeeded(PATH_CONFIG_FILE, path.Append(TEXT("config.xml")), false);
	//PathService::OverrideAndCreateIfNeeded(PATH_DB, path.Append(PRODUCT_DB_FILENAME), false);
	PathService::OverrideAndCreateIfNeeded(PATH_DIRTEMPLETE, path.Append(TEXT("templete")), false);
	PathService::OverrideAndCreateIfNeeded(PATH_DIRCACHE, path.Append(TEXT("cache")), false);
	return true;
}

bool InitPath_Developer()
{
	FilePath path;
	PathService::Get(base::DIR_EXE, &path);
	path = path.DirName().DirName().DirName();

	PathService::OverrideAndCreateIfNeeded(PATH_CONFIG_FILE, path.Append(TEXT("config.xml")), false);
	//PathService::OverrideAndCreateIfNeeded(PATH_DB, path.Append(TEXT("Database")).Append(PRODUCT_DB_FILENAME), false);
	PathService::OverrideAndCreateIfNeeded(PATH_SKIN, path.Append(TEXT("skin")).Append(PRODUCT_ID), false);
	PathService::OverrideAndCreateIfNeeded(PATH_DIRTEMPLETE, path.Append(TEXT("templete")), false);
	PathService::OverrideAndCreateIfNeeded(PATH_DIRCACHE, path.Append(TEXT("cache")), false);
	return true;
}
