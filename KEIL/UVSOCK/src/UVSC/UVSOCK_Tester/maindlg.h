// PwOptionsDlg.h : header file
//



#if !defined(AFX_PWOPTIONSDLG_H__D2D72840_F039_4358_BC31_B1840B7E2966__INCLUDED_)
#define AFX_PWOPTIONSDLG_H__D2D72840_F039_4358_BC31_B1840B7E2966__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/*
 * DTC-Profiles
 */
#if 0
 #include "dtc_Identify.h"
 #include "dtc_Make.h"
 #include "dtc_Timeout.h" 
 #include "dtc_Debug.h"
 #include "dtc_TargetMessaging.h"
 #include "dtc_TargetInfo.h"
#endif


#include "UVSOCK.h"
#include "Captain_SLOG_ListBox.h"

/*
 * Dialog Pages
 */
#include "PgIdenty.h"   // dtc_Identify Page.
#include "PgBuild.h"    // dtc_Make Page.
#include "PgDebug.h"    // dtc_Debug Page.
#include "PgTrgMsg.h"   // dtc_TargetMessaging Page
#include "PgTinfo.h"    // dtc_TargetInfo Page
#include "RTADlg.h"
#include "afxwin.h"


typedef enum {
  OUT_TYPE_GEN = 0,
  OUT_TYPE_RSP,
  OUT_TYPE_ASYNC,
  OUT_TYPE_DATA,
  OUT_TYPE_ERROR
} OUT_TYPE;


class CUv3ClientSocket : public CAsyncSocket  {
public:
                CUv3ClientSocket();
  virtual      ~CUv3ClientSocket();
  virtual void  OnReceive (int nErrorCode);
  void          OnConnect (int nErrorCode);
  void            OnClose (int nErrorCode);
  void     ProcessMessage (void);
};


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
extern bp_class  bpc;


extern "C" void DoEvents (void);

extern const char szBpt[6][12];         // Breakpoint-Types


/////////////////////////////////////////////////////////////////////////////
// CPwOptionsDlg dialog

class CPwOptionsDlg : public CDialog
{
// Construction
public:
	CPwOptionsDlg(CWnd* pParent = NULL);	// standard constructor
  ~CPwOptionsDlg() {                    // Make sure we destory the RTA window in the destructor
    m_RTADialog.DestroyWindow();
  }

	CUv3ClientSocket			m_oSocket; 
  CRTADlg               m_RTADialog;    // RTA Dialog

	void               Send (UVSOCK_CMD* psCmd);
  void        SendCommand (UV_OPERATION nCmd, int nLen, void *pData);
  void  CreateCommandSend (UV_OPERATION eCmd, int nLen, const void *pData);
  void         ConnectUV3 (void);

private:
  CFont m_EditFont;

public:
// Dialog Data
	//{{AFX_DATA(CPwOptionsDlg)
	enum { IDD = IDD_PWOPTIONS_DIALOG };
	CTreeCtrl       mc_tree;   // Profiles
	CTreeCtrl    mc_clients;   // clients
	CStatic	       mc_dummy;
  CEdit           mc_Edit;   // Output Control
  CCaptainSLOGBox	m_LogOut;  // Snazzy Output control
//CFont              Font;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPwOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPwOptionsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRefresh();
	afx_msg void OnSend();
	afx_msg void OnReceive();
	afx_msg LRESULT OnConnectUV3 (WPARAM wParam, LPARAM lParam);
  afx_msg void OnBnClickedButtonRTA();
  afx_msg void OnClearLog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
    PgIdenty    m_pgIdentify;           // Identify Page
    PgBuild     m_pgBuild;              // Make Page
    PgDebug     m_pgDebug;              // Debug Page
    PgTrgMsg    m_pgTrgMsg;             // Target Messaging Page
    CTargInfo   m_pgTargInfo;           // Target Info Page

    int anCurrentChildWindow;

    void PositionChild ();
    void     ShowChild (int nToShow);
    void       OutLine (OUT_TYPE type, char *fmt, ...);
    HTREEITEM   AddOne (CTreeCtrl&   tree,
                        HTREEITEM hParent,
                        LPSTR       pText,
                        HTREEITEM  hAfter,
                        LPARAM         pP);


    HTREEITEM      hRoot;  // = mc_tree.InsertItem ("Profiles");
    HTREEITEM  hIdentify;  // = mc_tree.InsertItem ("dtc_Identify);
    HTREEITEM     hBuild;  // = mc_tree,InsertItem ("dtc_Make);
    HTREEITEM     hDebug;  // = mc_tree,InsertItem ("dtc_Debug");
    HTREEITEM    hTrgMsg;  // = mc_tree,InsertItem ("dtc_TargetMessaging");
    HTREEITEM   hTrgInfo;  // = mc_tree.InsertItem ("dtc_TargetInfo");

    HTREEITEM     hWhoIs;  // = mc_tree.InsertItem ("WhoIs",  hIdentify,  0);


//---Event Objects:
#if 0
    class Ev_Id_IAm : public dtc_Identify::IAm::Handler  {
    public:
      void Handle (dtc_Identify::IAm& evt);
    } EV_ID_IAM;
#endif

#if 0
    class Ev_BuildInfo : public dtc_Make::BuildInfo::Handler  {
    public:
      void Handle (dtc_Make::BuildInfo& evt);
    } EV_BUILDINFO;
#endif

    CButton m_buttonRTA;
};



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PWOPTIONSDLG_H__D2D72840_F039_4358_BC31_B1840B7E2966__INCLUDED_)
