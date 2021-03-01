/*****************************************************************************
*           Change Log
*  Date     | Change
*-----------+-----------------------------------------------------------------
* 24-Jan-00 | Created
*****************************************************************************/
#include "stdafx.h"
#include "TraceEvent.h"
#include "resource.h"

IMPLEMENT_DYNAMIC(TraceEvent, CObject)
IMPLEMENT_DYNAMIC(TraceExtended, TraceEvent)
IMPLEMENT_DYNAMIC(TraceMessage, TraceEvent)
IMPLEMENT_DYNAMIC(TraceTrace, TraceMessage)
IMPLEMENT_DYNAMIC(TraceComment, TraceMessage)
IMPLEMENT_DYNAMIC(TraceFormatComment, TraceMessage)
IMPLEMENT_DYNAMIC(TraceError, TraceEvent)
IMPLEMENT_DYNAMIC(TraceFormatError, TraceError)
IMPLEMENT_DYNAMIC(TraceFormatMessage, TraceError)
IMPLEMENT_DYNAMIC(TraceWarning, TraceEvent)
IMPLEMENT_DYNAMIC(TraceData, TraceEvent)

CFont * TraceExtended::font = NULL;
CFont * TraceMessage::font = NULL;
CFont * TraceError::font = NULL;
CFont * TraceWarning::font = NULL;
CFont * TraceData::font = NULL;

UINT TraceEvent::counter = 0;

/****************************************************************************
*                            TraceEvent::showfile
* Result: CString
*       Nicely formatted string for writing to file
* Effect: 
*       Prepares a line for file output; uses fixed-width fields for the prefix
* Notes:
*	Cleverly uses the virtual show() method to get the rest of the string
****************************************************************************/

CString TraceEvent::showfile()
{
  CString t, s, id, thread, blank = "";
  
  id = showID();
  thread = showThread();

  if (id != blank){
    t.Format(_T("%06d | %s |"), seq, id);
  }
  else {
    t.Format(_T("%06d |          |"), seq);
  }

  if (thread != blank){
    s.Format(_T(" %s | "), thread);
  }
  else {
    s.Format(_T("         | "));
  }
  
  return t + s + show(0);

}

/****************************************************************************
*                              TraceEvent::show
* Result: CString
*       
* Effect: 
*       Formats the base string for the trace event
****************************************************************************/

CString TraceEvent::show(int line)
{
   CString s; // Currently nothing in the base string
   return s;
}

CString TraceEvent::showSeq() {
  CString s;
  s.Format(_T("%d "), seq);
  return s;
}

/****************************************************************************
*                             TraceEvent::showID
* Result: CString
*       The trace ID, or an empty string (now shows time!)
* Effect: 
*       Formats the trace ID.  Connectionless events have no id
****************************************************************************/
CString TraceEvent::showID()
{
  CString s = time.Format(_T("%H:%M:%S"));
  return s;
}

/****************************************************************************
*                       CString TraceEvent::showThread
* Result: CString
*       Thread ID, formatted - hijacked to show DEBUG / COMMENT / ETC
****************************************************************************/
#define ID_STRING_RES_OFFSET  20000
CString TraceEvent::showThread()
{
  //CString s;
  //s.Format(_T("%08x "), threadID);
  //return s;

  CString t;

  if(filterID != None)    
    t.LoadString(ID_STRING_RES_OFFSET + filterID);
    //t.Format(_T("%d  "), filterID);
  else
    t = _T("");

  return t; 
}

/****************************************************************************
*                             TraceEvent::cvtext
* Inputs:
*       CString s: String to convert
* Result: CString
*       String formatted for printout
****************************************************************************/

CString TraceEvent::cvtext(CString s)
{
  // NOT FORMATTING - TOO SLOW! 
#if 0
  CString str;
  int sLen = s.GetLength();

  for(int i = 0; i < sLen; i++)
  { /* convert to printable */
    str += cvtext(s[i]);
  } /* convert to printable */
  return str;     
#else
  return s;
#endif
}

/****************************************************************************
*                            TraceEvent::cvtext
* Inputs:
*       TCHAR ch: Character to display (8-bit or 16-bit)
* Result: CString
*       Displayable string
****************************************************************************/

CString TraceEvent::cvtext(TCHAR ch)
{
  BYTE ch8 = (BYTE)ch;
  CString s;

#ifdef _UNICODE
  BYTE ucp = (BYTE)(ch >> 8);
  if(ucp != 0)
    s.Format(_T(".x%02x."), ucp);
#endif     
  if(ch8 >= ' ' && ch8 < 0177)
  { /* simple char */
    return CString((TCHAR)ch8); // NYI: handle Unicode
  } /* simple char */

  switch(ch8)
  { /* ch */
  case '\a':
    return CString(_T("\\a"));
  case '\b':
    return CString(_T("\\b"));
  case '\f':
    return CString(_T("\\f"));
  case '\n':
    return CString(_T("\\n"));
  case '\r':
    return CString(_T("\\r"));
  case '\t':
    return CString(_T("\\t"));
  case '\v':
    return CString(_T("\\v"));
  case '\\':
    return CString(_T("\\\\"));
  case '\"':
    return CString(_T("\\\""));
  default:
    s.Format(_T("\\x%02x"), ch);
    return s;
  } /* ch */
}

/****************************************************************************
*		     TraceFormatComment::TraceFormatComment
* Inputs:
*       UINT id: ID for line
*	CString fmt: Formatting string
*	...: Arguments for formatting
* Effect: 
*       Constructor
****************************************************************************/


TraceFormatComment::TraceFormatComment(UINT id, CString fmt, ...) : TraceMessage(id, CString(""))
{
  CString s;

  va_list args;
  va_start(args, fmt);
  LPTSTR p = s.GetBuffer(2048);
  _vstprintf(p, fmt, args);
  s.ReleaseBuffer();
  va_end(args);

  int iStart = 0;
  lineCount = 0;
  while (iStart != -1) {
    CString sTemp = s.Tokenize("\n", iStart);
    if (iStart != -1) {
      messages.Add(sTemp);
      lineCount++;
    }      
  }
} // TraceFormatComment::TraceFormatComment

/****************************************************************************
*                   TraceFormatComment::TraceFormatComment
* Inputs:
*       UINT id: Line number
*	UINT fmt: ID of formatting string
*	...: Arguments
* Effect: 
*       Creates a trace comment entry, formatted
****************************************************************************/

TraceFormatComment::TraceFormatComment(UINT id, UINT fmt, ...) : TraceMessage(id, CString(""))
{
  CString s;

  va_list args;
  va_start(args, fmt);
  CString fmtstr;
  fmtstr.LoadString(fmt);
  LPTSTR p = s.GetBuffer(2048);
  _vstprintf(p, fmtstr, args);
  s.ReleaseBuffer();
  va_end(args);

  int iStart = 0;
  lineCount = 0;
  while (iStart != -1) {
    CString sTemp = s.Tokenize("\n", iStart);
    if (iStart != -1) {
      messages.Add(sTemp);
      lineCount++;
    }      
  }
} // TraceFormatComment::TraceFormatComment

/****************************************************************************
*                             TraceMessage::show
* Result: CString
*       Display string
****************************************************************************/

CString TraceMessage::show(int line)
{
  return TraceEvent::show(0) + messages[line];
}

/****************************************************************************
*		       TraceFormatError::TraceFormatError
* Inputs:
*       UINT id: ID for line
*	CString fmt: Formatting string
*	...: Arguments for formatting
* Effect: 
*       Constructor
****************************************************************************/

TraceFormatError::TraceFormatError(UINT id, CString fmt, ...) : TraceError(id, CString(""))
{
  CString s;

  va_list args;
  va_start(args, fmt);
  LPTSTR p = s.GetBuffer(2048);
  _vstprintf(p, fmt, args);
  s.ReleaseBuffer();
  va_end(args);

  int iStart = 0;
  lineCount = 0;
  while (iStart != -1) {
    CString sTemp = s.Tokenize("\n", iStart);
    if (iStart != -1) {
      Errors.Add(sTemp);
      lineCount++;
    }      
  }
} // TraceFormatError::TraceFormatError

/****************************************************************************
*                   TraceFormatError::TraceFormatError
* Inputs:
*       UINT id: Line number
*	UINT fmt: ID of formatting string
*	...: Arguments
* Effect: 
*       Creates a trace Error entry, formatted
****************************************************************************/

TraceFormatError::TraceFormatError(UINT id, UINT fmt, ...) : TraceError(id, CString(""))
{
  CString s;

  va_list args;
  va_start(args, fmt);
  CString fmtstr;
  fmtstr.LoadString(fmt);
  LPTSTR p = s.GetBuffer(2048);
  _vstprintf(p, fmtstr, args);
  s.ReleaseBuffer();
  va_end(args);

  int iStart = 0;
  lineCount = 0;
  while (iStart != -1) {
    CString sTemp = s.Tokenize("\n", iStart);
    if (iStart != -1) {
      Errors.Add(sTemp);
      lineCount++;
    }      
  }
} // TraceFormatError::TraceFormatError

/****************************************************************************
*                             TraceError::show
* Result: CString
*       Display string
****************************************************************************/

CString TraceError::show(int line)
{
  return TraceEvent::show(0) + Errors[line];
}


/****************************************************************************
*                             TraceWarning::show
* Result: CString
*       Display string
****************************************************************************/

CString TraceWarning::show(int line)
{
  return TraceEvent::show(0) + Errors[line];
}



/****************************************************************************
*                     TraceFormatMessage::TraceFormatMessage
* Inputs:
*       UINT id:
*	DWORD error:
* Effect: 
*       Constructor
****************************************************************************/

TraceFormatMessage::TraceFormatMessage(UINT id, DWORD err) : TraceError(id)
{
  LPTSTR s;
  if(::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM,
    NULL,
    err,
    0,
    (LPTSTR)&s,
    0,
    NULL) == 0)
  { /* failed */
    // See if it is a known error code
    CString t; 
    t.LoadString(err);
    if(t.GetLength() == 0 || t[0] != _T('®'))     
    { /* other error */                        
      CString fmt;                               
      fmt.LoadString(IDS_TRACELOG_UNKNOWN_ERROR);
      t.Format(fmt, err, err);                   
    } /* other error */                         
    else                                           
      if(t.GetLength() > 0 && t[0] == _T('®'))       
      { /* drop prefix */                         
        t = t.Mid(1);                              
      } /* drop prefix */                         
      Errors.Add(t);
  } /* failed */
  else
  { /* success */
    LPTSTR p = _tcschr(s, _T('\r'));
    if(p != NULL)
    { /* lose CRLF */
      *p = _T('\0');
    } /* lose CRLF */
    Errors.Add(s);
    ::LocalFree(s);
  } /* success */
}

/****************************************************************************
*                               TraceData::show
* Result: CString
*       The data, formatted according to the type of data stored
****************************************************************************/

CString TraceData::show(int line)
{
  return TraceEvent::show(0) + DataLines[line];
} // TraceData::show
