/*--------------------------------------------------------------------------
PgDebug.cpp

Debug profile events sender dialog page.
Copyright (c) 2003 Keil Elektronik GmbH and Keil Software, Inc.
All rights reserved.
--------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DlgApp.h"
#include "MainDlg.h"
#include "PgDebug.h"
#include "BrkDlg.h"


//#include "dtc_Debug.h"
//#include "dtc_TimeOut.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CPwOptionsDlg   *_dlg; // user for Text 'OutLine()'


/////////////////////////////////////////////////////////////////////////////
// PgDebug dialog


PgDebug::PgDebug(CWnd* pParent /*=NULL*/)
	: CDialog(PgDebug::IDD, pParent)
{
	//{{AFX_DATA_INIT(PgDebug)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void PgDebug::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PgDebug)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PgDebug, CDialog)
	//{{AFX_MSG_MAP(PgDebug)
	ON_BN_CLICKED(IDC_STARTDEBUG, OnStartdebug)
	ON_BN_CLICKED(IDC_STOPTDEBUG, OnStoptdebug)
	ON_BN_CLICKED(IDC_STARTEXEC, OnStartexec)
	ON_BN_CLICKED(IDC_STOPEXEC, OnStopexec)
	ON_BN_CLICKED(IDC_STEPINSTR, OnStepinstr)
	ON_BN_CLICKED(IDC_STEPINTO, OnStepinto)
	ON_BN_CLICKED(IDC_STEPOUT, OnStepout)
	ON_BN_CLICKED(IDC_STEPOVER, OnStepover)
	ON_BN_CLICKED(IDC_EXECUNTIL, OnExecuntil)
	ON_BN_CLICKED(IDC_GETMEMADR, OnGetmemadr)
	ON_BN_CLICKED(IDC_SETBP, OnSetbp)
	ON_BN_CLICKED(IDC_CLRBP, OnClrbp)
	ON_BN_CLICKED(IDC_READREG, OnReadreg)
	ON_BN_CLICKED(IDC_WRITEREG, OnWritereg)
	ON_BN_CLICKED(IDC_READMEM, OnReadmem)
	ON_BN_CLICKED(IDC_WRITEMEM, OnWritemem)
	ON_BN_CLICKED(IDC_DECMEMA, OnDecmema)
	ON_BN_CLICKED(IDC_CLRBPALL, OnClrbpall)
	ON_BN_CLICKED(IDC_ISRUNNING, OnIsrunning)
	ON_BN_CLICKED(IDC_CYC_TSTAMP, OnCycTstamp)
	ON_BN_CLICKED(IDC_SETINTERVAL, OnSetinterval)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_BN_CLICKED(IDC_GETVTR, OnGetvtr)
	ON_BN_CLICKED(IDC_SETVTR, OnSetvtr)
	ON_BN_CLICKED(IDC_GETVERS, OnGetvers)
	ON_BN_CLICKED(IDC_SERSEND_D, OnSersendD)
	ON_BN_CLICKED(IDC_GETSERO, OnGetsero)
    ON_BN_CLICKED(IDC_BREAKPOINTS, &PgDebug::OnBnClickedBreakpoints)
    ON_BN_CLICKED(IDC_IDENUM, &PgDebug::OnBnClickedIdenum)
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_MAPTOFL, &PgDebug::OnBnClickedMaptofl)
    ON_BN_CLICKED(IDC_ENUMSTACK, &PgDebug::OnBnClickedEnumstack)
    ON_BN_CLICKED(IDC_ENUMVTR, &PgDebug::OnBnClickedEnumvtr)
    ON_BN_CLICKED(IDC_ENUMVTRV, &PgDebug::OnBnClickedEnumvtrv)
    ON_BN_CLICKED(IDC_SHOWCODE, &PgDebug::OnBnClickedShowcode)
END_MESSAGE_MAP()


//	ON_BN_CLICKED(IDC_SETBPEXP, OnSetbpexp)
//	ON_BN_CLICKED(IDC_CLRBPEXP, OnClrbpexp)

/////////////////////////////////////////////////////////////////////////////
// PgDebug message handlers

void PgDebug::OnCancel()  {   // Catch ESC
//CDialog::OnCancel();
}

void PgDebug::OnOK()  {       // Catch Enter
//CDialog::OnOK();
}

BOOL PgDebug::OnInitDialog()  {
//CDialog::OnInitDialog();
  ((CEdit *) GetDlgItem (IDC_VTRNAME))->LimitText (32);
  ((CEdit *) GetDlgItem  (IDC_VTRVAL))->LimitText (32);
  ((CEdit *) GetDlgItem  (IDC_EDENID))->LimitText (256);
  return TRUE;
}


void PgDebug::OutLine (char *fmt, ...)  {
  char  szB [1028];
  va_list    argp;
  int           n;
  CEdit       *pE;

  pE = (CEdit *) GetDlgItem (IDC_ITEXT);
  if (pE == NULL) return;
  if (fmt == NULL)  {            // used to clear edit control
    pE->SetSel (0, -1);
    pE->Clear();
    return;
  }

#if 0
  switch ((WORD16) (BYTE) fmt[0])  {
    case 0xFF:
      pE->SetRedraw (FALSE);
      return;
    case 0xFE:
      pE->SetRedraw (TRUE);
      return;
  }
#endif

  va_start (argp, fmt);
  if (argp == NULL)  {         //  || argp[0] == 0)  {
    n = min (strlen (fmt), sizeof (szB) - 4);
    memcpy (szB, fmt, n);
  }
  else  {
    n = _vsnprintf (szB, sizeof (szB) - 4, fmt, argp);
  }
  szB [n] = 0;

  if (n > 0 && szB [n - 1] == '\n') --n;
  if (n > 0 && szB [n - 1] == '\r') --n;
  szB [n++] = '\r';
  szB [n++] = '\n';
  szB [n]   = 0;

  pE->SetSel (-1, -1);        // EOF
  pE->ReplaceSel (szB);
}




int PgDebug::TmSetup (void)  {
  ResetEvent (hTm);
  return (1);
}

 
int PgDebug::TimeOut (int nMsec)  {
  if (nMsec < 0) nMsec = 2000;      // default: 2 Seconds

#if 0
  DWORD Event = WaitForSingleObject (hTm, 2000);
  if (Event == WAIT_TIMEOUT) {   // if timeout -> return with error
    _dlg->OutLine ("*** No Response from Server.\n");
    MessageBeep (MB_OK);
    return (1);
  }
#endif
  return (0);
}


/*
 * Get Socket-Interface Version-Number
 */
void PgDebug::OnGetvers()  {
  _dlg->CreateCommandSend (UV_GEN_GET_VERSION, 0, NULL);
}


/*
 * Get Cycles and Time-Stamp
 */
void PgDebug::OnCycTstamp() {
  _dlg->CreateCommandSend (UV_DBG_TIME_INFO, 0, NULL);
}


/*
 * Is Debugger currently executing
 */
void PgDebug::OnIsrunning()  {
  _dlg->CreateCommandSend (UV_DBG_STATUS, 0, NULL);
}


void PgDebug::OnStartdebug()  {
  _dlg->CreateCommandSend (UV_DBG_ENTER, 0, NULL);
}


void PgDebug::OnStoptdebug()  {
  _dlg->CreateCommandSend (UV_DBG_EXIT, 0, NULL);
}


void PgDebug::OnStartexec()  {
  _dlg->CreateCommandSend (UV_DBG_START_EXECUTION, 0, NULL);
}

void PgDebug::OnStopexec()  {
  _dlg->CreateCommandSend (UV_DBG_STOP_EXECUTION, 0, NULL);
}

void PgDebug::OnSetinterval()  {
  float f = 1.0;  // 0.5;
  _dlg->CreateCommandSend (UV_DBG_SET_CALLBACK, sizeof (f), &f);
}

void PgDebug::OnReset()  {
  _dlg->CreateCommandSend (UV_DBG_RESET, 0, NULL);
}

void PgDebug::OnGetsero()  {
  _dlg->CreateCommandSend (UV_DBG_SERIAL_GET, 0, NULL);
}



void PgDebug::OnSersendD() {
  union  {
    BYTE   bbb[sizeof(SERIO) + 128];
    SERIO  ser;
  } s;
  int     nSize;

  memset (&s.ser, 0, sizeof (s.ser));
//s.ser.nChannel    = 0;          // channel #0:  /Ok for Atmel-Measure/
  s.ser.nChannel    = 1;          // channel #1:  /Ok for NXP LPC21xx Measure/
  s.ser.itemMode    = 0;          // Byte
  s.ser.nMany       = 2;          // 2 items
  s.ser.s.aBytes[0] = 'd';        // 'd'
  s.ser.s.aBytes[1] = 0x0A;       //    ,CR
  nSize = offsetof (SERIO, s.aBytes) + 2;  // +2 weil 2 Zeichen
  _dlg->CreateCommandSend (UV_DBG_SERIAL_PUT, nSize, &s.ser);
}




void PgDebug::OnGetvtr()  {         // Read VTR-Value
  //SSTR    str;
  VSET vs;
  int n;

  CEdit *pE = (CEdit *) GetDlgItem (IDC_VTRNAME);
  vs.str.nLen  = pE->GetWindowText (vs.str.szStr, sizeof (vs.str.szStr) - 1);
  vs.str.szStr [vs.str.nLen] = 0;
  if (vs.str.nLen == 0)  {
    MessageBeep (MB_OK);
    AfxMessageBox ("Bad or Missing VTR-Name");
    GetDlgItem (IDC_VTRNAME)->SetFocus();
    return;
  }
  n  = sizeof (vs) - sizeof (vs.str.szStr);
  n += vs.str.nLen + 1;
  _dlg->CreateCommandSend (UV_DBG_VTR_GET, n, &vs);
}



extern int bResult;
extern TVAL     tv;
extern "C" void DoEvents (void);


void PgDebug::OnSetvtr()  {
  VSET   vs;
  int     n;

  CEdit *pE = (CEdit *) GetDlgItem (IDC_VTRVAL);
  vs.str.nLen  = pE->GetWindowText (vs.str.szStr, sizeof (vs.str.szStr) - 1);
  vs.str.szStr [vs.str.nLen] = 0;
  if (vs.str.nLen == 0)  {
x:  MessageBeep (MB_OK);
    AfxMessageBox ("Bad or Missing VTR-Value");
    GetDlgItem (IDC_VTRVAL)->SetFocus();
    return;
  }

  bResult = 0;
  n  = sizeof (vs) - sizeof (vs.str.szStr);
  n += vs.str.nLen + 1;
  _dlg->CreateCommandSend (UV_DBG_CALC_EXPRESSION, n, &vs);

  while (!bResult)  {
    DoEvents();
  }
  if (bResult != 1)  {
    goto x;
  }

  vs.val.v.u64 = tv.v.u64;          // value
  vs.val.vType = tv.vType;          // Type

  pE = (CEdit *) GetDlgItem (IDC_VTRNAME);
  vs.str.nLen  = pE->GetWindowText (vs.str.szStr, sizeof (vs.str.szStr) - 1);
  vs.str.szStr [vs.str.nLen] = 0;
  if (vs.str.nLen == 0)  {
    MessageBeep (MB_OK);
    AfxMessageBox ("Bad or Missing VTR-Name");
    GetDlgItem (IDC_VTRNAME)->SetFocus();
    return;
  }

  n  = sizeof (vs) - sizeof (vs.str.szStr);
  n += vs.str.nLen + 1;
  _dlg->CreateCommandSend (UV_DBG_VTR_SET, n, &vs);
}




void PgDebug::OnStepinstr()  {      // StepInstruction Event
  _dlg->CreateCommandSend (UV_DBG_STEP_INSTRUCTION, 0, NULL);
}

void PgDebug::OnStepinto()  {       // StepInto Event
  _dlg->CreateCommandSend (UV_DBG_STEP_INTO, 0, NULL);
}

void PgDebug::OnStepout()  {        // StepOut Event
  _dlg->CreateCommandSend (UV_DBG_STEP_OUT, 0, NULL);
}

void PgDebug::OnStepover()  {       // StepOver Event
  _dlg->CreateCommandSend (UV_DBG_STEP_HLL, 0, NULL);
}




static __int64 memA = -1;


void PgDebug::OnGetmemadr()  {      // GetMemoryAddress Event
  //SSTR      str;
  VSET      vs;
  char      szB[256];
  int       n, i;

  CEdit *pE = (CEdit *) GetDlgItem (IDC_EDUNTIL);

  szB[0] = ' ';
  n = pE->GetWindowText (&szB[1], sizeof (szB) - 3);
  szB [n + 1] = 0;
  for ( i = 1 ; i < n ; ++i )  {
    if (szB[i] != ' ') break;
  }
  if (szB[i] != '&')  {
//    szB[0] = '&';
  }
  ++n;

  vs.str.nLen  = n;
  strcpy (vs.str.szStr, szB);

//str.nLen  = pE->GetWindowText (str.szStr, sizeof (str.szStr) - 1);
//str.szStr [str.nLen] = 0;

  if (vs.str.nLen == 0)  {
x:  MessageBeep (MB_OK);
    AfxMessageBox ("Bad or Missing Expression");
    GetDlgItem (IDC_EDUNTIL)->SetFocus();
    return;
  }

  bResult = 0;
  n  = sizeof (vs) - sizeof (vs.str.szStr);
  n += vs.str.nLen + 1;
  _dlg->CreateCommandSend (UV_DBG_CALC_EXPRESSION, n, &vs);

  while (!bResult)  {
    DoEvents();
  }
  if (bResult != 1)  {
    SetDlgItemText (IDC_SHOWMA, "Result := <invalid>");
    memA = -1;
    goto x;
  }

  memA = tv.v.i64;
  sprintf (szB, "Addr := 0x%08X", (DWORD) memA);
  SetDlgItemText (IDC_SHOWMA, szB);
}



void PgDebug::OnReadmem()  {        // ReadMemory Event
  AMEM     ma;
  int       n;

  if (memA == -1)  {                // MemoryAddress not set
    MessageBeep (MB_OK);
    AfxMessageBox ("Memory Address not set");
    GetDlgItem (IDC_EDUNTIL)->SetFocus();
    return;
  }    

  ma.nAddr  = (UINT) memA;          // Address
  ma.nBytes = 128;                  // read 128 Bytes
  n  = offsetof (AMEM, aBytes);
  _dlg->CreateCommandSend (UV_DBG_MEM_READ, n, &ma);
}



void PgDebug::OnWritemem()  {       // WriteMemory Event
  int       i, n;
  AMEM   *pA;
  BYTE    wB[512];

  if (memA == -1)  {                // MemoryAddress not set
    MessageBeep (MB_OK);
    AfxMessageBox ("Memory Address not set");
    GetDlgItem (IDC_EDUNTIL)->SetFocus();
    return;
  }

// Example: write 128 bytes to address 'memA':
  memset (wB, 0, sizeof (wB));
  pA = (AMEM *) (void *) &wB[0];
  pA->nAddr  = (UINT) memA;
  pA->nBytes = 128;                 // number of bytes to write
  for ( i = 0 ; i < 128 ; ++i )  {
    pA->aBytes[i] = (BYTE) i;
  }
  n  = offsetof (AMEM, aBytes);
  n += 128;                         // 
  _dlg->CreateCommandSend (UV_DBG_MEM_WRITE, n, pA);
}




#if 0
/*
 * TimeOut Event Tester
 */

class EV_TimeOut : public dtc_TimeoutClient  {  // ::Handler  {
public:
  void HandleTimeout (Id id);
} EV_TID;

static int z;
void EV_TimeOut::HandleTimeout (Id id)  {
  if (id == (Id) &EV_TID)  {
    z = 1;
  }
//delete this;
}
dtc_Timeout xT1 (1, EV_TID);
#endif



void PgDebug::OnExecuntil()  {      // ExecUntil Event
#if 0
  xT1.Start (2000);      // mSec

  if (memA == -1)  {                // MemoryAddress not set
    MessageBeep (MB_OK);
    return;
  }

  char   szB [128];
  sprintf (szB, "0x%X", (DWORD) memA);
  dtc_Debug::ExecuteUntil xE (szB);
  xE.Send();
#endif
}


void PgDebug::OnSetbp()  {          // SetBreakpoint Event
#if 0
  if (memA == -1)  {                // MemoryAddress not set
    MessageBeep (MB_OK);
    AfxMessageBox ("Bad or Missing Address");
    GetDlgItem (IDC_EDUNTIL)->SetFocus();
    return;
  }
  char   szB [128];
  sprintf (szB, "0x%X", (DWORD) memA);
  dtc_Debug::SetBreakpoint xS (szB);

  TmSetup();                        // Setup Timeout-handle
  xS.Send();
  TimeOut();                        // Wait for Reply of TimeOut
#endif
}


void PgDebug::OnClrbp()  {          // ClearBreakpoint Event
#if 0
  if (memA == -1)  {                // MemoryAddress not set
    MessageBeep (MB_OK);
    AfxMessageBox ("Bad or Missing Address");
    GetDlgItem (IDC_EDUNTIL)->SetFocus();
    return;
  }
  char   szB [128];
  sprintf (szB, "0x%X", (DWORD) memA);
  dtc_Debug::ClearBreakpoint xS (szB);

  TmSetup();                        // Setup Timeout-handle
  xS.Send();
  TimeOut();                        // Wait for Reply of TimeOut
#endif
}


/*
 * 11.3.2005: Clear All Breakpoints:
 *  instead of Address, send '*' as argument
 */
void PgDebug::OnClrbpall()  {       // Clear All Breakpoints Event
#if 0
  char   szB [16];

  szB [0] = '*';
  szB [1] = 0;
  dtc_Debug::ClearBreakpoint xS (szB);

  TmSetup();                        // Setup Timeout-handle
  xS.Send();
  TimeOut();                        // Wait for Reply of TimeOut
#endif
}




#if 0
void PgDebug::OnSetbpexp()  {       // SetBreakpointExpr Event
  char   szB[130];

  CEdit *pE = (CEdit *) GetDlgItem (IDC_EDUNTIL);
  int     n = pE->GetWindowText (szB, sizeof (szB) - 1);
  szB [n] = 0;
  if (n == 0)  {
    MessageBeep (MB_OK);
    AfxMessageBox ("Bad or Missing Address");
    GetDlgItem (IDC_EDUNTIL)->SetFocus();
    return;
  }

  dtc_Debug::SetBreakpointExpr xB (szB);  // const string& expr);
  TmSetup();                        // Setup Timeout-handle
  xB.Send();
  TimeOut();                        // Wait for Reply of TimeOut
}

void PgDebug::OnClrbpexp()  {       // ClearBreakpointExpr Event
  char   szB[130];

  CEdit *pE = (CEdit *) GetDlgItem (IDC_EDUNTIL);
  int     n = pE->GetWindowText (szB, sizeof (szB) - 1);
  szB [n] = 0;
  if (n == 0)  {
    MessageBeep (MB_OK);
    AfxMessageBox ("Bad or Missing Address");
    GetDlgItem (IDC_EDUNTIL)->SetFocus();
    return;
  }

  dtc_Debug::ClearBreakpointExpr xB (szB);  // const string& expr);
  TmSetup();                        // Setup Timeout-handle
  xB.Send();
  TimeOut();                        // Wait for Reply of TimeOut
}
#endif



void PgDebug::OnReadreg()  {        // ReadRegister Event
#if 0
  char   szB[130];

  CEdit *pE = (CEdit *) GetDlgItem (IDC_REGNAME);
  int     n = pE->GetWindowText (szB, sizeof (szB) - 1);
  szB [n] = 0;
  if (n == 0)  {
    MessageBeep (MB_OK);
    AfxMessageBox ("Missing Register Name");
    GetDlgItem (IDC_REGNAME)->SetFocus();
    return;
  }

  dtc_Debug::ReadRegister xR (szB);
  TmSetup();                        // Setup Timeout-handle
  xR.Send();
  TimeOut();                        // Wait for Reply of TimeOut
#endif
}



int CalcVal (char *pB, int &err)  {
  int      v = 0;
  int   base = 10;
  int     c1;

  while (*pB == ' ') ++pB;
  if (*pB == 0)  {
    err = 1;
    return (0);
  }
  if (pB [0] == '0' &&
     (pB [1] == 'x' || pB [1] == 'X'))  {
    pB  += 2;
    base = 16;
  }
  while (*pB)  {
    if (isdigit (*pB))  {            // 0..9
      v = v * base + (*pB - '0');
      ++pB;
      continue;
    }

    c1 = *pB;
    if (islower (c1))  {
      c1 = toupper (c1);
    }
    if (isxdigit (c1))  {            // A...F
      if (base == 10)  {
        err = 1;                     // illegal number
        return (0);
      }
      v = v * base + (c1 - '7');
      ++pB;
      continue;
    }
    err = 1;
    return (0);
  }
  return (v);
}
  

void PgDebug::OnWritereg()  {       // WriteRegister Event
#if 0
  char   szR[130],
         szV[130];
  int       error, v, n1, n2;

  CEdit *pE = (CEdit *) GetDlgItem (IDC_REGNAME);
  n1 = pE->GetWindowText (szR, sizeof (szR) - 1);
  szR [n1] = 0;                     // RegisterName
  if (n1 == 0)  {
    MessageBeep (MB_OK);
    AfxMessageBox ("Missing Register Name");
    GetDlgItem (IDC_REGNAME)->SetFocus();
    return;
  }

  CEdit *pV = (CEdit *) GetDlgItem (IDC_REGVAL);
  n2 = pV->GetWindowText (szV, sizeof (szV) - 1);
  szV [n2] = 0;                     // RegisterValue
  error = 0;
  v = CalcVal (szV, error);         // calc number
  if (error)  {
    MessageBeep (MB_OK);
    AfxMessageBox ("Bad or Missing Register Value");
    GetDlgItem (IDC_REGVAL)->SetFocus();
    return;
  }

  dtc_Debug::WriteRegister xW (szR, v);
  TmSetup();                        // Setup Timeout-handle
  xW.Send();
  TimeOut();                        // Wait for Reply of TimeOut
#endif
}



#if 0
static const char DtcMWR1[] =  {
  "*** #1 dtc_Debug::WriteMemory image test ! ***"
};
static const char DtcMWR2[] =  {
  "*** #2 DTC_DEBUG::WRITEMEMORY IMAGE TEST ! ***"
};
static int   n = 0;

void PgDebug::OnWritemem()  {       // WriteMempry Event
#if 0
  if (memA == -1)  {                // MemoryAddress not set
    MessageBeep (MB_OK);
    AfxMessageBox ("Memory Address not set");
    GetDlgItem (IDC_EDUNTIL)->SetFocus();
    return;
  }

  const char *pB = (n & 1) ? &DtcMWR2[0] : &DtcMWR1[0];

  dtc_Debug::WriteMemory xE ((dtc_Debug::MemAddress) memA,
                             sizeof (DtcMWR1) - 1,
                             pB);
  n ^= 1;

  TmSetup();                        // Setup Timeout-handle
  xE.Send();
  TimeOut (5000);                   // Wait for Reply of TimeOut
#endif
}
#endif



/*
 * MemAddress: Eingabe  $, [GetMemoryAddress], [Decode Addr.]
 *    GetMemoryAddress '$' ergibt current PC.
 * danach Decode Addr.     ergibt \module\func\lineno
 */

void PgDebug::OnDecmema()  {
#if 0
  if (memA == -1)  {                // MemoryAddress not set
    MessageBeep (MB_OK);
    AfxMessageBox ("Bad or Missing Address");
    GetDlgItem (IDC_EDUNTIL)->SetFocus();
    return;
  }
//char   szB [128];
//sprintf (szB, "0x%X", (DWORD) memA);
  dtc_Debug::DecodeMemAddress xD ((dtc_Debug::MemAddress) memA);

  TmSetup();                        // Setup Timeout-handle
  xD.Send();
  TimeOut();                        // Wait for Reply of TimeOut
#endif
}



/*
 * Start Breakpoint-dialog
 */
void PgDebug::OnBnClickedBreakpoints()  {
  CBrkDlg   dlg;

  dlg.DoModal();
}



void PgDebug::OnBnClickedIdenum()  {
  int         n, i;
  ENUMTPM   tpm;
  CEdit     *pE;

  pE = (CEdit *) GetDlgItem (IDC_EDENID);
  memset (&tpm, 0, sizeof (tpm));
  tpm.szID[0] = ' ';
  n = pE->GetWindowText (&tpm.szID[1], sizeof (tpm.szID) - 3);
  tpm.szID [n + 1] = 0;

  for ( i = 1 ; i < n ; ++i )  {
    if (tpm.szID[i] != ' ') break;
  }
  if (i == 0)  {
    MessageBeep (MB_OK);
    AfxMessageBox ("Bad or Missing Identifier");
    GetDlgItem (IDC_EDENID)->SetFocus();
    return;
  }

  tpm.Job = UV_TPENUM_MEMBERS;
  n = offsetof (ENUMTPM, szID) + strlen (tpm.szID) + 2;  // leading ' ' + 0.
  _dlg->CreateCommandSend (UV_DBG_ENUM_SYMTP, n, &tpm);
}


/*
 * Map current address to File and Linenumber
 */
void PgDebug::OnBnClickedMaptofl()  {
  ADRMTFL    *pA;
  BYTE        wB[sizeof (ADRMTFL) + 32];

  if (memA == -1)  {                // MemoryAddress not set
    MessageBeep (MB_OK);
    AfxMessageBox ("Memory Address not set");
    GetDlgItem (IDC_EDUNTIL)->SetFocus();
    return;
  }

  memset (wB, 0, sizeof (wB));
  pA = (ADRMTFL *) (void *) &wB[0];
  pA->nAdr  = memA;                 // address to resolve
//pA->bFull = 0;                    // want relative path of file
  pA->bFull = 1;                    // want full path file

  _dlg->CreateCommandSend (UV_DBG_ADR_TOFILELINE, sizeof (ADRMTFL), pA);
}


/*
 * Enumerate Call-Stack
 */
void PgDebug::OnBnClickedEnumstack()  {
  iSTKENUM    *pA;
  BYTE         wB[sizeof (iSTKENUM) + 32];

  memset (wB, 0, sizeof (wB));
  pA = (iSTKENUM *) (void *) &wB[0];
//pA->bFull = 0;                    // want relative path of file
  pA->bFull = 1;                    // want full path file

  _dlg->CreateCommandSend (UV_DBG_ENUM_STACK, sizeof (iSTKENUM), pA);
}


/*
 * Enumerate Vtr
 */
void PgDebug::OnBnClickedEnumvtr()  {
  iVTRENUM   iVt;

  memset (&iVt, 0, sizeof (iVt));
  iVt.bValue = 0;                   // name and type only, no value
  _dlg->CreateCommandSend (UV_DBG_ENUM_VTR, sizeof (iVt), &iVt);
}


/*
 * Enumerate Vtr - show also values
 */
void PgDebug::OnBnClickedEnumvtrv()  {
  iVTRENUM   iVt;

  memset (&iVt, 0, sizeof (iVt));
  iVt.bValue = 1;                   // name and type and value.
  _dlg->CreateCommandSend (UV_DBG_ENUM_VTR, sizeof (iVt), &iVt);
}


/*
 * Show Disassembly and HLL file (if possible) for given address
 */
void PgDebug::OnBnClickedShowcode()  {
  iSHOWSYNC  scy;

  if (memA == -1)  {                // MemoryAddress not set
    MessageBeep (MB_OK);
    AfxMessageBox ("Memory Address not set");
    GetDlgItem (IDC_EDUNTIL)->SetFocus();
    return;
  }

  memset (&scy, 0, sizeof (scy));
  scy.nAdr = memA;                  // address to show code for
  scy.bAsm = 1;
  scy.bHll = 1;
  _dlg->CreateCommandSend (UV_DBG_ADR_SHOWCODE, sizeof (scy), &scy);
}
