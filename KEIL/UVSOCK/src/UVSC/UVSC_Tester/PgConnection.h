/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2004-2016 KEIL - An ARM Company. All rights reserved.        */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CConnectionDlg dialog

class CConnectionDlg : public CDialog
{
// Construction
public:
	CConnectionDlg(CWnd* pParent = NULL);   // standard constructor
  ~CConnectionDlg();

// Dialog Data
	//{{AFX_DATA(CConnectionDlg)
	enum { IDD = IDD_CONNECTIONTAB };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

private:
  CButton m_CheckConnAuto;
  int m_IntConnAuto;
  CEdit m_EditConnCmd;
  CString m_StringConnCmd;
  CButton m_ButtonCmdBrowse;
  CComboBox m_ComboConnMode;
  int m_IntConnMode;
  CEdit m_EditConnPort;
  int m_IntConnPort;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnectionDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
  static void CALLBACK ConnectTimerCB(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
  void Connect();
  void ReConnect();
  void Connected();

private:
  void UpdateRegistyVals();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConnectionDlg)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
  afx_msg void OnConnAutoClicked();
  afx_msg void OnConnModeClicked();
  afx_msg void OnTest1();
  afx_msg void OnTest2();
  afx_msg void OnTest3();
  afx_msg void OnTest4();
  afx_msg void OnTest5();
  afx_msg void OnTest6();
  afx_msg void OnTest7();
  afx_msg void OnTest8();
  afx_msg void OnTest9();
  afx_msg void OnTest10();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

