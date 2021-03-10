#if !defined(AFX_PERIDIALOG_H__83C4810D_3E31_11D4_B027_E953FF205073__INCLUDED_)
#define AFX_PERIDIALOG_H__83C4810D_3E31_11D4_B027_E953FF205073__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PeriDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPeriDialog dialog

class CPeriDialog : public CDialog
{
// Construction
public:
    CPeriDialog::CPeriDialog (AGSIMENU *pMen, CWnd *pWnd);  // new constructor

    void        Update (void);                  // Update dialog function

// Dialog Data
    AGSIMENU    *pM;

// Dialog Data
	//{{AFX_DATA(CPeriDialog)
	enum { IDD = IDD_ADCON };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPeriDialog)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPeriDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnClose();
	afx_msg void OnSelchangeAdMode();
	afx_msg void OnSelchangeAdClock();
	afx_msg void OnSelchangeAdAcquisition();
	afx_msg void OnSelchangeAdChannel();
	afx_msg void OnKillfocusAdAdccon1();
	afx_msg void OnKillfocusAdAdccon2();
	afx_msg void OnKillfocusAdAdccon3();
	afx_msg void OnKillfocusAdAdcdata();
	afx_msg void OnKillfocusAdDmaphl();
	afx_msg void OnAdConvst();
	afx_msg void OnAdSconv();
	afx_msg void OnAdCconv();
	afx_msg void OnAdDma();
	afx_msg void OnAdExc();
	afx_msg void OnAdT2c();
	afx_msg void OnAdAdci();
	afx_msg void OnAdBusy();
	afx_msg void OnKillfocusAdAin0();
	afx_msg void OnKillfocusAdAin1();
	afx_msg void OnKillfocusAdAin2();
	afx_msg void OnKillfocusAdAin3();
	afx_msg void OnKillfocusAdAin4();
	afx_msg void OnKillfocusAdAin5();
	afx_msg void OnKillfocusAdAin6();
	afx_msg void OnKillfocusAdAin7();
	afx_msg void OnKillfocusAdTemp();
	afx_msg void OnKillfocusAdVref();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

extern AGSIMENU PeriMenu;  // Peripheral Dialog
extern BYTE initflag;

#endif // !defined(AFX_PERIDIALOG_H__83C4810D_3E31_11D4_B027_E953FF205073__INCLUDED_)
