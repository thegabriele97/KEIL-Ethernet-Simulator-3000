/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2004-2016 KEIL - An ARM Company. All rights reserved.        */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include "stdafx.h"
#include "Resource.h"
#include "PgRTA.h"
#include "MainDlg.h"
#include "Traceevent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRTADlg dialog


CRTADlg::CRTADlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRTADlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRTADlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRTADlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRTADlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
  DDX_Control(pDX, IDC_RTALISTOUT, m_RTALogOut);
}


BEGIN_MESSAGE_MAP(CRTADlg, CDialog)
	//{{AFX_MSG_MAP(CRTADlg)
	//}}AFX_MSG_MAP
 ON_BN_CLICKED(IDC_RTACLEARLOG, OnClearLog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRTADlg message handlers

void CRTADlg::OnCancel()  {       // Catch ESC
}

void CRTADlg::OnOK()  {       // Catch Enter
}



BOOL CRTADlg::OnInitDialog()  {
  CDialog::OnInitDialog();

  // Set up the log box
  m_RTALogOut.SetFont(80, "Lucida Console");
  m_RTALogOut.AddString(new TraceMessage(TraceEvent::None, "REAL-TIME AGENT LOG")); 
  m_RTALogOut.setLimit(500);
  m_RTALogOut.enableLimit(TRUE);
  //m_RTALogOut.setToDisk(TRUE, "default_log.txt");

  return TRUE;
}



void CRTADlg::OnClearLog (void) {
  m_RTALogOut.ResetContent();
}

void CRTADlg::OutLine (RTA_OUT_TYPE type, char *fmt, ...)  {
  char  szB [1028];
  va_list    argp;
  int n;

  va_start (argp, fmt);
  if (argp == NULL)  {         //  || argp[0] == 0)  {
    n = min (strlen (fmt), sizeof (szB) - 4);
    memcpy (szB, fmt, n);
  }
  else  {
    n = _vsnprintf (szB, sizeof (szB) - 4, fmt, argp);
  }
  szB [n] = 0;

  switch (type) {
    case RTA_OUT_TYPE_GEN:
      m_RTALogOut.AddString(new TraceMessage(TraceEvent::None, szB)); 
      break;
#if 0 // TBD
    case OUT_TYPE_RSP:
      m_RTALogOut.AddString(new TraceComment(TraceEvent::None, szB)); 
    break;
    case OUT_TYPE_ASYNC:
      m_RTALogOut.AddString(new TraceWarning(TraceEvent::None, szB)); 
    break;
    case OUT_TYPE_ERROR:
      m_RTALogOut.AddString(new TraceError(TraceEvent::None, szB)); 
    break;
    case OUT_TYPE_DATA:
      m_RTALogOut.AddString(new TraceData(TraceEvent::None, szB)); 
    break;
    case OUT_TYPE_CMD:
      m_RTALogOut.AddString(new TraceTrace(TraceEvent::None, szB));     
    break;
#endif
    default:
      m_RTALogOut.AddString(new TraceError(TraceEvent::None, szB)); 
  }
}

