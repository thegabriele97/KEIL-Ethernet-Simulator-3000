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
#include "PgDebug2.h"
#include "UVSCThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CUVSCTesterDlg   *_dlg;

extern "C" void DoEvents (void);


// Memory address evaluated by calc expression
static __int64 memA = -1;


/////////////////////////////////////////////////////////////////////////////
// CDebugDlg dialog


CDebug2Dlg::CDebug2Dlg(CWnd* pParent /*=NULL*/)
  : CDialog(CDebug2Dlg::IDD, pParent)
{
  //{{AFX_DATA_INIT(CDebugDlg)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void CDebug2Dlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CDebugDlg)
    // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDebug2Dlg, CDialog)
  //{{AFX_MSG_MAP(CDebugDlg)
  ON_BN_CLICKED(IDC_GETMEMADR, OnGetmemadr)       // 1018
  ON_BN_CLICKED(IDC_READMEM, OnReadmem)           // 1029
  ON_BN_CLICKED(IDC_WRITEMEM, OnWritemem)         // 1030
  ON_BN_CLICKED(IDC_ISRUNNING, OnIsrunning)       // 1013
  ON_BN_CLICKED(IDC_CYC_TSTAMP, OnCycTstamp)      // 1014
  ON_BN_CLICKED(IDC_GETVTR, OnGetvtr)             // 1037
  ON_BN_CLICKED(IDC_SETVTR, OnSetvtr)             // 1041
  ON_BN_CLICKED(IDC_SERSEND_D, OnSersendD)        // 1052
  ON_BN_CLICKED(IDC_GETSERO, OnGetsero)           // 1054
  ON_BN_CLICKED(IDC_IDENUM, OnBnClickedIdenum)              // 1036
  //}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_MAPTOFL, OnBnClickedMaptofl)            // 1038
  ON_BN_CLICKED(IDC_ENUMSTACK, OnBnClickedEnumstack)        // 1101
  ON_BN_CLICKED(IDC_ENUMVTR, OnBnClickedEnumvtr)            // 1102
  ON_BN_CLICKED(IDC_ENUMVTRV, OnBnClickedEnumvtrv)          // 1104
  ON_BN_CLICKED(IDC_SHOWCODE, OnBnClickedShowcode)          // 1039
  ON_BN_CLICKED(IDC_BUTTON_POWERSCALE_SHOWCODE, OnBnClickedPowerscaleShowcode)
  ON_BN_CLICKED(IDC_BUTTON_ITM_REGISTER, OnBnClickedButtonItmRegister)
  ON_BN_CLICKED(IDC_BUTTON_ITM_UNREGISTER, OnBnClickedButtonItmUnregister)
  ON_BN_CLICKED(IDC_BUTTON_EVTR_REGISTER,OnBnClickedButtonEvtrRegister)
  ON_BN_CLICKED(IDC_BUTTON_EVTR_UNREGISTER,OnBnClickedButtonEvtrUnregister)
  ON_BN_CLICKED(IDC_BUTTON_EVTR_GETSCVD,OnBnClickedButtonEvtrGetscvd)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDebugDlg message handlers

void CDebug2Dlg::OnCancel()  {   // Catch ESC
}

void CDebug2Dlg::OnOK()  {       // Catch Enter
}

BOOL CDebug2Dlg::OnInitDialog()  {
  CDialog::OnInitDialog();
  ((CEdit *) GetDlgItem (IDC_VTRNAME))->LimitText (32);
  ((CEdit *) GetDlgItem (IDC_VTRVAL))->LimitText (32);
  ((CEdit *) GetDlgItem (IDC_EDENID))->LimitText (256);

  return TRUE;
}


/*
 * Get Cycles and Time-Stamp
 */
void CDebug2Dlg::OnCycTstamp() {
  ThreadMsg msg;

  msg.cmd = UV_DBG_TIME_INFO;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}


/*
 * Is Debugger currently executing
 */
void CDebug2Dlg::OnIsrunning()  {
  ThreadMsg msg;

  msg.cmd = UV_DBG_STATUS;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}

void CDebug2Dlg::OnGetsero()  {
  // TODO
  ThreadMsg msg;

  msg.cmd = UV_DBG_SERIAL_GET;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}



void CDebug2Dlg::OnSersendD() {
  union  {
    BYTE   bbb[sizeof(SERIO) + 128];
    SERIO  ser;
  } s;
  int     nSize;

  memset (&s.ser, 0, sizeof (s.ser));
  s.ser.nChannel    = 1;          // channel #1:  /Ok for NXP LPC21xx Measure/
  s.ser.itemMode    = 0;          // Byte
  s.ser.nMany       = 2;          // 2 items
  s.ser.s.aBytes[0] = 'd';        // 'd'
  s.ser.s.aBytes[1] = 0x0A;       //    ,CR
  nSize = offsetof (SERIO, s.aBytes) + 2;  // +2 weil 2 Zeichen
  //_dlg->CreateCommandSend (UV_DBG_SERIAL_PUT, nSize, &s.ser);

  // TODO
}




void CDebug2Dlg::OnGetvtr()  {         // Read VTR-Value
  ThreadMsg msg;
  VSET *pVs = &msg.vsetData;
  int n;

  CEdit *pE = (CEdit *) GetDlgItem (IDC_VTRNAME);
  pVs->str.nLen  = pE->GetWindowText (pVs->str.szStr, sizeof (pVs->str.szStr) - 1);
  pVs->str.szStr [pVs->str.nLen] = 0;
  if (pVs->str.nLen == 0)  {
    MessageBeep (MB_OK);
    AfxMessageBox ("No VTR name specified");
    GetDlgItem (IDC_VTRNAME)->SetFocus();
    return;
  }
  n  = sizeof (VSET) - sizeof (pVs->str.szStr);
  n += pVs->str.nLen + 1;
  pVs->val.vType = VTT_void;
  pVs->val.v.u64 = 0;

  msg.cmd = UV_DBG_VTR_GET;
  msg.dataLen = n;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}


void CDebug2Dlg::OnSetvtr()  {
  ThreadMsg msg;
  VSET *pVs = &msg.vsetData;
  int     n;


  /* First call UV_DBG_CALC_EXPRESSION to convert the data specified in
     VTR value to a TVAL, and store it in TV to be written in UV_DBG_VTR_SET */
  CEdit *pE = (CEdit *) GetDlgItem (IDC_VTRVAL);
  pVs->str.nLen  = pE->GetWindowText (pVs->str.szStr, sizeof (pVs->str.szStr) - 1);
  pVs->str.szStr [pVs->str.nLen] = 0;
  if (pVs->str.nLen == 0)  {
    MessageBeep (MB_OK);
    AfxMessageBox ("VTR value not specified or invalid");
    GetDlgItem (IDC_VTRVAL)->SetFocus();
    return;
  }
  pVs->val.vType = VTT_void;
  pVs->val.v.u64 = 0;

  n  = sizeof (VSET) - sizeof (pVs->str.szStr);
  n += pVs->str.nLen + 1;

  msg.cmd = UV_DBG_CALC_EXPRESSION;
  msg.dataLen = n;
  msg.connHandle = _dlg->UVSCConnHandle;

  g_UVSCOpNotify = UVSC_OP_NOTIFY_PENDING;      // Set op pending
  if (!UVSCThread_Post(&msg, 0)) {
    g_UVSCOpNotify = UVSC_OP_NOTIFY_FAILED;     // Set op failed (avoids deadlock on queue error)
  }

  while (g_UVSCOpNotify == UVSC_OP_NOTIFY_PENDING)  {
    DoEvents();
  }
  if (g_UVSCOpNotify != UVSC_OP_NOTIFY_SUCCESS)  {
    MessageBeep (MB_OK);
    AfxMessageBox ("Calc Expression failed (possible VTR value invalid?)");
    GetDlgItem (IDC_VTRVAL)->SetFocus();
    return;
  }

  pVs->val.v.u64 = g_CalcExprRes.v.u64;          // value
  pVs->val.vType = g_CalcExprRes.vType;          // Type

  pE = (CEdit *) GetDlgItem (IDC_VTRNAME);
  pVs->str.nLen  = pE->GetWindowText (pVs->str.szStr, sizeof (pVs->str.szStr) - 1);
  pVs->str.szStr [pVs->str.nLen] = 0;
  if (pVs->str.nLen == 0)  {
    MessageBeep (MB_OK);
    AfxMessageBox ("No VTR name specified");
    GetDlgItem (IDC_VTRNAME)->SetFocus();
    return;
  }

  n  = sizeof (VSET) - sizeof (pVs->str.szStr);
  n += pVs->str.nLen + 1;

  msg.cmd = UV_DBG_VTR_SET;
  msg.dataLen = n;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}

void CDebug2Dlg::OnGetmemadr()  {      // GetMemoryAddress Event (CALC EXPRESSION)
  ThreadMsg msg;
  VSET      *pVs = &msg.vsetData;
  char      szB[256];
  int       n, i;

  CEdit *pE = (CEdit *) GetDlgItem (IDC_EDUNTIL);

  szB[0] = ' ';
  n = pE->GetWindowText (&szB[1], sizeof (szB) - 3);
  szB [n + 1] = 0;
  for ( i = 1 ; i < n ; ++i )  {
    if (szB[i] != ' ') break;
  }

  ++n;

  pVs->str.nLen  = n;
  strcpy (pVs->str.szStr, szB);

  if (pVs->str.nLen <= 1)  {
    MessageBeep (MB_OK);
    AfxMessageBox ("Bad or missing Expression");
    GetDlgItem (IDC_EDUNTIL)->SetFocus();
    return;
  }
  pVs->val.vType = VTT_void;
  pVs->val.v.u64 = 0;

  n  = sizeof (VSET) - sizeof (pVs->str.szStr);
  n += pVs->str.nLen + 1;

  msg.cmd = UV_DBG_CALC_EXPRESSION;
  msg.dataLen = n;
  msg.connHandle = _dlg->UVSCConnHandle;

  g_UVSCOpNotify = UVSC_OP_NOTIFY_PENDING;      // Set op pending
  if (!UVSCThread_Post(&msg, 0)) {
    g_UVSCOpNotify = UVSC_OP_NOTIFY_FAILED;     // Set op failed (avoids deadlock on queue error)
  }

  while (g_UVSCOpNotify == UVSC_OP_NOTIFY_PENDING)  {
    DoEvents();
  }
  if (g_UVSCOpNotify != UVSC_OP_NOTIFY_SUCCESS)  {
    SetDlgItemText (IDC_SHOWMA, "Result := <invalid>");
    memA = -1;
    MessageBeep (MB_OK);
    AfxMessageBox ("Calc Expression failed (possible bad expression?)");
    GetDlgItem (IDC_EDUNTIL)->SetFocus();
    return;
  }

  memA = g_CalcExprRes.v.i64;
  sprintf (szB, "Addr := 0x%08X", (DWORD) memA);
  SetDlgItemText (IDC_SHOWMA, szB);
}



void CDebug2Dlg::OnReadmem()  {        // ReadMemory Event
  ThreadMsg msg;
  AMEM      *pMa = &msg.amemData;
  int       n;

  if (memA == -1)  {                // MemoryAddress not set
    MessageBeep (MB_OK);
    AfxMessageBox ("Memory Address not set");
    GetDlgItem (IDC_EDUNTIL)->SetFocus();
    return;
  }    

  pMa->nAddr  = (UINT) memA;          // Address
  pMa->nBytes = 128;                // read 128 Bytes
  n  = sizeof (AMEM);
  n += 128;

  msg.cmd = UV_DBG_MEM_READ;
  msg.dataLen = n;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}



void CDebug2Dlg::OnWritemem()  {       // WriteMemory Event
  int       i, n;
  ThreadMsg msg;
  AMEM      *pA = &msg.amemData;

  if (memA == -1)  {                // MemoryAddress not set
    MessageBeep (MB_OK);
    AfxMessageBox ("Memory Address not set");
    GetDlgItem (IDC_EDUNTIL)->SetFocus();
    return;
  }

  // Example: write 128 bytes to address 'memA':
  memset (pA, 0, sizeof (AMEM));
  pA->nAddr  = (UINT) memA;
  pA->nBytes = 128;                 // number of bytes to write
  for ( i = 0 ; i < 128 ; ++i )  {
    pA->aBytes[i] = (BYTE) i;
  }
  n  = sizeof (AMEM);
  n += 128;                          

  msg.cmd = UV_DBG_MEM_WRITE;
  msg.dataLen = n;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);

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

void CDebug2Dlg::OnBnClickedIdenum()  {
  int         n, i;
  ThreadMsg msg;
  ENUMTPM   *pTpm = &msg.tpmData;
  CEdit     *pE;

  pE = (CEdit *) GetDlgItem (IDC_EDENID);
  memset (pTpm, 0, sizeof (ENUMTPM));
  pTpm->szID[0] = ' ';
  n = pE->GetWindowText (&pTpm->szID[1], sizeof (pTpm->szID) - 3);
  pTpm->szID [n + 1] = 0;

  for ( i = 1 ; i < n ; ++i )  {
    if (pTpm->szID[i] != ' ') break;
  }
  if (i == 0)  {
    MessageBeep (MB_OK);
    AfxMessageBox ("Bad or missing structure member");
    GetDlgItem (IDC_EDENID)->SetFocus();
    return;
  }

  pTpm->Job = UV_TPENUM_MEMBERS;
  n = offsetof (ENUMTPM, szID) + strlen (pTpm->szID) + 2;  // leading ' ' + 0.

  msg.cmd = UV_DBG_ENUM_SYMTP;
  msg.dataLen = n;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);

}


/*
 * Map current address to File and Linenumber
 */
void CDebug2Dlg::OnBnClickedMaptofl()  {
  ThreadMsg msg;
  ADRMTFL    *pA = &msg.adrmtflData;

  if (memA == -1)  {                // MemoryAddress not set
    MessageBeep (MB_OK);
    AfxMessageBox ("Memory Address not set");
    GetDlgItem (IDC_EDUNTIL)->SetFocus();
    return;
  }

  memset (pA, 0, sizeof (ADRMTFL));
  pA->nAdr  = memA;                 // address to resolve
//pA->bFull = 0;                    // want relative path of file
  pA->bFull = 1;                    // want full path file

  msg.cmd = UV_DBG_ADR_TOFILELINE;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}


/*
 * Enumerate Call-Stack
 */
void CDebug2Dlg::OnBnClickedEnumstack()  {
  ThreadMsg msg;
  iSTKENUM    *pA = &msg.iStkEnumData;
  
  memset (pA, 0, sizeof (iSTKENUM));
//pA->bFull = 0;                    // want relative path of file
  pA->bFull = 1;                    // want full path file

  msg.cmd = UV_DBG_ENUM_STACK;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}


/*
 * Enumerate Vtr
 */
void CDebug2Dlg::OnBnClickedEnumvtr()  {
  ThreadMsg msg;
  iVTRENUM  *piVt = &msg.iVTREnumData;

  memset (piVt, 0, sizeof (iVTRENUM));
  piVt->bValue = 0;                   // name and type only, no value

  msg.cmd = UV_DBG_ENUM_VTR;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);

}


/*
 * Enumerate Vtr - show also values
 */
void CDebug2Dlg::OnBnClickedEnumvtrv()  {
  ThreadMsg msg;
  iVTRENUM  *piVt = &msg.iVTREnumData;

  memset (piVt, 0, sizeof (iVTRENUM));
  piVt->bValue = 1;                   // name and type only, no value

  msg.cmd = UV_DBG_ENUM_VTR;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}


/*
 * Show Disassembly and HLL file (if possible) for given address
 */
void CDebug2Dlg::OnBnClickedShowcode()  {
  ThreadMsg msg;
  iSHOWSYNC  *pScy = &msg.iShowSyncData;

  if (memA == -1)  {                // MemoryAddress not set
    MessageBeep (MB_OK);
    AfxMessageBox ("Memory Address not set");
    GetDlgItem (IDC_EDUNTIL)->SetFocus();
    return;
  }

  memset (pScy, 0, sizeof (iSHOWSYNC));
  pScy->nAdr = memA;                  // address to show code for
  pScy->bAsm = 1;
  pScy->bHll = 1;

  msg.cmd = UV_DBG_ADR_SHOWCODE;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}



// UVSC_PSTAMP
void CDebug2Dlg::OnBnClickedPowerscaleShowcode() {
  ThreadMsg msg;
  UVSC_PSTAMP *pPSCData = &msg.powerScaleData;

  if (memA == -1)  {                // MemoryAddress not set
    MessageBeep (MB_OK);
    AfxMessageBox ("Memory Address not set");
    GetDlgItem (IDC_EDUNTIL)->SetFocus();
    return;
  }

  memset (pPSCData, 0, sizeof (UVSC_PSTAMP));
  pPSCData->nAdr        = memA;                  // address to show code for
  pPSCData->ticks       = 12345678;
  pPSCData->delta       = 0.12345678;
  pPSCData->time        = 0;
  pPSCData->showSyncErr = 1;

  msg.cmd = UV_DBG_POWERSCALE_SHOWCODE;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}





void CDebug2Dlg::OnBnClickedButtonItmRegister()
{
  ThreadMsg msg;
  ITMOUT *pM = &msg.itmIo;

  CEdit *pE = (CEdit *) GetDlgItem (IDC_EDIT_ITM_CHNO);

  char chNum[8];
  int nLen = pE->GetWindowText (chNum, 8 - 1);
  if (nLen == 0)  {
    MessageBeep (MB_OK);
    AfxMessageBox ("No ITM Channel number specified");
    GetDlgItem (IDC_EDIT_ITM_CHNO)->SetFocus();
    return;
  }
  pM->nChannel = atoi(chNum);
  pM->itemMode = 0;
  pM->nMany = 0;

  msg.cmd = UV_DBG_ITM_REGISTER;
  msg.dataLen = sizeof(ITMOUT);
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}


void CDebug2Dlg::OnBnClickedButtonItmUnregister()
{
  ThreadMsg msg;
  ITMOUT *pM = &msg.itmIo;

  CEdit *pE = (CEdit *) GetDlgItem (IDC_EDIT_ITM_CHNO);

  char chNum[8];
  int nLen = pE->GetWindowText (chNum, 8 - 1);
  if (nLen == 0)  {
    MessageBeep (MB_OK);
    AfxMessageBox ("No ITM Channel number specified");
    GetDlgItem (IDC_EDIT_ITM_CHNO)->SetFocus();
    return;
  }
  pM->nChannel = atoi(chNum);
  pM->itemMode = 0;
  pM->nMany = 0;

  msg.cmd = UV_DBG_ITM_UNREGISTER;
  msg.dataLen = sizeof(ITMOUT);
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}

void CDebug2Dlg::OnBnClickedButtonEvtrRegister()
{
  ThreadMsg msg;

  msg.cmd = UV_DBG_EVTR_REGISTER;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}

void CDebug2Dlg::OnBnClickedButtonEvtrUnregister()
{
  ThreadMsg msg;

  msg.cmd = UV_DBG_EVTR_UNREGISTER;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}

void CDebug2Dlg::OnBnClickedButtonEvtrGetscvd()
{
  ThreadMsg msg;

  msg.cmd = UV_DBG_EVTR_ENUMSCVDFILES;
  msg.connHandle = _dlg->UVSCConnHandle;
  UVSCThread_Post(&msg, 0);
}
