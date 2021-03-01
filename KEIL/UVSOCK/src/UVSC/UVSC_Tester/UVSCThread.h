/******************************************************************************/
/* UVSC Thread  -  UVSC Tester DLL interface thread                           */
/******************************************************************************/
/* Name:    UVSC Thread.h                                                   */
/* Purpose: Thread that calls UVSC (separate from UI as calls are blocking    */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2004-2016 KEIL - An ARM Company. All rights reserved.        */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#ifndef _UVSCThread_H_
#define _UVSCThread_H_

#include "UVSC_C.h"
#include "mainDlg.h"
#include "ThreadQueue.h"

#define THREADQ_MAX_DATA_SIZE   32768

typedef enum _tag_ControlMsgCmd {
  CONTROLMSG_EXIT = 0,
  CONTROLMSG_VERSION,
  CONTROLMSG_CONNECT,
  CONTROLMSG_DISCONNECT,
  CONTROLMSG_CMDOUTPUT,
  CONTROLMSG_BUILDOUTPUT,
  CONTROLMSG_LOGCONTROL,
  CONTROLMSG_LOGDISABLE,
  CONTROLMSG_LOGENABLE,
  CONTROLMSG_GETLASTERROR
} ControlMsgCmd;

typedef struct _tag_ThreadControlConnectData {
  int port;  
  UVSC_RUNMODE uvRunmode;
  char uvCmd[1024];
} ThreadControlConnectData;

typedef struct _tag_ThreadControlDisconnectData {
  bool terminate;
} ThreadControlDisconnectData;

typedef struct _tag_ThreadControlLogControlData {
  bool enableRaw;
  bool enableTrace;
  bool disableLogging;
} ThreadControlLogControlData;

typedef struct _tag_ThreadControlMsg {
  ControlMsgCmd cmd;
  union {
    ThreadControlConnectData connectData;
    ThreadControlDisconnectData disconnectData;
    ThreadControlLogControlData logControlData;
  };
} ThreadControlMsg;

typedef struct _tag_ThreadMsg {
  UV_OPERATION cmd;
  int connHandle;
  int dataLen;
  union {    
    ThreadControlMsg ctrlMsg;
    PRJDATA prjData;
    SSTR sstrData;
    TRNOPT trnOptData;
    iSTKENUM iStkEnumData;
    VSET vsetData;
    ENUMTPM tpmData;
    iVTRENUM iVTREnumData;
    AMEM amemData;
    ADRMTFL adrmtflData;
    iSHOWSYNC iShowSyncData;
    UVSC_PSTAMP powerScaleData;
    BKCHG bkchgData;
    BKPARM bkparmData;
    DBGTGTOPT dbgtgtopt; 
    EXECCMD execCmdData;
    iPATHREQ iPathReq;
    ITMOUT itmIo;
    // ...
    char rawData[THREADQ_MAX_DATA_SIZE];
  };
} ThreadMsg; 


void UVSCThread_Init (CUVSCTesterDlg *pDialog);
void UVSCThread_UnInit(void);
bool UVSCThread_Post(ThreadMsg *pMsg, DWORD timeout);

#endif /* _UVSCThread_H_ */


