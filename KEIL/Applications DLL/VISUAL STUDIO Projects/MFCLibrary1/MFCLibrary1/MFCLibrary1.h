// MFCLibrary1.h : main header file for the MFCLibrary1 DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "AGSI.h"

#define LPC_EMAC_CNT	46

#define DECLARE_ACCESSMEM_HANDLER(name) static void name(AGSIADDR addr, int reason)
#define OFFSET_AGSIADDR(ptr, offset) (ptr + (offset * sizeof ptr))

typedef void (AGSICALLBACKA)(AGSIADDR);
typedef void (AGSICALLBACKB)(AGSIADDR, int);

// Callback functions that are ONLY allowed to call during the initialisation process
typedef AGSIEXPORT BOOL(*tAgsiDefineSFR) (const char* pszSfrName, AGSIADDR dwAddress, AGSITYPE eType, BYTE bBitPos);
typedef AGSIEXPORT AGSIVTR(*tAgsiDefineVTR) (const char* pszVtrName, AGSITYPE eType, DWORD dwValue);
typedef AGSIEXPORT BOOL(*tAgsiDeclareInterrupt)(AGSIINTERRUPT* pInterrupt);
typedef AGSIEXPORT BOOL(*tAgsiSetWatchOnSFR) (AGSIADDR SFRAddress, AGSICALLBACK pfnReadWrite, AGSIACCESS eAccess);
typedef AGSIEXPORT BOOL(*tAgsiSetWatchOnVTR) (AGSIVTR hVTR, AGSICALLBACK pfnReadWrite, AGSIACCESS eAccess);
typedef AGSIEXPORT BOOL(*tAgsiSetWatchOnMemory) (AGSIADDR StartAddress, AGSIADDR EndAddress, AGSICALLBACKB pfnReadWrite, AGSIACCESS eAccess);
typedef AGSIEXPORT AGSITIMER(*tAgsiCreateTimer) (AGSICALLBACK pfnTimer);
typedef AGSIEXPORT BOOL(*tAgsiDefineMenuItem)(AGSIMENU* pDym);

// Callback functions that are NOT allowed to call during the initialisation process
typedef AGSIEXPORT BOOL(*tAgsiWriteSFR) (AGSIADDR SFRAddress, DWORD dwValue, DWORD dwMask);
typedef AGSIEXPORT BOOL(*tAgsiReadSFR)  (AGSIADDR SFRAddress, DWORD* pdwCurrentValue, DWORD* pdwPreviousValue, DWORD dwMask);
typedef AGSIEXPORT BOOL(*tAgsiWriteVTR) (AGSIVTR hVTR, DWORD dwValue);
typedef AGSIEXPORT BOOL(*tAgsiReadVTR) (AGSIVTR hVTR, DWORD* pdwCurrentValue);
typedef AGSIEXPORT BOOL(*tAgsiSetSFRReadValue)(DWORD dwValue);
typedef AGSIEXPORT BOOL(*tAgsiReadMemory) (AGSIADDR Address, DWORD dwCount, BYTE* pbValue);
typedef AGSIEXPORT BOOL(*tAgsiWriteMemory)(AGSIADDR Address, DWORD dwCount, BYTE* pbValue);
typedef AGSIEXPORT AGSIADDR(*tAgsiGetLastMemoryAddress)(void);
typedef AGSIEXPORT BOOL(*tAgsiIsSimulatorAccess)(void);
typedef AGSIEXPORT BOOL(*tAgsiSetTimer)(AGSITIMER hTimer, DWORD dwClock);
typedef AGSIEXPORT UINT64(*tAgsiGetStates)(void);
typedef AGSIEXPORT AGSIADDR(*tAgsiGetProgramCounter)(void);
typedef AGSIEXPORT DWORD(*tAgsiIsInInterrupt)(void);
typedef AGSIEXPORT BOOL(*tAgsiIsSleeping)(void);
typedef AGSIEXPORT void      (*tAgsiStopSimulator)(void);
typedef AGSIEXPORT void      (*tAgsiTriggerReset)(void);
typedef AGSIEXPORT void      (*tAgsiUpdateWindows)(void);
typedef AGSIEXPORT void      (*tAgsiHandleFocus) (HWND hwndDialog);
typedef AGSIEXPORT DWORD(*tAgsiGetExternalClockRate)(void);
typedef AGSIEXPORT DWORD(*tAgsiGetInternalClockRate)(void);
typedef AGSIEXPORT double    (*tAgsiGetClockFactor)(void);
typedef AGSIEXPORT void      (*tAgsiMessage)(const char* pszFormat, ...);
typedef AGSIEXPORT const char* (*tAgsiGetTargetKey)(const char* pszKey);
typedef AGSIEXPORT BOOL(*tAgsiSetTargetKey)(const char* pszKey, const char* pszString);
typedef AGSIEXPORT DWORD(*tAgsiGetSymbolByName) (AGSISYMDSC* vp);
typedef AGSIEXPORT DWORD(*tAgsiGetSymbolByValue)(AGSISYMDSC* vp);

static const AGSIADDR LPC_EMAC_BASE					= 0x50000000;

static const AGSIADDR LPC_EMAC_BASE_MACREGS			= LPC_EMAC_BASE + 0;
static const AGSIADDR LPC_EMAC_BASE_CTRLREGS		= LPC_EMAC_BASE + 0x100;
static const AGSIADDR LPC_EMAC_BASE_RXFLTREGS		= LPC_EMAC_BASE + 0x200;
static const AGSIADDR LPC_EMAC_BASE_MDLCTRLREGS		= LPC_EMAC_BASE + 0xfe0;

static const AGSIADDR LPC_EMAC_MAC1					= OFFSET_AGSIADDR(LPC_EMAC_BASE_MACREGS, 0);

static const AGSIADDR LPC_EMAC_RxDescriptor			= OFFSET_AGSIADDR(LPC_EMAC_BASE_CTRLREGS, 2);
static const AGSIADDR LPC_EMAC_RxDescriptorNumber	= OFFSET_AGSIADDR(LPC_EMAC_BASE_CTRLREGS, 4);
static const AGSIADDR LPC_EMAC_RxProduceIndex		= OFFSET_AGSIADDR(LPC_EMAC_BASE_CTRLREGS, 5);
static const AGSIADDR LPC_EMAC_RxConsumeIndex		= OFFSET_AGSIADDR(LPC_EMAC_BASE_CTRLREGS, 6);
static const AGSIADDR LPC_EMAC_TxDescriptor			= OFFSET_AGSIADDR(LPC_EMAC_BASE_CTRLREGS, 7);
static const AGSIADDR LPC_EMAC_TxProduceIndex		= OFFSET_AGSIADDR(LPC_EMAC_BASE_CTRLREGS, 10);
static const AGSIADDR LPC_EMAC_TxConsumeIndex		= OFFSET_AGSIADDR(LPC_EMAC_BASE_CTRLREGS, 11);

static const AGSIADDR LPC_EMAC_PowerDown			= OFFSET_AGSIADDR(LPC_EMAC_BASE_MDLCTRLREGS, 5);

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

typedef struct {
	DWORD mac_dest0;
	WORD  mac_dest1;

	DWORD mac_src0;
	WORD  mac_src1;

} ethframe;

DWORD GetFunctionPointers(void);
BOOL InitAction(AGSICONFIG* cfg);
void PostInitAction();
DECLARE_ACCESSMEM_HANDLER(OnEMACTxDescriptorWRAccessHandler);
DECLARE_ACCESSMEM_HANDLER(OnEMACRxDescriptorWRAccessHandler);
DECLARE_ACCESSMEM_HANDLER(OnEMACTxProduceIndexChangeHandler);
void OnNewFrameAvailableCallback(BYTE* frame, DWORD frameSize);
static void OnEMACMemoryAccessHandler(AGSIADDR addr, int reason);


class CMFCLibrary1App : public CWinApp {
public:
	CMFCLibrary1App();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
