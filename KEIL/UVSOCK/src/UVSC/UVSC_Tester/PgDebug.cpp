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
#include "PgDebug.h"
#include "UVSCThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CUVSCTesterDlg   *_dlg;

extern "C" void DoEvents (void);

/////////////////////////////////////////////////////////////////////////////
// CDebugDlg dialog


CDebugDlg::CDebugDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDebugDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDebugDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDebugDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDebugDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
  DDX_Text(pDX, IDC_CMDOUTPUTTEXT, m_StrCmdOutput);
  DDX_Text(pDX, IDC_STATIC_CMDLINETEXT, m_StrCmdLine);
}


BEGIN_MESSAGE_MAP(CDebugDlg, CDialog)
	//{{AFX_MSG_MAP(CDebugDlg)
	ON_BN_CLICKED(IDC_STARTDEBUG, OnStartdebug)     // 1005
	ON_BN_CLICKED(IDC_STOPTDEBUG, OnStoptdebug)     // 1006
	ON_BN_CLICKED(IDC_STARTEXEC, OnStartexec)       // 1007
	ON_BN_CLICKED(IDC_STOPEXEC, OnStopexec)         // 1008
	ON_BN_CLICKED(IDC_STEPINSTR, OnStepinstr)       // 1012
	ON_BN_CLICKED(IDC_STEPINTO, OnStepinto)         // 1010
	ON_BN_CLICKED(IDC_STEPOUT, OnStepout)           // 1011
	ON_BN_CLICKED(IDC_STEPOVER, OnStepover)         // 1009
	ON_BN_CLICKED(IDC_SETINTERVAL, OnSetinterval)   // 1015
	ON_BN_CLICKED(IDC_RESET, OnReset)               // 1019
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_BRKCREATEBUTTON, OnBnClickedBrkcreate)  // 1016
  ON_LBN_SELCHANGE(IDC_BPLIST, OnLbnSelchangeBplist)        // 1080
  ON_BN_CLICKED(IDC_BPKILLBUTTON, OnBnClickedBpkill)        // 1022
  ON_BN_CLICKED(IDC_BPENABLEBUTTON, OnBnClickedBpenable)    // 1023
  ON_BN_CLICKED(IDC_BPDISABLEBUTTON, OnBnClickedBpdisable)  // 1021
  ON_BN_CLICKED(IDC_BRKENUM, OnBnClickedBpEnum)             // 1024
  ON_CBN_SELCHANGE(IDC_BRKTYPE, OnCbnSelchangeBrktype)
  ON_BN_CLICKED(IDC_EXECCMD, OnExecCmd)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDebugDlg message handlers

void CDebugDlg::OnCancel()  {   // Catch ESC
}

void CDebugDlg::OnOK()  {       // Catch Enter
}

BOOL CDebugDlg::OnInitDialog()  {
  CDialog::OnInitDialog();

  CComboBox *pC = (CComboBox *) GetDlgItem (IDC_BRKTYPE);
  pC->AddString ("Execution");
  pC->AddString ("Read");
  pC->AddString ("Write");
  pC->AddString ("ReadWrite");
  pC->SetCurSel (0);                    // default Type: Execution-Breakpoint

  CEdit *pE = (CEdit *) GetDlgItem (IDC_BRKEXPR);
  pE->LimitText (128);

  pE = (CEdit *) GetDlgItem (IDC_BRKCMD);
  pE->LimitText (128);

  pE = (CEdit *) GetDlgItem (IDC_BRKCOUNT);
  pE->LimitText (12);

  pE = (CEdit *) GetDlgItem (IDC_ACCSIZE);
  pE->LimitText (12);

  pE = (CEdit *) GetDlgItem (IDC_EDCMD);
  pE->LimitText (128);

  InitBpListbox(0);

  return TRUE;
}


void CDebugDlg::OnStartdebug()  {
  ThreadMsg msg;

  msg.cmd = UV_DBG_ENTER;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}


void CDebugDlg::OnStoptdebug()  {
  ThreadMsg msg;

  msg.cmd = UV_DBG_EXIT;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}


void CDebugDlg::OnStartexec()  {
  ThreadMsg msg;

  msg.cmd = UV_DBG_START_EXECUTION;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}

void CDebugDlg::OnStopexec()  {
  ThreadMsg msg;

  msg.cmd = UV_DBG_STOP_EXECUTION;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}

void CDebugDlg::OnSetinterval()  {
  float f = 1.0;  // 0.5;
  //_dlg->CreateCommandSend (UV_DBG_SET_CALLBACK, sizeof (f), &f);
  // TODO
}

void CDebugDlg::OnReset()  {
  ThreadMsg msg;

  msg.cmd = UV_DBG_RESET;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}

void CDebugDlg::OnStepinstr()  {      // StepInstruction Event
  ThreadMsg msg;

  msg.cmd = UV_DBG_STEP_INSTRUCTION;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}

void CDebugDlg::OnStepinto()  {       // StepInto Event
  ThreadMsg msg;

  msg.cmd = UV_DBG_STEP_INTO;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}

void CDebugDlg::OnStepout()  {        // StepOut Event
  ThreadMsg msg;

  msg.cmd = UV_DBG_STEP_OUT;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}

void CDebugDlg::OnStepover()  {       // StepOver Event
  ThreadMsg msg;

  msg.cmd = UV_DBG_STEP_HLL;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}


void CDebugDlg::OnExecCmd() {
  ThreadMsg msg;
  EXECCMD  *pCmd = &msg.execCmdData;
  int       n;

  CEdit *pE = (CEdit *) GetDlgItem (IDC_EDCMD);

  pCmd->bEcho = 0;              // Don't echo the command when using the tester
  n = pE->GetWindowText (pCmd->sCmd.szStr, sizeof (SSTR) - sizeof(int) - 1);
  n++;
  pCmd->sCmd.szStr[n] = 0;
  pCmd->sCmd.nLen = n;

  msg.cmd = UV_DBG_EXEC_CMD;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);

  // Automatically get the command output
  GetCmdOutput();
}

void CDebugDlg::GetCmdOutput()  {
  ThreadMsg msg;

  msg.cmd = UV_NULL_CMD;
  msg.ctrlMsg.cmd = CONTROLMSG_CMDOUTPUT;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}

void CDebugDlg::SetCmdOutputText(const char *txt) {
  m_StrCmdOutput.Format("%s", txt);
  m_StrCmdOutput.Replace("\x0A", "\x0D\x0A");
  UpdateData(FALSE);
}

void CDebugDlg::SetCmdLineText(const char *txt) {
  m_StrCmdLine.Format("%s", txt);
  UpdateData(FALSE);
}

//
// Breakpoints
//
static int CheckInput (char *pB, int nLen)  {
  int     n, l;

  n = l = 0;
  while (n < nLen && pB[n] == ' ')  {
    ++n;
  }
  while (n < nLen && pB[n])  {
    ++l;
    ++n;
  }
  return (l);
}


/*
 * Define a Breakpoint
 */
void CDebugDlg::OnBnClickedBrkcreate()  {
  int     nCount, nAccSz, nR, n;
  BKTYPE   nType;
  ThreadMsg msg;
  BKPARM    *pBrk = &msg.bkparmData;
  char     szExpr[258], szCmd[256], szCount[64], szAccSz[64];
  CListBox  *pL;
  int        nS;


  nAccSz = 0;

//---Breapoint-Expression:
  n = GetDlgItemText (IDC_BRKEXPR, &szExpr[0], sizeof (szExpr) - 2);
  szExpr [n] = 0;
  if (CheckInput (szExpr, n) == 0)  {      // empty input
    MessageBeep (MB_OK);
    MessageBox ("Breakpoint expression required!");
    GetDlgItem (IDC_BRKEXPR)->SetFocus();
    return;
  }

//---Breapoint-Type:
  nType = (BKTYPE) ( ((CComboBox *) GetDlgItem (IDC_BRKTYPE))->GetCurSel() + 1 );
  switch (nType)  {
    case BRKTYPE_EXEC:                     // Exec-Break
      break;
    case BRKTYPE_READ:                     // Read-Access-Break
    case BRKTYPE_WRITE:                    // Write-Access-Break
    case BRKTYPE_READWRITE:                // Read/Write-Access-Break
//---requires Breapoint-Access-Size:
      n = GetDlgItemText (IDC_ACCSIZE, &szAccSz[0], sizeof (szAccSz) - 2);
      szAccSz [n] = 0;
      if (CheckInput (szExpr, n) == 0)  {  // empty input
        MessageBeep (MB_OK);
        MessageBox ("Access size required!");
        GetDlgItem (IDC_ACCSIZE)->SetFocus();
        return;
      }
      nR = sscanf (szAccSz, "%d", &nAccSz); // AccessSize
      if (nR == 0 || nR == EOF)  {
        MessageBeep (MB_OK);
        MessageBox ("Invalid Access size!");
        GetDlgItem (IDC_ACCSIZE)->SetFocus();
        return;
      }
      break;
  }

//---Breapoint-Count:
  n = GetDlgItemText (IDC_BRKCOUNT, &szCount[0], sizeof (szCount) - 2);
  szCount [n] = 0;
  if (CheckInput (szCount, n) == 0) nCount = 1;  // default count := 1
  else  {
    nR = sscanf (szCount, "%d", &nCount);        // count given
    if (nR == 0 || nR == EOF)  {
      MessageBeep (MB_OK);
      MessageBox ("Invalid Count!");
      GetDlgItem (IDC_BRKCOUNT)->SetFocus();
      return;
    }
  }

//---Breapoint-Command:
  n = GetDlgItemText (IDC_BRKCMD, &szCmd[0], sizeof (szCmd) - 2);
  szCmd [n] = 0;
  if (CheckInput (szCmd, n) == 0)  {      // command not used.
    szCmd[0] = 0;
  }

//---setup breakpoint info to send:
  memset (pBrk, 0, sizeof (BKPARM));      // clear buffer

  pBrk->type    = nType;                  // type of break := EXEC
  pBrk->count   = nCount;                 // number of occurrances before hit
  pBrk->nExpLen = strlen (szExpr) + 1;    // +1 because of zero terminator
  pBrk->nCmdLen = strlen (szCmd)  + 1;    // +1 because of zero terminator
  memcpy (pBrk->szBuffer, szExpr, pBrk->nExpLen);                // expr. + terminator
  memcpy (&pBrk->szBuffer[pBrk->nExpLen], szCmd, pBrk->nCmdLen); // expr. + terminator

  switch (nType)  {
    case BRKTYPE_EXEC:
      pBrk->accSize = 0;                  // irrelevant for EXEC break
      break;

    default:                              // type of break := READ/WRITE/READWRITE
      pBrk->accSize = nAccSz;             // access range
      break;
  }

  n = offsetof (BKPARM, szBuffer) + pBrk->nExpLen + pBrk->nCmdLen;

  msg.cmd = UV_DBG_CREATE_BP;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);

  //Sleep (100);
 
  pL = (CListBox *) GetDlgItem (IDC_BPLIST);
  nS = pL->GetCurSel();

  InitBpListbox(nS);
}




const char szBpt[6][12]  =  {             // Breakpoint-Types
  "<unknown>",
  "Exec",                                 // Execution-break
  "Read",                                 // Read-Access-break
  "Write",                                // Write-Access-break
  "ReadWrite",                            // ReadWrite-Access-break
  "Complex",                              // re-evalute expr. after each step
};

static int GenLine (int i, BKRSP *pB, char *pBuf)  {
  int       n;

  n  = sprintf (pBuf, "%03d: %s, %sabled, A=0x%08X, C=%d",
                    i, szBpt[pB->type], pB->enabled ? "en" : "dis", (DWORD) pB->nAddress, pB->count);
  n += sprintf (&pBuf[n], ", t=%08X", pB->nTickMark);
  if (pB->nExpLen > 1)  {
    n += sprintf (&pBuf[n], ", Expr: %s", pB->szBuffer);
  }
  return (n);
}


void CDebugDlg::InitBpListbox (int idx)  {
  CListBox  *pL;
  int         i, n, z, j;
  BKRSP     *pB;
  char      szB [512];
  ThreadMsg msg;

  m_bpc.Clear();                   // clear bp-list

  // Get list of breakpoints
  msg.cmd = UV_DBG_ENUMERATE_BP;
  msg.connHandle = _dlg->UVSCConnHandle;

  g_UVSCOpNotify = UVSC_OP_NOTIFY_PENDING;      // Set op pending
  if (!UVSCThread_Post(&msg, 0)) {
    g_UVSCOpNotify = UVSC_OP_NOTIFY_FAILED;     // Set op failed (avoids deadlock on queue error)
  }

  while (g_UVSCOpNotify == UVSC_OP_NOTIFY_PENDING)  {
    DoEvents();
  }

  pL = (CListBox *) GetDlgItem (IDC_BPLIST);
  pL->ResetContent();
  j  = 0;
  if (m_bpc.IsComplete())  {
    for ( i = 0 ; i < m_bpc.GetCount() ; ++i )  {
      pB = m_bpc.GetBp (i);
      n  = GenLine (i, pB, &szB[0]);
      z = pL->AddString (&szB[0]);
      if (z != LB_ERR)  {
        ++j;
      }
    }
  }

  // Calculate which item should be selected in the listbox
  if (j > idx) j = idx+1;
  pL->SetCurSel (j > 0 ? j-1 : -1);

  
  if (j > 0) {
    OnLbnSelchangeBplist();
  }
  else  {
    GetDlgItem (IDC_BPENABLEBUTTON)->EnableWindow (0);
    GetDlgItem (IDC_BPDISABLEBUTTON)->EnableWindow (0);
    GetDlgItem (IDC_BPKILLBUTTON)->EnableWindow (0);
  }
}

void CDebugDlg::OnLbnSelchangeBplist()  {
  CListBox  *pL;
  int        nS;
  BKRSP     *pB;

  pL = (CListBox *) GetDlgItem (IDC_BPLIST);
  nS = pL->GetCurSel();
  if (nS == LB_ERR)  {
    GetDlgItem (IDC_BPENABLEBUTTON)->EnableWindow (0);
    GetDlgItem (IDC_BPDISABLEBUTTON)->EnableWindow (0);
    GetDlgItem (IDC_BPKILLBUTTON)->EnableWindow (0);
    MessageBeep (MB_OK);
  }
  else  {
    pB = m_bpc.GetBp (nS);
    GetDlgItem (IDC_BPENABLEBUTTON)->EnableWindow  ( pB->enabled ? 0 : 1);
    GetDlgItem (IDC_BPDISABLEBUTTON)->EnableWindow ( pB->enabled ? 1 : 0);
    GetDlgItem (IDC_BPKILLBUTTON)->EnableWindow (1);
  }
}

void CDebugDlg::OnBnClickedBpkill()  {
  CListBox  *pL;
  int        nS;
  BKRSP     *pB;
  ThreadMsg msg;
  BKCHG     *pCbp = &msg.bkchgData;

  pL = (CListBox *) GetDlgItem (IDC_BPLIST);
  nS = pL->GetCurSel();
  if (nS == LB_ERR)  {
    MessageBeep (MB_OK);
  }
  else  {
    pB = m_bpc.GetBp (nS);

    memset (pCbp, 0, sizeof (BKCHG));
    pCbp->type      = CHG_KILLBP;
    pCbp->nTickMark = pB->nTickMark;

    msg.cmd = UV_DBG_CHANGE_BP;
    msg.connHandle = _dlg->UVSCConnHandle;
    UVSCThread_Post(&msg, 0);
    
    InitBpListbox(nS);
  }
}

void CDebugDlg::OnBnClickedBpenable()  {
  CListBox  *pL;
  int        nS;
  BKRSP     *pB;
  ThreadMsg msg;
  BKCHG     *pCbp = &msg.bkchgData;

  pL = (CListBox *) GetDlgItem (IDC_BPLIST);
  nS = pL->GetCurSel();
  if (nS == LB_ERR)  {
    MessageBeep (MB_OK);
  }
  else  {
    pB = m_bpc.GetBp (nS);

    memset (pCbp, 0, sizeof (BKCHG));
    pCbp->type      = CHG_ENABLEBP;
    pCbp->nTickMark = pB->nTickMark;

    msg.cmd = UV_DBG_CHANGE_BP;
    msg.connHandle = _dlg->UVSCConnHandle;
    UVSCThread_Post(&msg, 0);
    
    InitBpListbox(nS);
  }
}

void CDebugDlg::OnBnClickedBpdisable()  {
  CListBox  *pL;
  int        nS;
  BKRSP     *pB;
  ThreadMsg msg;
  BKCHG     *pCbp = &msg.bkchgData;

  pL = (CListBox *) GetDlgItem (IDC_BPLIST);
  nS = pL->GetCurSel();
  if (nS == LB_ERR)  {
    MessageBeep (MB_OK);
  }
  else  {
    pB = m_bpc.GetBp (nS);

    memset (pCbp, 0, sizeof (BKCHG));
    pCbp->type      = CHG_DISABLEBP;
    pCbp->nTickMark = pB->nTickMark;

    msg.cmd = UV_DBG_CHANGE_BP;
    msg.connHandle = _dlg->UVSCConnHandle;
    UVSCThread_Post(&msg, 0);
    
    InitBpListbox(nS);
  }
}

void CDebugDlg::OnBnClickedBpEnum()  {
  CListBox  *pL;
  int        nS;

  pL = (CListBox *) GetDlgItem (IDC_BPLIST);
  nS = pL->GetCurSel();
  InitBpListbox(nS);
}

void CDebugDlg::BpEnumUpdate(BKRSP *pBkptRsp, int *pBkptIndexes, int bkptCount) {
  int i;

  m_bpc.Clear();                   // clear bp-list

  for (i=0; i<bkptCount; i++) {
    m_bpc.AddBp (&pBkptRsp[i]);
  }

  m_bpc.Complete();                // set as completed.
}



//
// Breakpoint class implementation
//


/*
 * Breakpoint-container construction
 */
bp_class::bp_class()  {
  Clear();
}


/*
 * Breakpoint-container destruction
 */
bp_class::~bp_class()  {
}

void bp_class::Clear (void)  {
//memset (this, 0, sizeof (*this));
  nBreaks = 0;
  bDone   = 0;
  nIndex  = 0;
  memset (aBreaks, 0, sizeof (aBreaks));
}


void bp_class::Complete (void)  {
  bDone  = 1;
  nIndex = 0;
}

/*
 * Add a Breakpoint to breakpoint array
 */
int bp_class::AddBp (BKRSP *pB)  {
  if (nBreaks == (__NBREAKS - 1))  {    // more than 256 breaks
    return (0);                         // simply ignore it
  }

  int n = sizeof (BKRSP) + pB->nExpLen + 1;
  if (n > ((sizeof (aBreaks[0].aTmp) - 1)))  {
    n = sizeof (aBreaks[0].aTmp) - 1;
  }
  memcpy (&aBreaks[nBreaks], pB, n);
  aBreaks[nBreaks].aTmp[n] = 0;
  ++nBreaks;
  return (nBreaks);
}


void CDebugDlg::OnCbnSelchangeBrktype()
{
  // TODO: Add your control notification handler code here
}
