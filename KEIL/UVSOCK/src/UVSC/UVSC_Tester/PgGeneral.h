/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2004-2016 KEIL - An ARM Company. All rights reserved.        */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/



#pragma once

/////////////////////////////////////////////////////////////////////////////
// CGeneralDlg dialog

class CGeneralDlg : public CDialog
{
// Construction
public:
	CGeneralDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGeneralDlg)
	enum { IDD = IDD_GENERALTAB };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

private:



// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGeneralDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGeneralDlg)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedButton3();
  afx_msg void OnBnClickedButtonHide();
  afx_msg void OnBnClickedButtonShow();
  afx_msg void OnBnClickedButtonMinimize();
  afx_msg void OnBnClickedButtonRestore();
  afx_msg void OnBnClickedButtonExitUvision();
  afx_msg void OnBnClickedButtonMaximize();
  afx_msg void OnBnClickedGetLocalVersion();
  afx_msg void OnBnClickedGetRemoteVersion();
  afx_msg void OnBnClickedGetExtVersions();
  afx_msg void OnBnClickedGetLicInfo();
  afx_msg void OnBnClickedGetLastError();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

