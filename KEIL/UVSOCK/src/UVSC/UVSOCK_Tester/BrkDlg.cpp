// BrkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DlgApp.h"
#include "MainDlg.h"
#include "Resource.h"
#include "BrkDlg.h"

extern CPwOptionsDlg   *_dlg; // user for Text 'OutLine()'



// CBrkDlg dialog

IMPLEMENT_DYNAMIC(CBrkDlg, CDialog)

CBrkDlg::CBrkDlg(CWnd* pParent /*=NULL*/) : CDialog(CBrkDlg::IDD, pParent)  {
}

CBrkDlg::~CBrkDlg()  {
}

void CBrkDlg::DoDataExchange (CDataExchange* pDX)  {
  CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBrkDlg, CDialog)
    ON_BN_CLICKED(IDC_BRKCREATE, &CBrkDlg::OnBnClickedBrkcreate)
    ON_LBN_SELCHANGE(IDC_BPLIST, &CBrkDlg::OnLbnSelchangeBplist)
    ON_BN_CLICKED(IDC_BPKILL, &CBrkDlg::OnBnClickedBpkill)
    ON_BN_CLICKED(IDC_BPENABLE, &CBrkDlg::OnBnClickedBpenable)
    ON_BN_CLICKED(IDC_BPDISABLE, &CBrkDlg::OnBnClickedBpdisable)
END_MESSAGE_MAP()


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
void CBrkDlg::OnBnClickedBrkcreate()  {
  int     nCount, nAccSz, nR, n;
  BKTYPE   nType;
  BKPARM    *pBrk;
  xUC8       tmp[1024];
  char     szExpr[258], szCmd[256], szCount[64], szAccSz[64];

  nAccSz = 0;

//---Breapoint-Expression:
  n = GetDlgItemText (IDC_BRKEXPR, &szExpr[0], sizeof (szExpr) - 2);
  szExpr [n] = 0;
  if (CheckInput (szExpr, n) == 0)  {      // empty input
    MessageBeep (MB_OK);
    MessageBox ("Non empty Breakpoint-Expression required !");
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
        MessageBox ("Non empty Access-Size required !");
        GetDlgItem (IDC_ACCSIZE)->SetFocus();
        return;
      }
      nR = sscanf (szAccSz, "%d", &nAccSz); // AccessSize
      if (nR == 0 || nR == EOF)  {
        MessageBeep (MB_OK);
        MessageBox ("Invalid Access-Size !");
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
      MessageBox ("Invalid Count !");
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
  memset (&tmp, 0, sizeof (tmp));         // clear temp buffer
  pBrk        = (BKPARM *) &tmp[0];

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
  _dlg->CreateCommandSend (UV_DBG_CREATE_BP, n, pBrk);

  Sleep (100);
  InitBpListbox();
}



void CBrkDlg::OnOK()  {
  CDialog::OnOK();
}


void CBrkDlg::OnCancel()  {
  CDialog::OnCancel();
}

BOOL CBrkDlg::OnInitDialog()  {

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

  InitBpListbox();

  return (TRUE);
}


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


void CBrkDlg::InitBpListbox (void)  {
  CListBox  *pL;
  int         i, n, z, j;
  BKRSP     *pB;
  char      szB [512];

  _dlg->CreateCommandSend (UV_DBG_ENUMERATE_BP, 0, NULL);  // get list of breakpoints
  DoEvents();
  Sleep (300);
  DoEvents();

  pL = (CListBox *) GetDlgItem (IDC_BPLIST);
  pL->ResetContent();
  j  = 0;
  if (bpc.IsComplete())  {
    for ( i = 0 ; i < bpc.GetCount() ; ++i )  {
      pB = bpc.GetBp (i);
      n  = GenLine (i, pB, &szB[0]);
      z = pL->AddString (&szB[0]);
      if (z != LB_ERR)  {
        ++j;
      }
    }
  }

  pL->SetCurSel (j > 0 ? 0 : -1);

  pB = bpc.GetBp (0);
  if (pB == NULL)  {
    GetDlgItem (IDC_BPENABLE)->EnableWindow (0);
    GetDlgItem (IDC_BPDISABLE)->EnableWindow (0);
    GetDlgItem (IDC_BPKILL)->EnableWindow (0);
  }
  else  {
    GetDlgItem (IDC_BPENABLE)->EnableWindow  ( pB->enabled ? 0 : 1);
    GetDlgItem (IDC_BPDISABLE)->EnableWindow ( pB->enabled ? 1 : 0);
    GetDlgItem (IDC_BPKILL)->EnableWindow (1);
  }
}



void CBrkDlg::OnLbnSelchangeBplist()  {
  CListBox  *pL;
  int        nS;
  BKRSP     *pB;

  pL = (CListBox *) GetDlgItem (IDC_BPLIST);
  nS = pL->GetCurSel();
  if (nS == LB_ERR)  {
    GetDlgItem (IDC_BPENABLE)->EnableWindow (0);
    GetDlgItem (IDC_BPDISABLE)->EnableWindow (0);
    GetDlgItem (IDC_BPKILL)->EnableWindow (0);
    MessageBeep (MB_OK);
  }
  else  {
    pB = bpc.GetBp (nS);
    GetDlgItem (IDC_BPENABLE)->EnableWindow  ( pB->enabled ? 0 : 1);
    GetDlgItem (IDC_BPDISABLE)->EnableWindow ( pB->enabled ? 1 : 0);
    GetDlgItem (IDC_BPKILL)->EnableWindow (1);
  }
}

void CBrkDlg::OnBnClickedBpkill()  {
  CListBox  *pL;
  int        nS;
  BKRSP     *pB;
  BKCHG     cbp;

  pL = (CListBox *) GetDlgItem (IDC_BPLIST);
  nS = pL->GetCurSel();
  if (nS == LB_ERR)  {
    MessageBeep (MB_OK);
  }
  else  {
    pB = bpc.GetBp (nS);

    memset (&cbp, 0, sizeof (cbp));
    cbp.type      = CHG_KILLBP;
    cbp.nTickMark = pB->nTickMark;
    _dlg->CreateCommandSend (UV_DBG_CHANGE_BP, sizeof (cbp), &cbp);

    Sleep (100);
    InitBpListbox();
  }
}

void CBrkDlg::OnBnClickedBpenable()  {
  CListBox  *pL;
  int        nS;
  BKRSP     *pB;
  BKCHG     cbp;
  char      szB [512];

  pL = (CListBox *) GetDlgItem (IDC_BPLIST);
  nS = pL->GetCurSel();
  if (nS == LB_ERR)  {
    MessageBeep (MB_OK);
  }
  else  {
    pB = bpc.GetBp (nS);
    pB->enabled = 1;
    GenLine (nS, pB, &szB[0]);
    pL->DeleteString (nS);
    pL->InsertString (nS, szB);
    GetDlgItem (IDC_BPENABLE)->EnableWindow  (0);
    GetDlgItem (IDC_BPDISABLE)->EnableWindow (1);

    memset (&cbp, 0, sizeof (cbp));
    cbp.type      = CHG_ENABLEBP;
    cbp.nTickMark = pB->nTickMark;
    _dlg->CreateCommandSend (UV_DBG_CHANGE_BP, sizeof (cbp), &cbp);
  }
}

void CBrkDlg::OnBnClickedBpdisable()  {
  CListBox  *pL;
  int        nS;
  BKRSP     *pB;
  BKCHG     cbp;
  char      szB [512];

  pL = (CListBox *) GetDlgItem (IDC_BPLIST);
  nS = pL->GetCurSel();
  if (nS == LB_ERR)  {
    MessageBeep (MB_OK);
  }
  else  {
    pB = bpc.GetBp (nS);
    pB->enabled = 0;
    GenLine (nS, pB, &szB[0]);
    pL->DeleteString (nS);
    pL->InsertString (nS, szB);
    GetDlgItem (IDC_BPENABLE)->EnableWindow  (1);
    GetDlgItem (IDC_BPDISABLE)->EnableWindow (0);

    memset (&cbp, 0, sizeof (cbp));
    cbp.type      = CHG_DISABLEBP;
    cbp.nTickMark = pB->nTickMark;
    _dlg->CreateCommandSend (UV_DBG_CHANGE_BP, sizeof (cbp), &cbp);
  }
}
