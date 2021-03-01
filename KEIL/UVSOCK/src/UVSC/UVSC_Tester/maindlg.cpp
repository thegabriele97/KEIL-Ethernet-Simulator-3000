/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2004-2016 KEIL - An ARM Company. All rights reserved.        */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include "stdafx.h"
#include "DlgApp.h"
#include "MainDlg.h"
#include <string.h>
#include "Traceevent.h"
#include "UVSC_C.h"
#include "UVSCThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define MIN_AUTO_PORT_NUMBER  5101
#define MAX_AUTO_PORT_NUMBER  5110

// Dialog notify message
const UINT UWM_NOTIFY_DIALOG = RegisterWindowMessage ( "UWM_NOTIFY_DIALOG_UV3" );


// These externs are used to notify the main dialog thread of UVSC op completions
UVSC_OP_NOTIFY_TYPE g_UVSCOpNotify;
TVAL g_CalcExprRes;


CUVSCTesterDlg   *_dlg = NULL;

// For registry entries
static CString strLogContrl   = "LogControl";
static CString strRaw         = "Raw";
static CString strTrace       = "Trace";



/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
  CAboutDlg();

// Dialog Data
  //{{AFX_DATA(CAboutDlg)
  enum { IDD = IDD_ABOUTBOX };
  //}}AFX_DATA

  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAboutDlg)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  //{{AFX_MSG(CAboutDlg)
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)  {
  //{{AFX_DATA_INIT(CAboutDlg)
  //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CAboutDlg)
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
  //{{AFX_MSG_MAP(CAboutDlg)
    // No message handlers
  //}}AFX_MSG_MAP  
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CUVSCTesterDlg dialog

CUVSCTesterDlg::CUVSCTesterDlg(CWnd* pParent /*=NULL*/) : 
  CDialog(CUVSCTesterDlg::IDD, pParent),
  m_StrConnText("Not Connected"), 
  m_StrProgressText("") {


  //{{AFX_DATA_INIT(CUVSCTesterDlg)
  //}}AFX_DATA_INIT
  // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

  m_IntLogRaw = 0;
  m_IntLogTrace = 0; 
}

CUVSCTesterDlg::~CUVSCTesterDlg () {
  //CDialog::~CDialog();   // TdB: CRASH !!!, so I commented that out. Question is, why did it run in VS2008 ? Object is already deleted...
}


void CUVSCTesterDlg::DoDataExchange(CDataExchange* pDX)  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CUVSCTesterDlg)
  //}}AFX_DATA_MAP
  DDX_Control(pDX, IDC_LISTOUT, m_LogOut);
  DDX_Control(pDX, IDC_CONTROLTABS, m_ControlTabs);
  DDX_Check(pDX, IDC_CHECKLOGRAW, m_IntLogRaw);
  DDX_Check(pDX, IDC_CHECKLOGTRACE, m_IntLogTrace);   
  DDX_Text(pDX, IDC_STATIC_CONNTEXT, m_StrConnText);
  DDX_Text(pDX, IDC_STATIC_PROGRESSTEXT, m_StrProgressText);
  DDX_Control(pDX, IDC_PROGRESSBAR, m_ProgressBar);
  
}



BEGIN_MESSAGE_MAP(CUVSCTesterDlg, CDialog)
  //{{AFX_MSG_MAP(CUVSCTesterDlg)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_WM_MOVE()
  ON_WM_SHOWWINDOW()
  ON_BN_CLICKED(IDC_CLEARLOG, OnClearLog)
  ON_REGISTERED_MESSAGE(UWM_NOTIFY_DIALOG, OnNotifyDialog )
  ON_NOTIFY(TCN_SELCHANGE, IDC_CONTROLTABS, OnTabChange)
  ON_BN_CLICKED(IDC_CHECKLOGRAW, OnLogControlChange)
  ON_BN_CLICKED(IDC_CHECKLOGTRACE, OnLogControlChange)
  ON_WM_DESTROY()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUVSCTesterDlg message handlers


void NotifyDialog (NOTIFY_DIALOG msg, void *msgData) {
  _dlg->SendMessage(UWM_NOTIFY_DIALOG, (WPARAM)msg, (LPARAM)msgData);
}

LRESULT  CUVSCTesterDlg::OnNotifyDialog(WPARAM wParam, LPARAM lParam) {
  NotifyLogMsg *plogMsg;
  NotifyConnected *pConnectedData;
  NotifyBigString *pBigString;
  NotifyTVAL *pTVAL;
  NotifyBPEnum *pBPEnum;

  switch ((NOTIFY_DIALOG)wParam) {
    case NOTIFY_DIALOG_LOGMSG:
      plogMsg = (NotifyLogMsg *) lParam;
      OutLine (plogMsg->type, plogMsg->msg);
      break;

    case NOTIFY_DIALOG_RTAMSG:
      plogMsg = (NotifyLogMsg *) lParam;
      m_ControlTabs.GetRTA()->OutLine ((RTA_OUT_TYPE)plogMsg->type, plogMsg->msg);
      break;

    case NOTIFY_DIALOG_CONNECTED:
      pConnectedData = (NotifyConnected *)lParam;      
      OutLine(OUT_TYPE_GEN, "Connected to µVision (Handle: %d, Port: %d)\n", pConnectedData->handle, pConnectedData->port);
      m_StrConnText.Format("Connected (Handle: %d, Port: %d)\n", pConnectedData->handle, pConnectedData->port);
      UpdateData(FALSE);
      UVSCConnHandle = pConnectedData->handle;
      m_ControlTabs.GetConnection()->Connected();
      OnLogControlChange();               // Force the log options to be updated for this connection
      break;

    case NOTIFY_DIALOG_DISCONNECTED:
      pConnectedData = (NotifyConnected *)lParam; 
      OutLine (OUT_TYPE_ERROR, "Lost connection to µVision (Handle: %d)\n", pConnectedData->handle);
      m_StrConnText.Format("Not Connected\n");
      UpdateData(FALSE);
      m_ControlTabs.GetConnection()->ReConnect();
      break;

    case NOTIFY_DIALOG_CONNECTFAILED:
      m_StrConnText.Format("Not Connected\n");
      UpdateData(FALSE);
      m_ControlTabs.GetConnection()->ReConnect();
      break;

    case NOTIFY_DIALOG_BUILDOUTPUT:
      pBigString = (NotifyBigString *)lParam;
      m_ControlTabs.GetProject()->SetBuildOutputText(pBigString->str);
      break;

    case NOTIFY_DIALOG_CMDOUTPUT:
      pBigString = (NotifyBigString *)lParam;
      m_ControlTabs.GetDebug()->SetCmdOutputText(pBigString->str);
      break;

    case NOTIFY_DIALOG_CALCEXPR:
      pTVAL = (NotifyTVAL *)lParam;
      memcpy (&g_CalcExprRes, &pTVAL->tval, sizeof(TVAL));
      g_UVSCOpNotify = UVSC_OP_NOTIFY_SUCCESS;   // Notify any waiting function calls
      break;

    case NOTIFY_DIALOG_ENUMBP:
      pBPEnum = (NotifyBPEnum *)lParam;

      m_ControlTabs.GetDebug()->BpEnumUpdate(pBPEnum->bkptRsp,
                                             pBPEnum->bkptIndexes,
                                             pBPEnum->bkptCount);
      g_UVSCOpNotify = UVSC_OP_NOTIFY_SUCCESS;   // Notify any waiting function calls
      break;

    case NOTIFY_DIALOG_BUILDLINE:
      pBigString = (NotifyBigString *)lParam;
      m_ControlTabs.GetProject()->SetBuildLineText(pBigString->str);
      break;

    case NOTIFY_DIALOG_CMDLINE:
      pBigString = (NotifyBigString *)lParam;
      m_ControlTabs.GetDebug()->SetCmdLineText(pBigString->str);
      break;

    case NOTIFY_DIALOG_PBARINIT:
      pBigString = (NotifyBigString *)lParam;
      m_ProgressBar.SetRange32(0, atoi(pBigString->str));
      m_ProgressBar.SetPos(0);      
      m_StrProgressText.Format("");
      UpdateData(FALSE);
      m_ProgressBar.ShowWindow(SW_NORMAL);
      break;

    case NOTIFY_DIALOG_PBARTEXT:
      pBigString = (NotifyBigString *)lParam;
      m_StrProgressText.Format("%s", pBigString->str);
      UpdateData(FALSE);
      break;

    case NOTIFY_DIALOG_PBARPOS:
      pBigString = (NotifyBigString *)lParam;
      m_ProgressBar.SetPos(atoi(pBigString->str));
      break;

    case NOTIFY_DIALOG_PBARSTOP:
      pBigString = (NotifyBigString *)lParam;
      m_ProgressBar.SetPos(0);      
      m_StrProgressText.Format("");
      UpdateData(FALSE);
      m_ProgressBar.ShowWindow(SW_HIDE);
      break;

    case NOTIFY_DIALOG_READWRITE:
      g_UVSCOpNotify = UVSC_OP_NOTIFY_SUCCESS; 
      break;

    case NOTIFY_DIALOG_UVSCOPFAILED:
      g_UVSCOpNotify = UVSC_OP_NOTIFY_FAILED;    // Notify any waiting function calls
      break;

    default:
      ; // Do nothing
  }
  if (lParam) {
    // Release the memory allocated for this message
    free ((void *)lParam);
    lParam = NULL;
  }

  return (0);
}

extern "C" void DoEvents (void)  {
  MSG msg;

  while (::PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))  {
    if (!theApp.PreTranslateMessage (&msg))  {
      ::TranslateMessage (&msg);
      ::DispatchMessage (&msg);
    }
    theApp.OnIdle(0);            // updates user interface
    theApp.OnIdle(1);            // frees temporary objects
  }
}

BOOL CUVSCTesterDlg::OnInitDialog()  {  
  _dlg = this;
  
  CDialog::OnInitDialog();

  // Add "About..." menu item to system menu.

  // IDM_ABOUTBOX must be in the system command range.
  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu* pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != NULL)
  {
      CString strAboutMenu;
      strAboutMenu.LoadString(IDS_ABOUTBOX);
      if (!strAboutMenu.IsEmpty())
      {
          pSysMenu->AppendMenu(MF_SEPARATOR);
          pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
      }
  }

  // Set the icon for this dialog.  The framework does this automatically
  //  when the application's main window is not a dialog
  SetIcon(m_hIcon, TRUE);			// Set big icon
  SetIcon(m_hIcon, FALSE);		// Set small icon

  // Set the window title (with version number)
  unsigned int uiV1, uiV2;
  UVSC_Version(&uiV1, &uiV2);
  CString strTitle;
  strTitle.Format("UVSC Tester V%d.%02d", uiV1 / 100, uiV1 % 100);
  SetWindowText(strTitle);


  // Set up the log box
  m_LogOut.SetFont(80, "Lucida Console");
  m_LogOut.AddString(new TraceMessage(TraceEvent::None, "UVSC TESTER LOG")); 
  m_LogOut.setLimit(500);
  m_LogOut.enableLimit(TRUE);
  //m_LogOut.setToDisk(TRUE, "default_log.txt");

  // Initialise the control tabs
  m_ControlTabs.InitDialogs();
  m_ControlTabs.ActivateTabDialogs();

  // Start the UVSC Thread
  UVSCThread_Init(this);

  // Read the current connection settings from the registry, defaulting if none exist
  CWinApp* pApp = AfxGetApp();

  m_IntLogRaw = pApp->GetProfileInt(strLogContrl, strRaw, 0);
  m_IntLogTrace = pApp->GetProfileInt(strLogContrl, strTrace, 1);

  UpdateData(FALSE);  

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CUVSCTesterDlg::OnDestroy() {
  UVSCThread_UnInit();
  UpdateRegistyVals();            // Write form reg vals to the registry
}

void CUVSCTesterDlg::UpdateRegistyVals() {
  CWinApp* pApp = AfxGetApp();

  pApp->WriteProfileInt(strLogContrl, strRaw, m_IntLogRaw);
  pApp->WriteProfileInt(strLogContrl, strTrace, m_IntLogTrace);
}


void CUVSCTesterDlg::OnLogControlChange() {
  ThreadMsg msg;

  UpdateData(TRUE);

  msg.cmd = UV_NULL_CMD;
  msg.connHandle = UVSCConnHandle;
  msg.ctrlMsg.cmd = CONTROLMSG_LOGCONTROL;
  msg.ctrlMsg.logControlData.enableRaw = (m_IntLogRaw > 0);
  msg.ctrlMsg.logControlData.enableTrace = (m_IntLogTrace > 0);
  msg.ctrlMsg.logControlData.disableLogging = false;

  UVSCThread_Post(&msg, 0);
}


void CUVSCTesterDlg::OnClearLog (void) {
  m_LogOut.ResetContent();
}


void CUVSCTesterDlg::OutLine (OUT_TYPE type, char *fmt, ...)  {
  char  szB [1028];
  va_list    argp;
  int n;

  memset (szB,0,sizeof(szB));
  va_start (argp, fmt);
  if (argp == NULL)  {         //  || argp[0] == 0)  {
    n = min (strlen (fmt), sizeof (szB) - 4);
    memcpy (szB, fmt, n);
  }
  else  {
    n = _vsnprintf (szB, sizeof (szB) - 4, fmt, argp);
  }
  
  switch (type) {
    case OUT_TYPE_GEN:
      m_LogOut.AddString(new TraceMessage(TraceEvent::None, szB)); 
      break;
    case OUT_TYPE_RSP:
      m_LogOut.AddString(new TraceComment(TraceEvent::None, szB)); 
    break;
    case OUT_TYPE_ASYNC:
      m_LogOut.AddString(new TraceWarning(TraceEvent::None, szB)); 
    break;
    case OUT_TYPE_ERROR:
      m_LogOut.AddString(new TraceError(TraceEvent::None, szB)); 
    break;
    case OUT_TYPE_DATA:
      m_LogOut.AddString(new TraceData(TraceEvent::None, szB)); 
    break;
    case OUT_TYPE_CMD:
      m_LogOut.AddString(new TraceTrace(TraceEvent::None, szB));     
    break;
    default:
      m_LogOut.AddString(new TraceError(TraceEvent::None, szB)); 
  }
}

void CUVSCTesterDlg::OnTabChange(NMHDR *nmhdr, LRESULT *lresult) {
  m_ControlTabs.ActivateTabDialogs();
  *lresult = 0;
}

void CUVSCTesterDlg::OnSysCommand (UINT nID, LPARAM lParam)  {
  if ((nID & 0xFFF0) == IDM_ABOUTBOX)
  {
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
  }
  else
  {
    CDialog::OnSysCommand(nID, lParam);
  }
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUVSCTesterDlg::OnQueryDragIcon()  {
  return (HCURSOR) m_hIcon;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUVSCTesterDlg::OnPaint()  {
  if (IsIconic())
  {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  }
  else
  {
    CDialog::OnPaint();
  }
}


