
/*--------------------------------------------------------------------------
MainDlg.cpp

Dialog setup, dtc-events receiver and display module. 
Copyright (c) 2003 Keil Elektronik GmbH and Keil Software, Inc.
All rights reserved.
--------------------------------------------------------------------------*/


#include "stdafx.h"
#include "DlgApp.h"
#include "MainDlg.h"
#include <string.h>
#include "Traceevent.h"

UVSOCK_CMD   cmd;
bp_class     bpc;



/*
 * Profiles
 */




/*
 * Dialog-Pages
 */

#include "PgIdenty.h"   // dtc_Identify Page.
#include "PgBuild.h"    // dtc_Make Page.
#include "PgDebug.h"    // dtc_Debug Page.
//#include "PgTrgMsg.h"   // dtc_TargetMessaging Page.
#include "PgTinfo.h"    // dtc_TargetInfo Page.


//static LOGFONT xlFont = { -18,0,0,0,200,0,0,0,0,3,2,1,49, "Lucida Console", };

const UINT UWM_CONNECT_UV3 = RegisterWindowMessage ( "UV3_MSG_CONNECT_UV3" );

CPwOptionsDlg   *_dlg = NULL;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


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
// CPwOptionsDlg dialog




CPwOptionsDlg::CPwOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPwOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPwOptionsDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}



void CPwOptionsDlg::DoDataExchange(CDataExchange* pDX)  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CPwOptionsDlg)
  DDX_Control(pDX, IDC_TREE1, mc_tree);
  DDX_Control(pDX, IDC_TREE2, mc_clients);
  DDX_Control(pDX, IDC_S_COORDINATES, mc_dummy);
  DDX_Control(pDX, IDC_RECEIVED, mc_Edit);
  //}}AFX_DATA_MAP
  DDX_Control(pDX, IDC_BUTTONRTA, m_buttonRTA);
  DDX_Control(pDX, IDC_LISTOUT, m_LogOut);
}



BEGIN_MESSAGE_MAP(CPwOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CPwOptionsDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_MOVE()
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, OnDblclkTree1)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, OnClickTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnSelchangedTree1)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_BN_CLICKED(IDC_SEND, OnSend)
	ON_BN_CLICKED(IDC_RECEIVE, OnReceive)
  ON_BN_CLICKED(IDC_BUTTONRTA, OnBnClickedButtonRTA)
  ON_BN_CLICKED(IDC_CLEARLOG, OnClearLog)
  ON_REGISTERED_MESSAGE(UWM_CONNECT_UV3, OnConnectUV3 )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()





void CPwOptionsDlg::ConnectUV3 (void) {
  char acName[1025];
  unsigned long nLength = sizeof(acName) - 1;

//---Socket-Init:
  m_oSocket.Create(0, SOCK_STREAM);	
  GetComputerName(acName, &nLength);
#if  0
  if (m_oSocket.Connect (acName, 4823) == 0) {
#else
  if (m_oSocket.Connect ("127.0.0.1", 4823))  {
#endif
    //DWORD lastError = GetLastError();
    //if (lastError != WSAEWOULDBLOCK) {
    //  
    //}
  }
//---
}

LRESULT  CPwOptionsDlg::OnConnectUV3(WPARAM wParam, LPARAM lParam) {
  ConnectUV3();
  return (0);
}



/////////////////////////////////////////////////////////////////////////////
// CPwOptionsDlg message handlers


/*
 * Socket-Functions
 */
CUv3ClientSocket::CUv3ClientSocket()  {
}

CUv3ClientSocket::~CUv3ClientSocket()  {
}




/*
 * Breakpoint-container construction
 */
bp_class::bp_class()  {
  Clear();
}


/*
 * Breakpoint-container destruction
 */
bp_class::~bp_class()  {
}

void bp_class::Clear (void)  {
//memset (this, 0, sizeof (*this));
  nBreaks = 0;
  bDone   = 0;
  nIndex  = 0;
  memset (aBreaks, 0, sizeof (aBreaks));
}


void bp_class::Complete (void)  {
  bDone  = 1;
  nIndex = 0;
}



/*
 * Add a Breakpoint to breakpoint array
 */
int bp_class::AddBp (BKRSP *pB)  {
  if (nBreaks == (__NBREAKS - 1))  {    // more than 256 breaks
    return (0);                         // simply ignore it
  }

  int n = sizeof (BKRSP) + pB->nExpLen + 1;
  if (n > ((sizeof (aBreaks[0].aTmp) - 1)))  {
    n = sizeof (aBreaks[0].aTmp) - 1;
  }
  memcpy (&aBreaks[nBreaks], pB, n);
  aBreaks[nBreaks].aTmp[n] = 0;
  ++nBreaks;
  return (nBreaks);
}





int bResult;
TVAL     tv;

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


static void MemDump (void)  {
  int       m, v, z, aci;
  UINT     nA, n, i;
  AMEM    *pM;
  char    szB[256];

  pM  = (AMEM *) (void *) &cmd.data.cmdRsp.amem;
//if (pM->nErr)  {
//  sprintf (szB, "ReadMemory-Error at 0x%08X\n", pM->ErrAddr);
//  _dlg->OutLine ("%s", szB);
//}

  nA  = pM->nAddr;                       // Memory-Address
  n   = pM->nBytes;
  aci = 0;
  for ( i = 0 ; i < n ; )  {
    m = min (16, (n - i));
    v = sprintf (szB, "0x%08X: ", (DWORD) nA);
    for ( z = 0 ; z < m ; ++z )  {
      v += sprintf (&szB[v], "%02X ", pM->aBytes[aci] & 0xFF);
      ++aci;
    }
    szB [v] = 0;
    _dlg->OutLine (OUT_TYPE_DATA, szB);
    i  += m;
    nA += m;
  }
}


static void SerDump (void)  {
  int       n, v, nl;
  DWORD     i;
  SERIO   *pM;
  char    szB[256];

  pM = (SERIO *) (void *) &cmd.data.cmdRsp.serdat;
  n  = pM->nMany;
  v  = 0;
  for ( i = 0 ; i < pM->nMany ; ++i )  {
    nl = 0;
    switch (pM->itemMode)  {
      case 0:                            // Byte(s)
        if (isprint (pM->s.aBytes[i]))  {
          v += sprintf (&szB[v], "%c", pM->s.aBytes[i]);
        }
        else  {
          v += sprintf (&szB[v], "\\x%02X", pM->s.aBytes[i]);
        }
        if (v > 128 || pM->s.aBytes[i] == 0x0A) nl = 1;
        break;
      case 1:                            // Word16(s)
        v += sprintf (&szB[v], "%04X ", pM->s.aWords[i]);
        if (v > 128) nl = 1;
        break;
    }
    if (nl)  {
      szB[v] = 0;
      _dlg->OutLine (OUT_TYPE_DATA, "%s", szB);
      v = 0;
    }
  }
  if (v > 0)  {
    szB[v] = 0;
    _dlg->OutLine (OUT_TYPE_DATA, "%s", szB);
  }
}



#if 0
static void MemWritten (void)  {
  AMEM    *pM;
  char    szB[256];

  pM  = (AMEM *) (void *) &cmd.data;       // m_abBuffer[0];
  if (pM->nErr)  {
    sprintf (szB, "WriteMemory-Error at 0x%08X\n", pM->ErrAddr);
    _dlg->OutLine ("%s", szB);
  }
}


static void DccDump (PRJDATA *pJ)  {
  int       m, v, z, aci;
  UINT     nA, n, i;
  char    szB[256];

  nA  = 0;                                  // data-address counter
  n   = pJ->nRes;
  aci = 0;
  for ( i = 0 ; i < n ; )  {
    m = min (16, (n - i));
    v = sprintf (szB, "0x%02X: ", (DWORD) nA);
    for ( z = 0 ; z < m ; ++z )  {
      v += sprintf (&szB[v], "%02X ", pJ->szNames[aci] & 0xFF);
      ++aci;
    }
    szB [v] = 0;
    _dlg->OutLine (szB);
    i  += m;
    nA += m;
  }
}
#endif


//---Command codes:
//--- General functions:
#define UV_NULL_CMD_STR               "NULL_CMD             "   // not a command
#define UV_GEN_GET_VERSION_STR        "GEN_GET_VERSION      "   // get socket-interface Version number
#define UV_GEN_UI_UNLOCK_STR          "GEN_UI_UNLOCK        "   // Prevent message boxes from popping up in uVision
#define UV_GEN_UI_LOCK_STR            "GEN_UI_LOCK          "   // Allow message boxes to pop up in uVision
#define UV_GEN_HIDE_STR               "GEN_HIDE             "   // Completely hide the uVision window
#define UV_GEN_SHOW_STR               "GEN_SHOW             "   // Show the uVision window
#define UV_GEN_RESTORE_STR            "GEN_RESTORE          "   // Restore the uVision window
#define UV_GEN_MINIMIZE_STR           "GEN_MINIMIZE         "   // Minimise the uVision window
#define UV_GEN_MAXIMIZE_STR           "GEN_MAXIMIZE         "   // Maximise the uVision window
#define UV_GEN_EXIT_STR               "GEN_EXIT             "   // Close uVision
#define UV_GEN_GET_EXTVERSION_STR     "GEN_GET_EXTVERSION   "   // Get extended version info
#define UV_GEN_CHECK_LICENSE_STR      "GEN_CHECK_LICENSE    "   // Get license info

//--- Project functions:
#define UV_PRJ_LOAD_STR               "PRJ_LOAD             "   // Load Project  /TBD/
#define UV_PRJ_CLOSE_STR              "PRJ_CLOSE            "   // Load Project  /TBD/
#define UV_PRJ_ADD_GROUP_STR          "PRJ_ADD_GROUP        "   // Add a Group to current Project
#define UV_PRJ_DEL_GROUP_STR          "PRJ_DEL_GROUP        "   // Remove a Group from current Project
#define UV_PRJ_ADD_FILE_STR           "PRJ_ADD_FILE         "   // Add one or more files to group
#define UV_PRJ_DEL_FILE_STR           "PRJ_DEL_FILE         "   // Remove one or more files from group
#define UV_PRJ_BUILD_STR              "PRJ_BUILD            "   // Build the current Project
#define UV_PRJ_REBUILD_STR            "PRJ_REBUILD          "   // ReBuild the current Project
#define UV_PRJ_CLEAN_STR              "PRJ_CLEAN            "   // Clean current Project
#define UV_PRJ_BUILD_CANCEL_STR       "PRJ_BUILD_CANCEL     "   // Stop the Build/ReBuild
#define UV_PRJ_FLASH_DOWNLOAD_STR     "PRJ_FLASH_DOWNLOAD   "   // Download library to flash
#define UV_PRJ_GET_DEBUG_TARGET_STR   "PRJ_GET_DEBUG_TARGET "   // set the debug target
#define UV_PRJ_SET_DEBUG_TARGET_STR   "PRJ_SET_DEBUG_TARGET "   // get the debug target
#define UV_PRJ_GET_OPTITEM_STR        "PRJ_GET_OPTITEM      "   // read Prj-option item
#define UV_PRJ_SET_OPTITEM_STR        "PRJ_SET_OPTITEM      "   // write Prj-option item
#define UV_PRJ_ENUM_GROUPS_STR        "PRJ_ENUM_GROUPS      "   // enumerate groups
#define UV_PRJ_ENUM_FILES_STR         "PRJ_ENUM_FILES       "   // enumerate files
#define UV_PRJ_CMD_PROGRESS_STR       "PRJ_CMD_PROGRESS     "   // Progress-Bar commands
#define UV_PRJ_ACTIVE_FILES_STR       "PRJ_ACTIVE_FILES     "   // get number of active files
#define UV_PRJ_FLASH_ERASE_STR        "PRJ_FLASH_ERASE      "   ///< Erase flash device

//--- Debug functions:
#define UV_DBG_ENTER_STR              "DBG_ENTER            "   // start debugger
#define UV_DBG_EXIT_STR               "DBG_EXIT             "   // stop debugger
#define UV_DBG_START_EXECUTION_STR    "DBG_START_EXECUTION  "   // start execution
#define UV_DBG_STOP_EXECUTION_STR     "DBG_STOP_EXECUTION   "   // stop execution
#define UV_DBG_STATUS_STR             "DBG_STATUS           "   // is simulation running
#define UV_DBG_RESET_STR              "DBG_RESET            "   // reset debugger
#define UV_DBG_STEP_HLL_STR           "DBG_STEP_HLL         "   // Line-Step (HLL-Step)
#define UV_DBG_STEP_INTO_STR          "DBG_STEP_INTO        "   // one HLL or Asm Step
#define UV_DBG_STEP_INSTRUCTION_STR   "DBG_STEP_INSTRUCTION "   // Step one Asm-Instruction
#define UV_DBG_STEP_OUT_STR           "DBG_STEP_OUT         "   // Step out of current function
#define UV_DBG_CALC_EXPRESSION_STR    "DBG_CALC_EXPRESSION  "   // calculate an expression
#define UV_DBG_MEM_READ_STR           "DBG_MEM_READ         "   // Read a chunk of Memory
#define UV_DBG_MEM_WRITE_STR          "DBG_MEM_WRITE        "   // Write a chunk of Memory
#define UV_DBG_TIME_INFO_STR          "DBG_TIME_INFO        "   // get cycles and time-stamp
#define UV_DBG_SET_CALLBACK_STR       "DBG_SET_CALLBACK     "   // set time-interval
#define UV_DBG_VTR_GET_STR            "DBG_VTR_GET          "   // read vtr-value
#define UV_DBG_VTR_SET_STR            "DBG_VTR_SET          "   // write vtr-value
#define UV_DBG_SERIAL_GET_STR         "DBG_SERIAL_GET       "   // client wants serial output
#define UV_DBG_SERIAL_PUT_STR         "DBG_SERIAL_PUT       "   // App receives one or more BYTEs
#define UV_DBG_VERIFY_CODE_STR        "DBG_VERIFY_CODE      "   // Verify code in flash against built executable
#define UV_DBG_CREATE_BP_STR          "DBG_CREATE_BP        "   // create a breakpoint
#define UV_DBG_ENUMERATE_BP_STR       "DBG_ENUMERATE_BP     "   // client want's enumerated breakpoints
#define UV_DBG_CHANGE_BP_STR          "DBG_CHANGE_BP        "   // Bp changed (killed,enabled,disabled)
#define UV_DBG_ENUM_SYMTP_STR         "DBG_ENUM_SYMTP       "   // enumerate members
#define UV_DBG_ADR_TOFILELINE_STR     "DBG_ADR_TOFILELINE   "   ///< Map an address to code file & linenumber
#define UV_DBG_ENUM_STACK_STR         "DBG_ENUM_STACK       "   ///< Enumerate the call stack
#define UV_DBG_ENUM_VTR_STR           "DBG_ENUM_VTR         "   ///< Enumerate all virtual registers (VTRs)
#define UV_DBG_UNUSED_STR             "DBG_UNUSED           "   ///< Unused
#define UV_DBG_ADR_SHOWCODE_STR       "DBG_ADR_SHOWCODE     "   ///< Show disassembly and/or HLL (High Level Language) file for an address
#define UV_DBG_WAKE_STR               "DBG_WAKE             "   ///< Set sleep callback and/or wake up simulation
#define UV_DBG_SLEEP_STR              "DBG_SLEEP            "   ///< Sleep the simulation 
#define UV_DBG_AGDI_MSG_STR           "DBG_AGDI_MSG         "   ///< Notification from an AGDI DLL

//---Answers/Error from Server to Client:
#define UV_CMD_RESPONSE_STR           "CMD_RESPONSE         "   // Response to a command from the client

//---Asynchronous messages:
#define UV_ASYNC_MSG_STR              "ASYNC_MSG            "   // Response to a command from the client

//--- Special Asynchronous messages:
// Project functions:
#define UV_PRJ_BUILD_COMPLETE_STR     "PRJ_BUILD_COMPLETE   "   // A build has completed
#define UV_PRJ_BUILD_OUTPUT_STR       "PRJ_BUILD_OUTPUT     "   // Send Build output to client

//--- Debug functions:
#define UV_DBG_CALLBACK_STR           "DBG_CALLBACK         "   // The callback timeout has expired

//--- Response to UV_DBG_ENUMERATE_BP:
#define UV_DBG_BP_ENUM_START_STR      "DBG_BP_ENUM_START    "   // Start of enumeration (no breakpoint info)
#define UV_DBG_BP_ENUMERATED_STR      "DBG_BP_ENUMERATED    "   //   zero, one or more Response(s) with breakpoint info (BKRSP)
#define UV_DBG_BP_ENUM_END_STR        "DBG_BP_ENUM_END      "   // End of enumeration (no breakpoint info)

//--- Response to UV_PRJ_ENUM_GROUPS:
#define UV_PRJ_ENUM_GROUPS_START_STR  "PRJ_ENUM_GROUPS_START"   // Start of group enumeration
#define UV_PRJ_ENUM_GROUPS_ENU_STR    "PRJ_ENUM_GROUPS_ENU  "   // zero, one or more Responses with group name (SSTR)
#define UV_PRJ_ENUM_GROUPS_END_STR    "PRJ_ENUM_GROUPS_END  "   // End of group enumeration

//--- Response to UV_PRJ_ENUM_FILES:
#define UV_PRJ_ENUM_FILES_START_STR   "PRJ_ENUM_FILES_START "   // Start of files enumeration
#define UV_PRJ_ENUM_FILES_ENU_STR     "PRJ_ENUM_FILES_ENU   "   // zero, one or more Response(s) with file name (SSTR)
#define UV_PRJ_ENUM_FILES_END_STR     "PRJ_ENUM_FILES_END   "   // End of files enumeration

//--- Progress bar functions
#define UV_PRJ_PBAR_INIT_STR          "PRJ_PBAR_INIT        "   // Initialise the progress bar
#define UV_PRJ_PBAR_STOP_STR          "PRJ_PBAR_STOP        "   // Stop the progress bar
#define UV_PRJ_PBAR_SET_STR           "PRJ_PBAR_SET         "   // Set the position of the progress bar
#define UV_PRJ_PBAR_TEXT_STR          "PRJ_PBAR_TEXT        "   // Set the text of the progress bar

//--- Response to UV_DBG_ENUM_SYMTP:
#define UV_DBG_ENUM_SYMTP_START_STR   "DBG_ENUM_SYMTP_START "   // Start of group enumeration
#define UV_DBG_ENUM_SYMTP_ENU_STR     "DBG_ENUM_SYMTP_ENU   "   // zero, one or more Responses with group name (SSTR)
#define UV_DBG_ENUM_SYMTP_END_STR     "DBG_ENUM_SYMTP_END   "   // End of group enumeration

//--- Response to UV_DBG_ENUM_STACK:
#define UV_DBG_ENUM_STACK_START_STR   "DBG_ENUM_STACK_START "   ///< Start of stack enumeration
#define UV_DBG_ENUM_STACK_ENU_STR     "DBG_ENUM_STACK_ENU   "   ///< Stack enumeration, one or more Response(s) of structure stackdat
#define UV_DBG_ENUM_STACK_END_STR     "DBG_ENUM_STACK_END   "   ///< End of stack enumeration

//--- Response to UV_DBG_ENUM_VTR:
#define UV_DBG_ENUM_VTR_START_STR     "DBG_ENUM_VTR_START   "   ///< Start of vtr enumeration
#define UV_DBG_ENUM_VTR_ENU_STR       "DBG_ENUM_VTR_ENU     "   ///< Vtr enumeration, one or more Response(s) of structure iAVTR
#define UV_DBG_ENUM_VTR_END_STR       "DBG_ENUM_VTR_END     "   ///< End of vtr enumeration

//--- Real-Time Agent:
#define UV_RTA_MESSAGE_STR            "RTA_MESSAGE          "   // send a RTAgent-message
#define UV_RTA_INCOMPATIBLE_STR       "RTA_INCOMPATIBLE     "   ///< sent when the RTA version in target is incompatible with uVision

//--- Unknown command code:
#define UV_UNKNOWN_COMMAND_STR        "UNKNOWN_COMMAND ???  "  

//--- Test definititions (for testing only):
#define UV_TST_1_STR                  "TST_1                "
#define UV_TST_2_STR                  "TST_2                "
#define UV_TST_3_STR                  "TST_3                "
#define UV_TST_4_STR                  "TST_4                "
#define UV_TST_5_STR                  "TST_5                "
#define UV_TST_6_STR                  "TST_6                "
#define UV_TST_7_STR                  "TST_7                "
#define UV_TST_8_STR                  "TST_8                "
#define UV_TST_9_STR                  "TST_9                "
#define UV_TST_10_STR                 "TST_10               "



static const char szRt[][50] =  {
  "OK                        ",   ///< (0)  Command was executed successfully
  "UV_STATUS_FAILED          ", 	///< (1)  Command failed (generic error status)
  "UV_STATUS_NO_PROJECT      ",   ///< (2)  No project is loaded
  "UV_STATUS_WRITE_PROTECTED ",   ///< (3)  The current project is write protected
  "UV_STATUS_NO_TARGET       ",   ///< (4)  No target is selected
  "UV_STATUS_NO_TOOLSET      ",   ///< (5)  Toolset for current target is undefined
  "UV_STATUS_TARGET_BLOCKED  ",   ///< (6)  The target is blocked
  "UV_STATUS_ALREADY_PRESENT ",   ///< (7)  The group / file is already present
  "UV_STATUS_INVALID_NAME    ",   ///< (8)  Invalid group / file / project name
  "UV_STATUS_NOT_FOUND       ",   ///< (9)  File / group not found
  "UV_STATUS_WRONG_MODE      ",   ///< (10) Operation not possible in this mode
  "UV_STATUS_TARGET_EXECUTING",   ///< (11) Operation not possible when target is executing
  "UV_STATUS_TARGET_STOPPED  ",   ///< (12) Operation not possible when target is stopped
  "UV_STATUS_PARSE_ERROR     ",   ///< (13) Error parsing data in request
  "UV_STATUS_OUT_OF_RANGE    ",   ///< (14) Requested data out of range

  "UV_STATUS_BP_CANCELLED    ",   ///< (15) Create Breakpoint has been cancelled
  "UV_STATUS_BP_BADADDRESS   ",   ///< (16) Create Breakpoint: invalid address
  "UV_STATUS_BP_NOTSUPPORTED ",   ///< (17) Type of Breakpoint is not supported
  "UV_STATUS_BP_FAILED       ",   ///< (18) Create Breakpoint failed
  "UV_STATUS_BP_REDEFINED    ",   ///< (19) Async: a bp has been redefined
  "UV_STATUS_BP_DISABLED     ",   ///< (20) Async: a bp has been disabled
  "UV_STATUS_BP_ENABLED      ",   ///< (21) Async: a bp has been enabled
  "UV_STATUS_BP_CREATED      ",   ///< (22) Async: a bp has been created
  "UV_STATUS_BP_DELETED      ",   ///< (23) Async: a bp has been deleted
  "UV_STATUS_BP_NOTFOUND     ",   ///< (24) ena/dis/kill breakpoint not found (nTickMark)

  "UV_STATUS_BUILD_OK_WARNING",   ///< (25) A build was successful, but with warnings
  "UV_STATUS_BUILD_FAILED    ",   ///< (26) A build failed with errors
  "UV_STATUS_BUILD_CANCELLED ",   ///< (27) A build was cancelled

  "UV_STATUS_NOT_SUPPORTED   ",   ///< (28) Request for an unsupported operation
  "UV_STATUS_TIMEOUT         ",   ///< (29) Request timed-out
  "UV_STATUS_UNEXPECTED_MSG  ",   ///< (30) An unexpected message type was returned

  "UV_STATUS_VERIFY_FAILED   ",   ///< (31) Verifying the downloaded code failed

  "UV_STATUS_NO_ADRMAP       ",   ///< (32) Map address to file/line failed
  "UV_STATUS_INFO            ",   ///< (33) This is an information only message
  "UV_STATUS_???             "    ///< Invalid status!
};



static char *RspCmd (void)  {
  char    *pT, *pS;
  static  char szMsg[1024];

  pT = &szMsg[0];

  switch (cmd.data.cmdRsp.cmd) {
    //---Command codes:
    //--- General functions:
    case UV_NULL_CMD:             pS = UV_NULL_CMD_STR;             break;
    case UV_GEN_GET_VERSION:      pS = UV_GEN_GET_VERSION_STR;      break;
    case UV_GEN_UI_UNLOCK:        pS = UV_GEN_UI_UNLOCK_STR;        break;
    case UV_GEN_UI_LOCK:          pS = UV_GEN_UI_LOCK_STR;          break;
    case UV_GEN_HIDE:             pS = UV_GEN_HIDE_STR;             break;
    case UV_GEN_SHOW:             pS = UV_GEN_SHOW_STR;             break;
    case UV_GEN_RESTORE:          pS = UV_GEN_RESTORE_STR;          break;
    case UV_GEN_MINIMIZE:         pS = UV_GEN_MINIMIZE_STR;         break;
    case UV_GEN_MAXIMIZE:         pS = UV_GEN_MAXIMIZE_STR;         break;
    case UV_GEN_EXIT:             pS = UV_GEN_EXIT_STR;             break;
    case UV_GEN_GET_EXTVERSION:   pS = UV_GEN_GET_EXTVERSION_STR;   break;
    case UV_GEN_CHECK_LICENSE:    pS = UV_GEN_CHECK_LICENSE_STR;    break;

    //--- Project functions:
    case UV_PRJ_LOAD:             pS = UV_PRJ_LOAD_STR;             break;
    case UV_PRJ_CLOSE:            pS = UV_PRJ_CLOSE_STR;            break;
    case UV_PRJ_ADD_GROUP:        pS = UV_PRJ_ADD_GROUP_STR;        break;
    case UV_PRJ_DEL_GROUP:        pS = UV_PRJ_DEL_GROUP_STR;        break;
    case UV_PRJ_ADD_FILE:         pS = UV_PRJ_ADD_FILE_STR;         break;
    case UV_PRJ_DEL_FILE:         pS = UV_PRJ_DEL_FILE_STR;         break;
    case UV_PRJ_BUILD:            pS = UV_PRJ_BUILD_STR;            break;
    case UV_PRJ_REBUILD:          pS = UV_PRJ_REBUILD_STR;          break;
    case UV_PRJ_CLEAN:            pS = UV_PRJ_CLEAN_STR;            break;
    case UV_PRJ_BUILD_CANCEL:     pS = UV_PRJ_BUILD_CANCEL_STR;     break;
    case UV_PRJ_FLASH_DOWNLOAD:   pS = UV_PRJ_FLASH_DOWNLOAD_STR;   break;
    case UV_PRJ_GET_DEBUG_TARGET: pS = UV_PRJ_GET_DEBUG_TARGET_STR; break;
    case UV_PRJ_SET_DEBUG_TARGET: pS = UV_PRJ_SET_DEBUG_TARGET_STR; break;
    case UV_PRJ_GET_OPTITEM:      pS = UV_PRJ_GET_OPTITEM_STR;      break;
    case UV_PRJ_SET_OPTITEM:      pS = UV_PRJ_SET_OPTITEM_STR;      break;
    case UV_PRJ_ENUM_GROUPS:      pS = UV_PRJ_ENUM_GROUPS_STR;      break;
    case UV_PRJ_ENUM_FILES:       pS = UV_PRJ_ENUM_FILES_STR;       break;
    case UV_PRJ_CMD_PROGRESS:     pS = UV_PRJ_CMD_PROGRESS_STR;     break;
    case UV_PRJ_ACTIVE_FILES:     pS = UV_PRJ_ACTIVE_FILES_STR;     break;
    case UV_PRJ_FLASH_ERASE:      pS = UV_PRJ_FLASH_ERASE_STR;      break;

    //--- Debug functions:
    case UV_DBG_ENTER:            pS = UV_DBG_ENTER_STR;            break;
    case UV_DBG_EXIT:             pS = UV_DBG_EXIT_STR;             break;
    case UV_DBG_START_EXECUTION:  pS = UV_DBG_START_EXECUTION_STR;  break;
    case UV_DBG_STOP_EXECUTION:   pS = UV_DBG_STOP_EXECUTION_STR;   break;
    case UV_DBG_STATUS:           pS = UV_DBG_STATUS_STR;           break;
    case UV_DBG_RESET:            pS = UV_DBG_RESET_STR;            break;
    case UV_DBG_STEP_HLL:         pS = UV_DBG_STEP_HLL_STR;         break;
    case UV_DBG_STEP_INTO:        pS = UV_DBG_STEP_INTO_STR;        break;
    case UV_DBG_STEP_INSTRUCTION: pS = UV_DBG_STEP_INSTRUCTION_STR; break;
    case UV_DBG_STEP_OUT:         pS = UV_DBG_STEP_OUT_STR;         break;
    case UV_DBG_CALC_EXPRESSION:  pS = UV_DBG_CALC_EXPRESSION_STR;  break;
    case UV_DBG_MEM_READ:         pS = UV_DBG_MEM_READ_STR;         break;
    case UV_DBG_MEM_WRITE:        pS = UV_DBG_MEM_WRITE_STR;        break;
    case UV_DBG_TIME_INFO:        pS = UV_DBG_TIME_INFO_STR;        break;
    case UV_DBG_SET_CALLBACK:     pS = UV_DBG_SET_CALLBACK_STR;     break;
    case UV_DBG_VTR_GET:          pS = UV_DBG_VTR_GET_STR;          break;
    case UV_DBG_VTR_SET:          pS = UV_DBG_VTR_SET_STR;          break;
    case UV_DBG_SERIAL_GET:       pS = UV_DBG_SERIAL_GET_STR;       break;
    case UV_DBG_SERIAL_PUT:       pS = UV_DBG_SERIAL_PUT_STR;       break;
    case UV_DBG_VERIFY_CODE:      pS = UV_DBG_VERIFY_CODE_STR;      break;
    case UV_DBG_CREATE_BP:        pS = UV_DBG_CREATE_BP_STR;        break;
    case UV_DBG_ENUMERATE_BP:     pS = UV_DBG_ENUMERATE_BP_STR;     break;
    case UV_DBG_CHANGE_BP:        pS = UV_DBG_CHANGE_BP_STR;        break;
    case UV_DBG_ENUM_SYMTP:       pS = UV_DBG_ENUM_SYMTP_STR;       break;
    case UV_DBG_ADR_TOFILELINE:   pS = UV_DBG_ADR_TOFILELINE_STR;   break;
    case UV_DBG_ENUM_STACK:       pS = UV_DBG_ENUM_STACK_STR;       break;
    case UV_DBG_ENUM_VTR:         pS = UV_DBG_ENUM_VTR_STR;         break;
    case UV_DBG_ADR_SHOWCODE:     pS = UV_DBG_ADR_SHOWCODE_STR;     break;
    case UV_DBG_WAKE:             pS = UV_DBG_WAKE_STR;             break;
    case UV_DBG_SLEEP:            pS = UV_DBG_SLEEP_STR;            break;
    case UV_DBG_AGDI_MSG:         pS = UV_DBG_AGDI_MSG_STR;         break;

    //---Answers/Error from Server to Client:
    case UV_CMD_RESPONSE:         pS = UV_CMD_RESPONSE_STR;         break;
    
    //---Asynchronous messages:
    case UV_ASYNC_MSG:            pS = UV_ASYNC_MSG_STR;            break;
    
    //--- Special Asynchronous messages:
    //--- Project functions:
    case UV_PRJ_BUILD_COMPLETE:   pS = UV_PRJ_BUILD_COMPLETE_STR;   break;
    case UV_PRJ_BUILD_OUTPUT:     pS = UV_PRJ_BUILD_OUTPUT_STR;     break;
    
    //--- Debug functions:
    case UV_DBG_CALLBACK:         pS = UV_DBG_CALLBACK_STR;         break;
    
    //--- Response to UV_DBG_ENUMERATE_BP:  
    case UV_DBG_BP_ENUM_START:    pS = UV_DBG_BP_ENUM_START_STR;    break;
    case UV_DBG_BP_ENUMERATED:    pS = UV_DBG_BP_ENUMERATED_STR;    break;
    case UV_DBG_BP_ENUM_END:      pS = UV_DBG_BP_ENUM_END_STR;      break;

    //--- Response to UV_PRJ_ENUM_GROUPS:
    case UV_PRJ_ENUM_GROUPS_START:pS = UV_PRJ_ENUM_GROUPS_START_STR;break;
    case UV_PRJ_ENUM_GROUPS_ENU:  pS = UV_PRJ_ENUM_GROUPS_ENU_STR;  break;
    case UV_PRJ_ENUM_GROUPS_END:  pS = UV_PRJ_ENUM_GROUPS_END_STR;  break;

    //--- Response to UV_PRJ_ENUM_FILES:
    case UV_PRJ_ENUM_FILES_START: pS = UV_PRJ_ENUM_FILES_START_STR; break;
    case UV_PRJ_ENUM_FILES_ENU:   pS = UV_PRJ_ENUM_FILES_ENU_STR;   break;
    case UV_PRJ_ENUM_FILES_END:   pS = UV_PRJ_ENUM_FILES_END_STR;   break;    

    //--- Progress bar functions
    case UV_PRJ_PBAR_INIT:        pS = UV_PRJ_PBAR_INIT_STR;        break;
    case UV_PRJ_PBAR_STOP:        pS = UV_PRJ_PBAR_STOP_STR;        break;
    case UV_PRJ_PBAR_SET:         pS = UV_PRJ_PBAR_SET_STR;         break;
    case UV_PRJ_PBAR_TEXT:        pS = UV_PRJ_PBAR_TEXT_STR;        break;

    //--- Response to UV_DBG_ENUM_SYMTP:
    case UV_DBG_ENUM_SYMTP_START: pS = UV_DBG_ENUM_SYMTP_START_STR; break;
    case UV_DBG_ENUM_SYMTP_ENU:   pS = UV_DBG_ENUM_SYMTP_ENU_STR;   break;
    case UV_DBG_ENUM_SYMTP_END:   pS = UV_DBG_ENUM_SYMTP_END_STR;   break;

    //--- Response to UV_DBG_ENUM_STACK:
    case UV_DBG_ENUM_STACK_START: pS = UV_DBG_ENUM_STACK_START_STR; break;
    case UV_DBG_ENUM_STACK_ENU:   pS = UV_DBG_ENUM_STACK_ENU_STR;   break;
    case UV_DBG_ENUM_STACK_END:   pS = UV_DBG_ENUM_STACK_END_STR;   break;

    //--- Response to UV_DBG_ENUM_VTR:
    case UV_DBG_ENUM_VTR_START:   pS = UV_DBG_ENUM_VTR_START_STR;   break;
    case UV_DBG_ENUM_VTR_ENU:     pS = UV_DBG_ENUM_VTR_ENU_STR;     break;
    case UV_DBG_ENUM_VTR_END:     pS = UV_DBG_ENUM_VTR_END_STR;     break;

    //--- Real-Time Agent:
    case UV_RTA_MESSAGE:          pS = UV_RTA_MESSAGE_STR;          break;
    case UV_RTA_INCOMPATIBLE:     pS = UV_RTA_INCOMPATIBLE_STR;     break;    
    
    //--- Test definititions (for testing only):
    case UV_TST_1:                pS = UV_TST_1_STR;                break;
    case UV_TST_2:                pS = UV_TST_2_STR;                break;
    case UV_TST_3:                pS = UV_TST_3_STR;                break;
    case UV_TST_4:                pS = UV_TST_4_STR;                break;
    case UV_TST_5:                pS = UV_TST_5_STR;                break;
    case UV_TST_6:                pS = UV_TST_6_STR;                break;
    case UV_TST_7:                pS = UV_TST_7_STR;                break;
    case UV_TST_8:                pS = UV_TST_8_STR;                break;
    case UV_TST_9:                pS = UV_TST_9_STR;                break;
    case UV_TST_10:               pS = UV_TST_10_STR;               break;
     break;

    default:
     pS = UV_UNKNOWN_COMMAND_STR;
     break;
  }
  pT += sprintf (pT, "%s", pS);

  return (&szMsg[0]);
}


static char *RspText (void)  {
  char    *pT;
  static  char szMsg[1024];

  pT = &szMsg[0];

  switch (cmd.data.cmdRsp.status)  {
    case UV_STATUS_SUCCESS:           // Command was executed successfully
      pT += sprintf (pT, "[%s]", szRt[cmd.data.cmdRsp.status]);
      break;

    case UV_STATUS_FAILED: 	          ///< Operation failed: Generic / unknown error
    case UV_STATUS_NO_PROJECT:        ///< Operation failed: No project is currently open
    case UV_STATUS_WRITE_PROTECTED:   ///< Operation failed: The current project is write protected
    case UV_STATUS_NO_TARGET:         ///< Operation failed: No target is selected for the current project
    case UV_STATUS_NO_TOOLSET:        ///< Operation failed: No toolset is selected for the current target
    case UV_STATUS_NOT_DEBUGGING:     ///< Operation failed: The debugger is not running, this operation is only possible in debug mode
    case UV_STATUS_ALREADY_PRESENT:   ///< Operation failed: The group / file is already present in the current project
    case UV_STATUS_INVALID_NAME:      ///< Operation failed: One of the specified group / file / project name(s) is invalid
    case UV_STATUS_NOT_FOUND:         ///< Operation failed: File / group not found in the current project
    case UV_STATUS_DEBUGGING:         ///< Operation failed: The debugger is running, this operation is only possible when not in debug mode
    case UV_STATUS_TARGET_EXECUTING:  ///< Operation failed: The target is executing, this operation is not possible when target is executing
    case UV_STATUS_TARGET_STOPPED:    ///< Operation failed: The target is stopped, this operation is not possible when target is stopped
    case UV_STATUS_PARSE_ERROR:       ///< Operation failed: Error parsing data in request
    case UV_STATUS_OUT_OF_RANGE:      ///< Operation failed: Data in request is out of range
    case UV_STATUS_NOT_SUPPORTED:     ///< Operation failed: Requested operation is not supported
    case UV_STATUS_TIMEOUT:           ///< Operation failed: No response to the request occurred within the timeout period (UVSOCK Client DLL only)
    case UV_STATUS_UNEXPECTED_MSG:    ///< Operation failed: An unexpected message type was returned (UVSOCK Client DLL only)
    case UV_STATUS_VERIFY_FAILED:     ///< Operation failed: The code downloaded in the target differs from the current binary
    case UV_STATUS_NO_ADRMAP:         ///< Operation failed: The specified code address does not map to a file / line
    case UV_STATUS_INFO:              ///< General Info: This is an information only message. It may contain warning information pertinent to a later error condition.
    case UV_STATUS_NO_MEM_ACCESS:     ///< Operation failed: Memory access is blocked (most likely target does not support memory access while running)
    case UV_STATUS_FLASH_DOWNLOAD:    ///< Operation failed: The target is downloading FLASH, this operation is not possible when FLASH is downloading
    case UV_STATUS_BUILDING:          ///< Operation failed: A build is in progress, this operation is not possible when build is in progress
    case UV_STATUS_HARDWARE:          ///< Operation failed: The debugger is debugging hardware, this operation is not possible when debugging a hardware target
    case UV_STATUS_SIMULATOR:         ///< Operation failed: The debugger is debugging a simulation, this operation not possible when debugging a simulated target
      cmd.data.cmdRsp.err.str [cmd.data.cmdRsp.err.StrLen] = 0;
      if (cmd.data.cmdRsp.err.StrLen != 0)  {
        pT += sprintf (pT, "%s: %s", szRt[cmd.data.cmdRsp.status], cmd.data.cmdRsp.err.str);
      }
      else  {
        pT += sprintf (pT, "%s", szRt[cmd.data.cmdRsp.status]);
      }
      break;

    case UV_STATUS_BP_CANCELLED:      // create new breakpoint has been cancelled
    case UV_STATUS_BP_BADADDRESS:     // invalid address in create breakpoint
    case UV_STATUS_BP_NOTSUPPORTED:   // type of breakpoint is not supported (by target)
    case UV_STATUS_BP_FAILED:         // bp creation failed (syntax error, nested command etc.)
    case UV_STATUS_BP_REDEFINED:      // Async: a bp has been redefined
    case UV_STATUS_BP_DISABLED:       // Async: a bp has been disabled
    case UV_STATUS_BP_ENABLED:        // Async: a bp has been enabled
    case UV_STATUS_BP_CREATED:        // Async: a bp has been created
    case UV_STATUS_BP_DELETED:        // Async: a bp has been deleted
    case UV_STATUS_BP_NOTFOUND:       ///< Operation failed: Breakpoint with @a nTickMark cookie not found.

    case UV_DBG_BP_ENUM_START:        // Start of enumeration (no breakpoint info)
    case UV_DBG_BP_ENUMERATED:        //   zero, one or more Response(s) with breakpoint info (BKRSP)
    case UV_DBG_BP_ENUM_END:          // End of enumeration (no breakpoint info)

    case UV_DBG_ENUM_SYMTP_START:
    case UV_DBG_ENUM_SYMTP_ENU:
    case UV_DBG_ENUM_SYMTP_END:

    case UV_DBG_ENUM_STACK_START:     // Start of stack enumeration
    case UV_DBG_ENUM_STACK_ENU:       // Stack enumeration, one or more Response(s) of structure stackdat
    case UV_DBG_ENUM_STACK_END:       // End of stack enumeration

    case UV_DBG_ENUM_VTR_START:       // Start of vtr enumeration
    case UV_DBG_ENUM_VTR_ENU:         // Vtr enumeration, one or more Response(s) of structure iAVTR
    case UV_DBG_ENUM_VTR_END:         // End of vtr enumeration
      pT += sprintf (pT, "%s", szRt[cmd.data.cmdRsp.status]);
      break;

    default:
      pT += sprintf (pT, "Unknown Response 0x%X", cmd.data.cmdRsp);
      break;
  }

  return (&szMsg[0]);
}


static char *RunStop (void)  {
  static char szX[64];

  szX[0] = 0;
  switch (cmd.data.cmdRsp.nVal)  {
    case 0: return ("Stopped");
    case 1: return ("Running");
    default:
      sprintf (szX, "EXEC ?? 0x%X", cmd.data.cmdRsp.nVal);
      break;
  }
  return (szX);
}



const char szBpt[6][12]  =  {             // Breakpoint-Types
  "<unknown>",
  "Exec",                                 // Execution-break
  "Read",                                 // Read-Access-break
  "Write",                                // Write-Access-break
  "ReadWrite",                            // ReadWrite-Access-break
  "Complex",                              // re-evalute expr. after each step
};

static void BpMessage (int aSync)  {
  BKRSP     *pRsp;
  char      szB [1024];
  int         n;

  n = sprintf (szB, "%s - %s ", RspCmd(), RspText());
  pRsp = (BKRSP *) &cmd.data.cmdRsp.brk;
  if (pRsp->type != 0)  {
    n += sprintf (&szB[n], "type=%s, %sabled, addr=0x%08X, count=%d",
                            szBpt[pRsp->type], pRsp->enabled ? "en" : "dis", (DWORD) pRsp->nAddress, pRsp->count);
    if (pRsp->nExpLen > 1)  {
      n += sprintf (&szB[n], ", expr=%s", pRsp->szBuffer);
    }
  }
  szB[n++] = '\n';
  szB[n]   = 0;
  switch (cmd.data.cmdRsp.status)  {
    case UV_STATUS_SUCCESS:
    case UV_STATUS_BP_CREATED:
    case UV_STATUS_BP_DELETED:
    case UV_STATUS_BP_ENABLED:
    case UV_STATUS_BP_DISABLED:
      _dlg->OutLine (aSync ? OUT_TYPE_ASYNC : OUT_TYPE_RSP, "%s", szB);
      break;
    default:
      _dlg->OutLine (OUT_TYPE_ERROR, "%s", szB);
      break;
  }
}




static char szOjob[][20] =  {
  "",
  "OPT_LMISC",           // get Link-Misc string, valid for Target only
  "OPT_CMISC",           // get C-Misc string
  "OPT_AMISC",           // get Asm-Misc string
  "OPT_CINCL",           // get C-Include-path string
  "OPT_AINCL",           // get Asm-Include-path string
  "OPT_CDEF",            // get C-Defines string
  "OPT_ADEF",            // get Asm-Defines string
  "OPT_CUNDEF",          // get C-Undefines, valid for Group- and File-level only
  "/OPT_AUNDEF/",        // for completeness: Asm-Undefines, non-existant in uVision
  "OPT_COPTIMIZE",       // get C-Optimize [Level, Time]
  "OPT_CODEGEN",         // get/set ARM/Thumb mode for ARM7TDMI targets
  "OPT_MEMRANGES",       // get memory ranges
  "OPT_ASNMEMRANGES",    // get/set assigned ranges
};


static char szMType[][6] =  {
  "EMPTY", "ROM  ", "RAM  ",
};

static void ReadOptItem (void)  {
  char        szB [1024];
  int           n;
  TRNOPT      *pX;
  UV_MRANGE   *pR;
  UV_MEMINFO *pMI;
  UINT          i;

  n  = sprintf (szB, "%s - %s ", RspCmd(), RspText());
  pX = (TRNOPT *) &cmd.data.cmdRsp.trnopt;

  switch (pX->job)  {
    case OPT_MEMRANGES:                // decode memory ranges
      n += sprintf (&szB[n], "|%s| - %s\n", &pX->szBuffer [pX->iTarg], szOjob [pX->job]);
      _dlg->OutLine (OUT_TYPE_RSP, "%s", szB);

      pMI = (UV_MEMINFO *) &pX->szBuffer [pX->iItem];  // ranges are binary data - not directly printable
      pR  = &pMI->mr;                                  // first memory range
      for ( i = 0 ; i < pMI->nRanges ; ++i, ++pR )  {
        n  = sprintf (&szB[0], "%3d: %s - nStart=0x%08X, nSize=0x%08X ",
                      i, szMType[pR->mType & 3], (UINT) pR->nStart, (UINT) pR->nSize);
        n += sprintf (&szB[n], "defRam=%d, defRom=%d, Zi=%d\n", pR->dfltRam, pR->dfltRom, pR->isZiRam);
        _dlg->OutLine (OUT_TYPE_RSP, "%s", szB);
      }
      break;

    default:
      n += sprintf (&szB[n], "|%s|%s|%s| - %s: '%s'\n",
                    &pX->szBuffer[pX->iTarg],
                    &pX->szBuffer[pX->iGroup],
                    &pX->szBuffer[pX->iFile],
                    szOjob [pX->job],
                    &pX->szBuffer[pX->iItem]);
      _dlg->OutLine (OUT_TYPE_RSP, "%s", szB);
      break;
  }
}



static void DumpStopReason (BPREASON *pR, char *pS)  {
  char    *pRt;
  char    szBn[32];

  switch (pR->eReason)  {
    case STOPREASON_UNDEFINED:        // unknown stop reason
      pRt = "Unknown Reason";
      break;
    case STOPREASON_EXEC:             // Exec-breakpoint
      pRt = (pR->nBpNum != -1) ? "Executed" : "TempBp";
      break;
    case STOPREASON_READ:             // Read-Access breakpoint
      pRt = "Read-Access Bp";
      break;
    case STOPREASON_HIT_WRITE:        // Write-Access breakpoint
      pRt = "Write-Access Bp";
      break;
    case STOPREASON_HIT_COND:         // Conditional breakpoint
      pRt = "Conditional Bp";
      break;
    case STOPREASON_HIT_ESC:          // ESCape-Key has been pressed
      pRt = "Escape";
      break;
    case STOPREASON_HIT_VIOLA:        // Access-Violation happened
      pRt = "Access Violation";
      break;
    case STOPREASON_TIME_OVER:        // Interval time elapsed
      pRt = "Interval Time is over";
      break;
    case STOPREASON_UNDEFINS:         // Undefined instruction
      pRt = "Undefined Instruction";
      break;
    case STOPREASON_PABT:             // Prefetch abort
      pRt = "Prefetct Abort";
      break;
    case STOPREASON_DABT:             // Data Abort
      pRt = "Data Abort";
      break;
    case STOPREASON_NONALIGN:         // Non-aligned instruction
      pRt = "Non-aligned instruction";
      break;
  }

  switch (pR->nBpNum)  {
    case -1:
      sprintf (szBn, "None");
      break;
    default:
      sprintf (szBn, "%d", pR->nBpNum);
      break;
  }
  sprintf (pS, "[OK] StopReason:=%s, Bp=%s, nPC:=0x%08X, nAdr:=0x%08X\n", pRt, szBn, (DWORD) pR->nPC, (DWORD) pR->nAdr);
}

void CUv3ClientSocket::OnConnect(int nErrorCode) {
  if (nErrorCode != 0) {
    Close();                             // Close the current socket
    _dlg->SendMessage(UWM_CONNECT_UV3);  // Send message to the dialog window to start a reconnect
  }
  else {
    _dlg->OutLine (OUT_TYPE_GEN, "Connected to µVision\n");
  }
  CAsyncSocket::OnConnect(nErrorCode);
}


/*
 * The current socket was closed
 */
void CUv3ClientSocket::OnClose(int nErrorCode) {
  char acName[1025];
  unsigned long nLength = sizeof(acName) - 1;

  _dlg->OutLine (OUT_TYPE_GEN, "Disconnected from µVision\n");

  Close();                              // Close the current socket
  CAsyncSocket::OnClose(nErrorCode);
  Sleep (2000);                         // Wait 2 seconds - we don't want to overrun the stack with connects
  _dlg->SendMessage (UWM_CONNECT_UV3);  // Send message to the dialog window to start a reconnect
}



static const char  szTyp[][8] =  {      // VTT_type to text
  /* bt_void   */  "void",
  /* bt_bit    */  "bit",
  /* bt_char   */  "char",
  /* bt_uchar  */  "uchar",
  /* bt_int    */  "int",
  /* bt_uint   */  "uint",
  /* bt_short  */  "short",
  /* bt_ushort */  "ushort",
  /* bt_long   */  "long",
  /* bt_ulong  */  "ulong",
  /* bt_float  */  "float",
  /* bt_double */  "double",
  /* bt_ptr    */  "pointer",
  /* bt_union  */  "union",
  /* bt_struct */  "struct",
  /* bt_func   */  "funct",
  /* bt_string */  "string",
  /* bt_enum   */  "enum",
  /* bt_field  */  "field",
  /* bt_int64  */  "int64",
  /* bt_uint64 */  "uint64",
};



LARGE_INTEGER startTick;

void CUv3ClientSocket::ProcessMessage (void)  {
  int        n;
  CYCTS    *pT;
  VSET    *pVS;
  char     szB[1024], szT[256]; //, *p1;
  PRJDATA  *pJ;
  AMEM     *pA;
  SERIO    *pM;
  char     *pS;
  EXTVERS  *pX;
  AFLMAP  *pAF;

  bResult = 0;
  sprintf (szT, "c=%I64u t=%g ", cmd.cycles, cmd.tStamp);
  pS = szB;

  // Calculate the message duration
  LARGE_INTEGER endTick, freqTick;
  DWORD durationUs;
  QueryPerformanceCounter(&endTick);
  QueryPerformanceFrequency(&freqTick);
  durationUs = (DWORD) (((endTick.QuadPart - startTick.QuadPart) * 1000000ULL) / freqTick.QuadPart);

  switch (cmd.m_eCmd)  {                    // analyze command
    case UV_CMD_RESPONSE:   

      pS += sprintf (pS, "[%02d.%03d ms] %s ", durationUs/1000, durationUs-((durationUs/1000)*1000), RspCmd());

      if (cmd.data.cmdRsp.status == UV_STATUS_SUCCESS) {
        switch (cmd.data.cmdRsp.cmd)  {
          case UV_GEN_GET_VERSION:            // Socket-impl. version number
            sprintf (pS, "(V%d.%d) [OK]\n", cmd.data.cmdRsp.nVal / 100, cmd.data.cmdRsp.nVal % 100);          
            _dlg->OutLine (OUT_TYPE_RSP, "%s", szB);           // Write the line out
            break;
          case UV_DBG_STATUS:              // Query 'Executing' state
            sprintf (pS, "(%s) [OK]\n", RunStop());
            _dlg->OutLine (OUT_TYPE_RSP, "%s", szB);           // Write the line out
            break;

          case UV_DBG_TIME_INFO:           // Get Time-Info (cycles + tStamp)
            pT = (CYCTS *) (void *) &cmd.data.cmdRsp.time;
            sprintf (pS, "(%s, Cyc=%I64u, tStamp=%g) [OK]\n", RspText(), pT->cycles, pT->tStamp);
            _dlg->OutLine (OUT_TYPE_RSP, "%s", szB);           // Write the line out
            break;

          case UV_DBG_VTR_GET:
            pVS = &cmd.data.cmdRsp.vset;
            switch (pVS->val.vType)  {
              case VTT_float:
                sprintf (pS, "val := (%g) [OK]\n", (double) pVS->val.v.f);
                break;
              case VTT_double:
                sprintf (pS, "val := (%g) [OK]\n", pVS->val.v.d);
                break;
              default:
                sprintf (pS, "val := (0x%08X) [OK]\n", pVS->val.v.ul);
                break;
            }
            _dlg->OutLine (OUT_TYPE_RSP, "%s", szB);           // Write the line out
            break;

          case UV_DBG_CALC_EXPRESSION:
            bResult = 1;
            pVS = (VSET *) (void *) &cmd.data.cmdRsp.vset;
            tv = pVS->val;
            switch (pVS->val.vType)  {
              case VTT_float:
                sprintf (pS, "(val := %g) [OK]\n", (double) pVS->val.v.f);
                break;
              case VTT_double:
                sprintf (pS, "(val := %g) [OK]\n", pVS->val.v.d);
                break;
              default:
                sprintf (pS, "(val := 0x%08X) [OK]\n", pVS->val.v.ul);
                break;
            }
            _dlg->OutLine (OUT_TYPE_RSP, "%s", szB);           // Write the line out
            break;

          case UV_DBG_MEM_READ:          
            sprintf (pS, " [OK]\n");
            _dlg->OutLine (OUT_TYPE_RSP, "%s", szB);           // Write the line out
            MemDump();
            break;

          case UV_DBG_SERIAL_GET:          // Serial-Out Byte/Word16
            pM = (SERIO *) (void *) &cmd.data.cmdRsp.serdat;
            n  = pM->nMany;
            sprintf (pS, "[%d], nCh=%d, n=%d: [OK]\n", pM->itemMode==0 ? 8:16, pM->nChannel, pM->nMany);
            _dlg->OutLine (OUT_TYPE_RSP, "%s", szB);           // Write the line out
            SerDump();
            break;

          case UV_DBG_CREATE_BP:          // Breakpoint successfully created
            BpMessage (0);
            break;

          case UV_PRJ_GET_OPTITEM:        // Read-Project-Option item.
            ReadOptItem();
            break;

          case UV_DBG_ADR_TOFILELINE:     // address mapped to File/Line
            pAF = &cmd.data.cmdRsp.aflm;
            if (pAF->iFile > 0) {
              sprintf (pS, "(nAdr:=0x%08X, '%s():%s(%d)') [OK]\n", 
                       (DWORD) pAF->nAdr, 
                       &pAF->szFile[pAF->iFunc], 
                       &pAF->szFile[pAF->iFile], pAF->nLine);
            }
            else {
              sprintf (pS, "(nAdr:=0x%08X, '%s():%s(%d)') [OK]\n", 
                       (DWORD) pAF->nAdr, 
                       &pAF->szFile[pAF->iFunc], 
                       "Unknown File", 
                       pAF->nLine);
            }
            _dlg->OutLine (OUT_TYPE_RSP, "%s", szB);        // Write the line out            
            break;

          case UV_PRJ_ACTIVE_FILES:
            sprintf (pS, " := %d\n", cmd.data.cmdRsp.nVal);
            _dlg->OutLine (OUT_TYPE_RSP, "%s", szB);        // Write the line out
            break;

          case UV_GEN_GET_EXTVERSION:
            sprintf (pS, "[OK]\n");
            _dlg->OutLine (OUT_TYPE_RSP, "%s", szB);        // Write the line out
            pX = &cmd.data.cmdRsp.evers;
            if (pX->iV_Sock != 0)  {
              _dlg->OutLine (OUT_TYPE_RSP, "   %s\n", &pX->szBuffer [pX->iV_Sock]);
            }
            if (pX->iV_Uv3 != 0)  {
              _dlg->OutLine (OUT_TYPE_RSP, "   %s\n", &pX->szBuffer [pX->iV_Uv3]);
            }
            break;

          default:                        // OK for generic command
            sprintf (pS, "[OK]\n");
            _dlg->OutLine (OUT_TYPE_RSP, "%s", szB);        // Write the line out
            break;
        } // switch
//      _dlg->OutLine (OUT_TYPE_RSP, "%s", szB);           // Write the line out
      } // if ok
      else {
        switch (cmd.data.cmdRsp.cmd)  {
          case UV_DBG_MEM_READ:
            pA = &cmd.data.cmdRsp.amem;
            if (cmd.data.cmdRsp.status == UV_STATUS_FAILED)  {
              sprintf (szB, "UV_DBG_MEM_READ - failed at 0x%08X, bytes read:=%d\n", pA->ErrAddr, pA->nBytes);
              _dlg->OutLine (OUT_TYPE_ERROR, "%s", szB);    // Write the line out
              break;
            }
            MemDump();
            break;

          case UV_DBG_MEM_WRITE:
            pA = &cmd.data.cmdRsp.amem;
            if (cmd.data.cmdRsp.status == UV_STATUS_FAILED)  {
              sprintf (szB, "UV_DBG_MEM_WRITE - failed at 0x%08X, bytes read:=%d\n", pA->ErrAddr, pA->nBytes);
              break;
            }
            _dlg->OutLine (OUT_TYPE_ERROR, "%s", szB);       // Write the line out
            break;

          case UV_DBG_CREATE_BP:           // Breakpoint create failed.
            BpMessage (0);
            break;

//        case UV_PRJ_ENUM_GROUPS:
//        case UV_PRJ_ENUM_GROUPS:
//          break;

          case UV_DBG_ENUMERATE_BP:        // enumerate bp request failed.
            bpc.Clear();                   // clear bp-list
            bpc.Complete();                // set as completed.

//        case UV_PRJ_SET_OPTITEM:
//---TODO:
//        case UV_PRJ_GET_OPTITEM:
                                           // fall into 'default'

          default:                         // Normal error returns
            bResult = 2;
            sprintf (pS, "[%s]", RspText());
            _dlg->OutLine (OUT_TYPE_ERROR, "%s", szB);       // Write the line out
            break;
        }   
//      _dlg->OutLine (OUT_TYPE_ERROR, "%s", szB);           // Write the line out
      }      

      break;

//---Async-Messages:
    case UV_ASYNC_MSG:   
      pS += sprintf (pS, "%s ", RspCmd());

      if (cmd.data.cmdRsp.status == UV_STATUS_SUCCESS) {
        switch (cmd.data.cmdRsp.cmd) {
          //case UV_PRJ_BUILD_OUTPUT:            // Build/Rebuild-Output received.
          //  pJ = (PRJDATA *) (void *) &cmd.data;
          //  sprintf (szB, "(%s) [OK]\n", pJ->szNames);
          //  break;
          case UV_DBG_STOP_EXECUTION:          // execution stopped
          {
            BPREASON *pR = &cmd.data.cmdRsp.StopR;
            if (pR->nRes1 == 1 && pR->nRes2 == 0 && pR->StrLen == 0)  {   // Valid Stop-Reason is present
              DumpStopReason (pR, pS);
              break;
            }            
            sprintf (pS, "[OK]\n");
            break;
          }

          case 0x12345678:
          default:                             // OK for generic command
            sprintf (pS, "[OK]\n");
            break;
        }
        _dlg->OutLine (OUT_TYPE_ASYNC, "%s", szB);           // Write the line out
      }
      else {      // Error response
        sprintf (pS, "[%s]", RspText());
        _dlg->OutLine (OUT_TYPE_ERROR, "%s", szB);           // Write the line out
      }    
      break;  
      
      
//---Async-Messages:
    case UV_PRJ_BUILD_OUTPUT:            // Build/Rebuild-Output received.
      pJ = (PRJDATA *) (void *) &cmd.data;
      _dlg->OutLine (OUT_TYPE_ASYNC, "UV3_BUILD_OUTPUT: %s\n", pJ->szNames);
      break;

    case UV_PRJ_BUILD_COMPLETE:          // Build/Rebuild/Clean completed.
      pJ = (PRJDATA *) (void *) &cmd.data;
      _dlg->OutLine (OUT_TYPE_ASYNC, "UV_PRJ_BUILD_COMPLETE: Code=%d\n", pJ->nCode);
      break;

    case UV_DBG_CALLBACK:               // Timeout-Callback fired
      _dlg->OutLine (OUT_TYPE_ASYNC, "UV_DBG_CALLBACK (time elapsed) [%s]\n", RspText());
      break;

    case UV_RTA_MESSAGE:
      //_dlg->OutLine ("ASYNC: UV_RTA_MESSAGE\n");
      _dlg->m_RTADialog.ShowRTAMsg (&cmd.data.raw[0]);
      break;

    case UV_DBG_CHANGE_BP:
      BpMessage (1);
      break;

    case UV_DBG_BP_ENUM_START:          // Breakpoint enumeration starts
      _dlg->OutLine (OUT_TYPE_ASYNC, "UV_DBG_BP_ENUM_START %s\n", RspText());
      bpc.Clear();
      break;

    case UV_DBG_BP_ENUMERATED:          // an enumerated breakpoint comes in
      _dlg->OutLine (OUT_TYPE_ASYNC, "UV_DBG_BP_ENUMERATED %s\n", RspText());
      {
        BKRSP *pR = (BKRSP *) (void *) &cmd.data.cmdRsp.brk;
        bpc.AddBp (pR);
      }
      break;

    case UV_DBG_BP_ENUM_END:            // Breakpoint enumeration completed
      _dlg->OutLine (OUT_TYPE_ASYNC, "UV_DBG_BP_ENUM_END %s\n", RspText());
      bpc.Complete();
      break;


    case UV_PRJ_ENUM_GROUPS:           // Group enumeration starts
      _dlg->OutLine (OUT_TYPE_ASYNC, "UV_PRJ_ENUM_GROUPS %s\n", RspText());
      bpc.Clear();
      break;

    case UV_PRJ_ENUM_GROUPS_START:     // Group enumeration started
      _dlg->OutLine (OUT_TYPE_ASYNC, "UV_PRJ_ENUM_GROUPS_START %s\n", RspText());
      break;

    case UV_PRJ_ENUM_GROUPS_ENU:       // an enumerated group item comes in
      _dlg->OutLine (OUT_TYPE_ASYNC, "UV_PRJ_ENUM_GROUPS_ENU %s\n", RspText());
      {
        SSTR *pR = (SSTR *) (void *) &cmd.data.cmdRsp.str;
        _dlg->OutLine (OUT_TYPE_ASYNC, "  Group := '%s'\n", pR->szStr);
      }
      break;

    case UV_PRJ_ENUM_GROUPS_END:       // Group enumeration completed
      _dlg->OutLine (OUT_TYPE_ASYNC, "UV_PRJ_ENUM_GROUPS_END %s\n", RspText());
      break;


    case UV_PRJ_ENUM_FILES:            // Files enumeration starts
      _dlg->OutLine (OUT_TYPE_ASYNC, "UV_PRJ_ENUM_FILES %s\n", RspText());
      bpc.Clear();
      break;

    case UV_PRJ_ENUM_FILES_START:      // Files enumeration started
      _dlg->OutLine (OUT_TYPE_ASYNC, "UV_PRJ_ENUM_FILES_START %s\n", RspText());
      break;

    case UV_PRJ_ENUM_FILES_ENU:        // an enumerated file item comes in
      _dlg->OutLine (OUT_TYPE_ASYNC, "UV_PRJ_ENUM_FILES_ENU %s\n", RspText());
      {
        SSTR *pR = (SSTR *) (void *) &cmd.data.cmdRsp.str;
        _dlg->OutLine (OUT_TYPE_ASYNC, "  File := '%s'\n", pR->szStr);
      }
      break;

    case UV_PRJ_ENUM_FILES_END:        // File enumeration completed
      _dlg->OutLine (OUT_TYPE_ASYNC, "UV_PRJ_ENUM_FILES_END %s\n", RspText());
      break;

//---Member-Symbol enumeration:
    case UV_DBG_ENUM_SYMTP_START:      // Start of member numeration
      _dlg->OutLine (OUT_TYPE_ASYNC, "UV_DBG_ENUM_SYMTP_START %s\n", RspText());
      break;

    case UV_DBG_ENUM_SYMTP_ENU:        // zero, one or more ENUMTPM Responses
      _dlg->OutLine (OUT_TYPE_ASYNC, "UV_DBG_ENUM_SYMTP_ENU %s\n", RspText());
      {
        ENUMTPM *pR = (ENUMTPM *) (&cmd.data.cmdRsp.tpm);
        _dlg->OutLine (OUT_TYPE_ASYNC, "  nOffs:=%d, nSize=%d, '%s'\n", pR->nOffs, pR->nSize, pR->szID);
      }
      break;

    case UV_DBG_ENUM_SYMTP_END:        // End of member enumeration
      _dlg->OutLine (OUT_TYPE_ASYNC, "UV_DBG_ENUM_SYMTP_END %s\n", RspText());
      break;


//---Stack enumeration:
    case UV_DBG_ENUM_STACK_START:      // Start of call-stack numeration
      _dlg->OutLine (OUT_TYPE_ASYNC, "UV_DBG_ENUM_STACK_START %s\n", RspText());
      break;

    case UV_DBG_ENUM_STACK_ENU:       // zero, one or more Responses
      _dlg->OutLine (OUT_TYPE_ASYNC, "UV_DBG_ENUM_STACK_ENU %s\n", RspText());
      {
        STACKENUM *pR = (STACKENUM *) (&cmd.data.cmdRsp.tpm);
        //if (pR->nLineE && pR->szNames [pR->iCallee])  {   // callee line and file are valid
        //  _dlg->OutLine (OUT_TYPE_ASYNC,   " [%d] Callee:=0x%08X (%s(%d))\n", pR->nItem, (DWORD) pR->nAdr,    &pR->szNames[pR->iCallee], pR->nLineE);
        //}
        //else  {
          _dlg->OutLine (OUT_TYPE_ASYNC,   " [%d] Callee:=0x%08X\n", pR->nItem, (DWORD) pR->nAdr);
        //}


        //if (pR->nLineR && pR->szNames [pR->iCaller])  {   // caller line and file are valid
        //  _dlg->OutLine (OUT_TYPE_ASYNC,   " [%d] Caller:=0x%08X (%s(%d))\n", pR->nItem, (DWORD) pR->nRetAdr, &pR->szNames[pR->iCaller], pR->nLineR);
        //}
        //else  {
          _dlg->OutLine (OUT_TYPE_ASYNC,   " [%d] Caller:=0x%08X\n", pR->nItem, (DWORD) pR->nRetAdr);
        //}
      }
      break;

    case UV_DBG_ENUM_STACK_END:       // End of call-stack enumeration
      _dlg->OutLine (OUT_TYPE_ASYNC, "UV_DBG_ENUM_STACK_END %s\n", RspText());
      break;


//---VTR enumeration:
    case UV_DBG_ENUM_VTR_START:       // Start of vtr numeration
      _dlg->OutLine (OUT_TYPE_ASYNC, "UV_DBG_ENUM_VTR_START %s\n", RspText());
      break;

    case UV_DBG_ENUM_VTR_ENU:         // zero, one or more Responses
      _dlg->OutLine (OUT_TYPE_ASYNC, "UV_DBG_ENUM_VTR_ENU %s\n", RspText());
      {
        AVTR *pR = (AVTR *) (&cmd.data.cmdRsp.tpm);
        _dlg->OutLine (OUT_TYPE_ASYNC, " %4d: '%s', Type:=%s\n", pR->vtrNo, pR->szName, szTyp[pR->vtrType]);

        if (pR->bValue) switch (pR->vtrType)  {  // value was requested, so show it.
          case VTT_float:
            _dlg->OutLine (OUT_TYPE_ASYNC, "       val := (%g)\n", (double) pR->value.v.f);
            break;
          case VTT_double:
            _dlg->OutLine (OUT_TYPE_ASYNC, "       val := (%g)\n", pR->value.v.d);
            break;
          default:
            _dlg->OutLine (OUT_TYPE_ASYNC, "       val := (0x%08X)\n", pR->value.v.ul);
            break;
        }
      }
      break;

    case UV_DBG_ENUM_VTR_END:        // End of call-stack enumeration
      _dlg->OutLine (OUT_TYPE_ASYNC, "UV_DBG_ENUM_VTR_END %s\n", RspText());
      break;






#if 0
    case UV3_ERROR:                        // error-message
      pU = (UERR *) (void *) &cmd.m_abBuffer[0];
      sprintf (szB, "UV3_ERROR %d: '%s'\n", pU->nCode, pU->szText);
//    _dlg->OutLine ("%s", szB);
      _dlg->OutLine ("%-32s %s", szT, szB);
      bResult = 2;
      break;

    case UV3_DEBUGSTARTED:                 // Debugger has been started
//    _dlg->OutLine ("UV3_DEBUGSTARTED\n");
      _dlg->OutLine ("%-32s UV3_DEBUGSTARTED\n", szT);
			break;

    case UV3_DEBUGSTOPPED:                 // Debugger has been terminated
      _dlg->OutLine ("%-32s UV3_DEBUGSTOPPED\n", szT);
			break;

    case UV3_EXEC_RUNNING:                 // Server: sim is executing
      _dlg->OutLine ("%-32s UV3_EXEC_RUNNING\n", szT);
			break;

    case UV3_EXEC_STOPPED:                 // Server: sim is stopped
      _dlg->OutLine ("%-32s UV3_EXEC_STOPPED\n", szT);
			break;

    case UV3_CYCLES_TIME:                  // Server: cycles and Timestamp arrived
      pT = (CYCTS *) (void *) &cmd.m_abBuffer[0];
      sprintf (szB, "%-32s UV3_CYCLES_TIME: Cyc=%I64u, tStamp=%g\n", szT, pT->cycles, pT->tStamp);
      _dlg->OutLine (szB);
      break;

    case UV3_EXECUTION_STARTED:            // Server: execution has been started
      _dlg->OutLine ("%-32s UV3_EXECUTION_STARTED\n", szT);
			break;

    case UV3_EXECUTION_STOPPED:            // Server: execution has been stopped
      _dlg->OutLine ("%-32s UV3_EXECUTION_STOPPED\n", szT);
			break;

    case UV3_TIMEINTERVAL_OK:              // Server: Time-Interval has been setup
      _dlg->OutLine ("%-32s UV3_TIMEINTERVAL_OK\n", szT);
			break;

    case UV3_INTERVAL_ELAPSED:             // Server: Time-Interaval elapsed
      _dlg->OutLine ("%-32s UV3_TIMEINTERVAL_ELAPSED\n", szT);
      MessageBeep (MB_OK);
      break;

    case UV3_RESET_ISSUED:                 // a reset has been issued
      _dlg->OutLine ("%-32s UV3_RESET_ISSUED\n", szT);
      break;

    case UV3_VTRVAL:                      // Server sent vtr-value
      pV = (TVAL *) (void *) &cmd.m_abBuffer[0];
      switch (pV->vType)  {
        case VTT_float:
          sprintf (szB, "UV3_VTRVAL := %g", (double) pV->v.f);
          break;
        case VTT_double:
          sprintf (szB, "UV3_VTRVAL := %g", pV->v.d);
          break;
        default:
          sprintf (szB, "UV3_VTRVAL := 0x%08X", pV->v.ul);
          break;
      }
      _dlg->OutLine ("%-32s %s\n", szT, szB);
      break;

    case UV3_CALCEXPR_RESULT:
      bResult = 1;
      pV = (TVAL *) (void *) &cmd.m_abBuffer[0];
      tv = *pV;
      switch (pV->vType)  {
        case VTT_float:
          sprintf (szB, "UV3_CALCEXPR_RESULT := %g", (double) pV->v.f);
          break;
        case VTT_double:
          sprintf (szB, "UV3_CALCEXPR_RESULT := %g", pV->v.d);
          break;
        default:
          sprintf (szB, "UV3_CALCEXPR_RESULT := 0x%08X", pV->v.ul);
          break;
      }
      _dlg->OutLine ("%-32s %s\n", szT, szB);
      break;

    case UV3_VTRVALSET:                  // Ack. to UV3_SETRVAL
      _dlg->OutLine ("%-32s UV3_VTRVALSET\n", szT);
      break;

    case UV3_MEMORY_READ:                // Ack. to UV3_READ_MEMORY
      _dlg->OutLine ("%-32s UV3_MEMORY_READ:\n", szT);
      MemDump();
      break;

    case UV3_MEMORY_WRITTEN:             // Ack. to UV3_WRITE_MEMORY
      _dlg->OutLine ("%-32s UV3_MEMORY_WRITTEN", szT);
      MemWritten();
      break;

    case UV3_SERIALOUT:                  // Serial-Out Byte/Word16
      SerDump();
      break;

//---new Socket Commands:  /12.1.2007/
    case UV3_BUILD_OUTPUT:
      {
        pJ = (PRJDATA *) (void *) &cmd.m_abBuffer[0];
        _dlg->OutLine ("UV3_BUILD_OUTPUT: nRes=%d | %s\n", pJ->nRes, pJ->szNames);
        _dlg->m_pgBuild.OutLine ("%s", pJ->szNames);
      }
      break;

    case UV3_BUILD_DONE:                 // Send Build/Rebuild status to Client (Errors,Warnings,Cancelled)
      {
        pJ = (PRJDATA *) (void *) &cmd.m_abBuffer[0];
        switch (pJ->nCode)  {
          case 1:  p1 = "Build/Rebuild Ok.";                  break;
          case 2:  p1 = "Build/Rebuild Ok., with warning(s)"; break;
          case 3:  p1 = "Build/Rebuild failed with error(s)"; break;
          case 4:  p1 = "Build/Rebuild was cancelled";        break;
          default: p1 = "Unknown Build/Rebuild status";       break;
        }
        _dlg->OutLine ("UV3_BUILD_DONE: nCode=%d | %s\n", pJ->nCode, p1);
        _dlg->m_pgBuild.OutLine ("%s", p1);
      }
      break;

    case UV3_GROUP_CMDOK:
      _dlg->OutLine ("UV3_GROUP_CMDOK\n");
      _dlg->m_pgBuild.OutLine ("Group command was ok.");
      break;

    case UV3_FILE_CMDOK:
      _dlg->OutLine ("UV3_FILE_CMDOK\n");
      _dlg->m_pgBuild.OutLine ("File command was ok.");
      break;

    case UV3_GROUP_CMDFAILED:
      pU = (UERR *) (void *) &cmd.m_abBuffer[0];
      sprintf (szB, "UV3_GROUP_CMDFAILED %d: '%s'\n", pU->nCode, pU->szText);
      _dlg->OutLine ("%s", szB);
      _dlg->m_pgBuild.OutLine ("Group command failed: %s", pU->szText);
//    bResult = 2;
      break; 

    case UV3_FILE_CMDFAILED:
      pU = (UERR *) (void *) &cmd.m_abBuffer[0];
      sprintf (szB, "UV3_FILE_CMDFAILED %d: '%s'\n", pU->nCode, pU->szText);
      _dlg->OutLine ("%s", szB);
      _dlg->m_pgBuild.OutLine ("File command failed: %s", pU->szText);
//    bResult = 2;
      break; 


    case UV3_RTA_DCCMESSAGE:              //
      _dlg->m_RTADialog.ShowRTAMsg(&cmd.m_abBuffer[0]);
#if 0
      static char bstr[1000];
      char            *pbstr;
      unsigned int         i;
      int              ibstr;

      pJ   = (PRJDATA *) (void *) &cmd.m_abBuffer[0];
      pRTA = (rtah_msg_t *) (void *) pJ->szNames;

      // Lets process the message some if we know it
      switch (pRTA->header.split.type) {
        case 0x20:
          //if (_dlg->m_checkTrace.GetCheck() == BST_CHECKED) {
            _dlg->OutLine ("RTA (State): Clk = 0x%08X, Phase = %01X", 
                        pRTA->data[0],
                        pRTA->data[1]);
          //}
        break;

        case 0x21:
          //if (_dlg->m_checkState.GetCheck() == BST_CHECKED) {
            _dlg->OutLine ("RTA (Trace): %s()", 
                        (unsigned char *)&pRTA->data[0]);
          //}
        break;

        default:
        break; // default
      }

      // Print raw data
      //if (_dlg->m_checkRaw.GetCheck() == BST_CHECKED) {
        _dlg->OutLine ("UV3_RTA_MESSAGE: tcplen=%d route=0x%02X type=0x%02X seq=0x%02X intlen=0x%02X", 
                        pJ->nRes, 
                        pJ->nCode, 
                        pRTA->header.split.type, 
                        pRTA->header.split.seq_no, 
                        pRTA->header.split.length);
        // Print data from DCC message
        pbstr = bstr;
        for ( i = 0 ; i < pRTA->header.split.length ; ++i )  {
          if ((i % 8) == 0)  {
            if (i == 0) ibstr = sprintf (pbstr, " : ");
            else        ibstr = sprintf (pbstr, "\r\n : ");
            pbstr += ibstr;
          }
          ibstr  = sprintf (pbstr, " 0x%08X", pRTA->data[i]);
          pbstr += ibstr;
        }
        sprintf (pbstr, "\n");
        _dlg->OutLine (bstr);  
      //} // if
#endif
    break; // case UV3_RTA_DCCMESSAGE:

//--------------------------------------------------------


#endif
    default:                             // Error: unknown socket command
      _dlg->OutLine (OUT_TYPE_ERROR, "%-32s Unknown socket-command: 0x%04X, totlen:=%d", szT, cmd.m_eCmd, cmd.m_nTotalLen);
      break;
  }
}



/*
 * A socket-command is coming in
 */
void CUv3ClientSocket::OnReceive (int nErrorCode)  {
  int    nRead;

  static bool bMsgHeader = true;
  static bool  bMsgStart = true;
  static unsigned char *pCmdMsg;
  static UINT   msgLenRemaining;
  static UINT   headerRemaining;
  
  // We break this loop when all the data has be retrieved from the socket, or there was a socket error
  //while (1) {
    // Read the 'header' (ie the first 4 bytes representing the remaining data length)
    if (bMsgHeader) {
      if (bMsgStart) {
        pCmdMsg = (unsigned char *)&cmd;
        headerRemaining = sizeof(UINT);
        bMsgStart = false;
      }

//---12.3.2007: clear message first to avoid fooling RspText():
//    int n = sizeof (UVSOCK_CMD) + sizeof (UVSOCK_STATUS) + offsetof (UVSOCK_ERROR_RESPONSE, str) + 32;
      memset (&cmd, 0, 128);
//---
      
      nRead = Receive (pCmdMsg, headerRemaining);   // read the message total length

      if (nRead != SOCKET_ERROR)  {
        if (nRead == headerRemaining)  {
          pCmdMsg += nRead;
          msgLenRemaining = cmd.m_nTotalLen-sizeof(UINT);     // The total length word IS included in the total length
          bMsgHeader = false;
        }
        else if (nRead == 0)  {
          // Connection was closed by server
          //break;
        }
        else  {
          // Just carry on looping, reading data messages from the buffer
          pCmdMsg += nRead;                         // Record buffer pointer for next loop
          headerRemaining -= nRead;                 // Record length remaining for next loop
        }
      }

      else  {
        if (GetLastError() == WSAEWOULDBLOCK) {
          // There is no more data in the socket buffer
        }
        else {
          // There was a real socket error - we should handle this (probably auto-reconnect)
        }
        //break;
      }      
    }
    else  {
      nRead = Receive (pCmdMsg, msgLenRemaining);   // read message data (all or some)
      
      if (nRead != SOCKET_ERROR) {
        if (nRead == msgLenRemaining) {
          ProcessMessage();                         // Message complete - process it
          bMsgHeader = true;
          bMsgStart  = true;
        }
        else if (nRead == 0) {                      // Connection was closed by server
          //break;
        }
        else {                                      // Just carry on looping, reading data messages from the buffer
          pCmdMsg += nRead;                         // Record buffer pointer for next loop
          msgLenRemaining -= nRead;                 // Record length remaining for next loop
        }
      }
      else  {                                       // Out of data in this OnReceive, break out of the loop   
        if (GetLastError() == WSAEWOULDBLOCK)  {
        }
        else  {                                     // There was a real socket error -
                                                    // we should handle this (probably auto-reconnect)
        }
        //break;
      }
    }
  //}

#if 0
    case UV3_ERROR:                        // error-message
      pU = (UERR *) (void *) &cmd.m_abBuffer[0];
      sprintf (szB, "UV3_ERROR %d: '%s'\n", pU->nCode, pU->szText);
//    _dlg->OutLine ("%s", szB);
      _dlg->OutLine ("%-32s %s", szT, szB);
      bResult = 2;
      break;

    case UV3_DEBUGSTARTED:                 // Debugger has been started
//    _dlg->OutLine ("UV3_DEBUGSTARTED\n");
      _dlg->OutLine ("%-32s UV3_DEBUGSTARTED\n", szT);
			break;

    case UV3_DEBUGSTOPPED:                 // Debugger has been terminated
      _dlg->OutLine ("%-32s UV3_DEBUGSTOPPED\n", szT);
			break;

    case UV3_EXEC_RUNNING:                 // Server: sim is executing
      _dlg->OutLine ("%-32s UV3_EXEC_RUNNING\n", szT);
			break;

    case UV3_EXEC_STOPPED:                 // Server: sim is stopped
      _dlg->OutLine ("%-32s UV3_EXEC_STOPPED\n", szT);
			break;

    case UV3_CYCLES_TIME:                  // Server: cycles and Timestamp arrived
      pT = (CYCTS *) (void *) &cmd.m_abBuffer[0];
      sprintf (szB, "%-32s UV3_CYCLES_TIME: Cyc=%I64u, tStamp=%g\n", szT, pT->cycles, pT->tStamp);
      _dlg->OutLine (szB);
      break;

    case UV3_EXECUTION_STARTED:            // Server: execution has been started
      _dlg->OutLine ("%-32s UV3_EXECUTION_STARTED\n", szT);
			break;

    case UV3_EXECUTION_STOPPED:            // Server: execution has been stopped
      _dlg->OutLine ("%-32s UV3_EXECUTION_STOPPED\n", szT);
			break;

    case UV3_TIMEINTERVAL_OK:              // Server: Time-Interval has been setup
      _dlg->OutLine ("%-32s UV3_TIMEINTERVAL_OK\n", szT);
			break;

    case UV3_INTERVAL_ELAPSED:             // Server: Time-Interaval elapsed
      _dlg->OutLine ("%-32s UV3_TIMEINTERVAL_ELAPSED\n", szT);
      MessageBeep (MB_OK);
      break;

    case UV3_RESET_ISSUED:                 // a reset has been issued
      _dlg->OutLine ("%-32s UV3_RESET_ISSUED\n", szT);
      break;

    case UV3_VTRVAL:                      // Server sent vtr-value
      pV = (TVAL *) (void *) &cmd.m_abBuffer[0];
      switch (pV->vType)  {
        case VTT_float:
          sprintf (szB, "UV3_VTRVAL := %g", (double) pV->v.f);
          break;
        case VTT_double:
          sprintf (szB, "UV3_VTRVAL := %g", pV->v.d);
          break;
        default:
          sprintf (szB, "UV3_VTRVAL := 0x%08X", pV->v.ul);
          break;
      }
      _dlg->OutLine ("%-32s %s\n", szT, szB);
      break;

    case UV3_CALCEXPR_RESULT:
      bResult = 1;
      pV = (TVAL *) (void *) &cmd.m_abBuffer[0];
      tv = *pV;
      switch (pV->vType)  {
        case VTT_float:
          sprintf (szB, "UV3_CALCEXPR_RESULT := %g", (double) pV->v.f);
          break;
        case VTT_double:
          sprintf (szB, "UV3_CALCEXPR_RESULT := %g", pV->v.d);
          break;
        default:
          sprintf (szB, "UV3_CALCEXPR_RESULT := 0x%08X", pV->v.ul);
          break;
      }
      _dlg->OutLine ("%-32s %s\n", szT, szB);
      break;

    case UV3_VTRVALSET:                  // Ack. to UV3_SETRVAL
      _dlg->OutLine ("%-32s UV3_VTRVALSET\n", szT);
      break;

    case UV3_MEMORY_READ:                // Ack. to UV3_READ_MEMORY
      _dlg->OutLine ("%-32s UV3_MEMORY_READ:\n", szT);
      MemDump();
      break;

    case UV3_MEMORY_WRITTEN:             // Ack. to UV3_WRITE_MEMORY
      _dlg->OutLine ("%-32s UV3_MEMORY_WRITTEN", szT);
      MemWritten();
      break;

    case UV3_SERIALOUT:                  // Serial-Out Byte/Word16
      SerDump();
      break;

//---new Socket Commands:  /12.1.2007/
    case UV3_BUILD_OUTPUT:
      {
        pJ = (PRJDATA *) (void *) &cmd.m_abBuffer[0];
        _dlg->OutLine ("UV3_BUILD_OUTPUT: nRes=%d | %s\n", pJ->nRes, pJ->szNames);
        _dlg->m_pgBuild.OutLine ("%s", pJ->szNames);
      }
      break;

    case UV3_BUILD_DONE:                 // Send Build/Rebuild status to Client (Errors,Warnings,Cancelled)
      {
        pJ = (PRJDATA *) (void *) &cmd.m_abBuffer[0];
        switch (pJ->nCode)  {
          case 1:  p1 = "Build/Rebuild Ok.";                  break;
          case 2:  p1 = "Build/Rebuild Ok., with warning(s)"; break;
          case 3:  p1 = "Build/Rebuild failed with error(s)"; break;
          case 4:  p1 = "Build/Rebuild was cancelled";        break;
          default: p1 = "Unknown Build/Rebuild status";       break;
        }
        _dlg->OutLine ("UV3_BUILD_DONE: nCode=%d | %s\n", pJ->nCode, p1);
        _dlg->m_pgBuild.OutLine ("%s", p1);
      }
      break;

    case UV3_GROUP_CMDOK:
      _dlg->OutLine ("UV3_GROUP_CMDOK\n");
      _dlg->m_pgBuild.OutLine ("Group command was ok.");
      break;

    case UV3_FILE_CMDOK:
      _dlg->OutLine ("UV3_FILE_CMDOK\n");
      _dlg->m_pgBuild.OutLine ("File command was ok.");
      break;

    case UV3_GROUP_CMDFAILED:
      pU = (UERR *) (void *) &cmd.m_abBuffer[0];
      sprintf (szB, "UV3_GROUP_CMDFAILED %d: '%s'\n", pU->nCode, pU->szText);
      _dlg->OutLine ("%s", szB);
      _dlg->m_pgBuild.OutLine ("Group command failed: %s", pU->szText);
//    bResult = 2;
      break; 

    case UV3_FILE_CMDFAILED:
      pU = (UERR *) (void *) &cmd.m_abBuffer[0];
      sprintf (szB, "UV3_FILE_CMDFAILED %d: '%s'\n", pU->nCode, pU->szText);
      _dlg->OutLine ("%s", szB);
      _dlg->m_pgBuild.OutLine ("File command failed: %s", pU->szText);
//    bResult = 2;
      break; 


    case UV3_RTA_DCCMESSAGE:              //
      _dlg->m_RTADialog.ShowRTAMsg(&cmd.m_abBuffer[0]);
#if 0
      static char bstr[1000];
      char            *pbstr;
      unsigned int         i;
      int              ibstr;

      pJ   = (PRJDATA *) (void *) &cmd.m_abBuffer[0];
      pRTA = (rtah_msg_t *) (void *) pJ->szNames;

      // Lets process the message some if we know it
      switch (pRTA->header.split.type) {
        case 0x20:
          //if (_dlg->m_checkTrace.GetCheck() == BST_CHECKED) {
            _dlg->OutLine ("RTA (State): Clk = 0x%08X, Phase = %01X", 
                        pRTA->data[0],
                        pRTA->data[1]);
          //}
        break;

        case 0x21:
          //if (_dlg->m_checkState.GetCheck() == BST_CHECKED) {
            _dlg->OutLine ("RTA (Trace): %s()", 
                        (unsigned char *)&pRTA->data[0]);
          //}
        break;

        default:
        break; // default
      }

      // Print raw data
      //if (_dlg->m_checkRaw.GetCheck() == BST_CHECKED) {
        _dlg->OutLine ("UV3_RTA_MESSAGE: tcplen=%d route=0x%02X type=0x%02X seq=0x%02X intlen=0x%02X", 
                        pJ->nRes, 
                        pJ->nCode, 
                        pRTA->header.split.type, 
                        pRTA->header.split.seq_no, 
                        pRTA->header.split.length);
        // Print data from DCC message
        pbstr = bstr;
        for ( i = 0 ; i < pRTA->header.split.length ; ++i )  {
          if ((i % 8) == 0)  {
            if (i == 0) ibstr = sprintf (pbstr, " : ");
            else        ibstr = sprintf (pbstr, "\r\n : ");
            pbstr += ibstr;
          }
          ibstr  = sprintf (pbstr, " 0x%08X", pRTA->data[i]);
          pbstr += ibstr;
        }
        sprintf (pbstr, "\n");
        _dlg->OutLine (bstr);  
      //} // if
#endif
    break; // case UV3_RTA_DCCMESSAGE:

//--------------------------------------------------------


#endif

  CAsyncSocket::OnReceive (nErrorCode);
}




void CPwOptionsDlg::Send (UVSOCK_CMD *psCmd)  {
  m_oSocket.Send(psCmd, psCmd->m_nTotalLen);
}


void CPwOptionsDlg::OnSend()  {
}
void CPwOptionsDlg::OnReceive()  {
}



UVSOCK_CMD *CreateCommand (UV_OPERATION eCmd, int nLen, const void *pData)  {
  cmd.m_eCmd      = eCmd;
  cmd.m_nBufLen   = nLen;
  cmd.m_nTotalLen = (sizeof (UVSOCK_CMD) - SOCK_NDATA) + nLen;
  cmd.cycles      = 0;
  cmd.tStamp      = 0;
  if (nLen > 0)  {
    memcpy (&cmd.data, pData, nLen);
  }
  return (&cmd);
}


/*
 * Create and Send a Socket-Command
 */

void CPwOptionsDlg::CreateCommandSend (UV_OPERATION eCmd, int nLen, const void *pData)  {
  UVSOCK_CMD  *pCmd;

  QueryPerformanceCounter(&startTick);

  pCmd = CreateCommand (eCmd, nLen, pData);
  Send (pCmd);
}



void CPwOptionsDlg::SendCommand (UV_OPERATION nCmd, int nLen, void *pData)  {
#if 0
  UVSOCK_CMD  *psCmd;

  switch (nCmd)  {
    case UV3_READ_PROJECTNAME:
      psCmd = UVSOCK_CMD::CreateCommand (nCmd);
      Send (psCmd);
      break;
  }
#endif
}






static TV_INSERTSTRUCT  tvIns;
static TV_ITEM            tvI;

#define mTVI  (TVIF_TEXT | TVIF_PARAM /*| TVIF_IMAGE | TVIF_SELECTEDIMAGE*/ )

HTREEITEM CPwOptionsDlg::AddOne (CTreeCtrl  &tree,
                                 HTREEITEM hParent,
                                 LPSTR       pText,
                                 HTREEITEM  hAfter,
//                               int iImage,
//                               int iSelImg,
                                 LPARAM     lpUser)  {
  HTREEITEM        hItem;

  memset (&tvI, 0, sizeof (tvI));
  tvI.mask           = mTVI;         // text member is valid
  tvI.pszText        = pText;        // name
  tvI.cchTextMax     = strlen (pText) + 1;
//tvI.iImage         = iImage;       // future use
//tvI.iSelectedImage = iSelImg;      // future use
  tvI.lParam         = lpUser;       // lParam
  tvIns.item         = tvI;
  tvIns.hInsertAfter = hAfter;
  tvIns.hParent      = hParent;      // parent

  hItem = tree.InsertItem (&tvIns);
  return (hItem);
}



BOOL CPwOptionsDlg::OnInitDialog()  {
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


    // Set up the log box
    m_LogOut.SetFont(80, "Lucida Console");
    m_LogOut.AddString(new TraceMessage(TraceEvent::None, "SOCKET TESTER LOG")); 
    m_LogOut.setLimit(500);
    m_LogOut.enableLimit(TRUE);
    //m_LogOut.setToDisk(TRUE, "default_log.txt");



    //hRoot     = AddOne (mc_tree, TVI_ROOT,  "Profiles",    TVI_LAST, NULL);
    //hIdentify = AddOne (mc_tree, hRoot,     "Control",     TVI_LAST, NULL);
    //hBuild    = AddOne (mc_tree, hRoot,     "Make",        TVI_LAST, NULL);
    //hDebug    = AddOne (mc_tree, hRoot,     "Debug",       TVI_LAST, NULL);
//  hTrgMsg   = AddOne (mc_tree, hRoot, "TargetMessaging", TVI_LAST, NULL);
    //hTrgInfo  = AddOne (mc_tree, hRoot,     "TargetInfo",  TVI_LAST, NULL);

    AddOne (mc_tree, TVI_ROOT/*hBuild*/,   "uVision Control", TVI_LAST, IDD_TARGMESS);
    AddOne (mc_tree, TVI_ROOT/*hIdentify*/, "Project",     TVI_LAST, IDD_IDENTIFY);  // Page #1
    
    AddOne (mc_tree, TVI_ROOT/*hBuild*/,   "Build", TVI_LAST, IDD_MAKE);
    AddOne (mc_tree, TVI_ROOT/*hDebug*/,   "Debug", TVI_LAST, IDD_DEBUG);
//  AddOne (mc_tree, hTrgMsg,  "Target Messaging Events", TVI_LAST, IDD_TARGMESS);
    AddOne (mc_tree, TVI_ROOT/*hTrgInfo*/, "Testing",      TVI_LAST, IDD_TARGINFO);

    //mc_tree.Expand (hRoot,     TVE_EXPAND);
    //mc_tree.Expand (hIdentify, TVE_EXPAND);
    //mc_tree.Expand (hBuild,    TVE_EXPAND);
    //mc_tree.Expand (hDebug,    TVE_EXPAND);
//  mc_tree.Expand (hTrgMsg,   TVE_EXPAND);
    //mc_tree.Expand (hTrgInfo,  TVE_EXPAND);


    //->STEP: 
    //Create the child windows
    //note: you can create the child windows on demand, if you like,
    //I find it easier to do it at once when initializing the
    //dialog.
    //If you have too much (possible) child windows you may find it
    //easier to use a COject array.

//--- Create the sheets as modeless inlay:
//  aoSheet1.Create(IDD_SHEET_1, this);
//  aoSheet2.Create(IDD_SHEET_2, this);
//  aoSheet3.Create(IDD_SHEET_3, this);


    m_pgIdentify.Create (IDD_IDENTIFY, this);    // create Identify Page
    m_pgBuild.Create (IDD_MAKE, this);           // Make Page
    m_pgDebug.Create (IDD_DEBUG, this);          // Debug Page
    m_pgTrgMsg.Create (IDD_TARGMESS, this);      // Target Mesaging Page
    m_pgTargInfo.Create (IDD_TARGINFO, this);    // Target Info Page

    //Do NOT call "PositionChild from here because the window is
    //not created/shown yet and the retrieved positions will be
    //different from the positions the window is actually shown!

    //I use a identifier to keep track of which window is currently
    //displayed:
    //This is especially useful if you use a CObject array for
    //your child windows.
    anCurrentChildWindow = 1;

/////////////////////////////
    OnRefresh();           // Enumerate currently available client(s)
/////////////////////////////

    UINT nL = mc_Edit.GetLimitText();   // 15.10.2002, ca. 32K default.
    nL = 0x810000;                      // 512K+64K
    mc_Edit.SetLimitText (nL);          // ==========

// Change the font of the dialogbox
//  CFont Font;
//  Font.CreateFontIndirect (&xlFont);  // create the Font
//  mc_Edit.SetFont (&Font);
    m_EditFont.CreatePointFont (80, _T("Lucida Console")); // _T("Courier New"));
    mc_Edit.SetFont (&m_EditFont);


    // Start UV3 connection attempts
    ConnectUV3();

    return TRUE;  // return TRUE  unless you set the focus to a control
}



//->STEP:
//Create a method which is capable to move all the child windows
//in relation to the parent window.
//This method needs to be called from the OnMovemessage handler.
//In addition (if you want one of the windows to be displayed
//when the parent dialog is displayed, you need to add a
//OnShowWindow handler which calls this method.
void CPwOptionsDlg::PositionChild()  {
  CRect  mRec;
    
    //the sheet position should be right of the tree control we use
    //the standard member (a CStatic) we defined at the position
    //which upper, left corner should be identical to the inlay
    //window position. (We could use the tree control to compute
    //the position but it is easier to reuse a dummy windows
    //coordinates:-)
    mc_dummy.GetWindowRect (&mRec);

    //Recompute coordinates relative to parent window.
    //NOTE: you need to define the style of the child windows as "Child"
    //if you leave it as "popup" you will need to remove this
    //ScreenToClient recomputing.
    //But if you do so you will see the parent window affected by
    //the title bar getting disabled once the "child" window has the
    //focus.
    ScreenToClient (&mRec);

#if 0
    //Now move the child window to be at the correct position:
    //note: if you do not want your child window to become the
    //same size your dummy control has, you need to 
    aoSheet1.MoveWindow(mRec.left, 
                        mRec.top, 
                        mRec.Width(), 
                        mRec.Height(), 
                        TRUE);

    //Now move the child window to be at the correct position:
    aoSheet2.MoveWindow(mRec.left, 
                        mRec.top, 
                        mRec.Width(), 
                        mRec.Height(), 
                        TRUE);

    //Now move the child window to be at the correct position:
    aoSheet3.MoveWindow(mRec.left, 
                        mRec.top, 
                        mRec.Width(), 
                        mRec.Height(), 
                        TRUE);
#endif

//---Move the child window to be at the correct position:
    m_pgIdentify.MoveWindow (mRec.left, mRec.top, mRec.Width(), mRec.Height(), TRUE);
    m_pgBuild.MoveWindow (mRec.left, mRec.top, mRec.Width(), mRec.Height(), TRUE);
    m_pgDebug.MoveWindow (mRec.left, mRec.top, mRec.Width(), mRec.Height(), TRUE);
    m_pgTrgMsg.MoveWindow (mRec.left, mRec.top, mRec.Width(), mRec.Height(), TRUE);
    m_pgTargInfo.MoveWindow (mRec.left, mRec.top, mRec.Width(), mRec.Height(), TRUE);
}



/*
 * Show Dialog Page with Index 'nToShow'
 */

void CPwOptionsDlg::ShowChild (int nToShow)  {   //in: the window pos to show
  m_pgIdentify.ShowWindow (SW_HIDE);        // first, hide all Dlg-Pages:
  m_pgBuild.ShowWindow (SW_HIDE);
  m_pgDebug.ShowWindow (SW_HIDE);
  m_pgTrgMsg.ShowWindow (SW_HIDE);
  m_pgTargInfo.ShowWindow (SW_HIDE);

  switch (nToShow)  {
    case IDD_IDENTIFY:
      m_pgIdentify.ShowWindow (SW_SHOWNORMAL);
      break;
    case IDD_MAKE:
      m_pgBuild.ShowWindow (SW_SHOWNORMAL);
      break;
    case IDD_DEBUG:
      m_pgDebug.ShowWindow (SW_SHOWNORMAL);
      break;
    case IDD_TARGMESS:
      m_pgTrgMsg.ShowWindow (SW_SHOWNORMAL);
      break;
    case IDD_TARGINFO:
      m_pgTargInfo.ShowWindow (SW_SHOWNORMAL);
      break;
  }
}


void CPwOptionsDlg::OnClearLog (void) {
  m_LogOut.ResetContent();
}

void CPwOptionsDlg::OnMove (int x, int y)  {
  CDialog::OnMove(x, y);
//Avoid to call on window pre creation (which is also calling OnMove):
  if (IsWindowVisible())  {
    PositionChild();
  }
}



void CPwOptionsDlg::OnShowWindow (BOOL bShow, UINT nStatus)  {
  CDialog::OnShowWindow (bShow, nStatus);

// No need to recompute if window is going to be hidden or minimized:
  if (bShow)  {
    PositionChild();	
  }
}



void CPwOptionsDlg::OnSysCommand (UINT nID, LPARAM lParam)  {
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


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPwOptionsDlg::OnPaint()  {
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



// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPwOptionsDlg::OnQueryDragIcon()  {
  return (HCURSOR) m_hIcon;
}




//This method is only to demonstrate how the tree control does select
//one of the options child windows to be displayed:
void CPwOptionsDlg::OnDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult)  {
  *pResult = 0;
}


void CPwOptionsDlg::OnClickTree1(NMHDR* pNMHDR, LRESULT* pResult)  {
  *pResult = 0;
}




void CPwOptionsDlg::OnSelchangedTree1 (NMHDR* pNMHDR, LRESULT* pResult)  {
  HTREEITEM  hSel;
  DWORD        nR;

  hSel = mc_tree.GetSelectedItem();                // CurSelItem
  nR   = mc_tree.GetItemData (hSel);               // IdemData (Dialog Resource-ID)
//OutLine ("OnSelchangedTree1: nR = 0x%08X", nR);
  if (nR != NULL)  {         // some new Page was selected
    ShowChild (nR);
  }
  *pResult = 0;
}


void CPwOptionsDlg::OutLine (OUT_TYPE type, char *fmt, ...)  {
  char  szB [1028];
  va_list    argp;
  int n;

  va_start (argp, fmt);
  if (argp == NULL)  {         //  || argp[0] == 0)  {
    n = min (strlen (fmt), sizeof (szB) - 4);
    memcpy (szB, fmt, n);
  }
  else  {
    n = _vsnprintf (szB, sizeof (szB) - 4, fmt, argp);
  }
  szB [n] = 0;

  //if (n > 0 && szB [n - 1] == '\n') --n;
  //if (n > 0 && szB [n - 1] == '\r') --n;
  //szB [n++] = '\r';
  //szB [n++] = '\n';
  //szB [n]   = 0;

  switch (type) {
    case OUT_TYPE_GEN:
      m_LogOut.AddString(new TraceWarning(TraceEvent::None, szB)); 
      break;
    case OUT_TYPE_RSP:
      m_LogOut.AddString(new TraceComment(TraceEvent::None, szB)); 
    break;
    case OUT_TYPE_ASYNC:
      m_LogOut.AddString(new TraceMessage(TraceEvent::None, szB)); 
    break;
    case OUT_TYPE_ERROR:
      m_LogOut.AddString(new TraceError(TraceEvent::None, szB)); 
    break;
    case OUT_TYPE_DATA:
      m_LogOut.AddString(new TraceData(TraceEvent::None, szB)); 
    break;
    default:
      m_LogOut.AddString(new TraceError(TraceEvent::None, szB)); 
  }
}


void CPwOptionsDlg::OnBnClickedButtonRTA()  {  
  m_RTADialog.ShowWindow(SW_SHOWNORMAL);
}



void CPwOptionsDlg::OnRefresh()  {
  ClearClients();

//dtc_Identify::WhoIs wi (dtc_EventManager::GetApplID());  // 'WhoIs' Event

#if 0
  dtc_Identify::WhoIs wi;            // 'WhoIs' Event
  bool bOk = wi.Send();              // Send it.
  if (!bOk)  {
    OutLine (">>> No Client(s) are running ! <<<");
  }
#endif
}



/*
 * Clear Clients Tree-Control
 */

void ClearClients (void)  {
  _dlg->mc_clients.DeleteAllItems();       // first, clear Client tree control 
}






/*
 * DTC-Events relevant Functions
 */



#if 0
/*
 * here are the declarations for the dtc Events:
 */

class Ev_Project : public dtc_Identify::Project::Handler  {
public:
  void Handle (dtc_Identify::Project& ev);
} Ev_PROJECT;

class Ev_TargInfo : public dtc_TargetInfo::TargetInfo::Handler  {
public:
  void Handle (dtc_TargetInfo::TargetInfo& ev);
} Ev_Tinfo;

class Ev_BuildInfo : public dtc_Make::BuildInfo::Handler  {
public:
  void Handle (dtc_Make::BuildInfo& evt);
} EV_BUILDINFO;

class Ev_BuildEnd : public dtc_Make::BuildEnd::Handler  {
public:
  void Handle (dtc_Make::BuildEnd& evt);
} EV_BUILDEND;

class Ev_PrjFile : public dtc_Make::ProjectFile::Handler  {
public:
  void Handle (dtc_Make::ProjectFile& evt);
} EV_PRJFILE;

class Ev_SetStatus : public dtc_Make::SetStatus::Handler  {
public:
  void Handle (dtc_Make::SetStatus& evt);
} EV_SETSTATUS;


class Ev_DBError : public dtc_Debug::Error::Handler  {
public:
  void Handle (dtc_Debug::Error& evt);
} EV_DBGERROR;

class Ev_DbgStarted : public dtc_Debug::DebuggerStarted::Handler  {
public:
  void Handle (dtc_Debug::DebuggerStarted& evt);
} EV_DBGSTARTED;

class Ev_DbgStopped : public dtc_Debug::DebuggerStopped::Handler  {
public:
  void Handle (dtc_Debug::DebuggerStopped& evt);
} EV_DBGSTOPPED;

class Ev_DbgExecStarted : public dtc_Debug::ExecutionStarted::Handler  {
public:
  void Handle (dtc_Debug::ExecutionStarted& evt);
} EV_DBGEXECSTARTED;

class Ev_ExecStop : public dtc_Debug::ExecutionStopped::Handler  {
public:
  void Handle (dtc_Debug::ExecutionStopped& evt);
} EV_DBGEXECSTOPPED;

class Ev_RegVal : public dtc_Debug::RegisterValue::Handler  {
public:
  void Handle (dtc_Debug::RegisterValue& evt);
} EV_DBGREGVAL;

class Ev_RegWritten : public dtc_Debug::RegisterWritten::Handler  {
public:
  void Handle (dtc_Debug::RegisterWritten& evt);
} EV_DBGREGWRITTEN;

class Ev_MemAddr : public dtc_Debug::MemoryAddress::Handler  {
public:
  void Handle (dtc_Debug::MemoryAddress& evt);
} EV_DBGMEMADR;

class EV_MemCont : public dtc_Debug::MemoryContents::Handler  {
public:
  void Handle (dtc_Debug::MemoryContents& evt);
} EV_DBGMEMCONT;

class Ev_MemWritten : public dtc_Debug::MemoryWritten::Handler  {
public:
  void Handle (dtc_Debug::MemoryWritten& evt);
} EV_DBGMEMWRITTEN;

class Ev_BpSet : public dtc_Debug::BreakpointSet::Handler  {
public:
  void Handle (dtc_Debug::BreakpointSet& evt);
} Ev_DBGSETBP;

class Ev_BpCleared : public dtc_Debug::BreakpointCleared::Handler  {
public:
  void Handle (dtc_Debug::BreakpointCleared& evt);
} Ev_DBGBPCLEARED;



class Ev_MsgFromTarget : public dtc_TargetMessaging::MsgFromTarget::Handler  {
public:
  void Handle (dtc_TargetMessaging::MsgFromTarget& evt);
} Ev_MSGFROMTARGET;

class Ev_TmsgAttach : public dtc_TargetMessaging::Connected::Handler  {
public:
  void Handle (dtc_TargetMessaging::Connected& evt);
} Ev_TM_ATTACHED;

class Ev_TmsgDetach : public dtc_TargetMessaging::Disconnected::Handler  {
public:
  void Handle (dtc_TargetMessaging::Disconnected& evt);
} Ev_TM_DETACHED;



class Ev_GoodBy : public dtc_Identify::Goodbye::Handler  {
public:
  void Handle (dtc_Identify::Goodbye& evt);
} Ev_GOODBY;

class Ev_DecodedMA : public dtc_Debug::DecodedMemAddress::Handler  {
public:
  void Handle (dtc_Debug::DecodedMemAddress& evt);
} Ev_DBGDECODEDMEMA;
#endif




/*
 * Register the Events
 */

void RegisterEvents (void)  {
#if 0
//---8.4.2003: this is required before any other dtc Event !!!
  if (!dtc_EventManager::ActivateIPDispatcher())  {
    AfxMessageBox ("Could not Initialize ActivateIPDispatcher() !",
                    MB_APPLMODAL | MB_ICONEXCLAMATION);
    return;
  }
//---

  EV_BUILDINFO.Register();
  EV_BUILDEND.Register();
  EV_SETSTATUS.Register();
  EV_PRJFILE.Register();
  Ev_PROJECT.Register();
  Ev_GOODBY.Register();

  EV_DBGERROR.Register();
  EV_DBGSTARTED.Register();
  EV_DBGSTOPPED.Register();
  EV_DBGEXECSTOPPED.Register();
  EV_DBGEXECSTARTED.Register();
  EV_DBGREGVAL.Register();
  EV_DBGREGWRITTEN.Register();
  EV_DBGMEMCONT.Register();
  EV_DBGMEMWRITTEN.Register();
  EV_DBGMEMADR.Register();
  Ev_DBGSETBP.Register();
  Ev_DBGBPCLEARED.Register();
  Ev_DBGDECODEDMEMA.Register();

  Ev_MSGFROMTARGET.Register();       // dtc_TargetMessaging Message from Target Event
  Ev_TM_ATTACHED.Register();         // dtc_TargetMessaging connected Event
  Ev_TM_DETACHED.Register();         // dtc_TargetMessaging disconnected Event

  Ev_Tinfo.Register();               // dtc_TargetInfo Event
#endif
}



#if 0
/*
 * dtc_Identify::Receiver for 'IAm' Events
 */
void CPwOptionsDlg::Ev_Id_IAm::Handle (dtc_Identify::IAm& evt)  {
  char         *p1;
  HTREEITEM  hRoot;

  int serverid = evt.GetSource();

  SetEvent (hTm);                   // Signal Response
//--- Get client's product name:
  p1 = (char *) evt.GetProductName().c_str();
  hRoot = _dlg->AddOne (_dlg->mc_clients, TVI_ROOT, p1, TVI_LAST, NULL);

//--- Get client's list of supported profiles:
  dtc_Identify::IAm::Profiles::const_iterator it;
  for ( it = evt.GetProfiles().begin() ; it != evt.GetProfiles().end() ; ++it)  {
    p1 = (char *) (*it).c_str();
    _dlg->AddOne (_dlg->mc_clients, hRoot, p1, TVI_LAST, NULL);
  }
  _dlg->mc_clients.Expand (hRoot, TVE_EXPAND);
  _dlg->mc_clients.Invalidate();
}
#endif



#if 0

/*
 * dtc_Make::BuildInfo - Receiver
 */

void Ev_BuildInfo::Handle (dtc_Make::BuildInfo& evt)  {
  SetEvent (hTm);                   // Signal Response
  char *p1 = (char *) evt.GetText().c_str();
  char *p2 = (char *) evt.GetFile().c_str();  // name (may be empty)
  int   ln = evt.GetLine();                   // -1 := not specified

  if (ln != -1 && p2 && p2[0])  {             // everything is valid
    _dlg->OutLine ("dtc_Make::BuildInfo: %s(%d): %s", p2, ln, p1);
  }
  else  {                                     // GetText() is valid
    if (!p1 || !p1[0]) p1 = "<?_?_?_>";
    _dlg->OutLine ("dtc_Make::BuildInfo:  %s", p1);
  }
}


/*
 * dtc_Make::BuildEnd - Receiver
 */

void Ev_BuildEnd::Handle (dtc_Make::BuildEnd& evt)  {
  SetEvent (hTm);                   // Signal Response
  switch (evt.GetReason())  {
    case dtc_Make::BuildEnd::success:
      _dlg->OutLine ("dtc_Make::BuildEnd - Build Successful");
      break;
    case dtc_Make::BuildEnd::error:
      _dlg->OutLine ("dtc_Make::BuildEnd - Build has Error(s)");
      break;
    case dtc_Make::BuildEnd::stopped:
      _dlg->OutLine ("dtc_Make::BuildEnd - Build Stopped");
      break;
  }
}


/*
 * dtc_Make::ProjectFile - request was 'GetProjectFile'
 *  received info about a file of the server's currently active project.
 */

void Ev_PrjFile::Handle (dtc_Make::ProjectFile& evt)  {
  SetEvent (hTm);                   // Signal Response

  char *pN = (char *) evt.GetName().c_str();   // file name
  char *pP = (char *) evt.GetPath().c_str();   // file path
  char *pG = (char *) evt.GetGroup().c_str();  // file is member of Group 'pG'
  char *pT = (char *) evt.GetType().c_str();   // file type

  _dlg->OutLine ("-------------------------------");
  _dlg->OutLine ("dtc_Make::ProjectFile received:");
  _dlg->OutLine ("   File: '%s'", pN);         // file name
  _dlg->OutLine ("   Path: '%s'", pP);         // file path
  _dlg->OutLine ("   Type: '%s'", pT);         // file type
  _dlg->OutLine ("  Group: '%s'", pG);         // Group name
  _dlg->OutLine ("-------------------------------");
}


/*
 * dtc_Make::SetStatus - Receiver
 * Server sent a Status Event
 */

void Ev_SetStatus::Handle (dtc_Make::SetStatus& evt)  {
  SetEvent (hTm);                   // Signal Response
  char *p1 = (char *) evt.GetText().c_str();
  if (!p1 || !p1[0]) p1 = "";

  switch (evt.GetStatus())  {
    case dtc_Make::SetStatus::done:
      _dlg->OutLine ("dtc_Make::SetStatus::done: %s", p1);
      break;
    case dtc_Make::SetStatus::error:
      _dlg->OutLine ("dtc_Make::SetStatus::error:  %s", p1);
      break;
  }
}


/*
 * dtc_Debug::Error - Event - Receiver
 * Server sent an Error Event
 */

void Ev_DBError::Handle (dtc_Debug::Error& evt)  {
  SetEvent (hTm);                   // Signal Response
  char *p1 = (char *) evt.GetDescr().c_str();
  if (!p1 || !p1[0]) p1 = "";
  _dlg->OutLine ("dtc_Debug::Error:  %s", p1);
}


/*
 * dtc_Debug::DebuggerStarted Event - Receiver
 * Server says 'Debugger started'
 */

void Ev_DbgStarted::Handle (dtc_Debug::DebuggerStarted& evt)  {
  SetEvent (hTm);                   // Signal Response
  _dlg->OutLine ("dtc_Debug::DebuggerStarted");
}


/*
 * dtc_Debug::DebuggerStopped - Receiver
 * Server says 'Debugger stopped'
 */

void Ev_DbgStopped::Handle (dtc_Debug::DebuggerStopped& evt)  {
  SetEvent (hTm);                   // Signal Response
  _dlg->OutLine ("dtc_Debug::DebuggerStopped");
}


/*
 * dtc_Debug::ExecutionStopped - Receiver
 * Server says 'User Program Execution started'
 */

void Ev_DbgExecStarted::Handle (dtc_Debug::ExecutionStarted& evt)  {
  SetEvent (hTm);                   // Signal Response
  _dlg->OutLine ("dtc_Debug::ExecutionStarted");
}


/*
 * dtc_Debug::ExecutionStopped - Receiver
 * Server says 'User Program Execution stopped'
 */

void Ev_ExecStop::Handle (dtc_Debug::ExecutionStopped& evt)  {
  SetEvent (hTm);                   // Signal Response
  char *pAdr = (char *) evt.GetAddress().c_str(); // 16.9.2002
  _dlg->OutLine ("dtc_Debug::ExecutionStopped at %s", pAdr);
}


/*
 * dtc_Debug::RegisterValue - Receiver
 * Server returned some register value
 */

void Ev_RegVal::Handle (dtc_Debug::RegisterValue& evt)  {
  SetEvent (hTm);                   // Signal Response

  char *p1 = (char *) evt.GetName().c_str();   // RegName
  int    v = evt.GetValue();                   // Value
  _dlg->OutLine ("dtc_Debug::RegisterValue: %s = 0x%X", p1, v);
}


/*
 * dtc_Debug::RegisterWritten - Receiver
 * Server responds to a write register event
 */

void Ev_RegWritten::Handle (dtc_Debug::RegisterWritten& evt)  {
  SetEvent (hTm);                   // Signal Response

  char *p1 = (char *) evt.GetName().c_str();   // RegName
  int    v = evt.GetValue();                   // Value
  _dlg->OutLine ("dtc_Debug::RegisterWritten: %s = 0x%X", p1, v);
}


/*
 * dtc_Debug::MemoryContents - Receiver
 * Server responds to a read memory event
 */

void EV_MemCont::Handle (dtc_Debug::MemoryContents& evt)  {
  SetEvent (hTm);                   // Signal Response

  _dlg->OutLine ("dtc_Debug::MemoryContents received:");
  dtc_Debug::MemAddress nA = evt.GetAddress(); // Address
  int  n = evt.GetSize();                      // number of Bytes
  const char *pB = evt.GetValues();            // Buffer
  int  i, m, v, z, aci;
  char   szB[256];

  aci = 0;
  for ( i = 0 ; i < n ; )  {
    m = min (16, (n - i));
    v = sprintf (szB, "0x%08X: ", (DWORD) nA);
    for ( z = 0 ; z < m ; ++z )  {
//    v += sprintf (&szB[v], "%02X ", (DWORD) (BYTE) pB[z]);
//---7.8.2003, new code:
      v += sprintf (&szB[v], "%02X ", (DWORD) evt.GetValues()[aci]);
      ++aci;
//---
    }
    szB [v] = 0;
    _dlg->OutLine (szB);
    i  += m;
    nA += m;
//  pB += m;
  }
}


/*
 * dtc_Debug::MemoryWritten - Receiver
 * Server responds to a write memory event
 */
void Ev_MemWritten::Handle (dtc_Debug::MemoryWritten& evt)  {
  SetEvent (hTm);                   // Signal Response

  _dlg->OutLine ("dtc_Debug::MemoryWritten received:");
  dtc_Debug::MemAddress nA = evt.GetAddress(); // Address
  int  n = evt.GetSize();                      // number of Bytes
//const char *pB = evt.GetValues();            // Buffer
  int  i, m, v, z, aci;
  char   szB[256];

  aci = 0;
  for ( i = 0 ; i < n ; )  {
    m = min (16, (n - i));
    v = sprintf (szB, "0x%08X: ", (DWORD) nA);
    for ( z = 0 ; z < m ; ++z )  {
//    v += sprintf (&szB[v], "%02X ", (DWORD) (BYTE) pB[z]);
//---7.8.2003, new code:
      v += sprintf (&szB[v], "%02X ", (DWORD) evt.GetValues()[aci]);
      ++aci;
//---
    }
    szB [v] = 0;
    _dlg->OutLine (szB);
    i  += m;
    nA += m;
//  pB += m;
  }
}



/*
 * dtc_Debug::MemoryAddress - Receiver (result of calc-expr.)
 * Server responds to a request of an address calculation
 */

void Ev_MemAddr::Handle (dtc_Debug::MemoryAddress& evt)  {
  SetEvent (hTm);                   // Signal Response

  char  *p1 = (char *) evt.GetExpr().c_str();
  if (!p1 || !p1[0]) p1 = "<<>>";
  dtc_Debug::MemAddress nA = evt.GetAddress();

  if (evt.Successful())  {
    _dlg->OutLine ("dtc_Debug::MemoryAddress: Ok: 0x%X - '%s'", (DWORD) nA, p1);
    _dlg->m_pgDebug.MemAdrCB ((__int64) nA, p1, 1);
  }
  else  {
    _dlg->OutLine ("dtc_Debug::MemoryAddress: invalid expr: '%s'", p1);
    _dlg->m_pgDebug.MemAdrCB (-1, p1, 0);
  }
}


/*
 * dtc_Debug::BreakpointSet - Receiver (result of SetBreakpoint)
 * Server responds to a request to set a breakpoint
 */

void Ev_BpSet::Handle (dtc_Debug::BreakpointSet& evt)  {
  char  szB [128];

  SetEvent (hTm);                   // Signal Response

  char *pAdr = (char *) evt.GetAddress().c_str();
  int      n = sprintf (szB, "dtc_Debug::BreakpointSet at %s ", pAdr);
  if (evt.Successful())  {
    n += sprintf (&szB[n], "successful");
  }
  else  {
    n += sprintf (&szB[n], "failed");
  }
  _dlg->OutLine (szB);
}



/*
 * dtc_Debug::BreakpointCleared - Receiver (result of SetBreakpoint/SetBreakpointExpr)
 * Server responds to a request to clear a breakpoint
 */

void Ev_BpCleared::Handle (dtc_Debug::BreakpointCleared& evt)  {
  char  szB [128];

  SetEvent (hTm);                   // Signal Response

  char *pAdr = (char *) evt.GetAddress().c_str();
  int      n = sprintf (szB, "dtc_Debug::BreakpointCleared at %s ", pAdr);

  if (evt.Successful())  {
    n += sprintf (&szB[n], "successful");
  }
  else  {
    n += sprintf (&szB[n], "failed");
  }
  _dlg->OutLine (szB);
}



/*
 * Message from Target
 * Server sent a target message from the running user application
 */

void Ev_MsgFromTarget::Handle (dtc_TargetMessaging::MsgFromTarget& evt)  {
  char  szB [256];

  const dtc_TargetMessaging::Data  &rD = evt.GetData();
//const int nDsize = sizeof (dtc_TargetMessaging::Data);
  int     n;

  SetEvent (hTm);                   // Signal Response

  n = sprintf (szB, "SEQ=%d, CHA=%d, TSTAMP=%I64d | Data: ",
            evt.GetSeqNo(), evt.GetChannel(), evt.GetTimeStamp());
  sprintf (&szB[n], "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
           rD._bytes[0] & 0xFF, rD._bytes[1] & 0xFF, rD._bytes[2] & 0xFF, rD._bytes[3] & 0xFF,
           rD._bytes[4] & 0xFF, rD._bytes[5] & 0xFF, rD._bytes[6] & 0xFF, rD._bytes[7] & 0xFF,
           rD._bytes[8] & 0xFF, rD._bytes[9] & 0xFF, rD._bytes[10] & 0xFF,rD._bytes[11] & 0xFF);

  _dlg->OutLine (szB);
}


/*
 * Server sent a 'TargetMessaging connected' Event
 *  Note: the user application needs to be prepared for target messaging
 *        otherwise this message will never come in here.
 */

void Ev_TmsgAttach::Handle (dtc_TargetMessaging::Connected& evt)  {
  _dlg->OutLine ("dtc_TargetMessaging::Connected");
}


/*
 * Server sent a 'TargetMessaging disconnected' Event
 */

void Ev_TmsgDetach::Handle (dtc_TargetMessaging::Disconnected& evt)  {
  _dlg->OutLine ("dtc_TargetMessaging::Disconnected");
}




/*
 * Server responds to a GetProjectFile request by a Project event.
 */

void Ev_Project::Handle (dtc_Identify::Project& ev)  {
  char   szB [1024];

  SetEvent (hTm);                   // Signal Response
  LPCTSTR pPrj  = ev.GetProject().c_str();
  LPCTSTR pPath = ev.GetBaseFolder().c_str();

  sprintf (szB, "dtc_Identify::Project: Name = '%s', Base-Folder = '%s'", pPrj, pPath);
  _dlg->OutLine (szB);
}



/*
 * Server is disconnected (closed by user)
 */

void Ev_GoodBy::Handle (dtc_Identify::Goodbye& evt)  {
  SetEvent (hTm);                   // Signal Response
  _dlg->OutLine ("dtc_Identify::Goodbye");
}


/*
 * Receiver for 'DecodedMemAddress' Event
 * Server responds to a request to decode an address
 */

void Ev_DecodedMA::Handle (dtc_Debug::DecodedMemAddress& evt)  {
  char   szB [512];

  SetEvent (hTm);                   // Signal Response
  LPCTSTR pD = evt.GetResult().c_str();
  sprintf (szB, "dtc_Debug::DecodedMemAddress := '%s'", pD);
  _dlg->OutLine (szB);
}


/*
 * Server responds to a GetTargetInfo request with a TargetInfo Event
 */

void Ev_TargInfo::Handle (dtc_TargetInfo::TargetInfo& ev)  {
  SetEvent (hTm);                   // Signal Response

  char     *pEnd;
  LPCTSTR    Cpu = ev.GetName().c_str();    // Name of Cpu
  LPCTSTR Vendor = ev.GetDesc().c_str();    // Cpu Vendor
  int      align = ev.GetAlignment();       // default alignment

  dtc_TargetInfo::TargetInfo::ByteOrder endian;

  endian = ev.GetByteOrder();
  switch (endian)  {                      // Endian
    case dtc_TargetInfo::TargetInfo::unknown:
      pEnd = "<unknown>";
      break;
    case dtc_TargetInfo::TargetInfo::big_endian:
      pEnd = "Big";
      break;
    case dtc_TargetInfo::TargetInfo::little_endian:
      pEnd = "Little";
      break;
 }

 _dlg->OutLine ("dtc_TargetInfo::TargetInfo: Cpu=%s, Vendor=%s, Align=%d, Endian=%s",
                                             Cpu,       Vendor,    align,     pEnd);
}
#endif


