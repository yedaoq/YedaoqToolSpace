// CtrlSimpleType.cpp : 实现文件
//

#include "stdafx.h"
#include "ConfigDesigner.h"
#include "CtrlSimpleType.h"


// CCtrlSimpleType 对话框

IMPLEMENT_DYNCREATE(CCtrlSimpleType, CDialog)

CCtrlSimpleType::CCtrlSimpleType(CWnd* pParent /*=NULL*/)
	: CDialog(CCtrlSimpleType::IDD, CCtrlSimpleType::IDH, pParent)
{

}

CCtrlSimpleType::~CCtrlSimpleType()
{
}

void CCtrlSimpleType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CCtrlSimpleType::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

BEGIN_MESSAGE_MAP(CCtrlSimpleType, CDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CCtrlSimpleType)
END_DHTML_EVENT_MAP()



// CCtrlSimpleType 消息处理程序
