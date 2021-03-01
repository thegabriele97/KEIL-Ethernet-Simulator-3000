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
// CRTADlg dialog


class CPerfDlg : public CDialog
{
// Construction
public:
	CPerfDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRTADlg)
	enum { IDD = IDD_PERFTAB };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

private:
  // Memory address evaluated by calc expression
  __int64 rwMemAddr;
  unsigned int rwDataSize;
  unsigned int rwDataCount; 
  bool rwIncrementAddress;


public:
  

private:
  


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRTADlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

private:
  bool EvalMemExpr();
  void SetLogging(bool bEnableLog);

protected:

	// Generated message map functions
	//{{AFX_MSG(CRTADlg)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
  afx_msg void OnReadTest();
	afx_msg void OnWriteTest();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

