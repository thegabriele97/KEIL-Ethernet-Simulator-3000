// PwOptions.h : main header file for the PWOPTIONS application
//

#if !defined(AFX_PWOPTIONS_H__C77CFEAB_6C03_452C_AE0E_9803DB6AFE7C__INCLUDED_)
#define AFX_PWOPTIONS_H__C77CFEAB_6C03_452C_AE0E_9803DB6AFE7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


extern HANDLE    hTm;       // Timeout-Handle

/////////////////////////////////////////////////////////////////////////////
// CPwOptionsApp:
// See PwOptions.cpp for the implementation of this class
//

class CPwOptionsApp : public CWinApp
{
public:
	CPwOptionsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPwOptionsApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPwOptionsApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
extern CPwOptionsApp theApp;

extern void ClearClients (void);    // MainDlg.cpp


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PWOPTIONS_H__C77CFEAB_6C03_452C_AE0E_9803DB6AFE7C__INCLUDED_)
