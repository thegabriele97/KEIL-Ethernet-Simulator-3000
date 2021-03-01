/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2004-2016 KEIL - An ARM Company. All rights reserved.        */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/



#pragma once


/////////////////////////////////////////////////////////////////////////////
// CProjectDlg dialog

class CProjectDlg : public CDialog
{
// Construction
public:
  CProjectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CProjectDlg)
  enum { IDD = IDD_PROJECTTAB };
    // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA

  void SetBuildOutputText(const char *txt);
  void SetBuildLineText(const char *txt);
  int   CheckTname (char *pB);
  int  CheckOptStr (char *pB);
  int     GetGname (char *pB);
  int     GetFname (char *pB);

private:
  CString m_StrBuildOutput;
  CString m_StrBuildLine;

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CProjectDlg)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

  void GetBuildOutput();

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CProjectDlg)
  virtual void OnCancel();
  virtual void OnOK();
  virtual BOOL OnInitDialog();
  afx_msg void OnSendbuild();
  afx_msg void OnSendrebuild();
  afx_msg void OnSendstopbuild();
  afx_msg void OnCleanprj();
  afx_msg void OnAddgrp();
  afx_msg void OnDelgroup();
  afx_msg void OnAddfile();
  afx_msg void OnDelfile();
  afx_msg void OnFlashDownload();
  //}}AFX_MSG
  afx_msg void OnBnClickedEnumfiles();
  afx_msg void OnBnClickedEnumgroups();
  afx_msg void OnBnClickedEnumtargets();
  afx_msg void OnBnClickedSettargets();
  afx_msg void OnBnClickedSetOutputName();
  afx_msg void OnBnClickedGetCurrentTarget();
  afx_msg void OnBnClickedActfiles(); 
  afx_msg void OnBnClickedOptread();
  afx_msg void OnBnClickedOptwrite();
  afx_msg void OnBnClickedUseSim();
  afx_msg void OnBnClickedUseTrg();
  afx_msg void OnBnClickedGetDbgTarget();
  afx_msg void OnBnClickedCloseprj();
  afx_msg void OnBnClickedLoadprj();
  afx_msg void OnBnClickedPrjOutName();
  afx_msg void OnCbnCloseup();

  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


