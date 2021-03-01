#if !defined(AFX_PGDEBUG_H__BA20B348_0CC6_4FE9_81FD_C60C70C2FD82__INCLUDED_)
#define AFX_PGDEBUG_H__BA20B348_0CC6_4FE9_81FD_C60C70C2FD82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PgDebug.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PgDebug dialog

class PgDebug : public CDialog
{
// Construction
public:
	PgDebug(CWnd* pParent = NULL);   // standard constructor


    int TmSetup (void);
    int TimeOut (int nMsec = 2000);


    void MemAdrCB (__int64 nAdr, LPCTSTR pszExpr, int bOk);


// Dialog Data
	//{{AFX_DATA(PgDebug)
	enum { IDD = IDD_DEBUG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

    void OutLine (char *fmt, ...);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PgDebug)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PgDebug)
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
	afx_msg void OnExecuntil();
	afx_msg void OnGetmemadr();
	afx_msg void OnSetbp();
	afx_msg void OnClrbp();
	afx_msg void OnClrbpAll();
	afx_msg void OnReadreg();
	afx_msg void OnWritereg();
	afx_msg void OnReadmem();
	afx_msg void OnWritemem();
	afx_msg void OnDecmema();
	afx_msg void OnClrbpall();
	afx_msg void OnIsrunning();
	afx_msg void OnCycTstamp();
	afx_msg void OnSetinterval();
	afx_msg void OnReset();
	afx_msg void OnGetvtr();
	afx_msg void OnSetvtr();
	afx_msg void OnGetvers();
	afx_msg void OnSersendD();
	afx_msg void OnGetsero();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedBreakpoints();
    afx_msg void OnBnClickedIdenum();
public:
    afx_msg void OnBnClickedMaptofl();
public:
    afx_msg void OnBnClickedEnumstack();
public:
    afx_msg void OnBnClickedEnumvtr();
public:
    afx_msg void OnBnClickedEnumvtrv();
public:
    afx_msg void OnBnClickedShowcode();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

	afx_msg void OnSetbpexp();
	afx_msg void OnClrbpexp();

#endif // !defined(AFX_PGDEBUG_H__BA20B348_0CC6_4FE9_81FD_C60C70C2FD82__INCLUDED_)
