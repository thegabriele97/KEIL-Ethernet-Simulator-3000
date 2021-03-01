/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2004-2016 KEIL - An ARM Company. All rights reserved.        */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include "stdafx.h"
#include "DlgApp.h"
#include "PgProject.h"
#include "MainDlg.h"

#include "UVSCThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CUVSCTesterDlg   *_dlg;

/////////////////////////////////////////////////////////////////////////////
// CProjectDlg dialog


CProjectDlg::CProjectDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CProjectDlg::IDD, pParent)
{
  //{{AFX_DATA_INIT(CProjectDlg)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void CProjectDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CProjectDlg)
    // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
  DDX_Text(pDX, IDC_BUILDOUTPUTTEXT, m_StrBuildOutput);
  DDX_Text(pDX, IDC_STATIC_BUILDLINETEXT, m_StrBuildLine);
}


BEGIN_MESSAGE_MAP(CProjectDlg, CDialog)
  //{{AFX_MSG_MAP(CProjectDlg)
  ON_BN_CLICKED(IDC_SENDBUILD, OnSendbuild)
  ON_BN_CLICKED(IDC_SENDREBUILD, OnSendrebuild)
  ON_BN_CLICKED(IDC_SENDSTOPBUILD, OnSendstopbuild)
  ON_BN_CLICKED(IDC_CLEANPRJ, OnCleanprj)
  ON_BN_CLICKED(IDC_ADDGRP, OnAddgrp)
  ON_BN_CLICKED(IDC_DELGROUP, OnDelgroup)
  ON_BN_CLICKED(IDC_ADDFILE, OnAddfile)
  ON_BN_CLICKED(IDC_DELFILE, OnDelfile)
  ON_BN_CLICKED(IDC_FLASHDOWNLOAD, OnFlashDownload)
  //}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_ENUMFILES, &CProjectDlg::OnBnClickedEnumfiles)
  ON_BN_CLICKED(IDC_ENUMGROUPS, &CProjectDlg::OnBnClickedEnumgroups)
  ON_BN_CLICKED(IDC_ENUMTARGETS, &CProjectDlg::OnBnClickedEnumtargets)
  ON_BN_CLICKED(IDC_BUTTON_SET_TARGET, &CProjectDlg::OnBnClickedSettargets)
  ON_BN_CLICKED(IDC_BUTTON_SET_OUTPUTNAME, &CProjectDlg::OnBnClickedSetOutputName)
  ON_BN_CLICKED(IDC_BUTTON_GET_CURRENT_TARGET, &CProjectDlg::OnBnClickedGetCurrentTarget)
  ON_BN_CLICKED(IDC_ACTFILES, &CProjectDlg::OnBnClickedActfiles)
  ON_BN_CLICKED(IDC_OPTREAD, &CProjectDlg::OnBnClickedOptread)
  ON_BN_CLICKED(IDC_OPTWRITE, &CProjectDlg::OnBnClickedOptwrite)
  ON_BN_CLICKED(IDC_CLOSEPRJ, &CProjectDlg::OnBnClickedCloseprj)
  ON_BN_CLICKED(IDC_PRJREAD, &CProjectDlg::OnBnClickedLoadprj)
  ON_BN_CLICKED(IDC_PRJOUTNAME, &CProjectDlg::OnBnClickedPrjOutName)
  ON_BN_CLICKED(IDC_BUTTON_USIM, &CProjectDlg::OnBnClickedUseSim)
  ON_BN_CLICKED(IDC_BUTTON_UTRG, &CProjectDlg::OnBnClickedUseTrg)
  ON_BN_CLICKED(IDC_BUTTON_GETDBGTGT, &CProjectDlg::OnBnClickedGetDbgTarget)  

  ON_CBN_CLOSEUP(IDC_JOBTYPE, &CProjectDlg::OnCbnCloseup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectDlg message handlers

void CProjectDlg::OnCancel()  {   // Catch ESC

}

void CProjectDlg::OnOK()  {       // Catch Enter

}


#define _MAX_GROUPNAME_LENGTH  128    // max. length of GroupName


BOOL CProjectDlg::OnInitDialog()  {
  CDialog::OnInitDialog();
  ((CEdit *) GetDlgItem (IDC_EDGROUP))->LimitText (_MAX_GROUPNAME_LENGTH);

  CComboBox *pC = (CComboBox *) GetDlgItem (IDC_JOBTYPE);

  pC->InsertString (0, "L-Misc");
  pC->InsertString (1, "C-Misc");
  pC->InsertString (2, "A-Misc");
  pC->InsertString (3, "C-Include");
  pC->InsertString (4, "A-Include");
  pC->InsertString (5, "C-Define");
  pC->InsertString (6, "C-Undefine");
  pC->InsertString (7, "A-Define");
  pC->InsertString (8, "C-Optimize");
  pC->InsertString (9, "CodeGen (Arm/Thumb)");
  pC->InsertString (10, "Defined Memory Ranges");   // on Target-Level only
  pC->InsertString (11, "Assigned Memory Ranges");  // on Group- or File-Level only
  pC->InsertString (12, "Run User Programs Before Compilation of a C/C++ File 1");  // 
  pC->InsertString (13, "Run User Programs Before Compilation of a C/C++ File 2");  // 
  pC->InsertString (14, "Run User Programs Before Build / Rebuild 1");  // 
  pC->InsertString (15, "Run User Programs Before Build / Rebuild 2");  // 
  pC->InsertString (16, "Run User Programs After  Build / Rebuild 1");  // 
  pC->InsertString (17, "Run User Programs After  Build / Rebuild 2");  // 
  pC->InsertString (18, "Beep When Complete");  // 
  pC->InsertString (19, "Start Debugging");  // 

  pC->SetCurSel (0);

  CEdit *pE = (CEdit *) GetDlgItem (IDC_TNAME);
  pE->LimitText (128);

  pE = (CEdit *) GetDlgItem (IDC_GNAME);
  pE->LimitText (128);

  pE = (CEdit *) GetDlgItem (IDC_FNAME);
  pE->LimitText (256);

  pE = (CEdit *) GetDlgItem (IDC_OPTSTR);
  pE->LimitText (512);

  return TRUE;
}


void CProjectDlg::OnCbnCloseup()
{
  unsigned int index;
  char text[128] = { 0 };
  CWnd *pWnd;

  CComboBox *pC = (CComboBox *) GetDlgItem (IDC_JOBTYPE);
  index = pC->GetCurSel();

  if(index >= 12 && index <= 17) sprintf(text, "Format: bRUN bDOS16 User Program\ni.e.: 10c:\\Execute\\MyProgram.exe");
  else if(index == 18          ) sprintf(text, "Format: 'beep' or empty string");
  else if(index == 19          ) sprintf(text, "Format: 'start debug' or empty string");
  else                           sprintf(text, "");
  
  pWnd = GetDlgItem(IDC_STATIC_JOB_HELP);
  pWnd->SetWindowText(text);
}


/*
 * Build current Project
 */
void CProjectDlg::OnSendbuild()  {
  ThreadMsg msg;

  msg.cmd = UV_PRJ_BUILD;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);

  // Automatically get the build output
  GetBuildOutput();
}


/*
 * ReBuild current Project
 */
void CProjectDlg::OnSendrebuild()  {
  ThreadMsg msg;

  msg.cmd = UV_PRJ_REBUILD;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);

  // Automatically get the build output
  GetBuildOutput();
}


/*
 * Clean current Project
 */
void CProjectDlg::OnCleanprj()  {
  ThreadMsg msg;

  msg.cmd = UV_PRJ_CLEAN;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);

  // Automatically get the build output
  GetBuildOutput();
}


/*
 * Stop build/rebuild
 * NOTE: This function is called from directly the GUI thread, not through
 * the UVSC thread. This is necessary, because the build function
 * will not yet have returned.
 */
void CProjectDlg::OnSendstopbuild()  {
  UVSC_PRJ_BUILD_CANCEL(_dlg->UVSCConnHandle);
  //ThreadMsg msg;

  //msg.cmd = UV_PRJ_BUILD_CANCEL;
  //msg.connHandle = _dlg->UVSCConnHandle;
  //UVSCThread_Post(&msg, 0);
}



int GetItemName (CEdit *pE, char *pBuf)  {
  char          szB [_MAX_GROUPNAME_LENGTH];
  int       z, i, n;

  n = pE->GetWindowText (szB, _MAX_GROUPNAME_LENGTH);
  i = 0;
  for ( i = 0 ; i < n ; ++i )  {
    if (szB [i] != ' ') break;              // kill leading spaces
  }
  for ( z = 0 ; i < n ; ++i, ++z )  {
    pBuf[z] = szB[i];
  }
  pBuf [z] = 0;
  return (z);
}


/*
 * Add a Group to current Project
 */ 
void CProjectDlg::OnAddgrp()  {
  int             n;
  ThreadMsg     msg;
  PRJDATA       *pJ = &msg.prjData;
  CEdit         *pE;

  pE = (CEdit *) GetDlgItem (IDC_EDGROUP);
  n  = GetItemName (pE, &pJ->szNames[0]);
  if (n == 0)  {
    MessageBeep (MB_OK);
    MessageBox ("No group name specified!");
    pE->SetFocus();
    return;
  }

  pJ->nLen  = 0;
  pJ->nCode = 0;
  n += 1 + (sizeof (PRJDATA) - sizeof (pJ->szNames));   // account for nRes, nCode

  msg.cmd = UV_PRJ_ADD_GROUP;
  msg.dataLen = n;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}


/*
 * Remove a Group from current Project
 */ 
void CProjectDlg::OnDelgroup()  {
  int             n;
  ThreadMsg     msg;
  PRJDATA       *pJ = &msg.prjData;
  CEdit         *pE;

  pE = (CEdit *) GetDlgItem (IDC_EDGROUP);
  n  = GetItemName (pE, &pJ->szNames[0]);
  if (n == 0)  {
    MessageBeep (MB_OK);
    MessageBox ("No group name specified!");
    pE->SetFocus();
    return;
  }

  pJ->nLen  = 0;
  pJ->nCode = 0;
  n += 1 + (sizeof (PRJDATA) - sizeof (pJ->szNames));   // account for nRes, nCode

  msg.cmd = UV_PRJ_DEL_GROUP;
  msg.dataLen = n;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}


/*
 * Add File(s) to specified Group
 *   pJ->szNames[]:     'Group Name',0
 *                      'File Name 1', 0,      // full-path file name
 *                    [ 'File Name 2', 0, ]    // full-path file name
 *                    [  ...         , 0, ]    //
 *                       0                     // end of list marker
 */
void CProjectDlg::OnAddfile()  {
  int          j, n;
  ThreadMsg     msg;
  PRJDATA       *pJ = &msg.prjData;
  CEdit         *pG, *pE;

  pE = (CEdit *) GetDlgItem (IDC_EDGROUP);
  n  = GetItemName (pE, &pJ->szNames[0]);      // get current group name
  if (n == 0)  {
    MessageBeep (MB_OK);
    MessageBox ("No group name specified!");
    pE->SetFocus();
    return;
  }
  n  = n + 1;                                  // pos: after groupname-terminator

  pG = (CEdit *) GetDlgItem (IDC_EDFILENAME);
  j  = GetItemName (pG, &pJ->szNames[n]);      // get current file name
  if (j == 0)  {
    MessageBeep (MB_OK);
    MessageBox ("No file name specified!");
    pG->SetFocus();
    return;
  }
  pJ->szNames [n + j + 1] = 0;                 // one more zero at end of items

  n = n + j + 2;         // size: length(groupname) + 1(z) + length(filename) + 1(z) + 1(z)
  
  pJ->nLen  = 0;
  pJ->nCode = 0;
  n  = n + (sizeof (PRJDATA) - sizeof (pJ->szNames));   // account for nRes, nCode

  msg.cmd = UV_PRJ_ADD_FILE;
  msg.dataLen = n;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}


/*
 * Remove File(s) from specified Group
 *   pJ->szNames[]:     'Group Name',0
 *                      'File Name 1', 0,      // full-path file name
 *                    [ 'File Name 2', 0, ]    // full-path file name
 *                    [  ...         , 0, ]    //
 *                       0                     // end of list marker
 */
void CProjectDlg::OnDelfile()  {
  int          j, n;
  ThreadMsg     msg;
  PRJDATA       *pJ = &msg.prjData;
  CEdit         *pG, *pE;

  pE = (CEdit *) GetDlgItem (IDC_EDGROUP);
  n  = GetItemName (pE, &pJ->szNames[0]);      // get current group name
  if (n == 0)  {
    MessageBeep (MB_OK);
    MessageBox ("No group name specified!");
    pE->SetFocus();
    return;
  }
  n  = n + 1;                                  // pos: after groupname-terminator

  pG = (CEdit *) GetDlgItem (IDC_EDFILENAME);
  j  = GetItemName (pG, &pJ->szNames[n]);      // get current file name
  if (j == 0)  {
    MessageBeep (MB_OK);
    MessageBox ("No file name specified!");
    pG->SetFocus();
    return;
  }
  pJ->szNames [n + j + 1] = 0;                 // one more zero at end of items

  n = n + j + 2;         // size: length(groupname) + 1(z) + length(filename) + 1(z) + 1(z)
  
  pJ->nLen  = 0;
  pJ->nCode = 0;
  n  = n + (sizeof (PRJDATA) - sizeof (pJ->szNames));   // account for nRes, nCode

  msg.cmd = UV_PRJ_DEL_FILE;
  msg.dataLen = n;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}

void CProjectDlg::OnFlashDownload() {
  ThreadMsg msg;

  msg.cmd = UV_PRJ_FLASH_DOWNLOAD;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}


/*
 * Enumerate files of specified group
 */
void CProjectDlg::OnBnClickedEnumfiles()  {
  int             n;
  ThreadMsg     msg;
  SSTR          *pJ = &msg.sstrData;
  CEdit         *pE;

  pE = (CEdit *) GetDlgItem (IDC_EDGROUP);
  n  = GetItemName (pE, &pJ->szStr[0]);
  if (n == 0)  {
    MessageBeep (MB_OK);
    MessageBox ("No group name specified!");
    pE->SetFocus();
    return;
  }

  pJ->nLen  = n;
  n += 1 + sizeof (int);         // account for nLen, 0-terminator

  msg.cmd = UV_PRJ_ENUM_FILES;
  msg.dataLen = n;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);  
}


/*
 * Enumerate Groups
 */
void CProjectDlg::OnBnClickedEnumgroups()  {
  ThreadMsg msg;

  msg.cmd = UV_PRJ_ENUM_GROUPS;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}

/*
 * Enumerate Targets
 */
void CProjectDlg::OnBnClickedEnumtargets()  {
  ThreadMsg msg;

  msg.cmd = UV_PRJ_ENUM_TARGETS;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}

/*
 * Set a Target active
 */
void CProjectDlg::OnBnClickedSettargets()  {
  int             n;
  ThreadMsg     msg;
  PRJDATA       *pJ = &msg.prjData;
  CEdit         *pE;

  pE = (CEdit *) GetDlgItem (IDC_EDIT_SET_TARGET);
  n  = GetItemName (pE, &pJ->szNames[0]);
  if (n == 0)  {
    MessageBeep (MB_OK);
    MessageBox ("No target name specified!");
    pE->SetFocus();
    return;
  }

  pJ->nLen  = 0;
  pJ->nCode = 0;
  n += 1 + (sizeof (PRJDATA) - sizeof (pJ->szNames));   // account for nRes, nCode

  msg.cmd = UV_PRJ_SET_TARGET;
  msg.dataLen = n;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}


/*
 * Set output name
 */
void CProjectDlg::OnBnClickedSetOutputName()  {
  int             n;
  ThreadMsg     msg;
  PRJDATA       *pJ = &msg.prjData;
  CEdit         *pE;

  pE = (CEdit *) GetDlgItem (IDC_EDIT_SET_OUTPUTNAME);
  n  = GetItemName (pE, &pJ->szNames[0]);
  if (n == 0)  {
    MessageBeep (MB_OK);
    MessageBox ("No output name specified!");
    pE->SetFocus();
    return;
  }

  pJ->nLen  = 0;
  pJ->nCode = 0;
  n += 1 + (sizeof (PRJDATA) - sizeof (pJ->szNames));   // account for nRes, nCode

  msg.cmd = UV_PRJ_SET_OUTPUTNAME;
  msg.dataLen = n;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}


/*
 * Get Current Target
 */
void CProjectDlg::OnBnClickedGetCurrentTarget()  {
  CString      cs;
  ThreadMsg    msg;
  iPATHREQ    *pPathReq = &msg.iPathReq;
  int          n = 0;

  pPathReq->bFull = 1;
  n = sizeof(iPATHREQ);

  msg.cmd = UV_PRJ_GET_CUR_TARGET;
  msg.dataLen = n;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}



/*
 * Count active files of current target
 */
void CProjectDlg::OnBnClickedActfiles()  {
  ThreadMsg msg;

  msg.cmd = UV_PRJ_ACTIVE_FILES;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}

void CProjectDlg::SetBuildOutputText(const char *txt) {
  m_StrBuildOutput.Format("%s", txt);
  m_StrBuildOutput.Replace("\x0A", "\x0D\x0A");
  UpdateData(FALSE);
}

void CProjectDlg::SetBuildLineText(const char *txt) {
  m_StrBuildLine.Format("%s", txt);
  UpdateData(FALSE);
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


int CProjectDlg::CheckTname (char *pB)  {
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


int CProjectDlg::GetGname (char *pB)  {
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

int CProjectDlg::GetFname (char *pB)  {
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



void CProjectDlg::OnBnClickedOptread()  {
  char     szT[256], szG[256], szF[512];
  ThreadMsg msg;
  TRNOPT   *pX = &msg.trnOptData;
  int        n, l;

  if (!CheckTname (szT)) return;
  GetGname (szG);
  GetFname (szF);
  if (szF[0] != 0 && szG[0] == 0)  {    // File name, but no group name
    MessageBeep (MB_OK);
    MessageBox ("File name specified without a group name!");
    GetDlgItem (IDC_GNAME)->SetFocus();
    return;
  }

  memset (pX, 0, sizeof (msg.rawData));     // clear the whole buffer

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
    case 12:
      pX->job = OPT_UBCOMP1;           // Run User Programs Before Compilation of a C/C++ File 1
      break;
    case 13:
      pX->job = OPT_UBCOMP2;           // Run User Programs Before Compilation of a C/C++ File 2
      break;
    case 14:
      pX->job = OPT_UBBUILD1;          // Run User Programs Before Build / Rebuild 1
      break;
    case 15:
      pX->job = OPT_UBBUILD2;          // Run User Programs Before Build / Rebuild 2
      break;
    case 16:
      pX->job = OPT_UABUILD1;          // Run User Programs After  Build / Rebuild 1
      break;
    case 17:
      pX->job = OPT_UABUILD2;          // Run User Programs After  Build / Rebuild 2
      break;
    case 18:
      pX->job = OPT_UBEEP;             // Beep When Complete
      break;
    case 19:
      pX->job = OPT_USTARTDEB;         // Start Debugging
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

  msg.cmd = UV_PRJ_GET_OPTITEM;
  msg.dataLen = n + 16384;    // 16384 ensures space for the return data (size unknown)
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);

}



int CProjectDlg::CheckOptStr (char *pB)  {
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


void CProjectDlg::OnBnClickedOptwrite()  {
  char     opt[512];
  char     szT[256], szG[256], szF[512];
  ThreadMsg msg;
  TRNOPT   *pX = &msg.trnOptData;  
  int        n, l;

  if (!CheckTname (szT)) return;
  GetGname (szG);
  GetFname (szF);
  if (szF[0] != 0 && szG[0] == 0)  {    // File name, but no group name
    MessageBeep (MB_OK);
    MessageBox ("File name specified without a group name!");
    GetDlgItem (IDC_GNAME)->SetFocus();
    return;
  }

  if (!CheckOptStr (opt)) return;

  memset (pX, 0, sizeof (msg.rawData));     // clear the whole buffer
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
      AfxMessageBox ("Memory ranges are Read-Only!");
      return;
    case 11:                           // read MEMRANGES
      pX->job = OPT_ASNMEMRANGES;      // get/set assigned ranges
      break;
    case 12:
      pX->job = OPT_UBCOMP1;           // Run User Programs Before Compilation of a C/C++ File 1
      break;
    case 13:
      pX->job = OPT_UBCOMP2;           // Run User Programs Before Compilation of a C/C++ File 2
      break;
    case 14:
      pX->job = OPT_UBBUILD1;          // Run User Programs Before Build / Rebuild 1
      break;
    case 15:
      pX->job = OPT_UBBUILD2;          // Run User Programs Before Build / Rebuild 2
      break;
    case 16:
      pX->job = OPT_UABUILD1;          // Run User Programs After  Build / Rebuild 1
      break;
    case 17:
      pX->job = OPT_UABUILD2;          // Run User Programs After  Build / Rebuild 2
      break;
    case 18:
      pX->job = OPT_UBEEP;             // Beep When Complete
      break;
    case 19:
      pX->job = OPT_USTARTDEB;         // Start Debugging
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

  msg.cmd = UV_PRJ_SET_OPTITEM;
  msg.dataLen = n;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);

}

void CProjectDlg::OnBnClickedUseSim() {
  ThreadMsg msg;

  msg.cmd = UV_PRJ_SET_DEBUG_TARGET;  
  msg.connHandle = _dlg->UVSCConnHandle;
  msg.dbgtgtopt.target = UV_TARGET_SIM;
  UVSCThread_Post(&msg, 0);
}

void CProjectDlg::OnBnClickedUseTrg() {
  ThreadMsg msg;

  msg.cmd = UV_PRJ_SET_DEBUG_TARGET;  
  msg.connHandle = _dlg->UVSCConnHandle;
  msg.dbgtgtopt.target = UV_TARGET_HW;
  UVSCThread_Post(&msg, 0);
}

void CProjectDlg::OnBnClickedGetDbgTarget() {
  ThreadMsg msg;

  msg.cmd = UV_PRJ_GET_DEBUG_TARGET;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}


void CProjectDlg::OnBnClickedCloseprj()  {
  ThreadMsg msg;

  msg.cmd = UV_PRJ_CLOSE;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}



static const char SfT[] = "Select Project File";
static const char SfM[] = "Project Files (*.uv2 *.uvproj *.uvprojx)|*.uv2;*.uvproj;*.uvprojx|"  \
                          "Multi Project Workspaces (*.mpw *.uvmpw)|*.mpw;*.uvmpw||";
//                        "Dave Project Files (*.dpt)|*.dpt||";

/*
 * added .uvmpw file mask above to SfM[]  /23.7.2015/
 */


static char     lastPrj [MAX_PATH+10];

void CProjectDlg::OnBnClickedLoadprj()  {
  CString      cs;
  LPCTSTR      lp;
  ThreadMsg msg;
  PRJDATA     *pD = &msg.prjData;
  int           n = 0;

  CFileDialog cd (TRUE,                 // Open-File
                  NULL,
                  NULL,
                  OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLESIZING,
                  SfM);
  cd.m_ofn.lpstrTitle = SfT;
  if (lastPrj[0]) cd.m_ofn.lpstrInitialDir = &lastPrj[0];

  if (cd.DoModal() == IDOK)  {
    cs = cd.GetPathName();
    lp = (LPCTSTR) cs;

    strcpy (lastPrj, lp);               // keep the path
    int n = strlen (lastPrj);
    while ( n > 0 && (lastPrj[n-1] != '\\' && lastPrj[n-1] != ':'))  {
      --n;                              // remove file name from path
    }
    lastPrj [n] = 0;

    pD->nCode = 0;
    pD->nLen  = strlen (lp) + 1;
    memcpy (pD->szNames, lp, pD->nLen); // including zero-terminator
    n  = offsetof (PRJDATA,szNames) + pD->nLen;

    msg.cmd = UV_PRJ_LOAD;
    msg.dataLen = n;
    msg.connHandle = _dlg->UVSCConnHandle;
    UVSCThread_Post(&msg, 0);
  }
}


void CProjectDlg::OnBnClickedPrjOutName()  {
  CString      cs;
  ThreadMsg msg;
  iPATHREQ    *pPathReq = &msg.iPathReq;
  int           n = 0;

  pPathReq->bFull = 1;
  n = sizeof(iPATHREQ);

  msg.cmd = UV_PRJ_GET_OUTPUTNAME;
  msg.dataLen = n;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}


void CProjectDlg::GetBuildOutput()  {
  ThreadMsg msg;

  msg.cmd = UV_NULL_CMD;
  msg.ctrlMsg.cmd = CONTROLMSG_BUILDOUTPUT;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}
