// CtrlSimpleType.cpp : 实现文件
//

#include "stdafx.h"
#include "XSDEditor.h"
#include "CtrlSimpleType.h"
#include <atldef.h>


// CCtrlSimpleType 对话框

IMPLEMENT_DYNAMIC(CCtrlSimpleType, CDialog)

CCtrlSimpleType::CCtrlSimpleType(CWnd* pParent /*=NULL*/)
	: CDialog(CCtrlSimpleType::IDD, pParent)
{

}

CCtrlSimpleType::~CCtrlSimpleType()
{
}

void CCtrlSimpleType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TXTDESC, m_CtrlDesc);
	DDX_Control(pDX, IDC_LNKTYPES, m_CtrlTypes);
	DDX_Control(pDX, IDC_LNKBASE, m_CtrlBase);
	DDX_Control(pDX, IDC_CMBWHITE, m_CtrlWhite);
	DDX_Control(pDX, IDC_TXTRANGEU, m_CtrlRangeU);
	DDX_Control(pDX, IDC_TXTRANGEL, m_CtrlRangeL);
	DDX_Control(pDX, IDC_CHKOPENU, m_CtrlOpenU);
	DDX_Control(pDX, IDC_CHKOPENL, m_CtrlOpenL);
	DDX_Control(pDX, IDC_TXTDIGIT, m_CtrlDigit);
	DDX_Control(pDX, IDC_TXTFRACTION, m_CtrlFraction);
	DDX_Control(pDX, IDC_TXTLENU, m_CtrlLenU);
	DDX_Control(pDX, IDC_TXTLENL, m_CtrlLenL);
	DDX_Control(pDX, IDC_RDOLENFIX, m_CtrlLenFix);
	DDX_Control(pDX, IDC_RDONUM, m_CtrlRestrict);
	DDX_Control(pDX, IDC_RDOUNION, m_CtrlType);
}


BEGIN_MESSAGE_MAP(CCtrlSimpleType, CDialog)
	ON_BN_CLICKED(IDC_RDOUNION, &CCtrlSimpleType::OnBnClickedRdoType)
	ON_BN_CLICKED(IDC_RDOLIST, &CCtrlSimpleType::OnBnClickedRdoType)
	ON_BN_CLICKED(IDC_RDORESTRICT, &CCtrlSimpleType::OnBnClickedRdoType)
	ON_BN_CLICKED(IDC_RDONUM, &CCtrlSimpleType::OnBnClickedRdoType)
	ON_BN_CLICKED(IDC_RDOLEN, &CCtrlSimpleType::OnBnClickedRdoType)
	ON_BN_CLICKED(IDC_RDOENUM, &CCtrlSimpleType::OnBnClickedRdoType)
	ON_BN_CLICKED(IDC_RDOPATTERN, &CCtrlSimpleType::OnBnClickedRdoType)
END_MESSAGE_MAP()


// CCtrlSimpleType 消息处理程序

void CCtrlSimpleType::OnBnClickedRdoType()
{
	// TODO: 在此添加控件通知处理程序代码
	static INT Ctrls[] = { IDC_LNKTYPES, IDC_LNKBASE, 
		IDC_RDOENUM, IDC_RDOLEN, IDC_RDORANGE, IDC_RDOPATTERN,  
		IDC_TXTRANGEU, IDC_TXTRANGEL, IDC_CHKOPENL, IDC_CHKOPENU, 
		IDC_TXTDIGIT, IDC_TXTFRACTION,
		IDC_TXTLENL, IDC_TXTRANGEU, IDC_RDOLENFIX, IDC_RDOLENRANGE,
		IDC_LNKENUM	};

	HWND hwnd;
	for (int i = 0; i < count_of(Ctrls); ++i)
	{
		GetDlgItem(Ctrls[i], &hwnd);
		::EnableWindow(hwnd, false);
	}
}
