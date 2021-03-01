#if !defined(AFX_PGTINFO_H__8888335B_9287_4B39_BA28_C39703986D27__INCLUDED_)
#define AFX_PGTINFO_H__8888335B_9287_4B39_BA28_C39703986D27__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PgTinfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTargInfo dialog

class CTargInfo : public CDialog
{
// Construction
public:
	CTargInfo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTargInfo)
	enum { IDD = IDD_TARGINFO };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTargInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTargInfo)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnGettarginfo();
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

#endif // !defined(AFX_PGTINFO_H__8888335B_9287_4B39_BA28_C39703986D27__INCLUDED_)
