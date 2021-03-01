/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2004-2016 KEIL - An ARM Company. All rights reserved.        */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#pragma once

#include "AdvListBox.h"

/////////////////////////////////////////////////////////////////////////////
// CRTADlg dialog

typedef enum {
  RTA_OUT_TYPE_GEN = 0,
} RTA_OUT_TYPE;

class CRTADlg : public CDialog
{
// Construction
public:
	CRTADlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRTADlg)
	enum { IDD = IDD_RTATAB };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


public:
  void OutLine (RTA_OUT_TYPE type, char *fmt, ...);

private:
  CCaptainSLOGBox	m_RTALogOut;       // Snazzy Output Listbox


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRTADlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRTADlg)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
  afx_msg void OnClearLog();

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

