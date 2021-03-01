// OptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DlgApp.h"
#include "MainDlg.h"
#include "Resource.h"
#include "OptDlg.h"


extern CPwOptionsDlg   *_dlg; // user for Text 'OutLine()'


// COptDlg dialog

IMPLEMENT_DYNAMIC(COptDlg, CDialog)

COptDlg::COptDlg(CWnd* pParent /*=NULL*/) : CDialog(COptDlg::IDD, pParent)  {
}


COptDlg::~COptDlg()  {
}


void COptDlg::DoDataExchange (CDataExchange* pDX)  {
  CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptDlg, CDialog)
//  ON_BN_CLICKED(IDOK, &COptDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDC_OPTREAD, &COptDlg::OnBnClickedOptread)
    ON_BN_CLICKED(IDC_OPTWRITE, &COptDlg::OnBnClickedOptwrite)
END_MESSAGE_MAP()


// COptDlg message handlers

void COptDlg::OnOK()  {
  CDialog::OnOK();
}

void COptDlg::OnCancel()  {
  CDialog::OnCancel();
}


BOOL COptDlg::OnInitDialog()  {
  CComboBox *pC = (CComboBox *) GetDlgItem (IDC_JOBTYPE);

  pC->AddString ("L-Misc");
  pC->AddString ("C-Misc");
  pC->AddString ("A-Misc");
  pC->AddString ("C-Include");
  pC->AddString ("A-Include");
  pC->AddString ("C-Define");
  pC->AddString ("C-Undefine");
  pC->AddString ("A-Define");
  pC->AddString ("C-Optimize");
  pC->AddString ("CodeGen (Arm/Thumb)");
  pC->AddString ("Defined Memory Ranges");   // on Target-Level only
  pC->AddString ("Assigned Memory Ranges");  // on Group- or File-Level only

  pC->SetCurSel (0);

  CEdit *pE = (CEdit *) GetDlgItem (IDC_TNAME);
  pE->LimitText (128);

  pE = (CEdit *) GetDlgItem (IDC_GNAME);
  pE->LimitText (128);

  pE = (CEdit *) GetDlgItem (IDC_FNAME);
  pE->LimitText (256);

  pE = (CEdit *) GetDlgItem (IDC_OPTSTR);
  pE->LimitText (512);

  return (TRUE);
}


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


int COptDlg::CheckTname (char *pB)  {
  int        n, i, j;
  char     szB[128];
  
  n = GetDlgItemText (IDC_TNAME, &szB[0], sizeof (szB) - 2);
  szB[n] = 0;
  if (CheckInput (szB, n) == 0)  {      // empty input
    MessageBeep (MB_OK);
    MessageBox ("At least the target name is required !");
    GetDlgItem (IDC_TNAME)->SetFocus();
    return (0);
  }
  i = 0;
  while (szB[i] == ' ')  {
    ++i;
  }

  for ( j = 0 ; i < n ; ++i )  {
    pB [j] = szB[i];
    ++j;
  }
  pB [j] = 0;
  return (1);
}


int COptDlg::GetGname (char *pB)  {
  int        n, i, j;
  char     szB[128];

  n = GetDlgItemText (IDC_GNAME, &szB[0], sizeof (szB) - 2);
  szB[n] = 0;
  if (CheckInput (szB, n) == 0)  {      // empty input
    pB[0] = 0;
    return (1);
  }
  i = 0;
  while (szB[i] == ' ')  {
    ++i;
  }

  for ( j = 0 ; i < n ; ++i )  {
    pB [j] = szB[i];
    ++j;
  }
  pB [j] = 0;
  return (1);
}

int COptDlg::GetFname (char *pB)  {
  int        n, i, j;
  char     szB[256];

  n = GetDlgItemText (IDC_FNAME, &szB[0], sizeof (szB) - 2);
  szB[n] = 0;
  if (CheckInput (szB, n) == 0)  {      // empty input
    pB[0] = 0;
    return (1);
  }
  i = 0;
  while (szB[i] == ' ')  {
    ++i;
  }

  for ( j = 0 ; i < n ; ++i )  {
    pB [j] = szB[i];
    ++j;
  }
  pB [j] = 0;
  return (1);
}



void COptDlg::OnBnClickedOptread()  {
  char     szT[256], szG[256], szF[512];
  char     szX[16384];
  TRNOPT   *pX;
  int        n, l;

  if (!CheckTname (szT)) return;
  GetGname (szG);
  GetFname (szF);
  if (szF[0] != 0 && szG[0] == 0)  {    // File name, but no group name
    MessageBeep (MB_OK);
    MessageBox ("A File name also requires a Group name !");
    GetDlgItem (IDC_GNAME)->SetFocus();
    return;
  }

  memset (szX, 0, sizeof (szX));

  pX = (TRNOPT *) (void *) &szX[0];
  switch ( ((CComboBox *) GetDlgItem (IDC_JOBTYPE))->GetCurSel() )  {
    case 0:                            // L-Misc
      pX->job = OPT_LMISC;
      break;
    case 1:                            // C-Misc
      pX->job = OPT_CMISC;
      break;
    case 2:                            // A-Misc
      pX->job = OPT_AMISC;
      break;
    case 3:                            // C-Include
      pX->job = OPT_CINCL;
      break;
    case 4:                            // A-Include
      pX->job = OPT_AINCL;
      break;
    case 5:                            // C-Define
      pX->job = OPT_CDEF;
      break;
    case 6:                            // C-Undefine
      pX->job = OPT_CUNDEF;
      break;
    case 7:                            // A-Define
      pX->job = OPT_ADEF;
      break;
    case 8:                            // C-Optimize
      pX->job = OPT_COPTIMIZE;
      break;
    case 9:                            // Code-Gen: ARM/Thumb
      pX->job = OPT_CODEGEN;
      break;
    case 10:                           // read MEMRANGES
      pX->job = OPT_MEMRANGES;
      break;
    case 11:                           // read MEMRANGE Assignments
      pX->job = OPT_ASNMEMRANGES;      // get/set assigned ranges
      break;
  }

  n = 1;
  pX->iTarg = 1;                                // target-name starts at szBuffer[iTarg]
  l = strlen (szT);
  memcpy (&pX->szBuffer[n], szT, l + 1);        // put target-name into buffer
  n = n + (l + 1);                              // advance index by length + 1

  l = strlen (szG);
  if (l != 0)  {                                // group-name is present
    pX->iGroup = n;                             // group-name starts at szBuffer[iGroup]
    memcpy (&pX->szBuffer[n], szG, l + 1);      // put group-name into buffer
    n = n + (l + 1);                            // advance index by length + 1

    l = strlen (szF);
    if (l != 0)  {                              // File is present
      pX->iFile = n;                            // file-name starts at szBuffer[iFile]
      memcpy (&pX->szBuffer[n], szF, l + 1);
      n = n + (l + 1);
    }
  }

  n += offsetof (TRNOPT, szBuffer);

  CPwOptionsDlg *pM = (CPwOptionsDlg *) AfxGetApp()->m_pMainWnd;
  pM->CreateCommandSend (UV_PRJ_GET_OPTITEM, n, pX);
}



int COptDlg::CheckOptStr (char *pB)  {
  int        n, i, j;
  char     szB[512];
  
  n = GetDlgItemText (IDC_OPTSTR, &szB[0], sizeof (szB) - 2);
  szB[n] = 0;
  if (CheckInput (szB, n) == 0)  {      // empty input
    pB[0] = 0;
    return (1);
  }
  i = 0;
  while (szB[i] == ' ')  {
    ++i;
  }

  for ( j = 0 ; i < n ; ++i )  {
    pB [j] = szB[i];
    ++j;
  }
  pB [j] = 0;
  return (1);
}


void COptDlg::OnBnClickedOptwrite()  {
  char     opt[512];
  char     szT[256], szG[256], szF[512];
  char     szX[16384];
  TRNOPT   *pX;
  int        n, l;

  if (!CheckTname (szT)) return;
  GetGname (szG);
  GetFname (szF);
  if (szF[0] != 0 && szG[0] == 0)  {    // File name, but no group name
    MessageBeep (MB_OK);
    MessageBox ("A File name also requires a Group name !");
    GetDlgItem (IDC_GNAME)->SetFocus();
    return;
  }

  if (!CheckOptStr (opt)) return;

  memset (szX, 0, sizeof (szX));       // clear the whole buffer
  pX = (TRNOPT *) (void *) &szX[0];
  switch ( ((CComboBox *) GetDlgItem (IDC_JOBTYPE))->GetCurSel() )  {
    case 0:                            // L-Misc
      pX->job = OPT_LMISC;
      break;
    case 1:                            // C-Misc
      pX->job = OPT_CMISC;
      break;
    case 2:                            // A-Misc
      pX->job = OPT_AMISC;
      break;
    case 3:                            // C-Include
      pX->job = OPT_CINCL;
      break;
    case 4:                            // A-Include
      pX->job = OPT_AINCL;
      break;
    case 5:                            // C-Define
      pX->job = OPT_CDEF;
      break;
    case 6:                            // C-Undefine
      pX->job = OPT_CUNDEF;
      break;
    case 7:                            // A-Define
      pX->job = OPT_ADEF;
      break;
    case 8:                            // C-Optimize
      pX->job = OPT_COPTIMIZE;
      break;
    case 9:                            // Code-Gen: ARM/Thumb
      pX->job = OPT_CODEGEN;
      break;
    case 10:                           // OPT_MEMRANGES
      AfxMessageBox ("Memory Ranges are Read-Only !");
      return;
    case 11:                           // read MEMRANGES
      pX->job = OPT_ASNMEMRANGES;      // get/set assigned ranges
      break;
  }

  n = 1;
  pX->iTarg = 1;                                // target-name starts at szBuffer[iTarg]
  l = strlen (szT);
  memcpy (&pX->szBuffer[n], szT, l + 1);        // put target-name into buffer
  n = n + (l + 1);                              // advance index by length + 1

  l = strlen (szG);
  if (l != 0)  {                                // group-name is present
    pX->iGroup = n;                             // group-name starts at szBuffer[iGroup]
    memcpy (&pX->szBuffer[n], szG, l + 1);      // put group-name into buffer
    n = n + (l + 1);                            // advance index by length + 1

    l = strlen (szF);
    if (l != 0)  {                              // File is present
      pX->iFile = n;                            // file-name starts at szBuffer[iFile]
      memcpy (&pX->szBuffer[n], szF, l + 1);
      n = n + (l + 1);
    }
  }

  l = strlen (opt);
  if (l != NULL)  {
    pX->iItem = n;
    memcpy (&pX->szBuffer[n], opt, l + 1);
    n = n + (l + 1);
  }

  n += offsetof (TRNOPT, szBuffer);
  CPwOptionsDlg *pM = (CPwOptionsDlg *) AfxGetApp()->m_pMainWnd;
  pM->CreateCommandSend (UV_PRJ_SET_OPTITEM, n, pX);
}
