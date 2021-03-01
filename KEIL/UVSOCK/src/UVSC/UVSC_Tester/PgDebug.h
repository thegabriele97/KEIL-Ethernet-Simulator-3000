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
// Breakpoint storage class

class bp_class  {
public:
   bp_class();
  ~bp_class();

  void       Clear (void);
  int        AddBp (BKRSP *pB);
  void    Complete (void);

  int   IsComplete (void)  {
    return (bDone);
  }

  int     GetCount (void)  {
    return (nBreaks);
  }

  typedef union xbp  {
    BKRSP          bkr;
    unsigned char aTmp[300];
  } XBP;
#define __NBREAKS   256

  BKRSP  *GetBp (int i)  {
    return (&aBreaks[i].bkr);
  }

  int      nIndex;
  int       bDone;
  int     nBreaks;
  XBP     aBreaks [__NBREAKS];
};






/////////////////////////////////////////////////////////////////////////////
// CDebugDlg dialog

class CDebugDlg : public CDialog
{
// Construction
public:
	CDebugDlg(CWnd* pParent = NULL);   // standard constructor

  // Dialog Data
	//{{AFX_DATA(CDebugDlg)
	enum { IDD = IDD_DEBUGTAB };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

public:
  void BpEnumUpdate(BKRSP *pBkptRsp, int *pBkptIndexes, int bkptCount);
  void SetCmdOutputText(const char *txt);
  void SetCmdLineText(const char *txt);

private:
  CString   m_StrCmdOutput;
  CString   m_StrCmdLine;
  bp_class  m_bpc;

  void InitBpListbox (int idx);  

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
	afx_msg void OnStartdebug();
	afx_msg void OnStoptdebug();
	afx_msg void OnStartexec();
	afx_msg void OnStopexec();
	afx_msg void OnStepinstr();
	afx_msg void OnStepinto();
	afx_msg void OnStepout();
	afx_msg void OnStepover();
  afx_msg void OnExecCmd();
	afx_msg void OnClrbpAll();
	afx_msg void OnSetinterval();
	afx_msg void OnReset();
	//}}AFX_MSG
  afx_msg void OnBnClickedBrkcreate();
  afx_msg void OnLbnSelchangeBplist();
  afx_msg void OnBnClickedBpkill();
  afx_msg void OnBnClickedBpenable();
  afx_msg void OnBnClickedBpdisable();
  afx_msg void OnBnClickedBreakpoints();
  afx_msg void OnBnClickedBpEnum();
  afx_msg void GetCmdOutput();

  DECLARE_MESSAGE_MAP()

public:
  afx_msg void OnCbnSelchangeBrktype();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

