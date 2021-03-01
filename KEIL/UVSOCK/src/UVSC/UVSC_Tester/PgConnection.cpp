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
#include "PgConnection.h"
#include "UVSC_C.h"
#include "UVSCThread.h"

extern CUVSCTesterDlg   *_dlg; // user for Text 'OutLine()'
extern void uvsc_log_callback(const char* msg, int msgLen);
extern void uvsc_callback(void* cb_custom, UVSC_CB_TYPE type, UVSC_CB_DATA *data);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define DEFAULT_APPCMD          "C:\\KEIL\\UV3\\UV3.exe"

static CString strConnection  = "Connection";
static CString strAuto        = "Auto";
static CString strAppCmd      = "AppCmd";
static CString strPort        = "Port";
static CString strMode        = "Mode";

/////////////////////////////////////////////////////////////////////////////
// CConnectionDlg dialog


CConnectionDlg::CConnectionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConnectionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConnectionDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

  m_IntConnAuto = 0;
  m_IntConnMode = 0;
  m_IntConnPort = 0;

}

CConnectionDlg::~CConnectionDlg() {  
  UpdateRegistyVals();
}

void CConnectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectionDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

  DDX_Control(pDX, IDC_CHECK_CONNAUTO, m_CheckConnAuto);
  DDX_Check(pDX, IDC_CHECK_CONNAUTO, m_IntConnAuto);
  DDX_Control(pDX, IDC_EDIT_CONNCMD, m_EditConnCmd);
  DDX_Text(pDX, IDC_EDIT_CONNCMD, m_StringConnCmd);
  DDX_Control(pDX, IDC_BUTTON_CONNCMDBROWSE, m_ButtonCmdBrowse);
  DDX_Control(pDX, IDC_COMBO_CONNMODE, m_ComboConnMode);
  DDX_CBIndex(pDX, IDC_COMBO_CONNMODE, m_IntConnMode);
  DDX_Control(pDX, IDC_EDIT_CONNPORT, m_EditConnPort);
  DDX_Text(pDX, IDC_EDIT_CONNPORT, m_IntConnPort);
}


BEGIN_MESSAGE_MAP(CConnectionDlg, CDialog)
	//{{AFX_MSG_MAP(CConnectionDlg)
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
  ON_BN_CLICKED(IDC_CHECK_CONNAUTO, OnConnAutoClicked)
  ON_CBN_SELCHANGE(IDC_COMBO_CONNMODE, OnConnModeClicked)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectionDlg message handlers

void CConnectionDlg::OnCancel()  {   // Catch ESC

}

void CConnectionDlg::OnOK()  {       // Catch Enter

}

void CConnectionDlg::UpdateRegistyVals() {
  CWinApp* pApp = AfxGetApp();

  pApp->WriteProfileInt(strConnection, strAuto, m_IntConnAuto);
  pApp->WriteProfileString(strConnection, strAppCmd, m_StringConnCmd);
  pApp->WriteProfileInt(strConnection, strMode, m_IntConnMode);
  pApp->WriteProfileInt(strConnection, strPort, m_IntConnPort);
}

BOOL CConnectionDlg::OnInitDialog()  {
  CDialog::OnInitDialog();

  ((CEdit *) GetDlgItem (IDC_EDIT_CONNPORT))->LimitText (5);

  // Add the mode items to the ConnMode combo box
  m_ComboConnMode.InsertString(0, "NORMAL");
  m_ComboConnMode.InsertString(1, "LABVIEW");

  // Read the current connection settings from the registry, defaulting if none exist
  CWinApp* pApp = AfxGetApp();

  m_IntConnAuto = pApp->GetProfileInt(strConnection, strAuto, 1);
  m_StringConnCmd = pApp->GetProfileString(strConnection, strAppCmd, DEFAULT_APPCMD);
  m_IntConnMode = pApp->GetProfileInt(strConnection, strMode, 0);
  m_IntConnPort = pApp->GetProfileInt(strConnection, strPort, 4823);

  UpdateData(FALSE);

  OnConnAutoClicked();

  return TRUE;
}


#define CONNECT_CB_TIMER_ID    3
#define CONNECT_CB_TIMEOUT     1000

void CALLBACK CConnectionDlg::ConnectTimerCB(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
  CConnectionDlg * pThis = (CConnectionDlg *)CConnectionDlg::FromHandle(hwnd);
  ::KillTimer(hwnd, idEvent);

  pThis->Connect();

  // Start the connection timer again...
  //pThis->SetTimer(CONNECT_CB_TIMER_ID, CONNECT_CB_TIMEOUT, ConnectTimerCB); 
}

void CConnectionDlg::Connected() {
  KillTimer(CONNECT_CB_TIMER_ID);
}

void CConnectionDlg::Connect() {
  bool retVal = false;
  int port;
  char *uvCmd;
  UVSC_RUNMODE mode;


  if (m_IntConnAuto) {
    port = 0;
    uvCmd = (char *)(LPCSTR)m_StringConnCmd;
    mode = (UVSC_RUNMODE)m_IntConnMode;
  }
  else {
    port = m_IntConnPort;
    uvCmd = NULL;
    mode = UVSC_RUNMODE_NORMAL;
  }

  // Send thread connect message...
  ThreadMsg msg;

  msg.cmd = UV_NULL_CMD;
  msg.ctrlMsg.cmd = CONTROLMSG_CONNECT;
  msg.ctrlMsg.connectData.port = port;
  msg.ctrlMsg.connectData.uvRunmode = mode;
  if (uvCmd) {
    strcpy (msg.ctrlMsg.connectData.uvCmd, uvCmd);
  }
  else {
    msg.ctrlMsg.connectData.uvCmd[0] = 0;
  }

  UVSCThread_Post(&msg, 0);
}

void CConnectionDlg::ReConnect() {
  ThreadMsg msg;

  // Disconnect any current connection!
  msg.cmd = UV_NULL_CMD;
  msg.connHandle = _dlg->UVSCConnHandle;
  msg.ctrlMsg.cmd = CONTROLMSG_DISCONNECT;
  msg.ctrlMsg.disconnectData.terminate = false;
  UVSCThread_Post(&msg, 0);

  // Start the connection timer...
  CWnd::SetTimer(CONNECT_CB_TIMER_ID, CONNECT_CB_TIMEOUT, ConnectTimerCB);
}

void CConnectionDlg::OnConnAutoClicked() {
  if (m_CheckConnAuto.GetCheck() == BST_CHECKED) {
    m_EditConnCmd.SetReadOnly(FALSE);
    m_ButtonCmdBrowse.EnableWindow(TRUE);
    m_ComboConnMode.EnableWindow(TRUE);
    m_EditConnPort.SetReadOnly(TRUE);
  }
  else {
    m_EditConnCmd.SetReadOnly(TRUE);
    m_ButtonCmdBrowse.EnableWindow(FALSE);
    m_ComboConnMode.EnableWindow(FALSE);
    m_EditConnPort.SetReadOnly(FALSE);
  }
  UpdateData(TRUE);

  ReConnect();
}

void CConnectionDlg::OnConnModeClicked() {
  UpdateData(TRUE);
  ReConnect();
}


void CConnectionDlg::OnTest1() {
//  _dlg->CreateCommandSend (UV_TST_1, 0, NULL);
}
void CConnectionDlg::OnTest2() {
//  _dlg->CreateCommandSend (UV_TST_2, 0, NULL);
}
void CConnectionDlg::OnTest3() {
//  _dlg->CreateCommandSend (UV_TST_3, 0, NULL);
}
void CConnectionDlg::OnTest4() {
//  _dlg->CreateCommandSend (UV_TST_4, 0, NULL);
}
void CConnectionDlg::OnTest5() {
//  _dlg->CreateCommandSend (UV_TST_5, 0, NULL);
}
void CConnectionDlg::OnTest6() {
//  _dlg->CreateCommandSend (UV_TST_6, 0, NULL);
}
void CConnectionDlg::OnTest7() {
//  _dlg->CreateCommandSend (UV_TST_7, 0, NULL);
}
void CConnectionDlg::OnTest8() {
//  _dlg->CreateCommandSend (UV_TST_8, 0, NULL);
}
void CConnectionDlg::OnTest9() {
//  _dlg->CreateCommandSend (UV_TST_9, 0, NULL);
}
void CConnectionDlg::OnTest10() {
//  _dlg->CreateCommandSend (UV_TST_10, 0, NULL);
}
