/*--------------------------------------------------------------------------
MainApp.cpp

Application Module for the Tester Application.
Copyright (c) 2003 Keil Elektronik GmbH and Keil Software, Inc.
All rights reserved.
--------------------------------------------------------------------------*/


// PwOptions.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DlgApp.h"
#include "MainDlg.h"

//#include "PgIdenty.h"   // dtc_Identify Page.



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPwOptionsApp

BEGIN_MESSAGE_MAP(CPwOptionsApp, CWinApp)
	//{{AFX_MSG_MAP(CPwOptionsApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CPwOptionsApp construction

CPwOptionsApp::CPwOptionsApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}



/////////////////////////////////////////////////////////////////////////////
// The one and only CPwOptionsApp object

CPwOptionsApp theApp;
HANDLE           hTm;       // Timeout-Handle




/////////////////////////////////////////////////////////////////////////////
// CPwOptionsApp initialization

extern void RegisterEvents (void);

BOOL CPwOptionsApp::InitInstance()  {
  int    nResponse;

	if (!AfxSocketInit())  {        // Init Sockets
		AfxMessageBox ("Initialization of Windows-Sockets failed.");
		return FALSE;
	}


  hTm = CreateEvent (NULL, FALSE, FALSE, NULL);  // create Command-Timout-Handle

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	#ifdef _AFXDLL
		//Enable3dControls();			// Call this when using MFC in a shared DLL
	#else
		//Enable3dControlsStatic();	// Call this when linking to MFC statically
	#endif


//RegisterEvents();               // see MainDlg.cpp

	CPwOptionsDlg dlg;
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

#if 0
//---A Server would broadcast an GoodBye Event:
    dtc_Identify::Goodbye gb (dtc_EventManager::GetApplID());
    gb.Send();
//---
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
