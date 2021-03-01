/******************************************************************************/
/* UVSC Thread  -  UVSC Tester DLL interface thread                           */
/******************************************************************************/
/* Name:    UVSC Thread.cpp                                                   */
/* Purpose: Thread that calls UVSC (separate from UI as calls are blocking    */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2004-2016 KEIL - An ARM Company. All rights reserved.        */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include "stdafx.h"
#include "UVSCThread.h"
#include "UVSOCK_Log.h"

#include <process.h>


// Macros
#define MIN_AUTO_PORT_NUMBER  5101
#define MAX_AUTO_PORT_NUMBER  5110


// Type definitions
typedef struct _tag_UVSCThreadParams_t {
  CThreadMsgQueue *pThreadQ;
  CDialog *pDialog;
} UVSCThreadParams_t;


// Internal variables
static int initialised = 0;
static HANDLE threadHandle;
static CThreadMsgQueue *pThreadQ;
static UVSCThreadParams_t threadParams;
static bool bDisableLogging;

// Function prototypes
static bool ProcessControl (ThreadMsg *msg);
static unsigned int __stdcall UVSCThread_thread(void * pParam);
static void uvsc_log_callback(const char* msg, int msgLen);
static void uvsc_callback(void* cb_custom, UVSC_CB_TYPE type, UVSC_CB_DATA *data);
static void DoGetLastError (int iConnHandle);

static void DialogLogMsg (OUT_TYPE type, char *fmt, ...);
static void DialogRTAMsg (OUT_TYPE type, char *fmt, ...);
static void DialogReadWrite (void);
static void DialogConnected (int handle, int port);
static void DialogDisconnected (int handle);
static void DialogConnectFailed (void);
static void DialogCmdOutput (const char *output, int outputLen);
static void DialogBuildOutput (const char *output, int outputLen);
static void DialogCalcExpr (TVAL *pTval);
static void DialogEnumBP (BKRSP *pBkptRsp, int *pBkptIndexes, int bkptCount);
static void DialogBuildLine (const char *line, int lineLen);
static void DialogCmdLine (const char *line, int lineLen);
static void DialogPBarInit (const char *initStr, int initStrLen);
static void DialogPBarSetText (const char *textStr, int textStrLen);
static void DialogPBarSetPos (const char *posStr, int posStrLen);
static void DialogPBarStop (const char *stopStr, int stopStrLen);
static void DialogOpFailed (void);


// Create the UVSC thread task and queue
void UVSCThread_Init (CUVSCTesterDlg *pDialog) {
  if (!initialised) {
    pThreadQ = new CThreadMsgQueue(sizeof(ThreadMsg), 10);
    threadParams.pThreadQ = pThreadQ;
    threadParams.pDialog = pDialog;
    bDisableLogging = false;

    threadHandle = (HANDLE)_beginthreadex(NULL, 0, UVSCThread_thread, (void *)&threadParams, 0, NULL);
    initialised = 1;
  }
}

void UVSCThread_UnInit(void) {
  ThreadMsg msg;
  DWORD exitCode;

  if (initialised) {
    // Send thread exit message...
    msg.cmd = UV_NULL_CMD;
    msg.ctrlMsg.cmd = CONTROLMSG_EXIT;
    UVSCThread_Post(&msg, INFINITE);

    // Wait for thread to exit
    if (threadHandle) {
      while (1) {
        if (!GetExitCodeThread(threadHandle, &exitCode)) break;
        if (exitCode == STILL_ACTIVE) {
          /* Need a DoEvents here as this is called from the UI thread, and could
             otherwise deadlock with the UVSCThread sending a message to the UI, 
             and the UI waiting here for the thread to exit! */
          //Sleep (50);
          DoEvents();
        }
        else {
          break;
        }
      }
      CloseHandle(threadHandle);
    }

    if (pThreadQ) delete pThreadQ;      
    initialised = 0;
  }
}

bool UVSCThread_Post(ThreadMsg *pMsg, DWORD timeout) {
  bool retVal = false;

  if (initialised) {
    // Send thread message...
    if (pThreadQ) {
      if (pThreadQ->Post((char *)pMsg, timeout) == CThreadMsgQueue::QOK) {
        retVal = true;
      }
    }
  }

  return (retVal);
}




static bool ProcessControl (ThreadMsg *msg) {
  bool threadExit = false;
  UVSC_STATUS stat;
  int UVSCConnHandle, UVSCConnPort;
  UINT uiTemp, uiTemp2;
  int iTemp;
  char *pStr;

  switch (msg->ctrlMsg.cmd) {
    case CONTROLMSG_EXIT:
      threadExit = true;
      break;

    case CONTROLMSG_VERSION:
      // Get local version info
      UVSC_Version(&uiTemp, &uiTemp2);
      DialogLogMsg(OUT_TYPE_CMD, "UVSC_Version() = (UVSC: V%d.%02d, UVSOCK: V%d.%02d)\n",
                   uiTemp / 100, uiTemp % 100,
                   uiTemp2 / 100, uiTemp2 % 100);
      break;

    case CONTROLMSG_CONNECT:
      UVSCConnPort = msg->ctrlMsg.connectData.port;
      stat = UVSC_OpenConnection(NULL, 
                                 &UVSCConnHandle, 
                                 &UVSCConnPort,
                                 msg->ctrlMsg.connectData.uvCmd,
                                 msg->ctrlMsg.connectData.uvRunmode,                                                
                                 uvsc_callback, 
                                 NULL, 
                                 NULL,
                                 false, 
                                 uvsc_log_callback);
      if (stat == UVSC_STATUS_SUCCESS) {
        // Send a message to the dialog to indicate we are connected
        DialogConnected(UVSCConnHandle, UVSCConnPort);
      }
      else {
        DialogConnectFailed();
      }
      break;

    case CONTROLMSG_DISCONNECT:
      if (UVSC_CloseConnection(msg->connHandle, 
                               msg->ctrlMsg.disconnectData.terminate) 
                               == UVSC_STATUS_SUCCESS) {
        DialogLogMsg(OUT_TYPE_GEN, "Disconnected from µVision (Handle: %d)\n", msg->connHandle);
      }
      
      break;

    case CONTROLMSG_CMDOUTPUT:
      stat = UVSC_GetCmdOutputSize(msg->connHandle, &iTemp);
      if (stat == UVSC_STATUS_SUCCESS) {
        pStr = (char *)malloc(iTemp);
        if (pStr) {
          stat = UVSC_GetCmdOutput(msg->connHandle, pStr, iTemp);
          if (stat == UVSC_STATUS_SUCCESS) {
            DialogCmdOutput(pStr, iTemp);
          }
          free (pStr);
        }
      }
      break;

    case CONTROLMSG_BUILDOUTPUT:
      stat = UVSC_GetBuildOutputSize(msg->connHandle, &iTemp);
      if (stat == UVSC_STATUS_SUCCESS) {
        pStr = (char *)malloc(iTemp);
        if (pStr) {
          stat = UVSC_GetBuildOutput(msg->connHandle, pStr, iTemp);
          if (stat == UVSC_STATUS_SUCCESS) {
            DialogBuildOutput(pStr, iTemp);
          }
          free (pStr);
        }
      }
      break;

    case CONTROLMSG_LOGCONTROL:
      UVSC_LogControl (msg->connHandle, 
                       msg->ctrlMsg.logControlData.enableRaw, 
                       msg->ctrlMsg.logControlData.enableTrace);
      bDisableLogging = msg->ctrlMsg.logControlData.disableLogging;
      break;

    case CONTROLMSG_LOGENABLE:
      bDisableLogging = false;
      break;

    case CONTROLMSG_LOGDISABLE:
      bDisableLogging = true;
      break;

     case CONTROLMSG_GETLASTERROR:
       DoGetLastError(msg->connHandle);
      break;
      
    default:
      ; // Ignore unknown message
  }

  return (threadExit);
}


// Some variables for holding throw-away return values
#define SSTR_COUNT                  512
static SSTR sstr[SSTR_COUNT];
static int sstrIndexes[SSTR_COUNT]; 
static int sstrCount = SSTR_COUNT;

#define STACKENUM_COUNT             512
static STACKENUM StackEnum[STACKENUM_COUNT]; 
static int StackEnumCount = STACKENUM_COUNT;

#define AVTR_COUNT                 512
static AVTR avtr[AVTR_COUNT];
static int vtrIndexes[AVTR_COUNT]; 
static int vtrCount = AVTR_COUNT;

#define AFLMAP_LEN                  2048
static char aflmap[AFLMAP_LEN];
static AFLMAP *pAflMap = (AFLMAP *)aflmap; 
static int aflMapLen = AFLMAP_LEN;

#define BKPTRSP_COUNT               128
static BKRSP bkptRsp[BKPTRSP_COUNT]; 
static int bkptRspIndexes[BKPTRSP_COUNT]; 
static int bBkptRspCount = BKPTRSP_COUNT;

static UVLICINFO licinfo;

static unsigned int __stdcall UVSCThread_thread(void * pParam)  {
  UVSCThreadParams_t *pParams = (UVSCThreadParams_t *)pParam; 
  CUVSCTesterDlg *pDialog = (CUVSCTesterDlg *)pParams->pDialog;
  CThreadMsgQueue *pThreadQ = pParams->pThreadQ;
  ThreadMsg msg;
  UINT uiTemp, uiTemp2;
  int iTemp;
  UVSC_STATUS stat;
  
  if (!(pDialog) || !(pThreadQ)) {
    return (-1);
  }

  UVSC_Init(MIN_AUTO_PORT_NUMBER, MAX_AUTO_PORT_NUMBER);

  bool exitThread = false;

  while (!exitThread) {
    if (pThreadQ->Get((char *)&msg, 500) != CThreadMsgQueue::QTIMEOUT) {
      stat = UVSC_STATUS_SUCCESS;
      switch (msg.cmd) {

        case UV_NULL_CMD:       // NULL Command means control message!
          exitThread = ProcessControl (&msg);
          break;

        case UV_GEN_GET_VERSION:             
          // Get remote UVSOCK version
          stat = UVSC_GEN_UVSOCK_VERSION(msg.connHandle, &uiTemp, &uiTemp2);
          if (stat == UVSC_STATUS_SUCCESS) {
            DialogLogMsg(OUT_TYPE_CMD, "UVSC_GEN_UVSOCK_VERSION (UVSOCK: V%d.%02d)\n",uiTemp, uiTemp2);
          }
          break;

        case UV_GEN_UI_UNLOCK:
          // Not Used
          break;

        case UV_GEN_UI_LOCK:                    
          // Not Used
          break;

        case UV_GEN_HIDE:         
          stat = UVSC_GEN_HIDE(msg.connHandle);
          break;

        case UV_GEN_SHOW:    
          stat = UVSC_GEN_SHOW(msg.connHandle);
          break;

        case UV_GEN_RESTORE:                   
          stat = UVSC_GEN_RESTORE(msg.connHandle);
          break;

        case UV_GEN_MINIMIZE:                  
          stat = UVSC_GEN_MINIMIZE(msg.connHandle);
          break;

        case UV_GEN_MAXIMIZE:                  
          stat = UVSC_GEN_MAXIMIZE(msg.connHandle);
          break;

        case UV_GEN_EXIT:   
          stat = UVSC_CloseConnection(msg.connHandle, true);
          break;

        case UV_GEN_GET_EXTVERSION:  
          // Get remote version info (including extended)
          stat = UVSC_GEN_UVSOCK_VERSION(msg.connHandle, &uiTemp, &uiTemp2);
          if (stat == UVSC_STATUS_SUCCESS) {
            DialogLogMsg(OUT_TYPE_CMD, "UVSC_GEN_UVSOCK_VERSION (UVSOCK: V%d.%02d)\n",uiTemp, uiTemp2);
          }
          break;

        case UV_GEN_CHECK_LICENSE:
          stat = UVSC_GEN_CHECK_LICENSE(msg.connHandle, &licinfo);
          break;

        case UV_PRJ_LOAD: 
          stat = UVSC_PRJ_LOAD(msg.connHandle, &msg.prjData, msg.dataLen);
          break;

        case UV_PRJ_CLOSE:                      
          stat = UVSC_PRJ_CLOSE(msg.connHandle);
          break;

        case UV_PRJ_ADD_GROUP: 
          stat = UVSC_PRJ_ADD_GROUP(msg.connHandle, &msg.prjData, msg.dataLen);
          break;

        case UV_PRJ_DEL_GROUP:   
          stat = UVSC_PRJ_DEL_GROUP(msg.connHandle, &msg.prjData, msg.dataLen);
          break;

        case UV_PRJ_SET_TARGET: 
          stat = UVSC_PRJ_SET_TARGET(msg.connHandle, &msg.prjData, msg.dataLen);
          break;

        case UV_PRJ_SET_OUTPUTNAME: 
          stat = UVSC_PRJ_SET_OUTPUTNAME(msg.connHandle, &msg.prjData, msg.dataLen);
          break;

        case UV_PRJ_ADD_FILE:                 
          stat = UVSC_PRJ_ADD_FILE(msg.connHandle, &msg.prjData, msg.dataLen);
          break;

        case UV_PRJ_DEL_FILE:                 
          stat = UVSC_PRJ_DEL_FILE(msg.connHandle, &msg.prjData, msg.dataLen);
          break;

        case UV_PRJ_BUILD:  
          stat = UVSC_PRJ_BUILD(msg.connHandle, false);
          break;

        case UV_PRJ_REBUILD:                    
          stat = UVSC_PRJ_BUILD(msg.connHandle, true);
          break;

        case UV_PRJ_CLEAN:                       
          stat = UVSC_PRJ_CLEAN(msg.connHandle);
          break;

        case UV_PRJ_BUILD_CANCEL:          
          stat = UVSC_PRJ_BUILD_CANCEL(msg.connHandle);
          break;

        case UV_PRJ_FLASH_DOWNLOAD:     
          stat = UVSC_PRJ_FLASH_DOWNLOAD(msg.connHandle);
          break;

        case UV_PRJ_GET_DEBUG_TARGET:
          stat = UVSC_PRJ_GET_DEBUG_TARGET(msg.connHandle, &msg.dbgtgtopt);
          break;

        case UV_PRJ_SET_DEBUG_TARGET:
          stat = UVSC_PRJ_SET_DEBUG_TARGET(msg.connHandle, &msg.dbgtgtopt);
          break;

        case UV_PRJ_GET_OPTITEM:  
          stat = UVSC_PRJ_GET_OPTITEM(msg.connHandle, &msg.trnOptData, msg.dataLen);
          break;

        case UV_PRJ_SET_OPTITEM:          
          stat = UVSC_PRJ_SET_OPTITEM(msg.connHandle, &msg.trnOptData, msg.dataLen);
          break;

        case UV_PRJ_ENUM_GROUPS: 
          sstrCount = SSTR_COUNT;
          stat = UVSC_PRJ_ENUM_GROUPS(msg.connHandle, sstr, sstrIndexes, &sstrCount);
          break;

        case UV_PRJ_ENUM_TARGETS: 
          sstrCount = SSTR_COUNT;
          stat = UVSC_PRJ_ENUM_TARGETS(msg.connHandle, sstr, sstrIndexes, &sstrCount);
          break;

        case UV_PRJ_ENUM_FILES:            
          sstrCount = SSTR_COUNT;
          stat = UVSC_PRJ_ENUM_FILES(msg.connHandle, 
                                     &msg.sstrData, msg.dataLen, 
                                     sstr, sstrIndexes, &sstrCount);
          break;

        case UV_PRJ_CMD_PROGRESS:
          // Not used
          break;

        case UV_PRJ_GET_OUTPUTNAME:
          stat = UVSC_PRJ_GET_OUTPUTNAME(msg.connHandle, &msg.iPathReq, sstr, 512);
          break;

        case UV_PRJ_GET_CUR_TARGET:
          stat = UVSC_PRJ_GET_CUR_TARGET(msg.connHandle, &msg.iPathReq, sstr, 512);
          break;

        case UV_PRJ_ACTIVE_FILES:         
          stat = UVSC_PRJ_ACTIVE_FILES(msg.connHandle, &uiTemp);
          break;

        case UV_DBG_ENTER:    
          stat = UVSC_DBG_ENTER(msg.connHandle);
          break;

        case UV_DBG_EXIT: 
          stat = UVSC_DBG_EXIT(msg.connHandle);
          break;

        case UV_DBG_START_EXECUTION: 
          stat = UVSC_DBG_START_EXECUTION(msg.connHandle);
          break;

        case UV_DBG_STOP_EXECUTION:     
          stat = UVSC_DBG_STOP_EXECUTION(msg.connHandle);
          break;

        case UV_DBG_STATUS:  
          stat = UVSC_DBG_STATUS(msg.connHandle, &iTemp);
          break;

        case UV_DBG_RESET:     
          stat = UVSC_DBG_RESET(msg.connHandle);
          break;

        case UV_DBG_STEP_HLL:
          stat = UVSC_DBG_STEP_HLL(msg.connHandle);
          break;

        case UV_DBG_STEP_INTO:           
          stat = UVSC_DBG_STEP_INTO(msg.connHandle);
          break;

        case UV_DBG_STEP_INSTRUCTION:  
          stat = UVSC_DBG_STEP_INSTRUCTION(msg.connHandle);
          break;

        case UV_DBG_STEP_OUT:       
          stat = UVSC_DBG_STEP_OUT(msg.connHandle);
          break;

        case UV_DBG_CALC_EXPRESSION:   
          stat = UVSC_DBG_CALC_EXPRESSION(msg.connHandle, &msg.vsetData, msg.dataLen); 
          if (stat == UVSC_STATUS_SUCCESS) {
            // Message back to the dialog the expression result
            DialogCalcExpr (&msg.vsetData.val);
          }
          break;

        case UV_DBG_MEM_READ:
          stat = UVSC_DBG_MEM_READ(msg.connHandle, &msg.amemData, msg.dataLen);
          if (stat == UVSC_STATUS_SUCCESS) {
            DialogReadWrite();
          }
          break;

        case UV_DBG_MEM_WRITE:              
          stat = UVSC_DBG_MEM_WRITE(msg.connHandle, &msg.amemData, msg.dataLen);
          if (stat == UVSC_STATUS_SUCCESS) {
            DialogReadWrite();
          }
          break;

        case UV_DBG_TIME_INFO:  
          // TODO - Not yet in UVSC
          break;

        case UV_DBG_SET_CALLBACK:          
          break;

        case UV_DBG_VTR_GET:
          stat = UVSC_DBG_VTR_GET(msg.connHandle, &msg.vsetData, msg.dataLen);
          break;

        case UV_DBG_VTR_SET: 
          stat = UVSC_DBG_VTR_SET(msg.connHandle, &msg.vsetData, msg.dataLen);
          break;

        case UV_DBG_SERIAL_GET:  
          // TODO UVSC_DBG_SERIAL_GET(msg.connHandle, 
          break;

        case UV_DBG_ITM_REGISTER:
          UVSC_DBG_ITM_REGISTER(msg.connHandle, &msg.itmIo, msg.dataLen);
          break;

        case UV_DBG_ITM_UNREGISTER:
          UVSC_DBG_ITM_UNREGISTER(msg.connHandle, &msg.itmIo, msg.dataLen);
          break;

        case UV_DBG_SERIAL_PUT:             
          break;

        case UV_DBG_VERIFY_CODE:           
          break;

        case UV_DBG_CREATE_BP: 
          iTemp = sizeof (BKRSP);
          stat = UVSC_DBG_CREATE_BP(msg.connHandle, &msg.bkparmData, sizeof(BKPARM), bkptRsp, &iTemp);
          break;

        case UV_DBG_ENUMERATE_BP:   
          bBkptRspCount = BKPTRSP_COUNT;
          stat = UVSC_DBG_ENUMERATE_BP(msg.connHandle, bkptRsp, bkptRspIndexes, &bBkptRspCount);
          if (stat == UVSC_STATUS_SUCCESS) {
            // Message back to the dialog the expression result
            DialogEnumBP (bkptRsp, bkptRspIndexes, bBkptRspCount);
          }
          break;

        case UV_DBG_CHANGE_BP:   
          iTemp = sizeof (BKRSP);
          stat = UVSC_DBG_CHANGE_BP(msg.connHandle, &msg.bkchgData, sizeof(BKCHG), bkptRsp, &iTemp);
          break;

        case UV_DBG_ENUM_SYMTP:              
          // TODO - Not implemented in UVSC
          break;

        case UV_DBG_ENUM_VTR: 
          vtrCount = AVTR_COUNT;
          stat = UVSC_DBG_ENUM_VTR(msg.connHandle, &msg.iVTREnumData, avtr, vtrIndexes, &vtrCount);
          break;

        case UV_DBG_ENUM_STACK:
          StackEnumCount = STACKENUM_COUNT;
          stat = UVSC_DBG_ENUM_STACK(msg.connHandle, &msg.iStkEnumData, StackEnum, &StackEnumCount);
          break;

        case UV_DBG_ADR_TOFILELINE:
          aflMapLen = AFLMAP_LEN;
          stat = UVSC_DBG_ADR_TOFILELINE(msg.connHandle, &msg.adrmtflData, pAflMap, &aflMapLen);
          break;

        case UV_DBG_ADR_SHOWCODE:
          stat = UVSC_DBG_ADR_SHOWCODE(msg.connHandle, &msg.iShowSyncData);
          break;

        case UV_DBG_POWERSCALE_SHOWCODE:
          stat = UVSC_POWERSCALE_SHOWCODE(msg.connHandle, &msg.powerScaleData);
          break;

        case UV_DBG_EXEC_CMD:
          stat = UVSC_DBG_EXEC_CMD(msg.connHandle, &msg.execCmdData, sizeof(SSTR));
          break;

        case UV_DBG_EVTR_REGISTER:    
          stat = UVSC_DBG_EVTR_REGISTER(msg.connHandle);
          break;

        case UV_DBG_EVTR_UNREGISTER:    
          stat = UVSC_DBG_EVTR_UNREGISTER(msg.connHandle);
          break;

        case UV_DBG_EVTR_ENUMSCVDFILES:
          sstrCount = SSTR_COUNT;
          stat = UVSC_DBG_EVTR_ENUMSCVDFILES(msg.connHandle, sstr, &sstrCount);
          break;


        default:
          ; // Unknown command - ignore
      }
      if (stat != UV_STATUS_SUCCESS) {
        DialogOpFailed();                   // Message back to the dialog that the operation failed
        DoGetLastError(msg.connHandle);     // Get the last error message automatically
      }
    }
    // else go around the loop again :)
  }

  UVSC_UnInit();

  return 0;
}


static void uvsc_log_callback(const char* msg, int msgLen) {
  const char *pStatus;
  const char *pDir;
  OUT_TYPE type = OUT_TYPE_ERROR;

  if (!bDisableLogging) {
    if (msgLen >= 17) {
      pDir = &msg[12];
      if (strncmp(pDir, "[-->]", 5) == 0) {
        type = OUT_TYPE_GEN;
      }
      else if (strncmp(pDir, "[T->]", 5) == 0) {
        type = OUT_TYPE_CMD;
      }
      else if (strncmp(pDir, "[<-T]", 5) == 0) {
        type = OUT_TYPE_CMD;
      }
      else if (strncmp(pDir, "[RTA]", 5) == 0) {
        // Send message to RTA window
        DialogRTAMsg((OUT_TYPE)RTA_OUT_TYPE_GEN, "%s", msg);
        return;
      }
    }

    if ((type == OUT_TYPE_ERROR) && (msgLen >= 45)) {
      pStatus = &msg[42];
      if (strncmp(pDir, "[<--]", 5) == 0) {
        if (strncmp(pStatus, "[UV_S", 5) == 0) {
          type = OUT_TYPE_ERROR;
        }
        else {
          type = OUT_TYPE_RSP;
        }
      }
      else if (strncmp(pDir, "[<==]", 5) == 0) {
        if (strncmp(pStatus, "[UV_S", 5) == 0) {
          type = OUT_TYPE_ERROR;
        }
        else {
          type = OUT_TYPE_ASYNC;
        }
      }
    }
    DialogLogMsg (type, "%s", msg);
  } // if (!bDisableLogging)
}


static void uvsc_callback(void* cb_custom, UVSC_CB_TYPE type, UVSC_CB_DATA *data) {
  UVSOCK_CMD *pMsg;

  switch (type) {
    
    case UVSC_CB_DISCONNECTED:
      // The client has disconnected - notify GUI (which will attempt a reconnect)
      DialogDisconnected(data->iConnHandle);
      break;

    case UVSC_CB_BUILD_OUTPUT_MSG:
      // We have received a build output message
      pMsg = (UVSOCK_CMD *)data;
      DialogBuildLine((const char *)pMsg->data.prjdata.szNames, pMsg->data.prjdata.nLen);
      break;

    case UVSC_CB_CMD_OUTPUT_MSG:
      // We have received a build output message
      pMsg = (UVSOCK_CMD *)data;
      DialogCmdLine((const char *)pMsg->data.sstr.szStr, pMsg->data.sstr.nLen);
      break;      

    case UVSC_CB_PROGRESS_BAR_MSG:
      // We have received a progress bar message
      pMsg = (UVSOCK_CMD *)data;

      switch (pMsg->data.cmdRsp.cmd) {
        case UV_PRJ_PBAR_INIT:
          DialogPBarInit((const char *)pMsg->data.cmdRsp.err.str, pMsg->data.cmdRsp.err.StrLen);
          break;
        case UV_PRJ_PBAR_SET:
          DialogPBarSetPos((const char *)pMsg->data.cmdRsp.err.str, pMsg->data.cmdRsp.err.StrLen);
          break;
        case UV_PRJ_PBAR_TEXT:
          DialogPBarSetText((const char *)pMsg->data.cmdRsp.err.str, pMsg->data.cmdRsp.err.StrLen);
          break;
        case UV_PRJ_PBAR_STOP:
          DialogPBarStop((const char *)pMsg->data.cmdRsp.err.str, pMsg->data.cmdRsp.err.StrLen);
          break;
      }
      break;

    default:
      ; // Do Nothing
  }
}


static void DoGetLastError (int iConnHandle) {
  char APIString[UVSC_MAX_API_STR_SIZE];
  UV_OPERATION uvOp;
  UV_STATUS uvStatus;
  UVSC_STATUS stat;

  stat = UVSC_GetLastError (iConnHandle, &uvOp, &uvStatus, APIString, UVSC_MAX_API_STR_SIZE);
  if (stat == UVSC_STATUS_SUCCESS) {
    DialogLogMsg(OUT_TYPE_ERROR, "Error occurred: %s ==> %s\nError detail  : %s", CUVSC_Log::RspCmd(uvOp), CUVSC_Log::RspText(uvStatus), APIString); 
  }
}


// The following functions notify the GUI of an event (poss with data)

static void DialogLogMsg (OUT_TYPE type, char *fmt, ...) {
  va_list    argp;
  int n;

  NotifyLogMsg *pMsg = (NotifyLogMsg *)malloc(sizeof(NotifyLogMsg));

  if (pMsg) {
    pMsg->type = type;
    va_start (argp, fmt);
    if (argp == NULL)  {         //  || argp[0] == 0)  {
      n = min (strlen (fmt), sizeof (pMsg->msg) - 4);
      memcpy (pMsg->msg, fmt, n);
    }
    else  {
      n = _vsnprintf (pMsg->msg, sizeof (pMsg->msg) - 4, fmt, argp);
    }
    pMsg->msg [n] = 0;

    va_end(argp);
    NotifyDialog(NOTIFY_DIALOG_LOGMSG, pMsg);
  }
}

static void DialogRTAMsg (OUT_TYPE type, char *fmt, ...) {
  va_list    argp;
  int n;

  NotifyLogMsg *pMsg = (NotifyLogMsg *)malloc(sizeof(NotifyLogMsg));

  if (pMsg) {
    pMsg->type = (OUT_TYPE)type;
    va_start (argp, fmt);
    if (argp == NULL)  {         //  || argp[0] == 0)  {
      n = min (strlen (fmt), sizeof (pMsg->msg) - 4);
      memcpy (pMsg->msg, fmt, n);
    }
    else  {
      n = _vsnprintf (pMsg->msg, sizeof (pMsg->msg) - 4, fmt, argp);
    }
    pMsg->msg [n] = 0;

    va_end(argp);
    NotifyDialog(NOTIFY_DIALOG_RTAMSG, pMsg);
  }
}

static void DialogReadWrite (void) { 
  NotifyDialog(NOTIFY_DIALOG_READWRITE, NULL);
}

static void DialogConnected (int handle, int port) {
  NotifyConnected *pMsg = (NotifyConnected *)malloc(sizeof(NotifyConnected));

  if (pMsg) {
    pMsg->handle = handle;
    pMsg->port = port;
    NotifyDialog(NOTIFY_DIALOG_CONNECTED, pMsg);
  }
}

static void DialogDisconnected (int handle) {
  NotifyConnected *pMsg = (NotifyConnected *)malloc(sizeof(NotifyConnected));

  if (pMsg) {
    pMsg->handle = handle;
    pMsg->port = 0;
    NotifyDialog(NOTIFY_DIALOG_DISCONNECTED, pMsg);
  }
}

static void DialogConnectFailed (void) {
  NotifyDialog(NOTIFY_DIALOG_CONNECTFAILED, NULL);
}

static void DialogCmdOutput (const char *output, int outputLen) {
  NotifyBigString *pMsg = (NotifyBigString *)malloc(sizeof(NotifyBigString) + outputLen);

  if (pMsg) {
    pMsg->len = outputLen;
    memcpy (pMsg->str, output, outputLen);
    pMsg->str[outputLen] = 0;                   // Add terminator

    NotifyDialog(NOTIFY_DIALOG_CMDOUTPUT, pMsg);
  }
}

static void DialogBuildOutput (const char *output, int outputLen) {
  NotifyBigString *pMsg = (NotifyBigString *)malloc(sizeof(NotifyBigString) + outputLen);

  if (pMsg) {
    pMsg->len = outputLen;
    memcpy (pMsg->str, output, outputLen);
    pMsg->str[outputLen] = 0;                   // Add terminator

    NotifyDialog(NOTIFY_DIALOG_BUILDOUTPUT, pMsg);
  }
}

static void DialogCalcExpr (TVAL *pTval) {
  NotifyTVAL *pMsg = (NotifyTVAL *)malloc(sizeof(NotifyTVAL));

  if (pMsg) {
    memcpy (&pMsg->tval, pTval, sizeof(TVAL));
 
    NotifyDialog(NOTIFY_DIALOG_CALCEXPR, pMsg);
  }
}



static void DialogEnumBP (BKRSP *pBkptRsp, int *pBkptIndexes, int bkptCount) {
  int i;
  NotifyBPEnum *pMsg = (NotifyBPEnum *)malloc(sizeof(NotifyBPEnum));

  if (pMsg) {
    pMsg->bkptCount = bkptCount;
    for (i=0; i<bkptCount; i++) {      
      memcpy (&pMsg->bkptRsp[i], &pBkptRsp[i], sizeof(BKRSP));
      pMsg->bkptIndexes[i] = pBkptIndexes[i];
    }
 
    NotifyDialog(NOTIFY_DIALOG_ENUMBP, pMsg);
  }
}

static void DialogBuildLine (const char *line, int lineLen) {
  NotifyBigString *pMsg = (NotifyBigString *)malloc(sizeof(NotifyBigString) + lineLen);

  if (pMsg) {
    pMsg->len = lineLen;
    memcpy (pMsg->str, line, lineLen);
    pMsg->str[lineLen] = 0;                   // Add terminator

    NotifyDialog(NOTIFY_DIALOG_BUILDLINE, pMsg);
  }
}

static void DialogCmdLine (const char *line, int lineLen) {
  NotifyBigString *pMsg = (NotifyBigString *)malloc(sizeof(NotifyBigString) + lineLen);

  if (pMsg) {
    pMsg->len = lineLen;
    memcpy (pMsg->str, line, lineLen);
    pMsg->str[lineLen] = 0;                   // Add terminator

    NotifyDialog(NOTIFY_DIALOG_CMDLINE, pMsg);
  }
}

static void DialogPBarInit (const char *initStr, int initStrLen) {
  NotifyBigString *pMsg = (NotifyBigString *)malloc(sizeof(NotifyBigString) + initStrLen);

  if (pMsg) {
    pMsg->len = initStrLen;
    memcpy (pMsg->str, initStr, initStrLen);
    pMsg->str[initStrLen] = 0;                   // Add terminator

    NotifyDialog(NOTIFY_DIALOG_PBARINIT, pMsg);
  }
}

static void DialogPBarSetText (const char *textStr, int textStrLen) {
  NotifyBigString *pMsg = (NotifyBigString *)malloc(sizeof(NotifyBigString) + textStrLen);

  if (pMsg) {
    pMsg->len = textStrLen;
    memcpy (pMsg->str, textStr, textStrLen);
    pMsg->str[textStrLen] = 0;                   // Add terminator

    NotifyDialog(NOTIFY_DIALOG_PBARTEXT, pMsg);
  }
}

static void DialogPBarSetPos (const char *posStr, int posStrLen) {
  NotifyBigString *pMsg = (NotifyBigString *)malloc(sizeof(NotifyBigString) + posStrLen);

  if (pMsg) {
    pMsg->len = posStrLen;
    memcpy (pMsg->str, posStr, posStrLen);
    pMsg->str[posStrLen] = 0;                    // Add terminator

    NotifyDialog(NOTIFY_DIALOG_PBARPOS, pMsg);
  }
}

static void DialogPBarStop (const char *stopStr, int stopStrLen) {
  NotifyBigString *pMsg = (NotifyBigString *)malloc(sizeof(NotifyBigString) + stopStrLen);

  if (pMsg) {
    pMsg->len = stopStrLen;
    memcpy (pMsg->str, stopStr, stopStrLen);
    pMsg->str[stopStrLen] = 0;                   // Add terminator

    NotifyDialog(NOTIFY_DIALOG_PBARSTOP, pMsg);
  }
}

static void DialogOpFailed (void) {
  NotifyDialog(NOTIFY_DIALOG_UVSCOPFAILED, NULL);
}

