#pragma once

// CCtrlSimpleType 对话框

class CCtrlSimpleType : public CDialog
{
	DECLARE_DYNCREATE(CCtrlSimpleType)

public:
	CCtrlSimpleType(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCtrlSimpleType();
// 重写


// 对话框数据
	enum { IDD = IDD_DLG_SIMPLETYPE, IDH = IDR_HTML_CTRLSIMPLETYPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
};
