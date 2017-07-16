// VCamDemo.h : main header file for the VCAMDEMO application
//

#if !defined(AFX_VCAMDEMO_H__E16AF5E8_3157_4922_8D02_6A9CC8FEC75C__INCLUDED_)
#define AFX_VCAMDEMO_H__E16AF5E8_3157_4922_8D02_6A9CC8FEC75C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CVCamDemoApp:
// See VCamDemo.cpp for the implementation of this class
//

class CVCamDemoApp : public CWinApp
{
public:
	CVCamDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVCamDemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CVCamDemoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VCAMDEMO_H__E16AF5E8_3157_4922_8D02_6A9CC8FEC75C__INCLUDED_)
