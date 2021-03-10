// SPeriDLL.h : main header file for the SPERIDLL DLL
//

#if !defined(AFX_SPERIDLL_H__83C48106_3E31_11D4_B027_E953FF205073__INCLUDED_)
#define AFX_SPERIDLL_H__83C48106_3E31_11D4_B027_E953FF205073__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSPeriDLLApp
// See SPeriDLL.cpp for the implementation of this class
//

class CSPeriDLLApp : public CWinApp
{
public:
	CSPeriDLLApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSPeriDLLApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSPeriDLLApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

// Callback functions that are ONLY allowed to call during the initialisation process
typedef AGSIEXPORT BOOL      (*tAgsiDefineSFR) (const char* pszSfrName, AGSIADDR dwAddress, AGSITYPE eType, BYTE bBitPos);
typedef AGSIEXPORT AGSIVTR   (*tAgsiDefineVTR) (const char* pszVtrName, AGSITYPE eType, DWORD dwValue);
typedef AGSIEXPORT BOOL      (*tAgsiDeclareInterrupt)(AGSIINTERRUPT *pInterrupt);
typedef AGSIEXPORT BOOL      (*tAgsiSetWatchOnSFR) (AGSIADDR SFRAddress, AGSICALLBACK pfnReadWrite, AGSIACCESS eAccess);
typedef AGSIEXPORT BOOL      (*tAgsiSetWatchOnVTR) (AGSIVTR hVTR, AGSICALLBACK pfnReadWrite, AGSIACCESS eAccess);
typedef AGSIEXPORT BOOL      (*tAgsiSetWatchOnMemory) (AGSIADDR StartAddress, AGSIADDR EndAddress, AGSICALLBACK pfnReadWrite, AGSIACCESS eAccess);
typedef AGSIEXPORT AGSITIMER (*tAgsiCreateTimer) (AGSICALLBACK pfnTimer);
typedef AGSIEXPORT BOOL      (*tAgsiDefineMenuItem)(AGSIMENU *pDym);

// Callback functions that are NOT allowed to call during the initialisation process
typedef AGSIEXPORT BOOL      (*tAgsiWriteSFR) (AGSIADDR SFRAddress, DWORD dwValue, DWORD dwMask);
typedef AGSIEXPORT BOOL      (*tAgsiReadSFR)  (AGSIADDR SFRAddress, DWORD* pdwCurrentValue, DWORD* pdwPreviousValue, DWORD dwMask);
typedef AGSIEXPORT BOOL      (*tAgsiWriteVTR) (AGSIVTR hVTR, DWORD dwValue);
typedef AGSIEXPORT BOOL      (*tAgsiReadVTR) (AGSIVTR hVTR, DWORD* pdwCurrentValue);
typedef AGSIEXPORT BOOL      (*tAgsiSetSFRReadValue)(DWORD dwValue);
typedef AGSIEXPORT BOOL      (*tAgsiReadMemory) (AGSIADDR Address, DWORD dwCount, BYTE* pbValue);
typedef AGSIEXPORT BOOL      (*tAgsiWriteMemory)(AGSIADDR Address, DWORD dwCount, BYTE* pbValue);
typedef AGSIEXPORT AGSIADDR  (*tAgsiGetLastMemoryAddress)(void);
typedef AGSIEXPORT BOOL      (*tAgsiIsSimulatorAccess)(void);
typedef AGSIEXPORT BOOL      (*tAgsiSetTimer)(AGSITIMER hTimer, DWORD dwClock);
typedef AGSIEXPORT UINT64    (*tAgsiGetStates)(void);
typedef AGSIEXPORT AGSIADDR  (*tAgsiGetProgramCounter)(void);
typedef AGSIEXPORT DWORD     (*tAgsiIsInInterrupt)(void);
typedef AGSIEXPORT BOOL      (*tAgsiIsSleeping)(void);
typedef AGSIEXPORT void      (*tAgsiStopSimulator)(void);
typedef AGSIEXPORT void      (*tAgsiTriggerReset)(void);
typedef AGSIEXPORT void      (*tAgsiUpdateWindows)(void);
typedef AGSIEXPORT void      (*tAgsiHandleFocus) (HWND hwndDialog);
typedef AGSIEXPORT DWORD     (*tAgsiGetExternalClockRate)(void);
typedef AGSIEXPORT DWORD     (*tAgsiGetInternalClockRate)(void);
typedef AGSIEXPORT double    (*tAgsiGetClockFactor)(void);
typedef AGSIEXPORT void      (*tAgsiMessage)(const char* pszFormat, ...);
typedef AGSIEXPORT const char*(*tAgsiGetTargetKey)(const char* pszKey);
typedef AGSIEXPORT BOOL      (*tAgsiSetTargetKey)(const char* pszKey, const char *pszString);
typedef AGSIEXPORT DWORD     (*tAgsiGetSymbolByName) (AGSISYMDSC *vp);
typedef AGSIEXPORT DWORD     (*tAgsiGetSymbolByValue)(AGSISYMDSC *vp);

struct AGSIFUNCS {
  tAgsiDefineSFR             DefineSFR;
  tAgsiDefineVTR             DefineVTR;
  tAgsiSetWatchOnSFR         SetWatchOnSFR;
  tAgsiSetWatchOnVTR         SetWatchOnVTR;
  tAgsiSetWatchOnMemory      SetWatchOnMemory;
  tAgsiCreateTimer           CreateTimer;
  tAgsiDefineMenuItem        DefineMenuItem;
  tAgsiWriteSFR              WriteSFR;
  tAgsiReadSFR               ReadSFR;
  tAgsiWriteVTR              WriteVTR;
  tAgsiReadVTR               ReadVTR;
  tAgsiSetSFRReadValue       SetSFRReadValue;
  tAgsiReadMemory            ReadMemory;
  tAgsiWriteMemory           WriteMemory;
  tAgsiGetLastMemoryAddress  GetLastMemoryAddress;
  tAgsiIsSimulatorAccess     IsSimulatorAccess;
  tAgsiSetTimer              SetTimer;
  tAgsiGetStates             GetStates;
  tAgsiGetProgramCounter     GetProgramCounter;
  tAgsiDeclareInterrupt      DeclareInterrupt;
  tAgsiIsInInterrupt         IsInInterrupt;
  tAgsiIsSleeping            IsSleeping;
  tAgsiStopSimulator         StopSimulator;
  tAgsiTriggerReset          TriggerReset;
  tAgsiUpdateWindows         UpdateWindows;
  tAgsiHandleFocus           HandleFocus;
  tAgsiGetExternalClockRate  GetExternalClockRate;
  tAgsiGetInternalClockRate  GetInternalClockRate;
  tAgsiGetClockFactor        GetClockFactor;
  tAgsiMessage               Message;
  tAgsiGetTargetKey          GetTargetKey;
  tAgsiSetTargetKey          SetTargetKey;
  tAgsiGetSymbolByName       GetSymbolByName;
  tAgsiGetSymbolByValue      GetSymbolByValue;
};

extern struct AGSIFUNCS  Agsi;

union fv {      // float value union
  float f;
  DWORD DW;
};

// SFR's
#define IE       0xA8
#define IP       0xB8
#define IPH      0xB7
#define ADCCON1  0xEF
#define ADCCON2  0xD8
#define ADCCON3  0xF5
#define DMAL     0xD2
#define DMAH     0xD3
#define DMAP     0xD4
#define ADCDATAL 0xD9
#define ADCDATAH 0xDA

struct vtrlist {
  char     *pName;       // name of VTREG
  AGSITYPE Type;         // type of VTREG
  DWORD    Value;         // value of VTREG
  AGSIVTR  hVTR;         // handle of VTREG
};

// VTR's
extern struct vtrlist VTREG[];

#define AIN0   (VTREG[ 0].hVTR)
#define AIN1   (VTREG[ 1].hVTR)
#define AIN2   (VTREG[ 2].hVTR)
#define AIN3   (VTREG[ 3].hVTR)
#define AIN4   (VTREG[ 4].hVTR)
#define AIN5   (VTREG[ 5].hVTR)
#define AIN6   (VTREG[ 6].hVTR)
#define AIN7   (VTREG[ 7].hVTR)
#define TEMP   (VTREG[ 8].hVTR)
#define VREF   (VTREG[ 9].hVTR)
#define XTAL   (VTREG[10].hVTR)
#define CONVST (VTREG[11].hVTR)


void AdcConv(void);
void AdcCompleted(void);


#endif // !defined(AFX_SPERIDLL_H__83C48106_3E31_11D4_B027_E953FF205073__INCLUDED_)
