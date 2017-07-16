// VCamDemoDlg.h : header file
//

#if !defined(AFX_VCAMDEMODLG_H__54E855AE_96BF_4FFD_991D_E9C1CEFD1303__INCLUDED_)
#define AFX_VCAMDEMODLG_H__54E855AE_96BF_4FFD_991D_E9C1CEFD1303__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define GETFRAME_TIMER 100
#define MAX_WIDTH 640
#define MAX_HEIGHT 480
/////////////////////////////////////////////////////////////////////////////
// CVCamDemoDlg dialog

class CVCamDemoDlg : public CDialog
{
// Construction
public:
	CVCamDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CVCamDemoDlg)
	enum { IDD = IDD_VCAMDEMO_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVCamDemoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CVCamDemoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeComboCameraList();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
public:
	
	int			m_nCameraCount;//摄像头个数
	WCHAR*		m_pwstrCameraNames;//摄像头名字
	WCHAR*		m_pwstrCameraNameList[10];//摄像头名字列表
	
	int			m_nWidth;
	int			m_nHeight;
	UINT_PTR	m_GetFrameTimer;   // 定时器ID 
	BYTE*		m_pFrameData;
	BITMAPINFOHEADER m_bih;
	void		PhaseString(const WCHAR *pszStrIn, WCHAR cTok);
	void		InitCameraList();
	BOOL		DisplayCapturedBits(BYTE *pBuffer, BITMAPINFOHEADER *pbih, UINT nIDC);//显示图像到控件上
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VCAMDEMODLG_H__54E855AE_96BF_4FFD_991D_E9C1CEFD1303__INCLUDED_)
