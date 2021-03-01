/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2004-2016 KEIL - An ARM Company. All rights reserved.        */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#pragma once

#include "resource.h"


/*
 * Dialog Pages
 */
#include "PgRTA.h" 
#include "PgProject.h"
#include "PgDebug.h"  
#include "PgDebug2.h" 
#include "PgGeneral.h" 
#include "PgConnection.h"  
#include "PgPerf.h"  

#define NUM_TAB_PAGES   7

// CUVSCTesterTabs

class CUVSCTesterTabs : public CTabCtrl
{
	DECLARE_DYNAMIC(CUVSCTesterTabs)

public:
	CUVSCTesterTabs();
	virtual ~CUVSCTesterTabs();

  //Function to Create the dialog boxes during startup
  void InitDialogs();

  //Function to activate the tab dialog boxes
  void ActivateTabDialogs();

  CConnectionDlg *GetConnection();
  CGeneralDlg *GetGeneral();
  CProjectDlg *GetProject();
  CDebugDlg *GetDebug();
  CDebug2Dlg *GetDebug2();
  CRTADlg *GetRTA();

private:
  int m_nPageCount;

  // Array to hold the names of the dialog tabs
  const char *m_DialogName[NUM_TAB_PAGES];

  // Array to hold the list of dialog boxes/tab pages for CTabCtrl
  int m_DialogID[NUM_TAB_PAGES];

  // CDialog Array Variable to hold the dialogs 
  CDialog *m_Dialog[NUM_TAB_PAGES];

  CConnectionDlg  m_pgConnection;         // Connection Info Page
  CGeneralDlg     m_pgGeneral;            // General Page
  CProjectDlg     m_pgProject;            // Project Page
  CDebugDlg       m_pgDebug;              // Debug Control Page
  CDebug2Dlg      m_pgDebug2;             // Debug Information Page
  CRTADlg         m_pgRTA;                // RTA Page
  CPerfDlg        m_pgPerf;               // Performance Page

protected:
	DECLARE_MESSAGE_MAP()
};


