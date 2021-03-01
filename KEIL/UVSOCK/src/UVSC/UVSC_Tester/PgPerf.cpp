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
#include "PgPerf.h"
#include "UVSCThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CUVSCTesterDlg   *_dlg;
extern "C" void DoEvents (void);

/////////////////////////////////////////////////////////////////////////////
// CPerfDlg dialog


CPerfDlg::CPerfDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPerfDlg::IDD, pParent),
  rwMemAddr(-1), rwIncrementAddress(true)
{
	//{{AFX_DATA_INIT(CPerfDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPerfDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPerfDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPerfDlg, CDialog)
	//{{AFX_MSG_MAP(CPerfDlg)
  ON_BN_CLICKED(IDC_READTEST, OnReadTest)
	ON_BN_CLICKED(IDC_WRITETEST, OnWriteTest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRTADlg message handlers

void CPerfDlg::OnCancel()  {       // Catch ESC
}

void CPerfDlg::OnOK()  {       // Catch Enter
}



BOOL CPerfDlg::OnInitDialog()  {
  CDialog::OnInitDialog();



  return TRUE;
}

void CPerfDlg::OnReadTest() {
  ThreadMsg msg;
  AMEM      *pMa = &msg.amemData;
  char      szB[256];
  int n;
  unsigned int i;  
  DWORD startTime, endTime, duration;
  float avgDuration;

  SetLogging(false);

  if (EvalMemExpr()) {  
    startTime = GetTickCount();
    for (i=0; i<rwDataCount; i++) {
      // Set the message parameters
      if (rwIncrementAddress) {
        pMa->nAddr  = (UINT) rwMemAddr + (i * rwDataSize);  // Memory Address to read
      }
      else {
        pMa->nAddr  = (UINT) rwMemAddr;                     // Memory Address to read
      }
      pMa->nBytes = rwDataSize;                             // Data Size
      n  = sizeof (AMEM);
      n += rwDataSize;
      msg.cmd = UV_DBG_MEM_READ;
      msg.dataLen = n;
      msg.connHandle = _dlg->UVSCConnHandle;

      // Send the message
      g_UVSCOpNotify = UVSC_OP_NOTIFY_PENDING;      // Set op pending
      if (!UVSCThread_Post(&msg, 0)) {
        g_UVSCOpNotify = UVSC_OP_NOTIFY_FAILED;     // Set op failed (avoids deadlock on queue error)
      }

      sprintf(szB, "%d", i+1);
      CEdit *pE = (CEdit *) GetDlgItem (IDC_RWRESULT);
      pE->SetWindowText (szB);

      sprintf(szB, "0x%08X", pMa->nAddr);
      CEdit *pEAvg = (CEdit *) GetDlgItem (IDC_RWRESULTAVG);
      pEAvg->SetWindowText (szB);

      // Wait for a response
      while (g_UVSCOpNotify == UVSC_OP_NOTIFY_PENDING)  {
        DoEvents();
      }
      if (g_UVSCOpNotify != UVSC_OP_NOTIFY_SUCCESS)  {
        break;
      }
    } // for (i=0; i<rwDataCount; i++)
    endTime = GetTickCount();
    duration = endTime - startTime;

    avgDuration = (float)duration / rwDataCount;

    sprintf(szB, "Total %d ms", duration);
    CEdit *pE = (CEdit *) GetDlgItem (IDC_RWRESULT);
    pE->SetWindowText (szB);

    sprintf(szB, "Avg %0.2f ms", avgDuration);
    CEdit *pEAvg = (CEdit *) GetDlgItem (IDC_RWRESULTAVG);
    pEAvg->SetWindowText (szB);

  } // if (EvalMemExpr())



  SetLogging(true);
}

void CPerfDlg::OnWriteTest() {
  SetLogging(false);

  if (EvalMemExpr()) {
    
  }

  SetLogging(true);
}

bool CPerfDlg::EvalMemExpr() {
  ThreadMsg msg;
  VSET      *pVs = &msg.vsetData;
  char      szB[256];
  int       n, i;

  CEdit *pE = (CEdit *) GetDlgItem (IDC_STARTADDR);

  szB[0] = ' ';
  n = pE->GetWindowText (&szB[1], sizeof (szB) - 3);
  szB [n + 1] = 0;
  for ( i = 1 ; i < n ; ++i )  {
    if (szB[i] != ' ') break;
  }

  ++n;

  pVs->str.nLen  = n;
  strcpy (pVs->str.szStr, szB);

  if (pVs->str.nLen <= 1)  {
    MessageBeep (MB_OK);
    AfxMessageBox ("No Start Address entered!");
    GetDlgItem (IDC_STARTADDR)->SetFocus();
    return false;
  }
  pVs->val.vType = VTT_void;
  pVs->val.v.u64 = 0;

  n  = sizeof (VSET) - sizeof (pVs->str.szStr);
  n += pVs->str.nLen + 1;

  msg.cmd = UV_DBG_CALC_EXPRESSION;
  msg.dataLen = n;
  msg.connHandle = _dlg->UVSCConnHandle;

  g_UVSCOpNotify = UVSC_OP_NOTIFY_PENDING;      // Set op pending
  if (!UVSCThread_Post(&msg, 0)) {
    g_UVSCOpNotify = UVSC_OP_NOTIFY_FAILED;     // Set op failed (avoids deadlock on queue error)
  }

  while (g_UVSCOpNotify == UVSC_OP_NOTIFY_PENDING)  {
    DoEvents();
  }
  if (g_UVSCOpNotify != UVSC_OP_NOTIFY_SUCCESS)  {
    rwMemAddr = -1;
    MessageBeep (MB_OK);
    AfxMessageBox ("Calc Expression failed :(");
    GetDlgItem (IDC_STARTADDR)->SetFocus();
    return false;
  }

  rwMemAddr = g_CalcExprRes.v.i64;
  sprintf (szB, "0x%08X", (DWORD) rwMemAddr);
  SetDlgItemText (IDC_SHOWADDR, szB);

  CEdit *pEditDataSize = (CEdit *) GetDlgItem (IDC_DATASIZE);
  n = pEditDataSize->GetWindowText (&szB[0], sizeof (szB) - 3);
  if (!n) {
    AfxMessageBox ("No Data Size entered!");
    return false;
    GetDlgItem (IDC_DATASIZE)->SetFocus();
  }
  rwDataSize = atoi(szB);

  CEdit *pEditDataCount = (CEdit *) GetDlgItem (IDC_DATACOUNT);  
  n = pEditDataCount->GetWindowText (&szB[0], sizeof (szB) - 3);
  if (!n) {
    AfxMessageBox ("No Data Count entered!");
    GetDlgItem (IDC_DATACOUNT)->SetFocus();
    return false;
  }
  rwDataCount = atoi(szB);

  return true;
}


void CPerfDlg::SetLogging(bool bEnableLog) {
  ThreadMsg msg;

  msg.cmd = UV_NULL_CMD;
  msg.connHandle = _dlg->UVSCConnHandle;
  if (bEnableLog) {
    msg.ctrlMsg.cmd = CONTROLMSG_LOGENABLE;
  }
  else {
    msg.ctrlMsg.cmd = CONTROLMSG_LOGDISABLE;
  }

  UVSCThread_Post(&msg, 0);
}

