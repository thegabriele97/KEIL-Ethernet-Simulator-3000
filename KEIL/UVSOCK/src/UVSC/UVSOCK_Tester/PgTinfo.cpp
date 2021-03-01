// PgTinfo.cpp : implementation file
//

#include "stdafx.h"
#include "DlgApp.h"
#include "MainDlg.h"
#include "PgTinfo.h"


//#include "Dtc_TargetInfo.h"

extern CPwOptionsDlg   *_dlg; // user for Text 'OutLine()'

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTargInfo dialog


CTargInfo::CTargInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CTargInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTargInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTargInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTargInfo)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTargInfo, CDialog)
	//{{AFX_MSG_MAP(CTargInfo)
	ON_BN_CLICKED(IDC_GETTARGINFO, OnGettarginfo)
  ON_BN_CLICKED(IDC_BUTTON_TEST1, OnTest1)
  ON_BN_CLICKED(IDC_BUTTON_TEST2, OnTest2)
  ON_BN_CLICKED(IDC_BUTTON_TEST3, OnTest3)
  ON_BN_CLICKED(IDC_BUTTON_TEST4, OnTest4)
  ON_BN_CLICKED(IDC_BUTTON_TEST5, OnTest5)
  ON_BN_CLICKED(IDC_BUTTON_TEST6, OnTest6)
  ON_BN_CLICKED(IDC_BUTTON_TEST7, OnTest7)
  ON_BN_CLICKED(IDC_BUTTON_TEST8, OnTest8)
  ON_BN_CLICKED(IDC_BUTTON_TEST9, OnTest9)
  ON_BN_CLICKED(IDC_BUTTON_TEST10, OnTest10)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTargInfo message handlers

void CTargInfo::OnCancel()  {   // Catch ESC
//CDialog::OnCancel();
}

void CTargInfo::OnOK()  {       // Catch Enter
//CDialog::OnOK();
}

BOOL CTargInfo::OnInitDialog()  {
//CDialog::OnInitDialog();
  return TRUE;
}



/*
 * Request Target Info (dtc_TargetInfo::GetTargetInfo)
 */

void CTargInfo::OnGettarginfo()  {
#if 0
  dtc_TargetInfo::GetTargetInfo ev;
  ev.Send();
#endif
}

void CTargInfo::OnTest1() {
  _dlg->CreateCommandSend (UV_TST_1, 0, NULL);
}
void CTargInfo::OnTest2() {
  _dlg->CreateCommandSend (UV_TST_2, 0, NULL);
}
void CTargInfo::OnTest3() {
  _dlg->CreateCommandSend (UV_TST_3, 0, NULL);
}
void CTargInfo::OnTest4() {
  _dlg->CreateCommandSend (UV_TST_4, 0, NULL);
}
void CTargInfo::OnTest5() {
  _dlg->CreateCommandSend (UV_TST_5, 0, NULL);
}
void CTargInfo::OnTest6() {
  _dlg->CreateCommandSend (UV_TST_6, 0, NULL);
}
void CTargInfo::OnTest7() {
  _dlg->CreateCommandSend (UV_TST_7, 0, NULL);
}
void CTargInfo::OnTest8() {
  _dlg->CreateCommandSend (UV_TST_8, 0, NULL);
}
void CTargInfo::OnTest9() {
  _dlg->CreateCommandSend (UV_TST_9, 0, NULL);
}
void CTargInfo::OnTest10() {
  _dlg->CreateCommandSend (UV_TST_10, 0, NULL);
}
