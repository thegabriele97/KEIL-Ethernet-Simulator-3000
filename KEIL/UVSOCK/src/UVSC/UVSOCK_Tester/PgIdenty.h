#if !defined(AFX_PGIDENTY_H__51CE35B3_88AA_498F_86FD_34E8FB49578E__INCLUDED_)
#define AFX_PGIDENTY_H__51CE35B3_88AA_498F_86FD_34E8FB49578E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PgIdenty.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PgIdenty dialog

class PgIdenty : public CDialog
{
// Construction
public:
	PgIdenty(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(PgIdenty)
	enum { IDD = IDD_IDENTIFY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PgIdenty)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PgIdenty)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnUseSim();
  afx_msg void OnUseTrg();
  afx_msg void OnSendwhois();
	afx_msg void OnWhichdoc();
  
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedCloseprj();
    afx_msg void OnBnClickedLoadprj();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PGIDENTY_H__51CE35B3_88AA_498F_86FD_34E8FB49578E__INCLUDED_)
