#if !defined(AFX_PGTRGMSG_H__2FA9A757_0428_44E2_B912_6806D66E8F89__INCLUDED_)
#define AFX_PGTRGMSG_H__2FA9A757_0428_44E2_B912_6806D66E8F89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PgTrgMsg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PgTrgMsg dialog

class PgTrgMsg : public CDialog
{
// Construction
public:
	PgTrgMsg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(PgTrgMsg)
	enum { IDD = IDD_TARGMESS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PgTrgMsg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PgTrgMsg)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTmcha();
	afx_msg void OnMsgtotarg();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedButton3();
  afx_msg void OnBnClickedButtonHide();
  afx_msg void OnBnClickedButtonShow();
  afx_msg void OnBnClickedButtonMinimize();
  afx_msg void OnBnClickedButtonRestore();
  afx_msg void OnBnClickedButtonUilock();
  afx_msg void OnBnClickedButtonUiunlock();
  afx_msg void OnBnClickedButtonExitUvision();
  afx_msg void OnBnClickedButtonMaximize();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PGTRGMSG_H__2FA9A757_0428_44E2_B912_6806D66E8F89__INCLUDED_)
