// CtrlSimpleType.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ConfigDesigner.h"
#include "CtrlSimpleType.h"


// CCtrlSimpleType �Ի���

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
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

BEGIN_MESSAGE_MAP(CCtrlSimpleType, CDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CCtrlSimpleType)
END_DHTML_EVENT_MAP()



// CCtrlSimpleType ��Ϣ�������
