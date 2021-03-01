// SPeriDLL.cpp : Defines the initialization routines for the DLL.
// Sample Peripheral DLL: A/D Converter for ADuC812

#include "stdafx.h"
#include "Agsi.h"
#include "SPeriDLL.h"
#include "PeriDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CSPeriDLLApp

BEGIN_MESSAGE_MAP(CSPeriDLLApp, CWinApp)
	//{{AFX_MSG_MAP(CSPeriDLLApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static AGSICONFIG  AgsiConfig;
struct AGSIFUNCS   Agsi;

DWORD GetFunctionPointers(void) {           // get all function pointers

  Agsi.DefineSFR            = (tAgsiDefineSFR)             GetProcAddress(AgsiConfig.m_hInstance, "AgsiDefineSFR");
  Agsi.DefineVTR            = (tAgsiDefineVTR)             GetProcAddress(AgsiConfig.m_hInstance, "AgsiDefineVTR");
  Agsi.DeclareInterrupt     = (tAgsiDeclareInterrupt)      GetProcAddress(AgsiConfig.m_hInstance, "AgsiDeclareInterrupt");
  Agsi.SetWatchOnSFR        = (tAgsiSetWatchOnSFR)         GetProcAddress(AgsiConfig.m_hInstance, "AgsiSetWatchOnSFR");
  Agsi.SetWatchOnVTR        = (tAgsiSetWatchOnVTR)         GetProcAddress(AgsiConfig.m_hInstance, "AgsiSetWatchOnVTR");
  Agsi.SetWatchOnMemory     = (tAgsiSetWatchOnMemory)      GetProcAddress(AgsiConfig.m_hInstance, "AgsiSetWatchOnMemory");
  Agsi.CreateTimer          = (tAgsiCreateTimer)           GetProcAddress(AgsiConfig.m_hInstance, "AgsiCreateTimer");
  Agsi.DefineMenuItem       = (tAgsiDefineMenuItem)        GetProcAddress(AgsiConfig.m_hInstance, "AgsiDefineMenuItem");

  Agsi.WriteSFR             = (tAgsiWriteSFR)              GetProcAddress(AgsiConfig.m_hInstance, "AgsiWriteSFR");
  Agsi.ReadSFR              = (tAgsiReadSFR)               GetProcAddress(AgsiConfig.m_hInstance, "AgsiReadSFR");
  Agsi.WriteVTR             = (tAgsiWriteVTR)              GetProcAddress(AgsiConfig.m_hInstance, "AgsiWriteVTR");
  Agsi.ReadVTR              = (tAgsiReadVTR)               GetProcAddress(AgsiConfig.m_hInstance, "AgsiReadVTR");
  Agsi.SetSFRReadValue      = (tAgsiSetSFRReadValue)       GetProcAddress(AgsiConfig.m_hInstance, "AgsiSetSFRReadValue");
  Agsi.ReadMemory           = (tAgsiReadMemory)            GetProcAddress(AgsiConfig.m_hInstance, "AgsiReadMemory");
  Agsi.WriteMemory          = (tAgsiWriteMemory)           GetProcAddress(AgsiConfig.m_hInstance, "AgsiWriteMemory");
  Agsi.GetLastMemoryAddress = (tAgsiGetLastMemoryAddress)  GetProcAddress(AgsiConfig.m_hInstance, "AgsiGetLastMemoryAddress");
  Agsi.IsSimulatorAccess    = (tAgsiIsSimulatorAccess)     GetProcAddress(AgsiConfig.m_hInstance, "AgsiIsSimulatorAccess");
  Agsi.SetTimer             = (tAgsiSetTimer)              GetProcAddress(AgsiConfig.m_hInstance, "AgsiSetTimer");
  Agsi.GetStates            = (tAgsiGetStates)             GetProcAddress(AgsiConfig.m_hInstance, "AgsiGetStates");
  Agsi.GetProgramCounter    = (tAgsiGetProgramCounter)     GetProcAddress(AgsiConfig.m_hInstance, "AgsiGetProgramCounter");
  Agsi.IsInInterrupt        = (tAgsiIsInInterrupt)         GetProcAddress(AgsiConfig.m_hInstance, "AgsiIsInInterrupt");
  Agsi.IsSleeping           = (tAgsiIsSleeping)            GetProcAddress(AgsiConfig.m_hInstance, "AgsiIsSleeping");
  Agsi.StopSimulator        = (tAgsiStopSimulator)         GetProcAddress(AgsiConfig.m_hInstance, "AgsiStopSimulator");
  Agsi.TriggerReset         = (tAgsiTriggerReset)          GetProcAddress(AgsiConfig.m_hInstance, "AgsiTriggerReset");
  Agsi.UpdateWindows        = (tAgsiUpdateWindows)         GetProcAddress(AgsiConfig.m_hInstance, "AgsiUpdateWindows");
  Agsi.HandleFocus          = (tAgsiHandleFocus)           GetProcAddress(AgsiConfig.m_hInstance, "AgsiHandleFocus");
  Agsi.GetExternalClockRate = (tAgsiGetExternalClockRate)  GetProcAddress(AgsiConfig.m_hInstance, "AgsiGetExternalClockRate");
  Agsi.GetInternalClockRate = (tAgsiGetInternalClockRate)  GetProcAddress(AgsiConfig.m_hInstance, "AgsiGetInternalClockRate");
  Agsi.GetClockFactor       = (tAgsiGetClockFactor)        GetProcAddress(AgsiConfig.m_hInstance, "AgsiGetClockFactor");
  Agsi.Message              = (tAgsiMessage)               GetProcAddress(AgsiConfig.m_hInstance, "AgsiMessage");
  Agsi.GetTargetKey         = (tAgsiGetTargetKey)          GetProcAddress(AgsiConfig.m_hInstance, "AgsiGetTargetKey");
  Agsi.SetTargetKey         = (tAgsiSetTargetKey)          GetProcAddress(AgsiConfig.m_hInstance, "AgsiSetTargetKey");
  Agsi.GetSymbolByName      = (tAgsiGetSymbolByName)       GetProcAddress(AgsiConfig.m_hInstance, "AgsiGetSymbolByName");
  Agsi.GetSymbolByValue     = (tAgsiGetSymbolByValue)      GetProcAddress(AgsiConfig.m_hInstance, "AgsiGetSymbolByValue");

  if ((Agsi.DefineSFR            == NULL) ||
      (Agsi.DefineVTR            == NULL) ||
      (Agsi.DeclareInterrupt     == NULL) ||
      (Agsi.SetWatchOnSFR        == NULL) ||
      (Agsi.SetWatchOnVTR        == NULL) ||
      (Agsi.SetWatchOnMemory     == NULL) ||
      (Agsi.CreateTimer          == NULL) ||
      (Agsi.DefineMenuItem       == NULL) ||

      (Agsi.WriteSFR             == NULL) ||
      (Agsi.ReadSFR              == NULL) ||
      (Agsi.WriteVTR             == NULL) ||
      (Agsi.ReadVTR              == NULL) ||
      (Agsi.SetSFRReadValue      == NULL) ||
      (Agsi.ReadMemory           == NULL) ||
      (Agsi.WriteMemory          == NULL) ||
      (Agsi.GetLastMemoryAddress == NULL) ||
      (Agsi.IsSimulatorAccess    == NULL) ||
      (Agsi.SetTimer             == NULL) ||
      (Agsi.GetStates            == NULL) ||
      (Agsi.GetProgramCounter    == NULL) ||
      (Agsi.IsInInterrupt        == NULL) ||
      (Agsi.IsSleeping           == NULL) ||
      (Agsi.StopSimulator        == NULL) ||
      (Agsi.TriggerReset         == NULL) ||
      (Agsi.UpdateWindows        == NULL) ||
      (Agsi.HandleFocus          == NULL) ||
      (Agsi.GetExternalClockRate == NULL) ||
      (Agsi.GetInternalClockRate == NULL) ||
      (Agsi.GetClockFactor       == NULL) ||
      (Agsi.Message              == NULL) ||
      (Agsi.GetTargetKey         == NULL) ||
      (Agsi.SetTargetKey         == NULL) ||
      (Agsi.GetSymbolByName      == NULL) ||
      (Agsi.GetSymbolByValue     == NULL)) {

    return(FALSE);
  }

  return(1);
}


DWORD DefineAllSFR(void) {           // declare all special function registers and their bits
  BOOL ret = TRUE;

  ret &= Agsi.DefineSFR("ADCCON1",  ADCCON1,  AGSIBYTE, 0);  // If more SFR's are defined, do it in a table
  ret &= Agsi.DefineSFR("ADCCON2",  ADCCON2,  AGSIBYTE, 0);
  ret &= Agsi.DefineSFR("ADCCON3",  ADCCON3,  AGSIBYTE, 0);
  ret &= Agsi.DefineSFR("ADCDATAL", ADCDATAL, AGSIBYTE, 0);
  ret &= Agsi.DefineSFR("ADCDATAH", ADCDATAH, AGSIBYTE, 0);
  ret &= Agsi.DefineSFR("DMAL",     DMAL,     AGSIBYTE, 0);
  ret &= Agsi.DefineSFR("DMAH",     DMAH,     AGSIBYTE, 0);
  ret &= Agsi.DefineSFR("DMAP",     DMAP,     AGSIBYTE, 0);

  // It is only allowed to define bits which are bitaddressable.
  ret &= Agsi.DefineSFR("CS0",      ADCCON2,  AGSIBIT,  0);
  ret &= Agsi.DefineSFR("CS1",      ADCCON2,  AGSIBIT,  1);
  ret &= Agsi.DefineSFR("CS2",      ADCCON2,  AGSIBIT,  2);
  ret &= Agsi.DefineSFR("CS3",      ADCCON2,  AGSIBIT,  3);
  ret &= Agsi.DefineSFR("SCONV",    ADCCON2,  AGSIBIT,  4);
  ret &= Agsi.DefineSFR("CCNOV",    ADCCON2,  AGSIBIT,  5);
  ret &= Agsi.DefineSFR("DMA",      ADCCON2,  AGSIBIT,  6);
  ret &= Agsi.DefineSFR("ADCI",     ADCCON2,  AGSIBIT,  7);

  return(ret);
}


struct vtrlist VTREG[] = {           // VTR's definition: keep this table consistent with defines in SPeriDLL.h
  { "AIN0",   AGSIVTRFLOAT, 0x00000000, NULL },
  { "AIN1",   AGSIVTRFLOAT, 0x00000000, NULL },
  { "AIN2",   AGSIVTRFLOAT, 0x00000000, NULL },
  { "AIN3",   AGSIVTRFLOAT, 0x00000000, NULL },
  { "AIN4",   AGSIVTRFLOAT, 0x00000000, NULL },
  { "AIN5",   AGSIVTRFLOAT, 0x00000000, NULL },
  { "AIN6",   AGSIVTRFLOAT, 0x00000000, NULL },
  { "AIN7",   AGSIVTRFLOAT, 0x00000000, NULL },
  { "TEMP",   AGSIVTRFLOAT, 0x41C80000, NULL },  // 25 °C
  { "VREF",   AGSIVTRFLOAT, 0x40200000, NULL },  // 2.5 V
  { "XTAL",   AGSIVTRLONG,  0x00A8C000, NULL },  // 11.0592MHz oscillator frequency
  { "CONVST", AGSIVTRCHAR,  0x00000001, NULL },
};

DWORD DefineAllVTREG(void) {         // declare all virtual registers
  BOOL ret = TRUE;
  int i;

  for (i = 0; i < (sizeof(VTREG) / sizeof (VTREG[0])); i++) {
    VTREG[i].hVTR = Agsi.DefineVTR(VTREG[i].pName, VTREG[i].Type, VTREG[i].Value);
	if (!VTREG[i].hVTR) ret = FALSE;
  }

  return(ret);
}


AGSIINTERRUPT Interrupt = {     // Table of all Interrupts of this module
// vec    *mess           msfr mmask *mname  rsfr     rmask *rname  esfr  emask *ename  p0sfr p0mask *pname p1sfr p1mask pwl auto_reset
   0x33, "A/D Converter",  0,  0,     "",    ADCCON2, 0x80, "ADCI", IE,   0x40, "EADC", IP,   0x40,  "Pri", 0,    0x00,  7,  1   // ADCI
};


DWORD DefineAllInterrupts(void) {   // declare all interrupts

  if (!Agsi.DeclareInterrupt(&Interrupt)) return(FALSE);

  return(TRUE);
}


AGSITIMER Timer;

DWORD DefineAllWatches(void) {      // define all Watches
  BOOL ret = TRUE;

  ret &= Agsi.SetWatchOnSFR(ADCCON1, AdcConv, AGSIWRITE);
  ret &= Agsi.SetWatchOnSFR(ADCCON2, AdcConv, AGSIWRITE);
  ret &= Agsi.SetWatchOnSFR(ADCCON3, AdcConv, AGSIWRITE);
  ret &= Agsi.SetWatchOnVTR(CONVST,  AdcConv, AGSIWRITE);

  Timer = Agsi.CreateTimer(AdcCompleted);
  if (!Timer) return(FALSE);

  return(ret);
}


DWORD DefineAllMenuEntries(void) {  // declare all peripheral-menu entries and dialogs

  if (!Agsi.DefineMenuItem(&PeriMenu)) return(FALSE);

  return(TRUE);
}


DWORD ResetPeripheral(void) {       // reset all SFR's of this peripheral
  BOOL ret = TRUE;
  
  ret &= Agsi.WriteSFR(ADCCON1,  0x20, 0xFF);
  ret &= Agsi.WriteSFR(ADCCON2,  0x00, 0xFF);
  ret &= Agsi.WriteSFR(ADCCON3,  0x00, 0xFF);
  ret &= Agsi.WriteSFR(ADCDATAL, 0x00, 0xFF);
  ret &= Agsi.WriteSFR(ADCDATAH, 0x00, 0xFF);
  ret &= Agsi.WriteSFR(DMAL,     0x00, 0xFF);
  ret &= Agsi.WriteSFR(DMAH,     0x00, 0xFF);
  ret &= Agsi.WriteSFR(DMAP,     0x00, 0xFF);

  return(ret);
}


extern "C" DWORD AGSIEXPORT AgsiEntry (DWORD nCode, void *vp) {
  DWORD CpuType;

  switch (nCode) {
    case AGSI_CHECK:
      CpuType = *((DWORD *)vp);
      if (CpuType == 8051) return(1);  // This driver supports the 8051 family of microcontrollers 
      else                 return(0);  // Other microcontrollers are not supported by the driver
      break;

    case AGSI_INIT:                    // Declare all SFR's, VTREG's, Watches and Interrupts here
      AgsiConfig = *((AGSICONFIG *)vp);
//    AgsiConfig.m_hInstance;          // this pointer is used to get the function addresses of uVision
//    AgsiConfig.m_pszProjectPath;     // Path to application e.g. C:\KEIL\C51\EXAMPLES\HELLO
//    AgsiConfig.m_pszDevice;          // Simulated Device e.g. 52. This string is extracted out of the -p option.
//    AgsiConfig.m_pszConfiguration;   // complete dialog DLL options e.g. -p52 -dmydll ...
//    AgsiConfig.m_pszAppFile;         // name of loaded OMF file including path e.g. C:\KEIL\C51\EXAMPLES\HELLO\HELLO

      if (!GetFunctionPointers()) return(FALSE);   // get all function pointers for Agsi calls
      if (!DefineAllSFR()) return(FALSE);          // define all special function registers
      if (!DefineAllVTREG()) return(FALSE);        // define all virtual registers
      if (!DefineAllInterrupts()) return(FALSE);   // define all interrupts
	  if (!DefineAllWatches()) return(FALSE);      // define all watches
      if (!DefineAllMenuEntries()) return(FALSE);  // define all peripheral-menu entries and dialogs
      break;

    case AGSI_TERMINATE:               // Free all allocated memory, close all files ...
      break;

    case AGSI_RESET:                   // Reset all SFR's of this peripheral
      if (!ResetPeripheral()) return(FALSE);
	  break;

    case AGSI_PREPLL:                  // Recalculate all peripherals before clock prescaler/PLL is set to a new value
      break;

    case AGSI_POSTPLL:                 // Recalculate all peripherals after clock prescaler/PLL is set to a new value
      break;
  }
  return(TRUE);       // return OK
}


// --------------------------------------------------------------------------------------------------
// Simulation of A/D Converter for ADuC812

static WORD newadcval  = 0;
static WORD adcdisable = 0;

static DWORD  adccon1,  adccon2,  adccon3,  adcdatal,  adcdatah,  dmal,  dmah,  dmap;   // Current values
static DWORD  adccon1p, adccon2p, adccon3p, adcdatalp, adcdatahp, dmalp, dmahp, dmapp;  // Previous values


void AdcConv(void) {
  union  fv   ain, temp, vref;
  static BYTE convstp = 1;
  DWORD  convst;
  DWORD  tdiff;
  BYTE   ChanID;
  DWORD  dmaaddress;

  if (adcdisable) return;
  adcdisable = 1;     	

  Agsi.ReadSFR(ADCCON1, &adccon1,  &adccon1p,  0xFF);
  Agsi.ReadSFR(ADCCON2, &adccon2,  &adccon2p,  0xFF);
  Agsi.ReadSFR(ADCCON3, &adccon3,  &adccon3p,  0xFF);
  Agsi.ReadSFR(DMAL,    &dmal,     &dmalp,     0xFF);
  Agsi.ReadSFR(DMAH,    &dmah,     &dmahp,     0xFF);
  Agsi.ReadSFR(DMAP,    &dmap,     &dmapp,     0xFF);
  Agsi.ReadVTR(CONVST,  &convst);

  if ((adccon3 ^ adccon3p) & 0x80) { // bit BUSY (in ADCCON3) is Read-Only
    adccon3 = adccon3p;
    initflag = 2;
  }

  if (adccon1 & 0xC0) {  // ADC enabled
	if ((adccon3 & 0x80) == 0) {  // no conversion in progress
	  if ((adccon2 & 0x30) ||                                                       // SCONV or CCONV
		  ((adccon1 & 0x01) && ((convstp & 0x01) == 0) && ((convst & 0x01) == 1)))  // CONVST pin: 0 -> 1
	  {  // starting new conversion
		adccon3 |= 0x80;                 // set BUSY
        if (adccon2 & 0x40) {            // DMA Mode
		  dmaaddress = dmal + (dmah << 8) + (dmap << 16);
		  Agsi.ReadMemory(dmaaddress, 1, &ChanID);
		  adccon2 = (adccon2 & 0xF0) | (ChanID >> 4);
		}
        if ((adccon2 & 0x0F) == 0x08) {  // Temperature sensor
          Agsi.ReadVTR(TEMP, &temp.DW);
          ain.f = 0.600f - (0.003f * (temp.f - 25.0f));
		}
		else {
		  if ((adccon2 & 0x0F) < 0x08) { // Analog inputs
            Agsi.ReadVTR(VTREG[adccon2 & 0x07].hVTR, &ain.DW);
		  } else ain.f = 0.0;
		}
        Agsi.ReadVTR(VREF, &vref.DW);
        newadcval = 0xFFFF;
        if (ain.f > vref.f) newadcval = 0x0FFF;
		if (ain.f < 0.0f)   newadcval = 0;
		if (newadcval == 0xFFFF) {
		  newadcval = (WORD)(ain.f * 4096.0f / vref.f + 0.5f);
		  if (newadcval > 0x0FFF) newadcval = 0x0FFF;
		}
		tdiff = ((17 + ((adccon1 & 0x0C) >> 2)) << ((adccon1 & 0x30) >> 4)) / 12;
		Agsi.SetTimer(Timer, tdiff);
	  }
	}
	else { // conversion in progress
	  if ((adccon2p & 0x20) && ((adccon2 & 0x20) == 0)) {  // end Continous Mode
	    adccon3 &= ~0x80;                  // clear BUSY
		Agsi.SetTimer(Timer, 0xFFFFFFFF);  // no time watch
	  }
	}
  }
  else {
    adccon3 &= ~0x80;                      // clear BUSY
	Agsi.SetTimer(Timer, 0xFFFFFFFF);      // no time watch
  }

  convstp = (BYTE)convst;

  Agsi.WriteSFR(ADCCON2, adccon2, 0x0F);
  Agsi.WriteSFR(ADCCON3, adccon3, 0x80);

  adcdisable = 0;
}

void AdcCompleted(void) {
  DWORD  dmaaddress;
  BYTE   dmadata[2];
  BYTE   ChanID;

  if (adcdisable) return;
  adcdisable = 1;     	

  Agsi.ReadSFR(ADCCON2, &adccon2, &adccon2p, 0xFF);
  Agsi.ReadSFR(ADCCON3, &adccon3, &adccon3p, 0x80);
  Agsi.ReadSFR(DMAL,    &dmal,    &dmalp,    0xFF);
  Agsi.ReadSFR(DMAH,    &dmah,    &dmahp,    0xFF);
  Agsi.ReadSFR(DMAP,    &dmap,    &dmapp,    0xFF);

  adcdatal = newadcval & 0xFF;
  adcdatah = (newadcval >> 8) | ((adccon2 & 0x0F) << 4);

  if (adccon2 & 0x40) {                   // DMA Mode
	dmaaddress = dmal + (dmah << 8) + (dmap << 16);
	dmadata[0] = (BYTE)(((adccon2 & 0x0F) << 4) | adcdatah);
	dmadata[1] = (BYTE)adcdatal;
	Agsi.WriteMemory(dmaaddress, 2, dmadata);
    dmaaddress += 2;
	dmal = (BYTE)(dmaaddress & 0xFF);
	dmah = (BYTE)((dmaaddress >> 8) & 0xFF);
	dmap = (BYTE)((dmaaddress >> 16) & 0xFF);
	Agsi.ReadMemory(dmaaddress, 1, &ChanID);
	if ((ChanID & 0xF0) == 0xF0) {        // DMA Stop
	  adccon2 = (adccon2 & 0xAF) | 0x80;  // clear DMA, SCON and set ADCI
	}
	else {
	  adccon2 = adccon2 & 0xEF;           // clear SCON
	}
    adccon3 = 0;                          // clear BUSY
  }
  else {
    adccon2 = (adccon2 & 0xEF) | 0x80;    // clear SCON and set ADCI
	adccon3 = 0;                          // clear BUSY
  }

  Agsi.WriteSFR(ADCCON2,  adccon2,  0xFF);
  Agsi.WriteSFR(ADCCON3,  adccon3,  0x80);
  Agsi.WriteSFR(ADCDATAL, adcdatal, 0xFF);
  Agsi.WriteSFR(ADCDATAH, adcdatah, 0xFF);
  Agsi.WriteSFR(DMAL,     dmal,     0xFF);
  Agsi.WriteSFR(DMAH,     dmah,     0xFF);
  Agsi.WriteSFR(DMAP,     dmap,     0xFF);

  Agsi.SetTimer(Timer, 0xFFFFFFFF);       // delete time watch

  adcdisable = 0;
  if (adccon2 & 0x20) AdcConv();          // continous conversion
}


/////////////////////////////////////////////////////////////////////////////
// CSPeriDLLApp construction

CSPeriDLLApp::CSPeriDLLApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSPeriDLLApp object

CSPeriDLLApp theApp;
