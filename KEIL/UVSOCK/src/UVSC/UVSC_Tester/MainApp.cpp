/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2004-2016 KEIL - An ARM Company. All rights reserved.        */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include "stdafx.h"
#include "DlgApp.h"
#include "MainDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUVSCTesterApp

BEGIN_MESSAGE_MAP(CUVSCTesterApp, CWinApp)
	//{{AFX_MSG_MAP(CUVSCTesterApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CUVSCTesterApp construction

CUVSCTesterApp::CUVSCTesterApp() {

}



/////////////////////////////////////////////////////////////////////////////
// The one and only CUVSCTesterApp object

CUVSCTesterApp theApp;
HANDLE           hTm;       // Timeout-Handle




/////////////////////////////////////////////////////////////////////////////
// CUVSCTesterApp initialization

extern void RegisterEvents (void);

BOOL CUVSCTesterApp::InitInstance()  {
  int    nResponse;

	if (!AfxSocketInit())  {        // Init Sockets
		AfxMessageBox ("Initialization of Windows-Sockets failed.");
		return FALSE;
	}


  hTm = CreateEvent (NULL, FALSE, FALSE, NULL);  // create Command-Timout-Handle

  SetRegistryKey (_T("Keil"));

	CUVSCTesterDlg dlg;
	m_pMainWnd = &dlg;
	nResponse = dlg.DoModal();
 
	if (nResponse == IDOK)  {
	}
	else if (nResponse == IDCANCEL)  {
	}

  if (hTm != NULL)  {             // cleanup Timeout Handle.
    CloseHandle (hTm);
    hTm = NULL;
  }

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
