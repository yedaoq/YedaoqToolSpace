// 7z_simple_demo_mfcDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "7z_simple_demo_mfc.h"
#include "7z_simple_demo_mfcDlg.h"
#include "../7z_simple/7z_simple.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMy7z_simple_demo_mfcDlg 对话框




CMy7z_simple_demo_mfcDlg::CMy7z_simple_demo_mfcDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMy7z_simple_demo_mfcDlg::IDD, pParent)
	, m_Archieve(_T(""))
	, m_OutDir(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy7z_simple_demo_mfcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_Archieve);
	DDX_Text(pDX, IDC_EDIT2, m_OutDir);
}

BEGIN_MESSAGE_MAP(CMy7z_simple_demo_mfcDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CMy7z_simple_demo_mfcDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CMy7z_simple_demo_mfcDlg 消息处理程序

BOOL CMy7z_simple_demo_mfcDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_Archieve = TEXT("F:\\SqliteDEV.7z");
	m_OutDir = TEXT("D:\\");
	UpdateData(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMy7z_simple_demo_mfcDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMy7z_simple_demo_mfcDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMy7z_simple_demo_mfcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void UnpackResourceFile(LPCTSTR resource_str, CString m_OutDir ) 
{
	HRSRC hRes = FindResource(NULL, resource_str, _T("FILE"));
	//ASSERT(hRes != NULL);
	HGLOBAL hFileGlobal = ::LoadResource(NULL, hRes);
	//ASSERT(hFileGlobal != NULL);
	LPVOID lpData = LockResource(hFileGlobal);
	DWORD dwSize = SizeofResource(NULL, hRes);

	decompress_by_buffer(lpData, dwSize, m_OutDir);

	FreeResource(hFileGlobal);
}

void UnpackAppointedFile( CString archieve, CString outDir ) 
{
	decompress_by_path(archieve, outDir);
}

void CMy7z_simple_demo_mfcDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	if(m_Archieve == TEXT("1"))
	{
		UnpackResourceFile(MAKEINTRESOURCE(IDR_FILE1), m_OutDir);
	}
	else if(m_Archieve == TEXT("2"))
	{
		UnpackResourceFile(MAKEINTRESOURCE(IDR_FILE2), m_OutDir);
	}
	else if(m_Archieve == TEXT("3"))
	{
		UnpackResourceFile(MAKEINTRESOURCE(IDR_FILE3), m_OutDir);
	}
	else
	{
		UnpackAppointedFile(m_Archieve, m_OutDir);
	}
}
