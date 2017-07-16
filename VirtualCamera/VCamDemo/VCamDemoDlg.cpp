// VCamDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VCamDemo.h"
#include "VCamDemoDlg.h"

//=============在这加入视频采集模块头文件和lib
#include "include/VideoCaptureModuleDll.h"
#pragma comment (lib, "lib/VideoCaptureModule")
#include "include/VCam.h"
#pragma comment (lib, "lib/VCam")
//=============在这加入视频采集模块头文件和lib


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVCamDemoDlg dialog

CVCamDemoDlg::CVCamDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVCamDemoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVCamDemoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVCamDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVCamDemoDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CVCamDemoDlg, CDialog)
	//{{AFX_MSG_MAP(CVCamDemoDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO_CAMERA_LIST, OnSelchangeComboCameraList)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVCamDemoDlg message handlers

BOOL CVCamDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	//==================在这初始化摄像头列表
	SetDlgItemInt(IDC_EDIT_WIDTH, 320);
	SetDlgItemInt(IDC_EDIT_HEIGHT, 240);
	VideoCaptureModule_Initialize();
	VideoCaptureModule_GetCameraCount(m_nCameraCount);
	m_pwstrCameraNames = new WCHAR[1000];
	VideoCaptureModule_GetCameraName(m_pwstrCameraNames);
	PhaseString(m_pwstrCameraNames, L';');//把得到的摄像头字符串分解到字符串数组中
	InitCameraList();
	m_pFrameData = new BYTE[MAX_WIDTH * MAX_HEIGHT * 3];
	//==================在这初始化摄像头列表
	VCam_Initialize();
	//==================初始化虚拟视频头输出
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVCamDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVCamDemoDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVCamDemoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
void CVCamDemoDlg::PhaseString(const WCHAR *pszStrIn, WCHAR cTok)
{
	const WCHAR *p = pszStrIn;
	const WCHAR *e = pszStrIn;
	//MessageBox(pszStrIn);
	int i = 0;
	while(*p++)
	{
		if(*p == cTok || *p == L'\0')
		{
			if (p-e>0)
			{
				m_pwstrCameraNameList[i] = new WCHAR[p-e+1];
				wmemset(m_pwstrCameraNameList[i] ,0,p-e+1);
				wcsncpy(m_pwstrCameraNameList[i] ,e,p-e);
				i++;
			}
			e = p+1;
		}
	}
}
// 初始化已选中摄像头列表
void CVCamDemoDlg::InitCameraList()
{
	
	int nUSBNumber = 0;
	((CComboBox*)GetDlgItem(IDC_COMBO_CAMERA_LIST))->ResetContent();//消除现有所有内容
	int nCameraNumber = 0;
	while(nCameraNumber < m_nCameraCount)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_CAMERA_LIST))->AddString(m_pwstrCameraNameList[nCameraNumber]);
		if (_tcsstr(m_pwstrCameraNameList[nCameraNumber],_T("USB"))||_tcsstr(m_pwstrCameraNameList[nCameraNumber],_T("usb")))//;strCameraName.Find(_T("USB"))||strCameraName.Find(_T("usb")))
		{
			nUSBNumber = nCameraNumber;
		}
		nCameraNumber++;
	};
	
	((CComboBox*)GetDlgItem(IDC_COMBO_CAMERA_LIST))->SetCurSel(nUSBNumber);	
}


void CVCamDemoDlg::OnSelchangeComboCameraList() 
{
	// TODO: Add your control notification handler code here
	if (m_GetFrameTimer)
	{
		KillTimer(m_GetFrameTimer);
		m_GetFrameTimer = 0;
	}
	
	int nSelectedCameraNumber=((CComboBox*)GetDlgItem(IDC_COMBO_CAMERA_LIST))->GetCurSel();//当前选中的行。
	int nWidth = GetDlgItemInt(IDC_EDIT_WIDTH);
	int nHeight = GetDlgItemInt(IDC_EDIT_HEIGHT);

	VideoCaptureModule_SetSelectedCamera(nSelectedCameraNumber);
	VideoCaptureModule_SetFrameSize(nWidth, nHeight);
	VideoCaptureModule_Open();
	VideoCaptureModule_Play();
	VideoCaptureModule_GetFrameSize(m_nWidth, m_nHeight);
	//设置显示图像格式BMP头
	//BITMAPINFOHEADER m_bih;
	memset( &m_bih, 0, sizeof( m_bih ) );
	m_bih.biSize = sizeof( m_bih );
	m_bih.biWidth = m_nWidth;
	m_bih.biHeight = m_nHeight;
	m_bih.biPlanes = 1;
	m_bih.biBitCount = 24;
	m_GetFrameTimer = SetTimer(GETFRAME_TIMER, 67, NULL);

}

void CVCamDemoDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == m_GetFrameTimer)
	{
		VideoCaptureModule_GetFrame(m_pFrameData, m_nWidth, m_nHeight);//取视频帧
		DisplayCapturedBits(m_pFrameData, &m_bih, IDC_VIDEO_WINDOW);
		VCam_OutPutFrame(m_pFrameData);//仅限320*240输出.
	}
	CDialog::OnTimer(nIDEvent);
}
BOOL CVCamDemoDlg::DisplayCapturedBits(BYTE *pBuffer, BITMAPINFOHEADER *pbih, UINT nIDC)//显示图像到控件上
{
	// If we haven't yet snapped a still, return
	if (!pBuffer)
		return FALSE;
	
	// put bits into the preview window with StretchDIBits
	//
	HWND hwndStill = NULL;
	GetDlgItem( nIDC, &hwndStill );
	
	RECT rc;
	::GetWindowRect( hwndStill, &rc );
	long lStillWidth = rc.right - rc.left;
	long lStillHeight = rc.bottom - rc.top;
	
	HDC hdcStill = ::GetDC( hwndStill );
	PAINTSTRUCT ps;
	::BeginPaint(hwndStill, &ps);
	
	::SetStretchBltMode(hdcStill, COLORONCOLOR);
	::StretchDIBits( 
		hdcStill, 0, 0, 
		pbih->biWidth, pbih->biHeight, 
		0, 0, pbih->biWidth, pbih->biHeight, 
		pBuffer, 
		(BITMAPINFO*) pbih, 
		DIB_RGB_COLORS, 
		SRCCOPY );
//	RECT rect;
	//::DrawText(hdcStill,L"test",4,&rect,1);

//	::TextOut(hdcStill,0,0,L"test",4);
	::EndPaint(hwndStill, &ps);
	::ReleaseDC( hwndStill, hdcStill );    
	
	return TRUE;
}