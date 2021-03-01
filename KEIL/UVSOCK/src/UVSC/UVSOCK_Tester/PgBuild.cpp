/*--------------------------------------------------------------------------
PgBuild.cpp

Build profile events sender dialog page.
Copyright (c) 2003 Keil Elektronik GmbH and Keil Software, Inc.
All rights reserved.
--------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DlgApp.h"
#include "PgBuild.h"
#include "MainDlg.h"
#include "OptDlg.h"


//#include "dtc_Make.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PgBuild dialog


PgBuild::PgBuild(CWnd* pParent /*=NULL*/)
	: CDialog(PgBuild::IDD, pParent)
{
	//{{AFX_DATA_INIT(PgBuild)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void PgBuild::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PgBuild)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PgBuild, CDialog)
	//{{AFX_MSG_MAP(PgBuild)
	ON_BN_CLICKED(IDC_SENDBUILD, OnSendbuild)
	ON_BN_CLICKED(IDC_SENDREBUILD, OnSendrebuild)
	ON_BN_CLICKED(IDC_SENDSTOPBUILD, OnSendstopbuild)
	ON_BN_CLICKED(IDC_SETFILEGROUP, OnSetfilegroup)
	ON_BN_CLICKED(IDC_SETOPTIONS, OnSetoptions)
	ON_BN_CLICKED(IDC_GETPRJFILE, OnGetprjfile)
	ON_BN_CLICKED(IDC_CLEANPRJ, OnCleanprj)
	ON_BN_CLICKED(IDC_ADDGRP, OnAddgrp)
	ON_BN_CLICKED(IDC_DELGROUP, OnDelgroup)
	ON_BN_CLICKED(IDC_ADDFILE, OnAddfile)
	ON_BN_CLICKED(IDC_DELFILE, OnDelfile)
  ON_BN_CLICKED(IDC_FLASHDOWNLOAD, OnFlashDownload)
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_PRJOPTIONS2, &PgBuild::OnBnClickedPrjoptions)
    ON_BN_CLICKED(IDC_ENUMFILES, &PgBuild::OnBnClickedEnumfiles)
    ON_BN_CLICKED(IDC_ENUMGROUPS, &PgBuild::OnBnClickedEnumgroups)
    ON_BN_CLICKED(IDC_PROGRESS, &PgBuild::OnBnClickedProgress)
    ON_BN_CLICKED(IDC_ACTFILES, &PgBuild::OnBnClickedActfiles)
    ON_BN_CLICKED(IDC_GETEXTVERSION, &PgBuild::OnBnClickedGetextversion)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PgBuild message handlers

void PgBuild::OnCancel()  {   // Catch ESC
//CDialog::OnCancel();
}

void PgBuild::OnOK()  {       // Catch Enter
//CDialog::OnOK();
}


#define _MAX_GROUPNAME_LENGTH  128    // max. length of GroupName


BOOL PgBuild::OnInitDialog()  {
//CDialog::OnInitDialog();

  ((CEdit *) GetDlgItem (IDC_EDGROUP))->LimitText (_MAX_GROUPNAME_LENGTH);

  return TRUE;
}



/*
 * Build current Project
 */
void PgBuild::OnSendbuild()  {
  CPwOptionsDlg *pM;

  pM = (CPwOptionsDlg *) AfxGetApp()->m_pMainWnd;
  pM->CreateCommandSend (UV_PRJ_BUILD, 0, NULL);
}


/*
 * ReBuild current Project
 */
void PgBuild::OnSendrebuild()  {
  CPwOptionsDlg *pM;

  pM = (CPwOptionsDlg *) AfxGetApp()->m_pMainWnd;
  pM->CreateCommandSend (UV_PRJ_REBUILD, 0, NULL);
}


/*
 * Clean current Project
 */
void PgBuild::OnCleanprj()  {
  CPwOptionsDlg *pM;

  pM = (CPwOptionsDlg *) AfxGetApp()->m_pMainWnd;
  pM->CreateCommandSend (UV_PRJ_CLEAN, 0, NULL);
}


/*
 * Stop build/rebuild
 */
void PgBuild::OnSendstopbuild()  {
  CPwOptionsDlg *pM;

  pM = (CPwOptionsDlg *) AfxGetApp()->m_pMainWnd;
  pM->CreateCommandSend (UV_PRJ_BUILD_CANCEL, 0, NULL);
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
void PgBuild::OnAddgrp()  {
  CPwOptionsDlg *pM;
  int             n;
  unsigned char  pj[512];
  PRJDATA       *pJ;
  CEdit         *pE;

  pE = (CEdit *) GetDlgItem (IDC_EDGROUP);
  pJ = (PRJDATA *) &pj[0];         // create PRJDATA in temporary buffer
  n  = GetItemName (pE, &pJ->szNames[0]);
  if (n == 0)  {
    MessageBeep (MB_OK);
    MessageBox ("Invalid Group name !");
    pE->SetFocus();
    return;
  }

  pJ->nLen  = 0;
  pJ->nCode = 0;
// sizeof(group_name) + 1(zero) + (sizeof(PRJDATA) - sizeof(pJ->szNames)):
  n += 1 + (sizeof (PRJDATA) - sizeof (pJ->szNames));   // account for nRes, nCode

  pM = (CPwOptionsDlg *) AfxGetApp()->m_pMainWnd;
  pM->CreateCommandSend (UV_PRJ_ADD_GROUP, n, pJ);
}


/*
 * Remove a Group from current Project
 */ 
void PgBuild::OnDelgroup()  {
  CPwOptionsDlg *pM;
  int             n;
  unsigned char  pj[512];
  PRJDATA       *pJ;
  CEdit         *pE;

  pE = (CEdit *) GetDlgItem (IDC_EDGROUP);
  pJ = (PRJDATA *) &pj[0];         // create PRJDATA in temporary buffer
  n  = GetItemName (pE, &pJ->szNames[0]);
  if (n == 0)  {
    MessageBeep (MB_OK);
    MessageBox ("Invalid Group name !");
    pE->SetFocus();
    return;
  }

  pJ->nLen  = 0;
  pJ->nCode = 0;
// sizeof(group_name) + 1(zero) + (sizeof(PRJDATA) - sizeof(pJ->szNames)):
  n += 1 + (sizeof (PRJDATA) - sizeof (pJ->szNames));   // account for nRes, nCode
  pM = (CPwOptionsDlg *) AfxGetApp()->m_pMainWnd;
  pM->CreateCommandSend (UV_PRJ_DEL_GROUP, n, pJ);
}


/*
 * Add File(s) to specified Group
 *   pJ->szNames[]:     'Group Name',0
 *                      'File Name 1', 0,      // full-path file name
 *                    [ 'File Name 2', 0, ]    // full-path file name
 *                    [  ...         , 0, ]    //
 *                       0                     // end of list marker
 */
void PgBuild::OnAddfile()  {
  CPwOptionsDlg *pM;
  int          j, n;
  unsigned char  pj[512];
  PRJDATA       *pJ;
  CEdit         *pG, *pE;

  pE = (CEdit *) GetDlgItem (IDC_EDGROUP);
  pJ = (PRJDATA *) &pj[0];                     // create PRJDATA in temporary buffer
  n  = GetItemName (pE, &pJ->szNames[0]);      // get current group name
  if (n == 0)  {
    MessageBeep (MB_OK);
    MessageBox ("Invalid Group name !");
    pE->SetFocus();
    return;
  }
  n  = n + 1;                                  // pos: after groupname-terminator

  pG = (CEdit *) GetDlgItem (IDC_EDFILENAME);
  j  = GetItemName (pG, &pJ->szNames[n]);      // get current file name
  if (j == 0)  {
    MessageBeep (MB_OK);
    MessageBox ("Invalid File name !");
    pG->SetFocus();
    return;
  }
  pJ->szNames [n + j + 1] = 0;                 // one more zero at end of items

  n = n + j + 2;         // size: length(groupname) + 1(z) + length(filename) + 1(z) + 1(z)
  
  pJ->nLen  = 0;
  pJ->nCode = 0;
  n  = n + (sizeof (PRJDATA) - sizeof (pJ->szNames));   // account for nRes, nCode
  pM = (CPwOptionsDlg *) AfxGetApp()->m_pMainWnd;
  pM->CreateCommandSend (UV_PRJ_ADD_FILE, n, pJ);
}


/*
 * Remove File(s) from specified Group
 *   pJ->szNames[]:     'Group Name',0
 *                      'File Name 1', 0,      // full-path file name
 *                    [ 'File Name 2', 0, ]    // full-path file name
 *                    [  ...         , 0, ]    //
 *                       0                     // end of list marker
 */
void PgBuild::OnDelfile()  {
  CPwOptionsDlg *pM;
  int          j, n;
  unsigned char  pj[512];
  PRJDATA       *pJ;
  CEdit         *pG, *pE;

  pE = (CEdit *) GetDlgItem (IDC_EDGROUP);
  pJ = (PRJDATA *) &pj[0];                     // create PRJDATA in temporary buffer
  n  = GetItemName (pE, &pJ->szNames[0]);      // get current group name
  if (n == 0)  {
    MessageBeep (MB_OK);
    MessageBox ("Invalid Group name !");
    pE->SetFocus();
    return;
  }
  n  = n + 1;                                  // pos: after groupname-terminator

  pG = (CEdit *) GetDlgItem (IDC_EDFILENAME);
  j  = GetItemName (pG, &pJ->szNames[n]);      // get current file name
  if (j == 0)  {
    MessageBeep (MB_OK);
    MessageBox ("Invalid File name !");
    pG->SetFocus();
    return;
  }
  pJ->szNames [n + j + 1] = 0;                 // one more zero at end of items

  n = n + j + 2;         // size: length(groupname) + 1(z) + length(filename) + 1(z) + 1(z)
  
  pJ->nLen  = 0;
  pJ->nCode = 0;
  n  = n + (sizeof (PRJDATA) - sizeof (pJ->szNames));   // account for nRes, nCode
  pM = (CPwOptionsDlg *) AfxGetApp()->m_pMainWnd;
  pM->CreateCommandSend (UV_PRJ_DEL_FILE, n, pJ);
}

void PgBuild::OnFlashDownload() {
  CPwOptionsDlg *pM;

  pM = (CPwOptionsDlg *) AfxGetApp()->m_pMainWnd;
  pM->CreateCommandSend (UV_PRJ_FLASH_DOWNLOAD, 0, NULL);
}



void PgBuild::OutLine (char *fmt, ...)  {
  char  szB [1028];
  va_list    argp;
  int           n;
  CEdit       *pE;

  pE = (CEdit *) GetDlgItem (IDC_ITEXT);
  if (pE == NULL) return;
  if (fmt == NULL)  {            // used to clear edit control
    pE->SetSel (0, -1);
    pE->Clear();
    return;
  }

#if 0
  switch ((WORD16) (BYTE) fmt[0])  {
    case 0xFF:
      pE->SetRedraw (FALSE);
      return;
    case 0xFE:
      pE->SetRedraw (TRUE);
      return;
  }
#endif

  va_start (argp, fmt);
  if (argp == NULL)  {         //  || argp[0] == 0)  {
    n = min (strlen (fmt), sizeof (szB) - 4);
    memcpy (szB, fmt, n);
  }
  else  {
    n = _vsnprintf (szB, sizeof (szB) - 4, fmt, argp);
  }
  szB [n] = 0;

  if (n > 0 && szB [n - 1] == '\n') --n;
  if (n > 0 && szB [n - 1] == '\r') --n;
  szB [n++] = '\r';
  szB [n++] = '\n';
  szB [n]   = 0;

  pE->SetSel (-1, -1);        // EOF
  pE->ReplaceSel (szB);
}



void PgBuild::OnSetfilegroup()  {    // SetFileGroup-Event
#if 0
  OutLine ("Preparing SetFileGroup:");
  OutLine ("    Group: %s", "'Generated Files'");
  OutLine ("    Files: this.c, that.c, foo.c, bar.c");
  OutLine ("in Folder: %s", "D:\\Src32\\PrjFiles\\");
  OutLine ("==========================================");

  dtc_Make::SetFileGroup::FileNames file_names;
  file_names.push_back ("this.c");
  file_names.push_back ("that.c");
  file_names.push_back ("foo.c");
  file_names.push_back ("bar.c");
  dtc_Make::SetFileGroup sfg("Generated Files", "D:\\Src32\\PrjFiles\\", file_names);
  sfg.Send();
#endif
}


void PgBuild::OnSetoptions()  {     // SetOptions Event
#if 0
  OutLine ("Preparing SetOptions:");
  OutLine ("    Group: %s", "'Generated Files'");
  OutLine ("  Defines: __DEBUG__, __LOCAL_VARS__, __REENTRANT__");
  OutLine ("  IncDirs: D:\\Src\\Include\\Inc1\\, D:\\Src\\Include\\Inc2\\, D:\\Src\\Include\\");
  OutLine ("==========================================");

  dtc_Make::SetOptions::Defines     defs;       // defines
  dtc_Make::SetOptions::IncludeDirs incs;       // include directories

  defs.push_back ("__DEBUG__");
  defs.push_back ("__LOCAL_VARS__");
  defs.push_back ("__REENTRANT__");

  incs.push_back ("D:\\Src\\Include\\Inc1\\");
  incs.push_back ("D:\\Src\\Include\\Inc2\\");
  incs.push_back ("D:\\Src\\Include\\");

  dtc_Make::SetOptions xB (defs, incs, "Generated Files");
  xB.Send();
#endif
}



/*
 *  dtc_Make - GetProjectFile (Request) / ProjectFile (Reply)
 *
 *  get info about a file of the current Uv2(Server) project
 *
 *  GetProjectFile Event - Input:
 *
 *    a) a file name, such as Measure.C (no path allowed, just afile name)
 *
 *    b) a sequence number such as (0) to get info about the first file
 *       or (5) to get info about the six'th file. The sequence number is
 *       a brace enclosed decimal number.  If the (n'th) file or the specified
 *       file does not exist in the project, then the reply will be
 *
 *       dtc_Make::SetStatus sts (dtc_Make::SetStatus::error,
 *                                "Given file is not in project");
 *
 *       With the sequence number, the client can enumerate from (0) to (n-1)
 *       to get the file info for all project files until a SetStatus Error
 *       occurs.
 */

void PgBuild::OnGetprjfile()  {
#if 0
  char   szF [512];
  int      n, i;

// read file name or sequence number out of the edit control:
  n = GetDlgItemText (IDC_FILESEQ, szF, sizeof (szF) - 1);
  szF [n] = 0;
  for ( i = 0 ; i < n ; ++i )  {
    if (szF[i] != ' ') break;          // skip white spaces.
  }
  if (szF[i] == 0)  {                  // empty input
    AfxMessageBox ("Enter File name or Sequence number",
                    MB_APPLMODAL | MB_ICONEXCLAMATION);
    GetDlgItem (IDC_FILESEQ)->SetFocus();
    return;
  }

  dtc_Make::GetProjectFile xE (&szF[i]);
  xE.Send();
#endif
}


void PgBuild::OnBnClickedPrjoptions()  {
  COptDlg  dlg;

  dlg.DoModal();
}


/*
 * Enumerate files of specified group
 */
void PgBuild::OnBnClickedEnumfiles()  {
  CPwOptionsDlg *pM;
  int             n;
  unsigned char  pj[512];
  SSTR          *pJ;
  CEdit         *pE;

  pE = (CEdit *) GetDlgItem (IDC_EDGROUP);
  pJ = (SSTR *) &pj[0];           // create SSTR in temporary buffer
  n  = GetItemName (pE, &pJ->szStr[0]);
  if (n == 0)  {
    MessageBeep (MB_OK);
    MessageBox ("Invalid Group name !");
    pE->SetFocus();
    return;
  }

  pJ->nLen  = n;
//pJ->nCode = 0;
  n += 1 + sizeof (int);         // account for nLen, 0-terminator

  pM = (CPwOptionsDlg *) AfxGetApp()->m_pMainWnd;
  pM->CreateCommandSend (UV_PRJ_ENUM_FILES, n, pJ);
}


/*
 * Enumerate Groups
 */
void PgBuild::OnBnClickedEnumgroups()  {
  CPwOptionsDlg *pM;

  pM = (CPwOptionsDlg *) AfxGetApp()->m_pMainWnd;
  pM->CreateCommandSend (UV_PRJ_ENUM_GROUPS, 0, NULL);
}


/*
 * Test Progress-Bar commands
 */
void PgBuild::OnBnClickedProgress()  {
  unsigned char  uB[256];
  PGRESS   *pG;
  int        i, n;

  CPwOptionsDlg *pM = (CPwOptionsDlg *) AfxGetApp()->m_pMainWnd;

  memset (&uB[0], 0, sizeof (uB));
  pG = (PGRESS *) (void *) &uB[0];

//---Init bar in percent mode:
  pG->job = UV_PROGRESS_INIT;
  n = sprintf (&pG->szLabel[0], "Socket Progressbar test in percent mode: ");
  n = n + 1 + offsetof (PGRESS, szLabel);  // + zero terminator
  pM->CreateCommandSend (UV_PRJ_CMD_PROGRESS, n, pG);

//---loop:
  memset (pG, 0, sizeof (*pG));
  for ( i = 0 ; i <= 100 ; i += 2 )  {
    pG->job  = UV_PROGRESS_SETPOS;
    pG->perc = i;
    n = sizeof (PGRESS);
    pM->CreateCommandSend (UV_PRJ_CMD_PROGRESS, n, pG);
    Sleep (50);
  }
//---close bar:
  memset (pG, 0, sizeof (*pG));
  pG->job = UV_PROGRESS_CLOSE;
  n = sizeof (PGRESS);
  pM->CreateCommandSend (UV_PRJ_CMD_PROGRESS, n, pG);
//---------------------------------------------------------------

//---init bar in text mode:
  pG->job = UV_PROGRESS_INITTXT;        // init bar for text mode
  n = sprintf (&pG->szLabel[0], "Socket Progressbar test in text mode, Erasing: ");
  n = n + 1 + offsetof (PGRESS, szLabel);  // + zero terminator
  pM->CreateCommandSend (UV_PRJ_CMD_PROGRESS, n, pG);
//---loop:
  memset (pG, 0, sizeof (*pG));
  for ( i = 0 ; i <= 100 ; i += 2 )  {
    pG->job  = UV_PROGRESS_SETTEXT;     // set bar text
    pG->perc = i;
    n  = sprintf (pG->szLabel, "Erasing 0x%08X", i * 0x1000);
    n += 1 + offsetof (PGRESS, szLabel);
    pM->CreateCommandSend (UV_PRJ_CMD_PROGRESS, n, pG);
    Sleep (50);
  }
//---close bar:
  memset (pG, 0, sizeof (*pG));
  pG->job = UV_PROGRESS_CLOSE;
  n = sizeof (PGRESS);
  pM->CreateCommandSend (UV_PRJ_CMD_PROGRESS, n, pG);
//---------------------------------------------------------------
}


/*
 * Count active files of current target
 */
void PgBuild::OnBnClickedActfiles()  {
  CPwOptionsDlg *pM = (CPwOptionsDlg *) AfxGetApp()->m_pMainWnd;
  pM->CreateCommandSend (UV_PRJ_ACTIVE_FILES, 0, NULL);
}


/*
 * Get extended Version information
 */
void PgBuild::OnBnClickedGetextversion()  {
  CPwOptionsDlg *pM = (CPwOptionsDlg *) AfxGetApp()->m_pMainWnd;
  pM->CreateCommandSend (UV_GEN_GET_EXTVERSION, 0, NULL);
}
