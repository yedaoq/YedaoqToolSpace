#pragma once

// CCtrlSimpleType �Ի���

class CCtrlSimpleType : public CDialog
{
	DECLARE_DYNCREATE(CCtrlSimpleType)

public:
	CCtrlSimpleType(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCtrlSimpleType();
// ��д


// �Ի�������
	enum { IDD = IDD_DLG_SIMPLETYPE, IDH = IDR_HTML_CTRLSIMPLETYPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
};
