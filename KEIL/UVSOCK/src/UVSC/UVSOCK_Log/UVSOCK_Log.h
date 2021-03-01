/******************************************************************************/
/* UVSOCK  -  UVSOCK interface                                                */
/******************************************************************************/
/* Name:    UVSOCK_Log.h                                                      */
/* Purpose: A class for logging UVSC messages                                 */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2004-2016 KEIL - An ARM Company. All rights reserved.        */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#pragma once

#include "UVSC_C.h"


// CUVSC_Log
// This class is used for decoding and logging UVSOCK messages
// See UVSOCK_Log.cpp for the implementation of this class
//

/** UVSC logging callback function pointer definition
  *
  * Defines the format to which the UVSC logging callback function configured
  * through #UVSC_OpenConnection must correspond.
  */
typedef void (*log_cb)(const char* msg, int msgLen);

class CUVSC_Log {
public:
  enum Direction {
    Req,
    Rsp,
    Async,
    RTA,
  };  

  enum TraceType {
    Enter,
    Exit,
  };  

private:
  HANDLE m_fileHandle;
  log_cb m_Callback;
  HANDLE m_logMtx;
  DWORD m_TimeZero;
  bool m_EnableRaw;
  bool m_EnableTrace;

public:
  CUVSC_Log();
  ~CUVSC_Log();

  void Start (char *filename, bool append, log_cb callback);
  void Log (Direction dir, UVSOCK_CMD *msg, bool raw);
  void Trace (TraceType type, const char *msg, UVSC_STATUS status = UVSC_STATUS_SUCCESS);
  void Stop ();
  void Control (bool enableRaw, bool enableTrace);

  // Message decoding functions
  static const char *RspCmd (UV_OPERATION cmd);
  static const char *RspText (UV_STATUS err);
  static const char *UVSCStatusText (UVSC_STATUS status);
  static const char *UVSCStopReasonText (STOPREASON stopReason);
  static const char *UVSCBpTypeText (BKTYPE type);
  static const char *UVSCBpChgText (CHG_TYPE type);
  static const char *UVSCOptSelText (OPTSEL opt);
  static const char *UVSCVTTTypeText (VTT_TYPE vType);

  char *DecodeBkParm(char *pStr, BKPARM *bkparm);
  char *DecodeBkRsp(char *pStr, BKRSP *bkrsp);
  char *DecodeBkRsp(char *pStr, BKRSP *bkrsp, UV_STATUS nBpCode);
  char *DecodeBkChg(char *pStr, BKCHG *bkchg);
  char *DecodePrjActiveFiles(char *pStr, UINT nVal);
  char *DecodeGenGetVersion(char *pStr, UINT nVal);
  char *DecodeDbgStatus(char *pStr, UINT nVal);
  char *DecodePrjData(char *pStr, PRJDATA *prjData);
  char *DecodeTrnOpt(char *pStr, TRNOPT *trnopt);
  char *DecodeSstr(char *pStr, SSTR *sstr);
  char *DecodeExecCmd(char *pStr, EXECCMD *execcmd);
  char *DecodeTVal(char *pStr, TVAL *tval);
  char *DecodeVSet(char *pStr, VSET *vset);
  char *DecodeAMem(char *pStr, AMEM *amem, bool showData);
  char *DecodeSerIO(char *pStr, SERIO *serIO, bool showData);
  char *DecodeItmOut(char *pStr, ITMOUT *itmIout, bool showData);
  char *DecodeEvtrOut(char *pStr, EVTROUT *evtrOut, bool showData);
  char *DecodeItmRegister(char *pStr, ITMOUT *itmIO);
  char *DecodeItmUnregister(char *pStr, ITMOUT *itmIO);
  char *DecodeUVLicInfo(char *pStr, UVLICINFO *licinfo);
  char *DecodeDbgTrgOpt(char *pStr, DBGTGTOPT *dbgTrgOpt);
  char *DecodeStackEnum(char *pStr, STACKENUM *stackEnum);
  char *DecodeAflMap(char *pStr, ADRMTFL *adrFmtl);
  char *DecodeAflMap(char *pStr, AFLMAP *aflMap);
  char *DecodeIShowSync(char *pStr, iSHOWSYNC *iShowSync);
  char *DecodeAVtr(char *pStr, AVTR *iAVtr);
  char *DecodeIVtrEnum(char *pStr, iVTRENUM *iVtrEnum);
  char *DecodeBpReason(char *pStr, BPREASON *bpReason);
  char *DecodeIPathReq(char *pStr, iPATHREQ *iPathReq);
  char *DecodePowerScale_ShowCode (char *pStr, UVSC_PSTAMP *pPowerScaleData);
  char *DecodePowerScale_ShowPower(char *pStr, UVSC_PSTAMP *pPowerScaleData);       // [TdB: 23.11.2010] PowerScale 

  char *DecodeError(char *pStr, UVSOCK_CMD_RESPONSE *rsp);
  char *DecodeCmdResponse(char *pStr, UVSOCK_CMD_RESPONSE *rsp, UINT len);
  char *DecodeAsyncResponse(char *pStr, UVSOCK_CMD_RESPONSE *rsp, UINT len);
  char *DecodeMessageData(char *pStr, UVSOCK_CMD *msg, Direction dir);
  char *DecodeHeader(char *pStr, UVSOCK_CMD *msg, Direction dir);
  char *DecodeDirection(char *pStr, Direction dir);
  char *DecodeMessage(char *pStr, UVSOCK_CMD *msg, Direction dir);
  char *DecodeRaw(char *pStr, UVSOCK_CMD *msg, Direction dir);
   
  char *DecodeTraceStatus(char *pStr, UVSC_STATUS status, TraceType type);
  char *DecodeTraceMessage(char *pStr, const char *msg, TraceType type);
  char *DecodeTraceType(char *pStr, TraceType type);
  char *DecodeTrace(char *pStr, const char *msg, TraceType type, UVSC_STATUS status);
};


