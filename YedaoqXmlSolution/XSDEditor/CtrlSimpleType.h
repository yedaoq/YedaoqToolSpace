#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "XmlSchema.h"

// CCtrlSimpleType �Ի���

class CCtrlSimpleType : public CDialog
{
	DECLARE_DYNAMIC(CCtrlSimpleType)

public:
	CCtrlSimpleType(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCtrlSimpleType();

public:
	nsYedaoqXmlSchema::XmlSchemaSimpleType		XmlType;
	nsYedaoqXmlSchema::SimpleTypeFacetEnumData	DataEnum;
	nsYedaoqXmlSchema::SimpleTypeFacetLengthData	DataLength;
	nsYedaoqXmlSchema::SimpleTypeFacetPatternData	DataPattern;
	nsYedaoqXmlSchema::SimpleTypeFacetRangeData	DataRange;
	nsYedaoqXmlSchema::SimpleTypeUnionData		DataUnion;
	nsYedaoqXmlSchema::SimpleTypeListData			DataList;

// �Ի�������
	enum { IDD = IDD_DLG_SIMPLETYPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CRichEditCtrl m_CtrlDesc;
	CLinkCtrl m_CtrlTypes;
	CLinkCtrl m_CtrlBase;
	CComboBox m_CtrlWhite;
	CEdit m_CtrlRangeU;
	CEdit m_CtrlRangeL;
	CButton m_CtrlOpenU;
	CButton m_CtrlOpenL;
	CEdit m_CtrlDigit;
	CEdit m_CtrlFraction;
	CEdit m_CtrlLenU;
	CEdit m_CtrlLenL;
	CButton m_CtrlLenFix;
	CButton m_CtrlRestrict;
	CButton m_CtrlType;
	afx_msg void OnBnClickedRdoType();
};
