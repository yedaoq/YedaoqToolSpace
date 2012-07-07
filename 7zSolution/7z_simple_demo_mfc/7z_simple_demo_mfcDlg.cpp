// 7z_simple_demo_mfcDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "7z_simple_demo_mfc.h"
#include "7z_simple_demo_mfcDlg.h"
#include "../7z_simple/7z_simple.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CMy7z_simple_demo_mfcDlg �Ի���




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


// CMy7z_simple_demo_mfcDlg ��Ϣ�������

BOOL CMy7z_simple_demo_mfcDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_Archieve = TEXT("F:\\SqliteDEV.7z");
	m_OutDir = TEXT("D:\\");
	UpdateData(FALSE);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMy7z_simple_demo_mfcDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
