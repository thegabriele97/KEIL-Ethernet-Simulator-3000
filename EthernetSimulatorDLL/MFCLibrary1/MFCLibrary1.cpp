// MFCLibrary1.cpp : Defines the initialization routines for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "MFCLibrary1.h"
#include "AGSI.h"
#include "EthernetHub.h"
#include "EthernetClient.h"
#include "EthernetRouter.h"
#include <memory>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(CMFCLibrary1App, CWinApp)
END_MESSAGE_MAP()

AGSICONFIG  AgsiConfig;
AGSIFUNCS   Agsi;
EthernetHub Hub("AwesomeEthernetHub");
EthernetClient* Client;
EthernetRouter* Router;

static BOOL ISPOST_INIT = TRUE;
static AGSIADDR CurrentTxDescriptor;
static AGSIADDR CurrentRxDescriptor;


// AGSI function implementation
extern "C" DWORD AGSIEXPORT AgsiEntry(DWORD nCode, void* vp) {

	switch ((AGSIFUNC)nCode) {

	case AGSI_CHECK: {
		auto cpuType = *((DWORD*)vp);
	}
	break;

	case AGSI_INIT: {
        return InitAction((AGSICONFIG*)vp);
	}
	break;

    case AGSI_TERMINATE: {
        delete Client;
        delete Router;
    }
    break;

    default: {
        if (ISPOST_INIT) {
            ISPOST_INIT = FALSE;
            PostInitAction();
        }
    }
    break;

	}

	return TRUE;
}

BOOL InitAction(AGSICONFIG *cfg) {
    BOOL retVal;
    
    AgsiConfig = *cfg;
    if (!(retVal = GetFunctionPointers())) {
        return FALSE;
    }

    //Agsi.SetWatchOnMemory(LPC_EMAC_MAC1, LPC_EMAC_PowerDown, OnEMACMemoryAccessHandler, AGSIREADWRITE);
    retVal &= Agsi.SetWatchOnMemory(LPC_EMAC_TxDescriptor, LPC_EMAC_TxDescriptor, OnEMACTxDescriptorWRAccessHandler, AGSIWRITE);
    retVal &= Agsi.SetWatchOnMemory(LPC_EMAC_RxDescriptor, LPC_EMAC_RxDescriptor, OnEMACRxDescriptorWRAccessHandler, AGSIWRITE);
    retVal &= Agsi.SetWatchOnMemory(LPC_EMAC_TxProduceIndex, LPC_EMAC_TxProduceIndex, OnEMACTxProduceIndexChangeHandler, AGSIWRITE);

    if (!Hub.CreationSucceded()) {
        retVal &= ~Hub.FailedBcsAlreadyExists();
        if (!retVal) return FALSE;
    } else if (Hub.CreationSucceded()) {
        Hub.StartHandling();
    }

    // This clients and other ones can connect now..
    // Do it here for this session!
    Client = new EthernetClient(Hub);
    if (Client->FailedBcsNotValidHubPipe()) return FALSE;

    retVal &= Client->ConnectToHub();
    if (retVal) Client->StartHandling(OnNewFrameAvailableCallback);

    Router = new EthernetRouter(Hub, new BYTE[6]{ 0x1f, 0x3f, 0x6f, 0xaf, 0x4f, 0x5f }, new BYTE[]{ 192, 168, 2, 254 });
    Router->StartRouterHandling();

    return retVal;
}

void PostInitAction() {
    
}

DECLARE_ACCESSMEM_HANDLER(OnEMACTxDescriptorWRAccessHandler) {
    Agsi.ReadMemory(LPC_EMAC_TxDescriptor, sizeof CurrentTxDescriptor, (BYTE *)&CurrentTxDescriptor);
}

DECLARE_ACCESSMEM_HANDLER(OnEMACRxDescriptorWRAccessHandler) {
    Agsi.ReadMemory(LPC_EMAC_RxDescriptor, sizeof CurrentRxDescriptor, (BYTE*)&CurrentRxDescriptor);
}

DECLARE_ACCESSMEM_HANDLER(OnEMACTxProduceIndexChangeHandler) {
    static DWORD idx, consumeIdx;
    AGSIADDR tptr_ptr, tptr;
    AGSIADDR tx_ctrl_ptr, tx_ctrl;
    WORD frameSize;
    std::unique_ptr<BYTE> txBuffer;

    Agsi.ReadMemory(LPC_EMAC_TxProduceIndex, sizeof idx, (BYTE*)&idx);
    Agsi.ReadMemory(LPC_EMAC_TxConsumeIndex, sizeof consumeIdx, (BYTE*)&consumeIdx);

    if (idx == consumeIdx) return;

    // Intercepted a new ethernet frame
    // It's already complete, so we can read it!
    // (The ProduceIndex changes only when the frame is ready to be sent)

    // The actual idx is 1 step ahead the used idx
    // so we need to go back by 1 step uing consumeIdx
    idx ^= consumeIdx;
    consumeIdx ^= idx;
    idx ^= consumeIdx;

    // We're ready. Finally, Reading all things....
    tptr_ptr = CurrentTxDescriptor + (8 * idx);
    Agsi.ReadMemory(tptr_ptr, sizeof tptr, (BYTE*)&tptr);

    tx_ctrl_ptr = (CurrentTxDescriptor + 4) + (8 * idx);
    Agsi.ReadMemory(tx_ctrl_ptr, sizeof tx_ctrl, (BYTE*)&tx_ctrl);

    frameSize = tx_ctrl & 0x000007FF;
    txBuffer = std::unique_ptr<BYTE>(new BYTE[frameSize]);
    Agsi.ReadMemory(tptr, frameSize, txBuffer.get());

    Client->SendData(txBuffer.get(), frameSize);

    // Working done. Committing the result by
    // telling the device we sent it
    Agsi.WriteMemory(LPC_EMAC_TxConsumeIndex, sizeof consumeIdx, (BYTE*)&consumeIdx);

    return;
}

void OnNewFrameAvailableCallback(BYTE* frame, DWORD frameSize) {
    DWORD produceIdx, nRxFrag;
    AGSIADDR rptr_ptr, stat_ptr, rptr;

    // We need to know NOT the actual Consume Index,
    // but the Produce Index!! That if there are no pending data in the 
    // receiver buffer, it will be equal to the consume index, else
    // it will be n-steps ahead. This is done in order to not 
    // overwrite possible data while they are processed
    // maybe because the receiver code is under debugging. 
    // This is why the rx buffer is made of 4 fragments (or at least more then the tx buffer),
    // because the receiver maybe is slow to read them
    Agsi.ReadMemory(LPC_EMAC_RxProduceIndex, sizeof produceIdx, (BYTE*)&produceIdx);

    rptr_ptr = CurrentRxDescriptor + 8 * produceIdx;
    stat_ptr = (CurrentRxDescriptor + 4 * 8) + 8 * produceIdx;

    // Obtaining buffer pointer
    Agsi.ReadMemory(rptr_ptr, sizeof rptr, (BYTE*)&rptr);

    // Writing in memory the size and data
    // The size is +3: the EMAC driver will read it and will remove
    // -3 from this value
    frameSize = (frameSize + 3) & 0x000007FF;
    Agsi.WriteMemory(stat_ptr, sizeof frameSize, (BYTE*)&frameSize);
    Agsi.WriteMemory(rptr, frameSize, frame); // Writing in current Rx Buffer

    // The current produce Index is incremented it by 1,
    // and wrapped to the number of maximum fragments available in the receiver's EMAC peripheral
    Agsi.ReadMemory(LPC_EMAC_RxDescriptorNumber, sizeof nRxFrag, (BYTE*)&nRxFrag);
    
    // Committing the result: the receiver will notice it
    // and will use its consume idx as pointer that is equal to our ex produce idx used in a few lines above
    // (at the beginning of this function)
    if (++produceIdx > nRxFrag) produceIdx = 0;
    Agsi.WriteMemory(LPC_EMAC_RxProduceIndex, sizeof produceIdx, (BYTE*)&produceIdx);

    return;
}

static void OnEMACMemoryAccessHandler(AGSIADDR addr, int reason) {
    BYTE buff[10];

    //Agsi.ReadMemory(0x50000000, 1, buff);

    *((DWORD*)buff) = 0xaa;
    //Agsi.WriteMemory(0x50000000, 1, buff);
    //Agsi.ReadMemory(0x50000000, 1, buff + sizeof(DWORD)); 
}

DWORD GetFunctionPointers(void) {           // get all function pointers

    Agsi.DefineSFR = (tAgsiDefineSFR)GetProcAddress(AgsiConfig.m_hInstance, "AgsiDefineSFR");
    Agsi.DefineVTR = (tAgsiDefineVTR)GetProcAddress(AgsiConfig.m_hInstance, "AgsiDefineVTR");
    Agsi.DeclareInterrupt = (tAgsiDeclareInterrupt)GetProcAddress(AgsiConfig.m_hInstance, "AgsiDeclareInterrupt");
    Agsi.SetWatchOnSFR = (tAgsiSetWatchOnSFR)GetProcAddress(AgsiConfig.m_hInstance, "AgsiSetWatchOnSFR");
    Agsi.SetWatchOnVTR = (tAgsiSetWatchOnVTR)GetProcAddress(AgsiConfig.m_hInstance, "AgsiSetWatchOnVTR");
    Agsi.SetWatchOnMemory = (tAgsiSetWatchOnMemory)GetProcAddress(AgsiConfig.m_hInstance, "AgsiSetWatchOnMemory");
    Agsi.CreateTimer = (tAgsiCreateTimer)GetProcAddress(AgsiConfig.m_hInstance, "AgsiCreateTimer");
    Agsi.DefineMenuItem = (tAgsiDefineMenuItem)GetProcAddress(AgsiConfig.m_hInstance, "AgsiDefineMenuItem");

    Agsi.WriteSFR = (tAgsiWriteSFR)GetProcAddress(AgsiConfig.m_hInstance, "AgsiWriteSFR");
    Agsi.ReadSFR = (tAgsiReadSFR)GetProcAddress(AgsiConfig.m_hInstance, "AgsiReadSFR");
    Agsi.WriteVTR = (tAgsiWriteVTR)GetProcAddress(AgsiConfig.m_hInstance, "AgsiWriteVTR");
    Agsi.ReadVTR = (tAgsiReadVTR)GetProcAddress(AgsiConfig.m_hInstance, "AgsiReadVTR");
    Agsi.SetSFRReadValue = (tAgsiSetSFRReadValue)GetProcAddress(AgsiConfig.m_hInstance, "AgsiSetSFRReadValue");
    Agsi.ReadMemory = (tAgsiReadMemory)GetProcAddress(AgsiConfig.m_hInstance, "AgsiReadMemory");
    Agsi.WriteMemory = (tAgsiWriteMemory)GetProcAddress(AgsiConfig.m_hInstance, "AgsiWriteMemory");
    Agsi.GetLastMemoryAddress = (tAgsiGetLastMemoryAddress)GetProcAddress(AgsiConfig.m_hInstance, "AgsiGetLastMemoryAddress");
    Agsi.IsSimulatorAccess = (tAgsiIsSimulatorAccess)GetProcAddress(AgsiConfig.m_hInstance, "AgsiIsSimulatorAccess");
    Agsi.SetTimer = (tAgsiSetTimer)GetProcAddress(AgsiConfig.m_hInstance, "AgsiSetTimer");
    Agsi.GetStates = (tAgsiGetStates)GetProcAddress(AgsiConfig.m_hInstance, "AgsiGetStates");
    Agsi.GetProgramCounter = (tAgsiGetProgramCounter)GetProcAddress(AgsiConfig.m_hInstance, "AgsiGetProgramCounter");
    Agsi.IsInInterrupt = (tAgsiIsInInterrupt)GetProcAddress(AgsiConfig.m_hInstance, "AgsiIsInInterrupt");
    Agsi.IsSleeping = (tAgsiIsSleeping)GetProcAddress(AgsiConfig.m_hInstance, "AgsiIsSleeping");
    Agsi.StopSimulator = (tAgsiStopSimulator)GetProcAddress(AgsiConfig.m_hInstance, "AgsiStopSimulator");
    Agsi.TriggerReset = (tAgsiTriggerReset)GetProcAddress(AgsiConfig.m_hInstance, "AgsiTriggerReset");
    Agsi.UpdateWindows = (tAgsiUpdateWindows)GetProcAddress(AgsiConfig.m_hInstance, "AgsiUpdateWindows");
    Agsi.HandleFocus = (tAgsiHandleFocus)GetProcAddress(AgsiConfig.m_hInstance, "AgsiHandleFocus");
    Agsi.GetExternalClockRate = (tAgsiGetExternalClockRate)GetProcAddress(AgsiConfig.m_hInstance, "AgsiGetExternalClockRate");
    Agsi.GetInternalClockRate = (tAgsiGetInternalClockRate)GetProcAddress(AgsiConfig.m_hInstance, "AgsiGetInternalClockRate");
    Agsi.GetClockFactor = (tAgsiGetClockFactor)GetProcAddress(AgsiConfig.m_hInstance, "AgsiGetClockFactor");
    Agsi.Message = (tAgsiMessage)GetProcAddress(AgsiConfig.m_hInstance, "AgsiMessage");
    Agsi.GetTargetKey = (tAgsiGetTargetKey)GetProcAddress(AgsiConfig.m_hInstance, "AgsiGetTargetKey");
    Agsi.SetTargetKey = (tAgsiSetTargetKey)GetProcAddress(AgsiConfig.m_hInstance, "AgsiSetTargetKey");
    Agsi.GetSymbolByName = (tAgsiGetSymbolByName)GetProcAddress(AgsiConfig.m_hInstance, "AgsiGetSymbolByName");
    Agsi.GetSymbolByValue = (tAgsiGetSymbolByValue)GetProcAddress(AgsiConfig.m_hInstance, "AgsiGetSymbolByValue");

    return !((Agsi.DefineSFR == NULL) ||
        (Agsi.DefineVTR == NULL) ||
        (Agsi.DeclareInterrupt == NULL) ||
        (Agsi.SetWatchOnSFR == NULL) ||
        (Agsi.SetWatchOnVTR == NULL) ||
        (Agsi.SetWatchOnMemory == NULL) ||
        (Agsi.CreateTimer == NULL) ||
        (Agsi.DefineMenuItem == NULL) ||

        (Agsi.WriteSFR == NULL) ||
        (Agsi.ReadSFR == NULL) ||
        (Agsi.WriteVTR == NULL) ||
        (Agsi.ReadVTR == NULL) ||
        (Agsi.SetSFRReadValue == NULL) ||
        (Agsi.ReadMemory == NULL) ||
        (Agsi.WriteMemory == NULL) ||
        (Agsi.GetLastMemoryAddress == NULL) ||
        (Agsi.IsSimulatorAccess == NULL) ||
        (Agsi.SetTimer == NULL) ||
        (Agsi.GetStates == NULL) ||
        (Agsi.GetProgramCounter == NULL) ||
        (Agsi.IsInInterrupt == NULL) ||
        (Agsi.IsSleeping == NULL) ||
        (Agsi.StopSimulator == NULL) ||
        (Agsi.TriggerReset == NULL) ||
        (Agsi.UpdateWindows == NULL) ||
        (Agsi.HandleFocus == NULL) ||
        (Agsi.GetExternalClockRate == NULL) ||
        (Agsi.GetInternalClockRate == NULL) ||
        (Agsi.GetClockFactor == NULL) ||
        (Agsi.Message == NULL) ||
        (Agsi.GetTargetKey == NULL) ||
        (Agsi.SetTargetKey == NULL) ||
        (Agsi.GetSymbolByName == NULL) ||
        (Agsi.GetSymbolByValue == NULL));
}

// CMFCLibrary1App construction
CMFCLibrary1App::CMFCLibrary1App() {
	
}

// The one and only CMFCLibrary1App object
CMFCLibrary1App theApp;


// CMFCLibrary1App initialization
BOOL CMFCLibrary1App::InitInstance() {
	CWinApp::InitInstance();

	return TRUE;
}
