/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2004-2016 KEIL - An ARM Company. All rights reserved.        */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/


#pragma once


#include "UVSOCK.h"
#include "AdvListBox.h"
#include "UVSCTesterTabs.h"

#include "afxwin.h"

typedef enum {
  OUT_TYPE_GEN = 0,
  OUT_TYPE_RSP,
  OUT_TYPE_ASYNC,
  OUT_TYPE_DATA,
  OUT_TYPE_ERROR,
  OUT_TYPE_CMD
} OUT_TYPE;


typedef enum {
  NOTIFY_DIALOG_LOGMSG = 0,
  NOTIFY_DIALOG_RTAMSG,
  NOTIFY_DIALOG_CONNECTED,
  NOTIFY_DIALOG_DISCONNECTED,
  NOTIFY_DIALOG_CONNECTFAILED,
  NOTIFY_DIALOG_BUILDOUTPUT,
  NOTIFY_DIALOG_CMDOUTPUT,
  NOTIFY_DIALOG_CALCEXPR,
  NOTIFY_DIALOG_ENUMBP,
  NOTIFY_DIALOG_BUILDLINE,
  NOTIFY_DIALOG_CMDLINE,
  NOTIFY_DIALOG_PBARINIT,
  NOTIFY_DIALOG_PBARTEXT,
  NOTIFY_DIALOG_PBARPOS,
  NOTIFY_DIALOG_PBARSTOP,
  NOTIFY_DIALOG_UVSCOPFAILED,
  NOTIFY_DIALOG_READWRITE,
} NOTIFY_DIALOG;

typedef struct {
  OUT_TYPE type;
  char msg[4096];
} NotifyLogMsg;


typedef struct {
  int handle;
  int port;
} NotifyConnected;

typedef struct {
  int len;
  char str[1];
} NotifyBigString;

typedef struct {
  TVAL tval;
} NotifyTVAL;

typedef struct {
  BKRSP bkptRsp[128]; 
  int bkptIndexes[128]; 
  int bkptCount;
} NotifyBPEnum;


typedef enum {
  UVSC_OP_NOTIFY_PENDING = 0,
  UVSC_OP_NOTIFY_FAILED,
  UVSC_OP_NOTIFY_SUCCESS
} UVSC_OP_NOTIFY_TYPE;


extern void NotifyDialog (NOTIFY_DIALOG msg, void *msgData);
extern "C" void DoEvents (void);


// These externs are used to notify the main dialog thread of UVSC op completions
extern UVSC_OP_NOTIFY_TYPE g_UVSCOpNotify;
extern TVAL g_CalcExprRes;

/////////////////////////////////////////////////////////////////////////////
// CUVSCTesterDlg dialog

class CUVSCTesterDlg : public CDialog
{
// Construction
public:
	CUVSCTesterDlg(CWnd* pParent = NULL);	// standard constructor
  ~CUVSCTesterDlg();                  

// Dialog Data
	//{{AFX_DATA(CUVSCTesterDlg)
	enum { IDD = IDD_UVSCTESTER_DIALOG }; 
	//}}AFX_DATA

  CCaptainSLOGBox	m_LogOut;       // Snazzy Output Listbox
  CUVSCTesterTabs m_ControlTabs;  // Control tabs 

private:
  CString m_StrConnText;
  CString m_StrProgressText;
  CProgressCtrl m_ProgressBar;
  CFont m_EditFont;
  int m_IntLogRaw;
  int m_IntLogTrace; 

  void UpdateRegistyVals();
 
public:
  int UVSCConnHandle;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUVSCTesterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CUVSCTesterDlg)
	virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
  afx_msg LRESULT OnNotifyDialog(WPARAM wParam, LPARAM lParam);
  afx_msg void OnClearLog();
  afx_msg void OnTabChange(NMHDR *nmhdr, LRESULT *lresult);
  afx_msg void OnLogControlChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
    void       OutLine (OUT_TYPE type, char *fmt, ...);
    
};



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
