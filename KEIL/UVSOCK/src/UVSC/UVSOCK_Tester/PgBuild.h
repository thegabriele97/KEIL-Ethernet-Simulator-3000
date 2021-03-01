#if !defined(AFX_PGBUILD_H__1BB7323A_2349_4A2B_A868_B22024DFC4EE__INCLUDED_)
#define AFX_PGBUILD_H__1BB7323A_2349_4A2B_A868_B22024DFC4EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PgBuild.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PgBuild dialog

class PgBuild : public CDialog
{
// Construction
public:
	PgBuild(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(PgBuild)
	enum { IDD = IDD_MAKE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


    void OutLine (char *fmt, ...);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PgBuild)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PgBuild)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSendbuild();
	afx_msg void OnSendrebuild();
	afx_msg void OnSendstopbuild();
	afx_msg void OnSetfilegroup();
	afx_msg void OnSetoptions();
	afx_msg void OnGetprjfile();
	afx_msg void OnCleanprj();
	afx_msg void OnAddgrp();
	afx_msg void OnDelgroup();
	afx_msg void OnAddfile();
	afx_msg void OnDelfile();
  afx_msg void OnFlashDownload();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedPrjoptions();
    afx_msg void OnBnClickedEnumfiles();
    afx_msg void OnBnClickedEnumgroups();
    afx_msg void OnBnClickedProgress();
    afx_msg void OnBnClickedActfiles();
    afx_msg void OnBnClickedGetextversion();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PGBUILD_H__1BB7323A_2349_4A2B_A868_B22024DFC4EE__INCLUDED_)
