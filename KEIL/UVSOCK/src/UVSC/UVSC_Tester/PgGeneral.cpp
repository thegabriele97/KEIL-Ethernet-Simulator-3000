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

#include "PgGeneral.h"

#include "UVSCThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CUVSCTesterDlg   *_dlg;

/////////////////////////////////////////////////////////////////////////////
// CGeneralDlg dialog


CGeneralDlg::CGeneralDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGeneralDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGeneralDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGeneralDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGeneralDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGeneralDlg, CDialog)
	//{{AFX_MSG_MAP(CGeneralDlg)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_BUTTON_HIDE, &CGeneralDlg::OnBnClickedButtonHide)
  ON_BN_CLICKED(IDC_BUTTON_SHOW, &CGeneralDlg::OnBnClickedButtonShow)
  ON_BN_CLICKED(IDC_BUTTON_MINIMIZE, &CGeneralDlg::OnBnClickedButtonMinimize)
  ON_BN_CLICKED(IDC_BUTTON_RESTORE, &CGeneralDlg::OnBnClickedButtonRestore)
  ON_BN_CLICKED(IDC_BUTTON_EXIT_UVISION, &CGeneralDlg::OnBnClickedButtonExitUvision)
  ON_BN_CLICKED(IDC_BUTTON_MAXIMIZE, &CGeneralDlg::OnBnClickedButtonMaximize)
  ON_BN_CLICKED(IDC_GETVERSION, &CGeneralDlg::OnBnClickedGetLocalVersion)
  ON_BN_CLICKED(IDC_GETREMOTEVERSION, &CGeneralDlg::OnBnClickedGetRemoteVersion)
  ON_BN_CLICKED(IDC_GETEXTVERSION, &CGeneralDlg::OnBnClickedGetExtVersions)
  ON_BN_CLICKED(IDC_GETLICENSEINFO, &CGeneralDlg::OnBnClickedGetLicInfo)
  ON_BN_CLICKED(IDC_GETLASTERROR,  &CGeneralDlg::OnBnClickedGetLastError)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeneralDlg message handlers


void CGeneralDlg::OnCancel()  {   // Catch ESC
}

void CGeneralDlg::OnOK()  {       // Catch Enter
}


BOOL CGeneralDlg::OnInitDialog()  {
  CDialog::OnInitDialog();
  return TRUE;
}



void CGeneralDlg::OnBnClickedButtonHide() {
  ThreadMsg msg;

  msg.cmd = UV_GEN_HIDE;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}

void CGeneralDlg::OnBnClickedButtonShow() {
  ThreadMsg msg;

  msg.cmd = UV_GEN_SHOW;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}

void CGeneralDlg::OnBnClickedButtonMinimize() {
  ThreadMsg msg;

  msg.cmd = UV_GEN_MINIMIZE;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}

void CGeneralDlg::OnBnClickedButtonRestore() {
  ThreadMsg msg;

  msg.cmd = UV_GEN_RESTORE;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}

void CGeneralDlg::OnBnClickedButtonExitUvision() {
  ThreadMsg msg;

  msg.cmd = UV_GEN_EXIT;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);

  // Kickstart the connection timer
  _dlg->m_ControlTabs.GetConnection()->ReConnect();
}

void CGeneralDlg::OnBnClickedButtonMaximize() {
  ThreadMsg msg;

  msg.cmd = UV_GEN_MAXIMIZE;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}


void CGeneralDlg::OnBnClickedGetLocalVersion()  {
  ThreadMsg msg;

  msg.cmd = UV_NULL_CMD;
  msg.ctrlMsg.cmd = CONTROLMSG_VERSION;
  UVSCThread_Post(&msg, 0);
}

void CGeneralDlg::OnBnClickedGetRemoteVersion()  {
  ThreadMsg msg;

  msg.cmd = UV_GEN_GET_VERSION;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}


void CGeneralDlg::OnBnClickedGetExtVersions()  {
  ThreadMsg msg;

  msg.cmd = UV_GEN_GET_EXTVERSION;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}

void CGeneralDlg::OnBnClickedGetLicInfo() {
  ThreadMsg msg;

  msg.cmd = UV_GEN_CHECK_LICENSE;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}

void CGeneralDlg::OnBnClickedGetLastError() {
  ThreadMsg msg;

  msg.cmd = UV_NULL_CMD;
  msg.ctrlMsg.cmd = CONTROLMSG_GETLASTERROR;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}



