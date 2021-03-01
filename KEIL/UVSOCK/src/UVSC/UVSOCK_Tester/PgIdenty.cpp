
PgIdentify.cpp

Identify profile events sender dialog page.
Copyright (c) 2003 Keil Elektronik GmbH and Keil Software, Inc.
All rights reserved.
--------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DlgApp.h"
#include "PgIdenty.h"
#include "MainDlg.h"

//#include "dtc_Identify.h"

extern CPwOptionsDlg   *_dlg;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PgIdenty dialog


PgIdenty::PgIdenty(CWnd* pParent /*=NULL*/)
	: CDialog(PgIdenty::IDD, pParent)
{
	//{{AFX_DATA_INIT(PgIdenty)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void PgIdenty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PgIdenty)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PgIdenty, CDialog)
	//{{AFX_MSG_MAP(PgIdenty)
	ON_BN_CLICKED(IDC_SENDWHOIS, OnSendwhois)
	ON_BN_CLICKED(IDC_WHICHDOC, OnWhichdoc)
  ON_BN_CLICKED(IDC_BUTTON_USIM, OnUseSim)
  ON_BN_CLICKED(IDC_BUTTON_UTRG, OnUseTrg)
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_CLOSEPRJ, &PgIdenty::OnBnClickedCloseprj)
    ON_BN_CLICKED(IDC_PRJREAD, &PgIdenty::OnBnClickedLoadprj)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PgIdenty message handlers

void PgIdenty::OnCancel()  {       // Catch ESC
//CDialog::OnCancel();
}

void PgIdenty::OnOK()  {       // Catch Enter
//CDialog::OnOK();
}



BOOL PgIdenty::OnInitDialog()  {
//CDialog::OnInitDialog();
  return TRUE;
}



void PgIdenty::OnUseSim() {
  MessageBeep (MB_OK);
//_dlg->CreateCommandSend (UV_PRJ_SET_TARGET_SIM, 0, NULL);
}

void PgIdenty::OnUseTrg() {
  MessageBeep (MB_OK);
//_dlg->CreateCommandSend (UV_PRJ_SET_TARGET_HW, 0, NULL);
}

/*
 * Send a 'WhoIs' Event
 */

void PgIdenty::OnSendwhois()  {
#if 0
  ClearClients();

#if 0
  dtc_Identify::IAm::Profiles profiles;
//---create the list of supported Profiles:
  profiles.push_back ("dtc_Identify");
  profiles.push_back ("dtc_Make");
  profiles.push_back ("dtc_Debug");
  profiles.push_back ("dtc_TargetInfo");
  profiles.push_back ("dtc_TargetMessaging");

//CString title = "µVision2";
  dtc_Identify::IAm i_am (dtc_EventManager::GetApplID(), (LPCTSTR) "DTCT-Tester", profiles);
  bool ok = i_am.Send();
  if (!ok)  {
    AfxMessageBox ("Could not send IAm Event (no Server ???)");
  }
#endif

  dtc_Identify::WhoIs xB;  // (dtc_EventManager::GetApplID());
  xB.Send ();
#endif
}


/*
 * Query current Project and BaseFolder
 */
void PgIdenty::OnWhichdoc()  {
#if 0
  dtc_Identify::WhichProject xB;
  xB.Send();
#endif
}



void PgIdenty::OnBnClickedCloseprj()  {
  _dlg->CreateCommandSend (UV_PRJ_CLOSE, 0, NULL);
}



static const char SfT[] = "Select Project File";
static const char SfM[] = "Project Files (*.uv2 *.mpw)|*.uv2;*.mpw|"  \
                          "Multi Project Workspaces (*.mpw)|*.mpw||";
//                        "Dave Project Files (*.dpt)|*.dpt||";

static char     lastPrj [MAX_PATH+10];

void PgIdenty::OnBnClickedLoadprj()  {
  CString      cs;
  LPCTSTR      lp;
  PRJDATA     *pD;
  char        szB[1024];
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

    pD = (PRJDATA *) &szB[0];
    pD->nCode = 0;
    pD->nLen  = strlen (lp) + 1;
    memcpy (pD->szNames, lp, pD->nLen); // including zero-terminator
    n  = offsetof (PRJDATA,szNames) + pD->nLen;
    _dlg->CreateCommandSend (UV_PRJ_LOAD, n, pD);
  }
}


