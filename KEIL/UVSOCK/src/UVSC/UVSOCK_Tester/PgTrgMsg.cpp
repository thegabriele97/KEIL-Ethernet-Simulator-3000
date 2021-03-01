/*--------------------------------------------------------------------------
PgTrgMsg.cpp

Target messaging profile events sender dialog page.
Copyright (c) 2003 Keil Elektronik GmbH and Keil Software, Inc.
All rights reserved.
--------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DlgApp.h"
#include "MainDlg.h"

#include "PgTrgMsg.h"

//#include "dtc_TargetMessaging.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CPwOptionsDlg   *_dlg;

/////////////////////////////////////////////////////////////////////////////
// PgTrgMsg dialog


PgTrgMsg::PgTrgMsg(CWnd* pParent /*=NULL*/)
	: CDialog(PgTrgMsg::IDD, pParent)
{
	//{{AFX_DATA_INIT(PgTrgMsg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void PgTrgMsg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PgTrgMsg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PgTrgMsg, CDialog)
	//{{AFX_MSG_MAP(PgTrgMsg)
	ON_CBN_SELCHANGE(IDC_TMCHA, OnSelchangeTmcha)
	ON_BN_CLICKED(IDC_MSGTOTARG, OnMsgtotarg)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_BUTTON_HIDE, &PgTrgMsg::OnBnClickedButtonHide)
  ON_BN_CLICKED(IDC_BUTTON_SHOW, &PgTrgMsg::OnBnClickedButtonShow)
  ON_BN_CLICKED(IDC_BUTTON_MINIMIZE, &PgTrgMsg::OnBnClickedButtonMinimize)
  ON_BN_CLICKED(IDC_BUTTON_RESTORE, &PgTrgMsg::OnBnClickedButtonRestore)
  ON_BN_CLICKED(IDC_BUTTON_UILOCK, &PgTrgMsg::OnBnClickedButtonUilock)
  ON_BN_CLICKED(IDC_BUTTON_UIUNLOCK, &PgTrgMsg::OnBnClickedButtonUiunlock)
  ON_BN_CLICKED(IDC_BUTTON_EXIT_UVISION, &PgTrgMsg::OnBnClickedButtonExitUvision)
  ON_BN_CLICKED(IDC_BUTTON_MAXIMIZE, &PgTrgMsg::OnBnClickedButtonMaximize)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PgTrgMsg message handlers


void PgTrgMsg::OnCancel()  {   // Catch ESC
//CDialog::OnCancel();
}

void PgTrgMsg::OnOK()  {       // Catch Enter
//CDialog::OnOK();
}


BOOL PgTrgMsg::OnInitDialog()  {
 int       i;
  char   szB [32];

 CComboBox *pB = (CComboBox *) GetDlgItem (IDC_TMCHA);
 pB->ResetContent();
 for ( i = 0 ; i < 32 ; ++i )  {
   sprintf (szB, "  %d", i);
   pB->AddString (szB);
 }
 
 pB->SetCurSel (0);
//CDialog::OnInitDialog();
  return TRUE;
}


#if 0
dtc_TargetMessaging::Data          Msg;    // 12 Bytes
dtc_TargetMessaging::TimeStamp  tStamp = 0;    // TimeStamp
int Seq = 0;
#endif


void PgTrgMsg::OnSelchangeTmcha()  {
}

void PgTrgMsg::OnMsgtotarg()  {
#if 0
  int channel = ((CComboBox *) GetDlgItem (IDC_TMCHA))->GetCurSel();

  Msg._bytes[0]  = 'K';
  Msg._bytes[1]  = 'E';
  Msg._bytes[2]  = 'I';
  Msg._bytes[3]  = 'L';
  Msg._bytes[4]  = '_';
  Msg._bytes[5]  = 'T';
  Msg._bytes[6]  = 'O';
  Msg._bytes[7]  = 'T';
  Msg._bytes[8]  = 'A';
  Msg._bytes[9]  = 'R';
  Msg._bytes[10] = 'G';
  Msg._bytes[11] = '!';

  dtc_TargetMessaging::MsgToTarget xT (Msg, Seq, channel, tStamp);
  xT.Send();

  tStamp += 64;
  ++Seq;
#endif
}

void PgTrgMsg::OnBnClickedButtonHide()
{
  _dlg->CreateCommandSend (UV_GEN_HIDE, 0, NULL);
}

void PgTrgMsg::OnBnClickedButtonShow()
{
  _dlg->CreateCommandSend (UV_GEN_SHOW, 0, NULL);
}

void PgTrgMsg::OnBnClickedButtonMinimize()
{
  _dlg->CreateCommandSend (UV_GEN_MINIMIZE, 0, NULL);
}

void PgTrgMsg::OnBnClickedButtonRestore()
{
  _dlg->CreateCommandSend (UV_GEN_RESTORE, 0, NULL);
}

void PgTrgMsg::OnBnClickedButtonUilock()
{
  _dlg->CreateCommandSend (UV_GEN_UI_LOCK, 0, NULL);
}

void PgTrgMsg::OnBnClickedButtonUiunlock()
{
  _dlg->CreateCommandSend (UV_GEN_UI_UNLOCK, 0, NULL);
}

void PgTrgMsg::OnBnClickedButtonExitUvision()
{
  _dlg->CreateCommandSend (UV_GEN_EXIT, 0, NULL);
}

void PgTrgMsg::OnBnClickedButtonMaximize()
{
  _dlg->CreateCommandSend (UV_GEN_MAXIMIZE, 0, NULL);
}
