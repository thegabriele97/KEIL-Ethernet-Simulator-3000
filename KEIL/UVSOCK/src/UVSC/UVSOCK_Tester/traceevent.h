#ifndef _TRACEEVENT_H_
#define _TRACEEVENT_H_

/****************************************************************************
*                           class TraceEvent
* Inputs:
*	UINT id: TelaConsole connection ID, or TraceEvent::None
* Notes:
*	This constructor is used only by its subclasses.  This is a virtual
*	class and is never instantiated on its own
****************************************************************************/

class TraceEvent : public CObject {
  DECLARE_DYNAMIC(TraceEvent)
protected:
  static UINT counter;
  TraceEvent(UINT id) { time = CTime::GetCurrentTime(); 
  filterID = id; 
  threadID = ::GetCurrentThreadId(); 
  seq = counter++;
  lineCount = 1;
  selected  = false;
  }
public:
  enum { 
    None    = -1, 
    Error   = 0,
    Warning = 1,
    Message = 2,
    Trace   = 3,
    Comment = 4,
    Data    = 5,
  };
  virtual ~TraceEvent() { }
  virtual COLORREF textcolor() = 0;
  virtual CString show(int line);
  virtual CString showSeq();
  virtual CString showID();
  virtual CString showfile();
  virtual CString showThread();
  virtual UINT displayHeight() { return 1; }
  virtual UINT getLineCount() { return lineCount; }
  virtual UINT getID() {return filterID; }
  virtual CFont * getFont() { return NULL; }
  virtual void setSelected(bool s) { selected = s; }
  virtual bool getSelected() { return selected; }
  virtual void setItemNumber(UINT no) { itemNumber = no; }
  virtual UINT getItemNumber() { return itemNumber; }
  virtual UINT getItemCount() { return lineCount; }
  static CString cvtext(TCHAR ch); // useful for various subclasses
  static CString cvtext(CString s); // useful for various subclasses
protected:
  CTime time;
  UINT  filterID;
  DWORD threadID;
  UINT  seq;
  UINT  lineCount;  
private:
  bool selected;
  UINT itemNumber;
};


class TraceExtended: public TraceEvent {
  DECLARE_DYNAMIC(TraceExtended)
public:
  TraceExtended(TraceEvent *owner, UINT id, const CString & s) : TraceEvent(id) {
    lineCount = 1;
    message = s;
    this->owner = owner;
  }
  TraceExtended(UINT id, UINT u) : TraceEvent(id){ 
    CString s;
    lineCount = 1;
    s.LoadString(u); 
    message = s; 
  }
  virtual ~TraceExtended() { }
  virtual COLORREF textcolor() { return owner->textcolor(); }
  virtual CString show(int line) {
    return TraceEvent::show(0) + message;
  };
  virtual CString showThread() { CString s= _T("       "); return s; };
  virtual CString showID() { CString s = _T("        "); return s; };
  virtual CFont * getFont() { return font; }
  static CFont * setFont(CFont * newfont) { CFont * old = font; font = newfont; return old; }
  virtual void setSelected(bool s) { owner->setSelected(s); }
  virtual bool getSelected() { return owner->getSelected(); }
  virtual UINT getItemCount() { return owner->getLineCount(); }
  virtual TraceEvent * getOwner() { return owner; }
protected:
  static CFont *  font;
  CString message;
  TraceEvent *owner;
};


/****************************************************************************
*                           class TraceMessage
* Inputs:
*	UINT id: Telaconsole connection ID, or TraceEvent::None
*	CString s: String to display
* Inputs:
*	UINT id: Telaconsole connection ID, or TraceEvent::None
*	UINT u: IDS_ index of string to display
****************************************************************************/

class TraceMessage: public TraceEvent {
  DECLARE_DYNAMIC(TraceMessage)
public:
  TraceMessage(UINT id, const CString & s) : TraceEvent(id) {
    int iStart = 0;
    lineCount = 0;
    while (iStart != -1) {
      CString sTemp = s.Tokenize("\n", iStart);
      if (iStart != -1) {
        messages.Add(sTemp);
        lineCount++;
      }      
    }
  }
  TraceMessage(UINT id, UINT u) : TraceEvent(id){ 
    CString s; 
    s.LoadString(u); 
    messages.Add(s); 
  }
  virtual ~TraceMessage() {
    messages.RemoveAll();
  }
  virtual COLORREF textcolor() { return RGB(255, 255, 255); /* White */ }
  virtual CString show(int line);
  virtual CFont * getFont() { return font; }
  static CFont * setFont(CFont * newfont) { CFont * old = font; font = newfont; return old; }
protected:
  static CFont *  font;
  CStringArray messages;
};

// TraceTrace clase
class TraceTrace : public TraceMessage
{
  DECLARE_DYNAMIC(TraceTrace)
public:
  TraceTrace(UINT id, CString s) : TraceMessage(id, s) { }
  TraceTrace(UINT id, UINT u) : TraceMessage(id, u) { }
  virtual ~TraceTrace() { }
  virtual COLORREF textcolor() { return RGB(145,145,255); } /* Blue */ 
};

// TraceComment class
class TraceComment : public TraceMessage
{
  DECLARE_DYNAMIC(TraceComment)
public:
  TraceComment(UINT id, CString s) : TraceMessage(id, s) { }
  TraceComment(UINT id, UINT u) : TraceMessage(id, u) { }
  virtual ~TraceComment() { }
  virtual COLORREF textcolor() { return RGB(128, 255, 128); } /* Green */ 
};


/****************************************************************************
*                          class TraceFormatComment
* Inputs:
*       UINT id: Line number
*	CString fmt: Formatting string
*	...
****************************************************************************/

class TraceFormatComment : public TraceMessage
{
  DECLARE_DYNAMIC(TraceFormatComment)
public:
  TraceFormatComment(UINT id, CString fmt, ...);
  TraceFormatComment(UINT id, UINT fmt, ...);
  virtual COLORREF textcolor() { return RGB(128, 255, 128); } /* Green */ 
}; // class TraceFormatComment

/****************************************************************************
*                           class TraceError
* Inputs:
*	UINT id: TelaConsole connection ID, or TraceEvent::None
* Inputs:
*	UINT id: TelaConsole connection ID, or TraceEvent::None
*	CString s: String to display
* Inputs:
*	UINT id: TelaConsole connection ID, or TraceEvent::None
*	UINT u: IDS_ token index of string to display
* Notes:
*	The first form is not used directly, only by subclasses
****************************************************************************/

class TraceError: public TraceEvent {
  DECLARE_DYNAMIC(TraceError)
public:
  TraceError(UINT id, const CString & s) : TraceEvent(id) {
    int iStart = 0;
    lineCount = 0;
    while (iStart != -1) {
      CString sTemp = s.Tokenize("\n", iStart);
      if (iStart != -1) {
        Errors.Add(sTemp);
        lineCount++;
      }
    }
  }
  TraceError(UINT id, UINT u) : TraceEvent(id){ 
    CString s; 
    s.LoadString(u); 
    Errors.Add(s);
  }
  virtual ~TraceError() {
    Errors.RemoveAll();
  }
  virtual COLORREF textcolor() { return RGB(225, 19, 24); /* Red */}
  virtual CString show(int line);
  virtual CFont * getFont() { return font; }
  static CFont * setFont(CFont * newfont) { CFont * old = font; font = newfont; return old; }
protected:
  TraceError(UINT id) : TraceEvent(id) {}
  CStringArray Errors;
  static CFont * font;
};

/****************************************************************************
*			    class TraceFormatMessage
* Inputs:
*	UINT id: TelaConsole connection ID, or TraceEvent::None
*	DWORD error: Error code from GetLastError()
****************************************************************************/

class TraceFormatMessage: public TraceError {
  DECLARE_DYNAMIC(TraceFormatMessage)
public:
  TraceFormatMessage(UINT id, DWORD error);
  virtual ~TraceFormatMessage() { }
};

/****************************************************************************
*			     class TraceFormatError
* Inputs:
*       UINT id: Line number
*	CString fmt: Formatting string
*	...
****************************************************************************/

class TraceFormatError : public TraceError
{
  DECLARE_DYNAMIC(TraceFormatError)
public:
  TraceFormatError(UINT id, CString fmt, ...);
  TraceFormatError(UINT id, UINT fmt, ...);
}; // class TraceFormatError




/****************************************************************************
*                           class TraceWarning
* Inputs:
*	UINT id: TelaConsole connection ID, or TraceEvent::None
* Inputs:
*	UINT id: TelaConsole connection ID, or TraceEvent::None
*	CString s: String to display
* Inputs:
*	UINT id: TelaConsole connection ID, or TraceEvent::None
*	UINT u: IDS_ token index of string to display
* Notes:
*	The first form is not used directly, only by subclasses
****************************************************************************/

class TraceWarning: public TraceEvent {
  DECLARE_DYNAMIC(TraceWarning)
public:
  TraceWarning(UINT id, const CString & s) : TraceEvent(id) {
    int iStart = 0;
    lineCount = 0;
    while (iStart != -1) {
      CString sTemp = s.Tokenize("\n", iStart);
      if (iStart != -1) {
        Errors.Add(sTemp);
        lineCount++;
      }
    }
  }
  TraceWarning(UINT id, UINT u) : TraceEvent(id){ 
    CString s; 
    s.LoadString(u); 
    Errors.Add(s);
  }
  virtual ~TraceWarning() {
    Errors.RemoveAll();
  }
  virtual COLORREF textcolor() { return RGB(255, 92, 15); /* Orange */}
  virtual CString show(int line);
  virtual CFont * getFont() { return font; }
  static CFont * setFont(CFont * newfont) { CFont * old = font; font = newfont; return old; }
protected:
  TraceWarning(UINT id) : TraceEvent(id) {}
  CStringArray Errors;
  static CFont * font;
};









/****************************************************************************
*                           class TraceData
* Inputs:
*	UINT id: ID of message
*	LPBYTE data: 8-bit data string, possibly containing NUL bytes
*	UINT length: Length of data string
****************************************************************************/

class TraceData: public TraceEvent {
  DECLARE_DYNAMIC(TraceData)
public:
  enum DataWidth {
    _8BIT = 0,
    _16BIT = 1,
    _32BIT = 2,
    _64BIT = 3
  };
public:
  TraceData(UINT id, DataWidth width, LPBYTE data, UINT length) : TraceEvent(id) {
    CString t, str;
    lineCount = 0;
    switch (width) {
      case _64BIT: // Not supported (does 32 bit)
      case _32BIT:
        for(UINT i = 0; i < length/4; i++)
        { /* convert */
          if (((i % 4) == 0) && (i != 0)) {
            DataLines.Add(str);
            str.Empty();
            lineCount++;
          }
          t.Format(_T("%08X "), ((UINT *)data)[i]);
          str += t;
        } /* convert */
        DataLines.Add(str);
        lineCount++;
      break;

      case _16BIT:
        for(UINT i = 0; i < length/2; i++)
        { /* convert */
          if (((i % 8) == 0) && (i != 0)) {
            DataLines.Add(str);
            str.Empty();
            lineCount++;
          }
          t.Format(_T("%04X "), ((short *)data)[i]);
          str += t;
        } /* convert */
        DataLines.Add(str);
        lineCount++;
      break;

      case _8BIT:
      default:
        for(UINT i = 0; i < length; i++)
        { /* convert */
          if (((i % 16) == 0) && (i != 0)) {
            DataLines.Add(str);
            str.Empty();
            lineCount++;
          }
          t.Format(_T("%02X "), data[i]);
          str += t;
        } /* convert */
        DataLines.Add(str);
        lineCount++;
      break;
    }
  }
  TraceData(UINT id, const CString & s) : TraceEvent(id) {
    int iStart = 0;
    lineCount = 0;
    while (iStart != -1) {
      CString sTemp = s.Tokenize("\n", iStart);
      if (iStart != -1) {
        DataLines.Add(sTemp);
        lineCount++;
      }
    }
  }
  virtual ~TraceData() {
    DataLines.RemoveAll(); 
  }
  virtual COLORREF textcolor() { return RGB(255,128,255); /* Pink */}
  virtual CString show(int line);
  virtual CFont * getFont() { return font; }
  static CFont * setFont(CFont * newfont) { CFont * old = font; font = newfont; return old; }
protected:
  static CFont * font;
  CStringArray DataLines;
};


#endif /* _TRACEEVENT_H_ */