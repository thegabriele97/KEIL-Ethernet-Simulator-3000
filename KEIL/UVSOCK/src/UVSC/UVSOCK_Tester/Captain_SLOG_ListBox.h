#if !defined(__CAPTAIN_SLOG_LISTBOX_H__)
#define __CAPTAIN_SLOG_LISTBOX_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CCaptainSLOGBox.h : header file
//

//#include "LSRgnBmpScrollButton.h"
#include <afxtempl.h>


class TraceEvent;
class CPrinter;

/////////////////////////////////////////////////////////////////////////////
// CCaptainSLOGBox window
typedef CMap<long,long,long,long> CMapWordToWord;

class CCaptainSLOGBox : public CListBox
{
  // Construction
public:
  CCaptainSLOGBox();

  // Attributes
public:
  int doingDelayedPaint;

  // Operations
public:
	BOOL doSave(BOOL mode);
	BOOL toNext();
	BOOL toPrev();
	void toTop();
	void toEnd();
	BOOL canNext();
	BOOL canPrev();
	BOOL canTop();
	BOOL canEnd();
	BOOL Copy();
	void SelectAll();
	BOOL canCopy();
	BOOL canClearAll();
	BOOL canSave();
	BOOL canSaveAs();
	BOOL canSelectAll();
	void setBoundary();
  void setLimit(int limit);
	void enableLimit(BOOL enable);
	BOOL setToDisk(BOOL v, const CString & filename);
	BOOL canPrint();
	void Print();
  void SetFont(int nPointSize, CString FaceName);
  void SetColor(COLORREF Color,COLORREF SelColor,COLORREF ShadowColor = RGB(0,0,0));
  BOOL SetTopIndex(int Index, bool redraw = 1);
  int ScrollUp(int Lines=1);
  int ScrollDown(int Lines=1);
  //int AddString(CString Text,DWORD ItemData = 0,BOOL Enabled = TRUE);
  int AddString(TraceEvent * e);
  //int InsertString(int Index,CString Text, DWORD ItemData = 0,BOOL Enabled = TRUE);
  int InsertString(int index, TraceEvent * e);
  int DeleteString(int n);
  virtual void ResetContent(BOOL bInvalidate = TRUE);
  static void CALLBACK ReDraw(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CCaptainSLOGBox)
protected:
  virtual void PreSubclassWindow();
  //}}AFX_VIRTUAL

  // Implementation
public:
  virtual ~CCaptainSLOGBox();

  // Generated message map functions
protected:
  int            m_ItemHeight;
  COLORREF       m_Color;
  COLORREF       m_SelColor;
  COLORREF       m_ShadowColor;
  CFont          m_Font;
  CBitmap        m_Bmp;
  BOOL           m_HasBackGround;
  int            m_ShadowOffset;
  BOOL           m_Shadow;
  long           m_PointSize;   
  int indent1;
  int indent2;
  UINT horizontalExtent;
  int focusIndex;
  CString SaveFileName;
  int limit;
	BOOL limited;
  BOOL toDisk;
  UINT deleted;
	int boundary;
  CPrinter * printer;
  

  void DelayedPaint();
  void RefreshListBox();
  void printItem(CDC & dc, int i);
  BOOL isVisible(int n, BOOL fully);
  int visibleCount();
  void appendToFile(TraceEvent * e);
  void clearAllSelections();
	int findNext(int i);
	int findPrev(int i);
  BOOL sendToClipboard(const CString & s);
  void recomputeHorizontalExtent();
  int setFocusIndex(int focusIdx);
  int getFocusIndex();
  int getItemIndex(int currentIdx, TraceEvent *e);

  // Virtual
  virtual CFont *GetFont();
  virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
  virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
  virtual void DrawItem(CDC &Dc,int Index,CRect &Rect,BOOL Selected);
  virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);


  //{{AFX_MSG(CCaptainSLOGBox)
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg void OnPaint();
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );

  afx_msg BOOL OnLbnSelchange();
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnDestroy();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnMove(int x, int y);
  afx_msg void OnSize(UINT nType, int cx, int cy);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#define UWM_TRACE_CHANGE_MSG _T("UWM_TRACE_CHANGE-{4DCA78F1-DA49-11D3-9FE9-006067718D09}")

#endif // !defined(__CAPTAIN_SLOG_LISTBOX_H__)

