// 7z_simple_demo_mfc.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMy7z_simple_demo_mfcApp:
// �йش����ʵ�֣������ 7z_simple_demo_mfc.cpp
//

class CMy7z_simple_demo_mfcApp : public CWinApp
{
public:
	CMy7z_simple_demo_mfcApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMy7z_simple_demo_mfcApp theApp;