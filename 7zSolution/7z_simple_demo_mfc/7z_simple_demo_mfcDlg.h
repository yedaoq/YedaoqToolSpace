// 7z_simple_demo_mfcDlg.h : ͷ�ļ�
//

#pragma once


// CMy7z_simple_demo_mfcDlg �Ի���
class CMy7z_simple_demo_mfcDlg : public CDialog
{
// ����
public:
	CMy7z_simple_demo_mfcDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MY7Z_SIMPLE_DEMO_MFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_Archieve;
	CString m_OutDir;
	afx_msg void OnBnClickedOk();
};
