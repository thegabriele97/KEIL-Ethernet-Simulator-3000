/******************************************************************************/
/* UVSC Tester -  UVSOCK Client DLL interface tester                          */
/******************************************************************************/
/* Name:    ThreadQueue.cpp                                                   */
/* Purpose: Class for managing thread message queueing                        */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2004-2016 KEIL - An ARM Company. All rights reserved.        */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include "stdafx.h"
#include "ThreadQueue.h"

//===========================================================================
// Thread Message Queue Class
//===========================================================================

// Mutex lock helper macros
#define MSGQUEUE_LOCK()     WaitForSingleObject (m_QueueMtx, INFINITE)
#define MSGQUEUE_UNLOCK()   ReleaseMutex (m_QueueMtx)

CThreadMsgQueue::CThreadMsgQueue(int iQItemSize, int iQSize) {    
  m_QueueMtx = CreateMutex(NULL, FALSE, NULL);                    // Don't name, otherwise all objects get same mutex!!!
  m_QueueEmptySem = CreateSemaphore(NULL, 0, 1, NULL);            // Don't name, otherwise all objects get same semaphore!!!
  m_QueueFullSem = CreateSemaphore(NULL, 1, 1, NULL);             // Don't name, otherwise all objects get same semaphore!!!
  m_Buffers = (char *)malloc(iQItemSize * iQSize);
  m_QSize = iQSize;
  m_QItemSize = iQItemSize;
  m_QCount = 0;
  m_QRIndex = 0;
  m_QWIndex = 0;
}
 
CThreadMsgQueue::~CThreadMsgQueue() {
  free (m_Buffers);
  CloseHandle(m_QueueMtx);
  CloseHandle(m_QueueEmptySem);
  CloseHandle(m_QueueFullSem);
}

int CThreadMsgQueue::GetCount() {
  int count;

  MSGQUEUE_LOCK();
  count = m_QCount;
  MSGQUEUE_UNLOCK();

  return (count);
}

CThreadMsgQueue::QSTATUS CThreadMsgQueue::Post(char *msg, DWORD timeout) {
  QSTATUS retVal = QOK;
  DWORD waitRes;

  waitRes = WaitForSingleObject (m_QueueFullSem, timeout);

  if (waitRes == WAIT_OBJECT_0) {
    MSGQUEUE_LOCK();
    if (m_QCount == 0) {
      // Q is no longer empty
      ReleaseSemaphore(m_QueueEmptySem, 1, NULL);
    } 
    memcpy(&(m_Buffers[m_QWIndex * m_QItemSize]), msg, m_QItemSize);
    m_QWIndex++;
    if (m_QWIndex == m_QSize) m_QWIndex = 0;        // Wrap Q write index
    m_QCount++;

    if (m_QCount != m_QSize) {
      // Q is not full, release the Q full semaphore
      ReleaseSemaphore(m_QueueFullSem, 1, NULL);
    }
    MSGQUEUE_UNLOCK();
  }
  else if (waitRes == WAIT_TIMEOUT) {
    // Queue is full!
    retVal = QTIMEOUT;
  }
  else {
    retVal = QFAILED;
  }
  return (retVal);
}

CThreadMsgQueue::QSTATUS CThreadMsgQueue::Get(char *msg, DWORD timeout) {
  QSTATUS retVal = QOK;
  DWORD waitRes;

  waitRes = WaitForSingleObject (m_QueueEmptySem, timeout);

  if (waitRes == WAIT_OBJECT_0) {
    MSGQUEUE_LOCK();
    if (m_QCount == m_QSize) {
      // Q is no longer full
      ReleaseSemaphore(m_QueueFullSem, 1, NULL);
    }
    // Copy buffer
    memcpy(msg, &(m_Buffers[m_QRIndex * m_QItemSize]), m_QItemSize);

    // Increment indexes
    m_QRIndex++;
    if (m_QRIndex == m_QSize) m_QRIndex = 0;        // Wrap Q read index
    m_QCount--;

    if (m_QCount != 0) {
      // Q is not empty, release the Q empty semaphore
      ReleaseSemaphore(m_QueueEmptySem, 1, NULL);
    }
    MSGQUEUE_UNLOCK();
  }
  else if (waitRes == WAIT_TIMEOUT) {
    // Q is empty!
    retVal = QTIMEOUT;
  }
  else {
    retVal = QFAILED;
  }
  return (retVal);
}

CThreadMsgQueue::QSTATUS CThreadMsgQueue::Flush() {
  QSTATUS retVal = QFAILED;
  char *pTempBuf;

  pTempBuf = (char *)malloc(m_QItemSize);
  if (pTempBuf) {
    while (Get(pTempBuf, 0) == QOK);    // Read the queue till it is empty
    free (pTempBuf);
    retVal = QOK;
  }
  
  return (retVal);
}





