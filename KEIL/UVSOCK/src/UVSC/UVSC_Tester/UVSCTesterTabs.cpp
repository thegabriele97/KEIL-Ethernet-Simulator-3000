/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2004-2016 KEIL - An ARM Company. All rights reserved.        */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include "stdafx.h"
#include "UVSCTesterTabs.h"


// CUVSCTesterTabs

IMPLEMENT_DYNAMIC(CUVSCTesterTabs, CTabCtrl)

CUVSCTesterTabs::CUVSCTesterTabs() {
  int i = 0;

  // Assign the dialog names
  m_DialogName[i] = "Connection";
  m_DialogID[i] = IDD_CONNECTIONTAB;
  m_Dialog[i] = (CDialog *)&m_pgConnection;
  i++;

  m_DialogName[i] = "General";
  m_DialogID[i] = IDD_GENERALTAB;
  m_Dialog[i] = (CDialog *)&m_pgGeneral;
  i++;

  m_DialogName[i] = "Project";
  m_DialogID[i] = IDD_PROJECTTAB;
  m_Dialog[i] = (CDialog *)&m_pgProject;
  i++;

  m_DialogName[i] = "Debug Control";
  m_DialogID[i] = IDD_DEBUGTAB;
  m_Dialog[i] = (CDialog *)&m_pgDebug;
  i++;

  m_DialogName[i] = "Debug Information";
  m_DialogID[i] = IDD_DEBUGTAB2;
  m_Dialog[i] = (CDialog *)&m_pgDebug2;
  i++;

  m_DialogName[i] = "Real-Time Agent";
  m_DialogID[i] = IDD_UNKNOWNTAB;
  m_Dialog[i] = (CDialog *)&m_pgRTA; 
  i++;

  m_DialogName[i] = "Performance";
  m_DialogID[i] = IDD_PERFTAB;
  m_Dialog[i] = (CDialog *)&m_pgPerf; 
  
  m_nPageCount = NUM_TAB_PAGES;
}

CUVSCTesterTabs::~CUVSCTesterTabs() {
}

BEGIN_MESSAGE_MAP(CUVSCTesterTabs, CTabCtrl)
END_MESSAGE_MAP()

//Function to Create the dialog boxes during startup
void CUVSCTesterTabs::InitDialogs() {
  int i;

  // Create the dialog pages and add them as tabs
  for (i=0; i<m_nPageCount; i++) {
    m_Dialog[i]->Create(m_DialogID[i],GetParent());
    InsertItem(i, m_DialogName[i]);
  }
}

// Function to activate the tab dialog boxes
void CUVSCTesterTabs::ActivateTabDialogs() {
  int nSel = GetCurSel();

  if (nSel > -1) {
    if(m_Dialog[nSel]->m_hWnd) m_Dialog[nSel]->ShowWindow(SW_HIDE);

    CRect l_rectClient;
    CRect l_rectWnd;

    GetClientRect(l_rectClient);
    AdjustRect(FALSE,l_rectClient);
    GetWindowRect(l_rectWnd);
    GetParent()->ScreenToClient(l_rectWnd);
    l_rectClient.OffsetRect(l_rectWnd.left,l_rectWnd.top);

    for(int nCount=0; nCount < m_nPageCount; nCount++){
      m_Dialog[nCount]->SetWindowPos(&wndTop, l_rectClient.left, l_rectClient.top, l_rectClient.Width(), l_rectClient.Height(), SWP_HIDEWINDOW);
    }
    m_Dialog[nSel]->SetWindowPos(&wndTop, l_rectClient.left, l_rectClient.top, l_rectClient.Width(), l_rectClient.Height(), SWP_SHOWWINDOW);

    m_Dialog[nSel]->ShowWindow(SW_SHOW);
  }
}

CConnectionDlg *CUVSCTesterTabs::GetConnection() {
  return (&m_pgConnection);
}

CGeneralDlg *CUVSCTesterTabs::GetGeneral() {
  return (&m_pgGeneral);
}

CProjectDlg *CUVSCTesterTabs::GetProject() {
  return (&m_pgProject);
}

CDebugDlg *CUVSCTesterTabs::GetDebug() {
  return (&m_pgDebug);
}

CDebug2Dlg *CUVSCTesterTabs::GetDebug2() {
  return (&m_pgDebug2);
}

CRTADlg *CUVSCTesterTabs::GetRTA() {
  return (&m_pgRTA);
}





