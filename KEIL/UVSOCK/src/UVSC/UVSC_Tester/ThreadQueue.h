/******************************************************************************/
/* UVSC Tester -  UVSOCK Client DLL interface tester                          */
/******************************************************************************/
/* Name:    ThreadQueue.h                                                     */
/* Purpose: Class for managing thread message queueing                        */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2004-2016 KEIL - An ARM Company. All rights reserved.        */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#pragma once

// CThreadMsgQueue
// This class is used to queue thread messages
// See ThreadQueue.cpp for the implementation of this class
//

class CThreadMsgQueue {

public:
  enum QSTATUS {QOK, QTIMEOUT, QFAILED};

private:
  HANDLE m_QueueMtx;
  HANDLE m_QueueEmptySem;
  HANDLE m_QueueFullSem;
  char *m_Buffers;
  int m_QSize;
  int m_QItemSize;
  int m_QRIndex;
  int m_QWIndex;
  int m_QCount;

public:
  CThreadMsgQueue(int iQItemSize, int iQSize);
  virtual ~CThreadMsgQueue();

  int GetCount();
  QSTATUS Post(char *msg, DWORD timeout);
  QSTATUS Get(char *msg, DWORD timeout);
  QSTATUS Flush();
};
