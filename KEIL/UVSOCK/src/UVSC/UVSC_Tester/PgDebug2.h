/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2004-2016 KEIL - An ARM Company. All rights reserved.        */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/



#pragma once

#include "UVSC_C.h"

/////////////////////////////////////////////////////////////////////////////
// CDebugDlg dialog

class CDebug2Dlg : public CDialog
{
// Construction
public:
	CDebug2Dlg(CWnd* pParent = NULL);   // standard constructor

  // Dialog Data
	//{{AFX_DATA(CDebugDlg)
	enum { IDD = IDD_DEBUGTAB2 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

public:

private:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDebugDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDebugDlg)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnGetmemadr();
	afx_msg void OnReadmem();
  afx_msg void OnWritemem();
	afx_msg void OnIsrunning();
	afx_msg void OnCycTstamp();
	afx_msg void OnGetvtr();
	afx_msg void OnSetvtr();
	afx_msg void OnSersendD();
	afx_msg void OnGetsero();
	//}}AFX_MSG
  afx_msg void OnBnClickedIdenum();
  afx_msg void OnBnClickedMaptofl();
  afx_msg void OnBnClickedEnumstack();
  afx_msg void OnBnClickedEnumvtr();
  afx_msg void OnBnClickedEnumvtrv();
  afx_msg void OnBnClickedShowcode();
  afx_msg void OnBnClickedPowerscaleShowcode();
  afx_msg void OnBnClickedButtonItmRegister();
  afx_msg void OnBnClickedButtonItmUnregister();
  afx_msg void OnBnClickedButtonEvtrRegister();
  afx_msg void OnBnClickedButtonEvtrUnregister();
  afx_msg void OnBnClickedButtonEvtrGetscvd();

  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

