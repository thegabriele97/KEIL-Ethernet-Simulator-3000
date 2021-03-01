// Captain_SLOG_ListBox.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "Captain_SLOG_ListBox.h"
#include "Printer.h"
#include "TraceEvent.h"
//#include "Trace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MIN_LOG_ENTRIES 20

static UINT UWM_TRACE_CHANGE = ::RegisterWindowMessage(UWM_TRACE_CHANGE_MSG);

/////////////////////////////////////////////////////////////////////////////
// CCaptainSLOGBox

CCaptainSLOGBox::CCaptainSLOGBox()
:  m_ItemHeight(20),
   m_HasBackGround(FALSE),
   m_Color(RGB(0,0,0)),
   m_SelColor(RGB(255,0,0)),
   m_ShadowColor(RGB(0,0,0)),
   m_Shadow(FALSE),
   m_ShadowOffset(3)
{
   m_SelColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
   m_Color = GetSysColor(COLOR_WINDOWTEXT);

    toDisk = FALSE;
    horizontalExtent = 0;
    indent1 = 0;
    indent2 = 0;
    limit = 0x7FFFFFFF; // actually, this is ignored if 'limited' is FALSE
    limited = FALSE;
    boundary = 0;
    deleted = 0;
    focusIndex = 0;
    doingDelayedPaint = 0;
}

CCaptainSLOGBox::~CCaptainSLOGBox()
{
}


BEGIN_MESSAGE_MAP(CCaptainSLOGBox, CListBox)
	//{{AFX_MSG_MAP(CCaptainSLOGBox)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
  ON_WM_HSCROLL()
  ON_WM_MOUSEWHEEL()
   ON_CONTROL_REFLECT_EX(LBN_SELCHANGE, OnLbnSelchange)
   ON_WM_KEYDOWN()
   ON_WM_CHAR()
   ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
   ON_WM_MOVE()
   ON_WM_SIZE()
   ON_WM_DESTROY()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCaptainSLOGBox message handlers

void CCaptainSLOGBox::PreSubclassWindow() 
{
	CListBox::PreSubclassWindow();

   ModifyStyle(0,SS_NOTIFY|LBS_NOSEL);

   CWnd *pParent = GetParent();
   if (pParent)
   {
      CRect Rect;
      GetClientRect(&Rect);
      ClientToScreen(&Rect);
      pParent->ScreenToClient(&Rect);
      CDC *pDC = pParent->GetDC();
      m_Bmp.CreateCompatibleBitmap(pDC,Rect.Width(),Rect.Height());
   }
}


void CCaptainSLOGBox::SetFont(int nPointSize, CString FaceName)
{
   //m_Shadow = Shadow;
   //m_ShadowColor = ShadowColor;
   //m_ShadowOffset = SOffset;
   //m_Color = Color;
   //m_SelColor = SelColor;
   m_PointSize = nPointSize;
   CDC *DC = GetDC();
   m_Font.DeleteObject();
   m_Font.CreatePointFont(nPointSize, FaceName);
   if (GetCount())
   {
      CRect Rect;
      Rect.SetRectEmpty();

      CFont *oldFont = DC->SelectObject(&m_Font);
      CString Text;
      GetText(0,Text);
      DC->DrawText(Text,&Rect,DT_CALCRECT|DT_EXPANDTABS|DT_NOPREFIX);
      m_ItemHeight = static_cast<long>(Rect.Height());   
      DC->SelectObject(oldFont);

   }

    // Re-add all objects to the list - windoze only sizes the box
    // of an item when it is added - goooood ooone!
    RefreshListBox();

   ReleaseDC(DC);
  
   // Redraw
   RedrawWindow(0,0,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW);
}

void CCaptainSLOGBox::SetColor(COLORREF Color,COLORREF SelColor,COLORREF ShadowColor)
{
   m_ShadowColor = ShadowColor;
   m_Color = Color;
   m_SelColor = SelColor;
}

void CCaptainSLOGBox::ResetContent(BOOL bInvalidate)
{
  int count = GetCount(); 
  setFocusIndex(0);

  /* GetCount must be re-evaluated after each delete
     because it could have deleted more than one line! */
  while (GetCount() > 1) {
    CCaptainSLOGBox::DeleteString(1);
  }

  recomputeHorizontalExtent();

  GetParent()->SendMessage(UWM_TRACE_CHANGE);
}

void CCaptainSLOGBox::RefreshListBox() {
  int i, count;
  TraceEvent *e;

  indent1 = 0;                  // Force indent recalc
  count = GetCount();
  SetRedraw(FALSE);
  for (i=0; i<count; i++) {
    e = (TraceEvent *)GetItemDataPtr(i);
    CListBox::DeleteString(i);
    CListBox::InsertString(i, (LPCTSTR)e );
  }
  SetRedraw(TRUE);

  recomputeHorizontalExtent();
}

void CCaptainSLOGBox::enableLimit(BOOL enable)
{
  limited = enable;
  BOOL changed = FALSE;
  while(limited && GetCount() > limit - 1)
  { /* trim */
    changed = TRUE;
    CCaptainSLOGBox::DeleteString(0);
  } /* trim */
  if(changed)
  { /* handle change */
    recomputeHorizontalExtent();
    GetParent()->SendMessage(UWM_TRACE_CHANGE);
  } /* handle change */
} // CCaptainSLOGBox::EnableLimit


#define REDRAW_CALLBACK_TIMER_ID    99
#define REDRAW_CALLBACK_TIMEOUT     100
void CALLBACK CCaptainSLOGBox::ReDraw(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
  CCaptainSLOGBox * pThis = (CCaptainSLOGBox *)CCaptainSLOGBox::FromHandle(hwnd);
  
  ::SendMessage(hwnd, WM_SETREDRAW,TRUE,0);
  ::RedrawWindow(hwnd,0,0,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW/*|RDW_INTERNALPAINT*/);
  ::KillTimer(hwnd, idEvent);

  //pThis->recomputeHorizontalExtent();

  pThis->doingDelayedPaint = FALSE;
}

void CCaptainSLOGBox::DelayedPaint() {
  if (!doingDelayedPaint) {    
    doingDelayedPaint = TRUE;
    SetRedraw(FALSE);
    CWnd::SetTimer(REDRAW_CALLBACK_TIMER_ID, REDRAW_CALLBACK_TIMEOUT, ReDraw);
  } 
}

int CCaptainSLOGBox::DeleteString(int n) {
  int retVal = 0;
  // Delete the extended elements
  if ((n < 0) || (n >= GetCount())) return (LB_ERR);

  /* Stop the listbox from drawing, and set a re-draw callback. This is the
     effective way to delete lots of items to a list box with double buffering
     and the rest :) - finally worked this out! Google no help! */
  DelayedPaint();

  TraceEvent *e = (TraceEvent *)GetItemDataPtr(n);
  int lineIndex = getItemIndex(n, e);
  e = (TraceEvent *)GetItemDataPtr(lineIndex);
  int lineCount = e->getLineCount();
  int topIndex = GetTopIndex();             // Remember top index

  // Must delete extended entries first (replace with pad line)
  for (int ext = lineIndex+1; ext < lineIndex+lineCount; ext++) {
    SetRedraw(FALSE);
    e = (TraceEvent *)GetItemDataPtr(ext);
    delete e;
    e = new TraceMessage(TraceEvent::None, "...");
    e->setItemNumber(0);
    CListBox::SetItemDataPtr(ext, (void *) e);
            
  }
  // Delete the original string
  if (retVal != LB_ERR) {
    e = (TraceEvent *)GetItemDataPtr(lineIndex);
    delete e;                   // Here the assigned TraceEvent is deleted
    retVal = CListBox::DeleteString(lineIndex);    
    SetRedraw(FALSE);
    SetTopIndex(topIndex, 0);
  }
  

  return (retVal);
}


void CCaptainSLOGBox::DeleteItem(LPDELETEITEMSTRUCT dis) 
{
  TraceEvent * e = (TraceEvent *)dis->itemData;
  CListBox::DeleteItem(dis);
  //delete e; now done in deletestring because otherwise
  // RefreshListBox cannot work because windoze deletes the
  // items and there is no way of stopping it.
  // On exit, items are deleted by OnDestroy()

  // Note that we must use PostMessage here, because the item
  // would not be deleted until this method returns. This
  // means that if we use SendMessage, we might try to use
  // a pointer to deleted memory
  //GetParent()->PostMessage(UWM_TRACE_CHANGE);
}

void CCaptainSLOGBox::OnDestroy(){
  int i, count;
  TraceEvent *e;

  CListBox::OnDestroy();

  count = GetCount();
  for (i=0; i<count; i++) {
    e = (TraceEvent *)GetItemDataPtr(i);
    delete e;
  }
}


int CCaptainSLOGBox::AddString(TraceEvent * e)
{
  int count;
  static bool bAddingExtendedItems = FALSE;
  BOOL changed = FALSE;

  /* TODO: NEED multithread protection here as it adds more that
     one item. Otherwise they may get mixed up between threads */

  // During shutdown transient, the window may actually be gone
  if(!::IsWindow(m_hWnd)){ 
    delete e; // already logged to file if file logging on
    return LB_ERR;
  } 

  /* Stop the listbox from drawing, and set a re-draw callback. This is the
     effective way to add lots of items to a list box with double buffering
     and the rest :) - finally worked this out! Google no help! */
  DelayedPaint();
  

  // If we've exceeded the number of lines of trace that are set,
  // delete enough leading lines to get below the limit
  // Line Limit
  int lineCount = e->getLineCount();
  count = GetCount();
  int top = GetTopIndex();
  changed = FALSE;
  BOOL first = TRUE;
  while(limited && count > limit - 1) { 
    //if(first)
    //  CCaptainSLOGBox::DeleteString(boundary + 1);
    //first = FALSE;
    CCaptainSLOGBox::DeleteString(boundary + 1);
    deleted++;
    changed = TRUE;
    count = GetCount();
  } 

  //if(!first) {
  //  InsertString(boundary + 1, new TraceFormatComment(TraceEvent::None, IDS_TRACELOG_DELETED_ENTRIES, deleted));
  //}

  // We want to auto-scroll the list box if the last line is showing,
  // even if partially  
  BOOL visible = FALSE;
  CRect r;
  count = GetCount();  
  if(count > 0){
    visible = isVisible(count - lineCount, FALSE); // any part showing?
  } 
  else {
    visible = TRUE;
  }

  // Add the element to the list box
  int n = CListBox::AddString((LPCTSTR) e); 
  e->setItemNumber(0);                              // Set the items line index
  if (toDisk) appendToFile(e);                      // Write Item to file if enabled

  // Create and add the extended elements  
  for (int ext = 1; ext < lineCount; ext++) {
    TraceExtended *extTrace = new TraceExtended(e, e->getID(), e->show(ext));
    extTrace->setItemNumber(ext);
    n = CListBox::AddString((LPCTSTR) extTrace );
    if (toDisk) appendToFile(extTrace);  // Write Item to file if enabled
  }

  // If the last line had been visible, and the just-added line is
  // not visible, scroll the list box up to make it visible.

  // Note: This depends on the box being LBS_OWNERDRAWFIXED.  The
  // computation for variable is much more complex!
  if(visible) {
    visible = isVisible(n, TRUE); // not all of it showing?
    if(!visible) {
      int top = GetTopIndex();
      if(top + lineCount >= GetCount()) {
        // Not enough items to scroll
      }
      else { 
        SetTopIndex(top + lineCount + 1, 0);
      } 
    } 
  }
  

  // Get the width and compute the scroll width
  // Optimization: if the current width is less than the current
  // maximum, we don't need to do anything
  CClientDC dc(this);
  CString s = e->show(0);
  UINT w = indent1 + indent2 + dc.GetTextExtent(s, s.GetLength()).cx;
  if(w > horizontalExtent) { /* recompute */
    horizontalExtent = w;
    SetHorizontalExtent(horizontalExtent);
  } 
  else {
    // Moved horizontal extent recalc to delayed redraw because it is EXPENSIVE!    
  }

  // Notification, notify the parent to update the controls (enabled / disabled)
  GetParent()->SendMessage(UWM_TRACE_CHANGE);
  return n;
}

// This is only a basic implementation. Should be more like AddString
// but this is ok for adding the delete items message
int CCaptainSLOGBox::InsertString(int index, TraceEvent * e) { 
  int retVal;
  int count, topIndex;

  count = GetCount();

  // Check if this is really an add
  if (index >= count) {
    return AddString(e);
  }

  /* Stop the listbox from drawing, and set a re-draw callback. This is the
     effective way to insert lots of items to a list box with double buffering
     and the rest :) - finally worked this out! Google no help! */
  DelayedPaint();

  topIndex = GetTopIndex();             // Remember top index
  e->setItemNumber(0);                  // Set the new items line index
  retVal = CListBox::InsertString(index, (LPCTSTR)e); 
  SetTopIndex(topIndex, 0);

  return retVal;
}

BOOL CCaptainSLOGBox::isVisible(int n, BOOL fully)
{
  CRect item;
  CRect r;
  GetItemRect(n, &item);
  GetClientRect(&r);

  if(!fully && (item.top < r.bottom) && (item.bottom > r.top))
    return TRUE;
  if(fully && (item.bottom < r.bottom) /*&& (item.bottom < r.bottom) - not needed*/)
    return TRUE;

  return FALSE;
}

int CCaptainSLOGBox::visibleCount()
{
  int count;
  CRect item;
  CRect r;

  if (GetCount() > 0){
    GetItemRect(0, &item);
    GetClientRect(&r);
    count = ((r.bottom - r.top) / (item.bottom - item.top));
  }
  else {
    count = 0;
  }

  return (count);
}

void CCaptainSLOGBox::recomputeHorizontalExtent()
{
  horizontalExtent = 0;
  CClientDC dc(this);
  CFont *oldFont = dc.SelectObject(&m_Font);
  for(int i = 0; i < CListBox::GetCount(); i++)
  { /* scan */
    TraceEvent * e = (TraceEvent *)GetItemDataPtr(i);
    if(e == NULL)
      continue;
    CString s = e->show(0);
    UINT w = dc.GetTextExtent(s).cx;
    w += indent1 + indent2;
    if(w > horizontalExtent)
      horizontalExtent = w;
  } /* scan */
  dc.SelectObject(oldFont);

  SetHorizontalExtent(horizontalExtent);
}


BOOL CCaptainSLOGBox::OnEraseBkgnd(CDC* pDC) 
{
   return FALSE;
}

CFont *CCaptainSLOGBox::GetFont()
{
   if (m_Font.m_hObject == NULL)
   {
      return CListBox::GetFont();
   }
   return &m_Font;
}

int CCaptainSLOGBox::getItemIndex(int currentIdx, TraceEvent *e) {
  return currentIdx - e->getItemNumber();
}

int CCaptainSLOGBox::setFocusIndex(int focusIdx) {
  int retVal = -1;  // Errorcode
  TraceEvent *e;
  int count = GetCount();

  if ((focusIdx >= 0) && (focusIdx < count)) {
    e = (TraceEvent *)GetItemDataPtr(focusIdx);
    focusIndex = focusIdx - e->getItemNumber();
    CListBox::SetCurSel(focusIndex);
    retVal = focusIndex;
    GetParent()->SendMessage(UWM_TRACE_CHANGE);
  }
  
  return (retVal);
}

// Safe way of getting focus index in case its item was deleted
int CCaptainSLOGBox::getFocusIndex() {
  TraceEvent *e;
  int count = GetCount();

  if (count == 0) return LB_ERR;

  if (focusIndex < 0) focusIndex = 0;
  else if (focusIndex >= count) focusIndex = count-1;

  e = (TraceEvent *)GetItemDataPtr(focusIndex);
  focusIndex = focusIndex - e->getItemNumber();
  
  return (focusIndex);
}

void CCaptainSLOGBox::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
  LPDRAWITEMSTRUCT d = lpDrawItemStruct;
  if(d->itemID == -1) return;

  // Get the actual event data associated with this item
  TraceEvent *e = (TraceEvent *)d->itemData;

  if (d->itemAction & ODA_SELECT) { // We manage the actual selection state internally
    d->itemState &=~ ODS_SELECTED;
    e->setSelected(!e->getSelected());     // Toggle sel state of message

    /* If the item has changed select state, it should be the new focus item */
    //eOldFocus = (TraceEvent *)GetItemDataPtr(focusIndex);
    // Access focus index directly here so it doesn't redraw
    focusIndex = getItemIndex(d->itemID, e);    
    // We are done - this item was actually drawn in the redraw loop
  }
}


void CCaptainSLOGBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
   CDC *DC = GetDC();
   CRect Rect;
   Rect.SetRectEmpty();

   CFont *oldFont = DC->SelectObject(GetFont());
   DC->DrawText("XYZ",&Rect,DT_CALCRECT|DT_EXPANDTABS|DT_NOPREFIX);
   m_ItemHeight = lpMeasureItemStruct->itemHeight = static_cast<long>(Rect.Height());

   if(indent1 == 0) { 
      static const CString prefix1 = _T("00000000000");
      indent1 = DC->GetTextExtent(prefix1, prefix1.GetLength()).cx;
      static const CString prefix2 = _T("000000000");
      indent2 = DC->GetTextExtent(prefix2, prefix2.GetLength()).cx;
   }


   DC->SelectObject(oldFont);
   ReleaseDC(DC);
}


void CCaptainSLOGBox::DrawItem(CDC &Dc,int Index,CRect &Rect,BOOL Selected)
{
  int x, itemIndex, focusIdx;
  CFont * textfont = NULL;
  COLORREF txcolor;
  COLORREF bkcolor;
  TraceEvent *e = (TraceEvent *)GetItemDataPtr(Index);

   if (Index == LB_ERR || Index >= GetCount() || e == NULL)
      return;

   CRect itemRect = Rect;
   Dc.SetBkMode(TRANSPARENT);

//   CDC memdc;
//   memdc.CreateCompatibleDC(&Dc);


//   CBitmap *oldbmp = memdc.SelectObject(&m_Bmp);
//   Dc.BitBlt(TheRect.left,TheRect.top,TheRect.Width(),TheRect.Height(),&memdc,TheRect.left,TheRect.top,SRCCOPY);

   // Set the font to draw in
   textfont = GetFont();
   if (textfont != NULL) {
      Dc.SelectObject(textfont);
   }

   // Choose colours
   itemIndex = getItemIndex(Index, e);
   focusIdx = getFocusIndex();
   if (focusIdx == itemIndex) {
      bkcolor = RGB(0,0,128);//::GetSysColor(COLOR_HIGHLIGHT);
      txcolor = RGB(255,255,0);//::GetSysColor(COLOR_HIGHLIGHTTEXT);
   }
   else { // Normal colours
      txcolor = e->textcolor();
      bkcolor = RGB(0,0,0);
   }

 // Draw background - optimisation - don't bother unless selected!
   if (focusIdx == itemIndex){
      CBrush bg(bkcolor);
      Dc.FillRect(itemRect, &bg);
    } 

  // Set the text colour
  Dc.SetTextColor(txcolor);
  Dc.SetBkMode(TRANSPARENT);

  CString seq = e->showSeq();
  Dc.TextOut(itemRect.left /*+ indent1 - width*/, itemRect.top, seq);

  //CString id = e->showID();
  //Dc.TextOut(itemRect.left /*+ indent1 - width*/, itemRect.top, id);

  //CString t = e->showThread();
  //Dc.TextOut(itemRect.left + indent1, itemRect.top, t);

  CString s = e->show(0);
  Dc.TextOut(itemRect.left + indent1/* + indent2*/, itemRect.top, s);
  //Dc.DrawText(Text,itemRect,DT_LEFT|DT_EXPANDTABS|DT_NOPREFIX);

  Dc.SelectStockObject(WHITE_PEN);

  // Draw vertical lines
  x = itemRect.left + indent1 - 18;
  Dc.MoveTo(x, itemRect.top);
  Dc.LineTo(x, itemRect.bottom);
  x = itemRect.left + indent1 + indent2 - 10;
  //Dc.MoveTo(x, itemRect.top);
  //Dc.LineTo(x, itemRect.bottom);
}


void CCaptainSLOGBox::OnPaint() 
{
  CRect Rect;

  CPaintDC dc(this); // device context for painting
  
  GetClientRect(&Rect);
  int Width = Rect.Width(); 
  int Height = Rect.Height();

  CDC MemDC;
  MemDC.CreateCompatibleDC(&dc);   
  CBitmap MemBmp;
  MemBmp.CreateCompatibleBitmap(&dc,Width,Height); 
  CBitmap *pOldMemBmp = MemDC.SelectObject(&MemBmp);
   
  //CBrush bg(RGB(0,0,0));
  //MemDC.FillRect(Rect, &bg);

  Rect.top = 0;
  int VPos = GetScrollPos(SB_HORZ);
  Rect.left = -VPos;
  Rect.bottom = Rect.top + m_ItemHeight;//GetItemHeight(0);
  Rect.right = Width;
   
  int size = GetCount();
  for (int i = GetTopIndex(); i < size && Rect.top <= Height;++i)
  {
    DrawItem(MemDC,i,Rect,GetSel(i));
    Rect.OffsetRect(0,m_ItemHeight/*GetItemHeight(i)*/);
  }

  dc.BitBlt(0,0,Width,Height,&MemDC,0,0,SRCCOPY);
  
  MemDC.SelectObject(pOldMemBmp);
}






/****************************************************************************
*                              TraceList::doSave
* Inputs:
*       BOOL mode: TRUE for Save, FALSE for Save As
* Result: BOOL
*       TRUE if save occurred
*	FALSE if save was cancelled
* Effect: 
*       Does a save
****************************************************************************/

BOOL CCaptainSLOGBox::doSave(BOOL mode)
{
  BOOL prompt = FALSE;

  if(mode) {
    if(SaveFileName.GetLength() == 0) { 
      prompt = TRUE;
    } 
  }
  else { 
    prompt = TRUE;
  }

  CString name;

  if(prompt) { /* get file name */
    CString filter;
    filter.LoadString(IDS_TRACELOG_FILTER);
    CFileDialog dlg(FALSE, _T(".log"), NULL,
      OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
      filter);

    CString s;
    s.LoadString(mode ? IDS_TRACELOG_FILE_SAVE : IDS_TRACELOG_FILE_SAVE_AS);
    dlg.m_ofn.lpstrTitle = s;

    switch(dlg.DoModal()) { /* domodal */
      case 0: // should never happen
      case IDCANCEL:
        return FALSE;
    }

    // We get here, it must be IDOK

    name = dlg.GetPathName();
  }
  else { /* already have name */
    name = SaveFileName;
  }

  TRY
  {
    CStdioFile f(name, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive);
    for(int i = 0; i < GetCount(); i++) { /* write each */
      TraceEvent * e = (TraceEvent *)GetItemDataPtr(i);
      f.WriteString(e->showfile());

      f.WriteString(_T("\n"));
    }

    f.Close();

    SaveFileName = name;

    // Tell user file was saved OK
    if (prompt) {
      AddString(new TraceFormatComment(TraceEvent::None, 
                                       "Log saved as [%s]\n"
                                       "All new entries will be appended to this file :)", 
                                       SaveFileName));
    }

    return TRUE;
  }
  CATCH(CFileException, e) { /* error */
    DWORD err = ::GetLastError();
    //AfxMessageBox(IDS_TRACELOG_FILE_OPEN_FAILED, MB_ICONERROR | MB_OK);
    AddString(new TraceFormatError(TraceEvent::None, IDS_TRACELOG_FILE_OPEN_FAILED, name));
    AddString(new TraceFormatMessage(TraceEvent::None, err));
    return FALSE; // return without doing anything further
  }
  END_CATCH
    ASSERT(FALSE); // should never get here
  return FALSE;
}



/****************************************************************************
*                          CCaptainSLOGBox::appendToFile
* Inputs:
*       TraceEvent * event:
* Result: void
*       
* Effect: 
*       Logs the event to the current file
****************************************************************************/

void CCaptainSLOGBox::appendToFile(TraceEvent * e)
{
  if(SaveFileName.GetLength() == 0)
    return; // shouldn't be here

  TRY {
    CStdioFile f(SaveFileName, CFile::modeReadWrite | CFile::modeCreate |
      CFile::modeNoTruncate | CFile::shareExclusive);


    f.SeekToEnd();
    f.WriteString(e->showfile());
    f.WriteString(_T("\n"));
    f.Close();
  }
  CATCH(CFileException, e) 
  { 
    // Do nothing.
  }
  END_CATCH
}

/****************************************************************************
*                              CCaptainSLOGBox::toTop
* Result: void
*       
* Effect: 
*       Positions the list box to the top
****************************************************************************/

void CCaptainSLOGBox::toTop()
{
  if(GetCount() == 0)
    return;

  //e = (TraceEvent *)GetItemDataPtr(0);
  setFocusIndex(0);
  SetTopIndex(0);
  
  RedrawWindow(0,0,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW);
  //GetParent()->SendMessage(UWM_TRACE_CHANGE);
}

/****************************************************************************
*                              CCaptainSLOGBox::toEnd
* Result: void
*       
* Effect: 
*       Moves to the end of the box
****************************************************************************/

void CCaptainSLOGBox::toEnd()
{
  TraceEvent *e;

  int n = GetCount();
  if(n == 0)
    return;

  e = (TraceEvent *)GetItemDataPtr(n-1);
  setFocusIndex(getItemIndex(n-1, e));

  SetTopIndex(n - 1);
  
  RedrawWindow(0,0,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW);
  //GetParent()->SendMessage(UWM_TRACE_CHANGE);
}

/****************************************************************************
*                              CCaptainSLOGBox::canTop
* Result: BOOL
*       TRUE if toTop has meaning
****************************************************************************/

BOOL CCaptainSLOGBox::canTop()
{
  int count = GetCount();

  if(count < 2)
    return FALSE;

  if(getFocusIndex() == 0)
    return FALSE;
  return TRUE;  
}

/****************************************************************************
*                              CCaptainSLOGBox::canEnd
* Result: BOOL
*       TRUE if toEnd has meaning
****************************************************************************/

BOOL CCaptainSLOGBox::canEnd()
{
  int focusIdx = getFocusIndex();
  int count = GetCount();

  if((count < 2) || focusIdx == LB_ERR)
    return FALSE;

  TraceEvent *e = (TraceEvent *)GetItemDataPtr(focusIdx);

  if((focusIdx + e->getItemCount()) >= (UINT)GetCount())
    return FALSE;
  return TRUE;     
}

/****************************************************************************
*                            CCaptainSLOGBox::findNext
* Inputs:
*       int i: Starting position
* Result: int
*       Next error position, or LB_ERR if not found
****************************************************************************/

int CCaptainSLOGBox::findNext(int i)
{
  int count = GetCount();

  for(++i ; i < count; i++)
  { /* scan forward */
    TraceEvent * e = (TraceEvent *)GetItemDataPtr(i);
    if(e->IsKindOf(RUNTIME_CLASS(TraceError)) || e->IsKindOf(RUNTIME_CLASS(TraceWarning)))
    { /* found it */
      return i;
    } /* found it */
  } /* scan forward */
  return LB_ERR;
}

/****************************************************************************
*                            CCaptainSLOGBox::findPrev
* Inputs:
*       int i: Starting position
* Result: int
*       Previous error position, or LB_ERR if not found
****************************************************************************/

int CCaptainSLOGBox::findPrev(int i)
{
  int count = GetCount();
  if(count == 0)
    return LB_ERR;

  for(--i ; i >= 0; i--)
  { /* scan backward */
    TraceEvent * e = (TraceEvent *)GetItemDataPtr(i);
    if(e->IsKindOf(RUNTIME_CLASS(TraceError)) || e->IsKindOf(RUNTIME_CLASS(TraceWarning)))
    { /* found it */
      return i;
    } /* found it */
  } /* scan backward */
  return LB_ERR;
}

/****************************************************************************
*                             CCaptainSLOGBox::toNext
* Result: BOOL

*	FALSE if we didn't move
* Effect: 
*       Sets the selection to the next error
****************************************************************************/

BOOL CCaptainSLOGBox::toNext()
{
  TraceEvent *e;
  int itemCount, scrollIndex;
  int start = getFocusIndex();
  int bottomIndex = GetCount()-1;

  int i = findNext(start);
  if(i == LB_ERR)
    return FALSE;

  setFocusIndex(i);
  e = (TraceEvent *)GetItemDataPtr(getFocusIndex());
  itemCount = e->getItemCount();
  scrollIndex = (getFocusIndex() + itemCount-1) + 3 /* possible offset*/;
  if (scrollIndex > bottomIndex) scrollIndex = bottomIndex;
  if (!isVisible(scrollIndex, FALSE)) {
    SetTopIndex(scrollIndex - visibleCount() );
  }
  else {
    RedrawWindow(0,0,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW);
  }

  //GetParent()->SendMessage(UWM_TRACE_CHANGE);
  return TRUE;
}

/****************************************************************************
*                             CCaptainSLOGBox::toPrev
* Result: BOOL
*       TRUE if we moved
*	FALSE if we didn't move
* Effect: 
*       Sets the selection to the previous error
****************************************************************************/

BOOL CCaptainSLOGBox::toPrev() {
  int scrollIndex;
  int start = getFocusIndex();

  int i = findPrev(start);
  if(i == LB_ERR)
    return FALSE;

  setFocusIndex(i);

  scrollIndex = getFocusIndex() - 3 /* possible offset*/;
  if (scrollIndex < 0) scrollIndex = 0;
  if (!isVisible(scrollIndex, FALSE)) {
    SetTopIndex(scrollIndex);
  }
  else {
    RedrawWindow(0,0,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW);
  }

  //GetParent()->SendMessage(UWM_TRACE_CHANGE);
  return TRUE;
}

/****************************************************************************
*                             CCaptainSLOGBox::canPrev
* Result: BOOL
*       TRUE if there is a previous error
****************************************************************************/

BOOL CCaptainSLOGBox::canPrev()
{
  return findPrev(getFocusIndex()) != LB_ERR;
}

/****************************************************************************
*                             CCaptainSLOGBox::canNext
* Result: BOOL
*       TRUE if there is a next error
****************************************************************************/

BOOL CCaptainSLOGBox::canNext()
{
  return findNext(getFocusIndex()) != LB_ERR;
}

/****************************************************************************
*                             CCaptainSLOGBox::canCopy
* Result: BOOL
*       TRUE if Copy operation can be done
*	FALSE if nothing to copy
****************************************************************************/

BOOL CCaptainSLOGBox::canCopy() {
  return GetCount() > 1;
} 


/****************************************************************************
*                           CCaptainSLOGBox::canClearAll
* Result: BOOL
*       TRUE if ClearAll makes sense
****************************************************************************/

BOOL CCaptainSLOGBox::canClearAll()
{
  return GetCount() > 1;
}


/****************************************************************************
*                              CCaptainSLOGBox::Copy
* Result: BOOL
*       TRUE if copy succeeded
*	FALSE if copy failed
* Effect: 
*       Copies selected items to the clipboard
****************************************************************************/

BOOL CCaptainSLOGBox::Copy()
{
  CString s;
  TraceEvent * e;
  int count = CListBox::GetCount();
  for(int i = 0; i < count; i++)
  { 
    e = (TraceEvent *)CListBox::GetItemDataPtr(i);
    //if (e->getSelected()) {
      s += e->showfile();
      s += "\n";
    //}
  }

  return sendToClipboard(s);
}

/****************************************************************************
*                       CCaptainSLOGBox::clearAllSelections
* Result: void
*       
* Effect: 
*       Clears all the selections in the list box
****************************************************************************/

void CCaptainSLOGBox::clearAllSelections()
{
  TraceEvent * e;
  int count = CListBox::GetCount();
  for(int i = 0; i < count; i++)
  { 
    e = (TraceEvent *)CListBox::GetItemDataPtr(i);
    if (e->getSelected()) {
      e->setSelected(false);
      SetSel(i, TRUE);  // Force redraw
      SetSel(i, FALSE);
    }
  } 
}





void CCaptainSLOGBox::setBoundary()
{
  boundary = AddString(new TraceMessage(TraceEvent::None,
    CString("================================================================")));
  if(limit > 0 && limit < boundary + MIN_LOG_ENTRIES)
    limit = boundary + MIN_LOG_ENTRIES;
} // CCaptainSLOGBox::setBoundary

/****************************************************************************
*                             CCaptainSLOGBox::canSave
* Result: BOOL
*       TRUE if modified and nonempty
****************************************************************************/

BOOL CCaptainSLOGBox::canSave() {
  return CListBox::GetCount() > 1;
} 

/****************************************************************************
*                            CCaptainSLOGBox::canSaveAs
* Result: BOOL
*       TRUE if a SaveAs is valid
****************************************************************************/

BOOL CCaptainSLOGBox::canSaveAs() {
  return CListBox::GetCount() > 1;
}


void CCaptainSLOGBox::setLimit(int limit)
{
  if(limit < 0)
  { /* disable */
    limited = FALSE;
    return;
  } /* disable */

  if(limit < MIN_LOG_ENTRIES) 
    limit = MIN_LOG_ENTRIES;
  if(limit < boundary)
    limit = boundary + MIN_LOG_ENTRIES;
  this->limit = limit; 
  limited = TRUE;     
}

/****************************************************************************
*                            CTraceList::setToDisk
* Inputs:
*       BOOL v: Value to set
* Result: BOOL
*       TRUE if successful, FALSE if failed to open
* Effect: 
*       If not already set, requests a file to log to
* Notes:
*	Case	toDisk		v	Effect
*	I	FALSE		FALSE	ignored
*	II	TRUE		FALSE	toDisk = FALSE
*	III	TRUE		TRUE	ignored
*	IV	FALSE		TRUE	prompt for name
****************************************************************************/

BOOL CCaptainSLOGBox::setToDisk(BOOL v, const CString & filename)
{
  if(!toDisk && !v){ 
    return FALSE; // ignore false-to-false
  }

  //if(toDisk) {
  //  toDisk = v;
  //  return TRUE;
  //}

  // Must specify a filename
  if(filename.GetLength() == 0) {
    return FALSE;
  }

  SaveFileName = filename;
  toDisk = doSave(TRUE); // use current filename and save current state

  if (!toDisk) {
    AddString(new TraceWarning(TraceEvent::None, "Default logging disabled"));
  }
  else {
    AddString(new TraceFormatComment(TraceEvent::None, "Default logging enabled [%s]", filename));
  }

  return toDisk;
}

/****************************************************************************
*                         CCaptainSLOGBox::sendToClipboard
* Inputs:
*       const CString & s: String reference
* Result: BOOL
*       
* Effect: 
*       Puts the string on the clipboard
****************************************************************************/

BOOL CCaptainSLOGBox::sendToClipboard(const CString & s)
{
  HGLOBAL mem = ::GlobalAlloc(GMEM_MOVEABLE, s.GetLength() + 1);
  if(mem == NULL)
    return FALSE;
  LPTSTR p = (LPTSTR)::GlobalLock(mem);
  lstrcpy(p, (LPCTSTR)s);
  ::GlobalUnlock(mem);

  if(!OpenClipboard())
  { /* failed */
    ::GlobalFree(mem);
    return FALSE;
  } /* failed */
  EmptyClipboard();
  SetClipboardData(CF_TEXT, mem);
  CloseClipboard();
  return TRUE;
} // CCaptainSLOGBox::sendToClipboard

/****************************************************************************
*                            CCaptainSLOGBox::canPrint
* Result: BOOL
*       TRUE if printing is possible
*	FALSE if nothing to print
****************************************************************************/

BOOL CCaptainSLOGBox::canPrint()
{
  return GetCount() > 1;
} // CCaptainSLOGBox::canPrint

/****************************************************************************
*                            CCaptainSLOGBox::printItem
* Inputs:
*       int i: Index of item to print
* Result: void
*       
* Effect: 
*       Prints the item. Handles page breaks, etc.
****************************************************************************/

void CCaptainSLOGBox::printItem(CDC & dc, int i)
{
  CFont * textfont = NULL;
  int saved = dc.SaveDC();
  TraceEvent * e = (TraceEvent *)GetItemDataPtr(i);
  CString s = e->showfile();

  dc.SetTextColor(e->textcolor());    // Set colour

  textfont = e->getFont();            // Set font
  if(textfont != NULL)
    dc.SelectObject(textfont);

  printer->PrintLine(s);
  dc.RestoreDC(saved);
} // CCaptainSLOGBox::printItem

/****************************************************************************
*                              CCaptainSLOGBox::Print
* Result: void
*       
* Effect: 
*       Prints the contents of the box. 
****************************************************************************/

void CCaptainSLOGBox::Print()
{
  TraceEvent * e;

  CPrintDialog dlg(FALSE,
    PD_ALLPAGES |
    PD_HIDEPRINTTOFILE |
    PD_NOPAGENUMS |
    PD_RETURNDC |
    PD_USEDEVMODECOPIES);
  if(GetSelCount() < 2)
  { /* enable selection */
    dlg.m_pd.Flags |= PD_NOSELECTION;
  } /* enable selection */
  if(GetSelCount() > 10) // randomly-chosen number
  { /* use selection */
    dlg.m_pd.Flags |= PD_SELECTION;
  } /* use selection */
  switch(dlg.DoModal())
  { /* DoModal */
  case 0:
  case IDCANCEL:
    return;
  case IDOK:
    break;
  default:
    ASSERT(FALSE); // impossible condition
    return;
  } /* DoModal */

  CDC dc;
  dc.Attach(dlg.m_pd.hDC);
  printer = new CPrinter(&dc);
  if(printer->StartPrinting())
  { /* success */
    int count = GetCount();

    if(dlg.m_pd.Flags & PD_SELECTION)
    { /* use selection */
      for(int i = 0; i < count; i++)
      { 
        e = (TraceEvent *)CListBox::GetItemDataPtr(i);
        if (e->getSelected()) {
          printItem(dc, i);
        }
      } 
#if 0
      int * selections = new int[count];
      GetSelItems(count, selections);
      for(int i = 0; i < count; i++)
      { /* print each */
        printItem(dc, selections[i]);
      } /* print each */
      delete [] selections;
#endif
    } /* use selection */
    else
    { /* print all */
      for(int i = 0; i < count; i++)
      { /* print line */
        printItem(dc, i);
      } /* print line */

      printer->EndPrinting();
    } /* print all */
  } /* success */
  delete printer;
  ::DeleteDC(dc.Detach());
} // CCaptainSLOGBox::Print

/****************************************************************************
*                          CCaptainSLOGBox::canSelectAll
* Result: BOOL
*       TRUE if anything to select
****************************************************************************/

BOOL CCaptainSLOGBox::canSelectAll()
{
  return GetCount() > 0;
} // CCaptainSLOGBox::canSelectAll

/****************************************************************************
*                            CCaptainSLOGBox::SelectAll
* Result: void
*       
* Effect: 
*       Selects all items
****************************************************************************/

void CCaptainSLOGBox::SelectAll()
{
  SetRedraw(FALSE);
  for(int i = 0; i < GetCount(); i++)
    SetSel(i, TRUE);
  SetRedraw(TRUE);
} // CCaptainSLOGBox::SelectAll








void CCaptainSLOGBox::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
  DelayedPaint();
   //SetRedraw(FALSE);
   CListBox::OnVScroll(nSBCode,nPos,pScrollBar);
   //SetRedraw(TRUE);

   //RedrawWindow(0,0,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW);
}

#include <crtdbg.h>
void CCaptainSLOGBox::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
  SetRedraw(FALSE);
  CListBox::OnHScroll(nSBCode,nPos,pScrollBar);
  SetRedraw(TRUE);
  RedrawWindow(0,0,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW);
}

BOOL CCaptainSLOGBox::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt ) {
  short i;
  short count = 5;  // Scroll 5 lines at a time
  
  //this->SendMessage(WM_VSCROLL, SB_THUMBPOSITION, 90);
	if (zDelta > 0) {
    for (i=0; i<count; i++) { 
      this->SendMessage(WM_VSCROLL, SB_LINEUP, NULL);
    }
	}
	else {
		for (i=0; i<count; i++) { 
      this->SendMessage(WM_VSCROLL, SB_LINEDOWN, NULL);
    }
	}

  return TRUE;
}

BOOL CCaptainSLOGBox::OnLbnSelchange()
{
   GetParent()->SendMessage(UWM_TRACE_CHANGE);
   RedrawWindow(0,0,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW);
   return FALSE;
}

int CCaptainSLOGBox::SetTopIndex(int Index, bool redraw)
{
   int Ret;

   if (redraw) {
    SetRedraw(FALSE);
    Ret = CListBox::SetTopIndex(Index);
    SetRedraw(TRUE);
    RedrawWindow(0,0,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW);
   }
   else {
     Ret = CListBox::SetTopIndex(Index);
   }
   return Ret;
}

int CCaptainSLOGBox::ScrollUp(int Lines)
{
  int topIndex = GetTopIndex();
  int i, currentIndex, scrollIndex;

  for (i = 0; i<Lines; i++) {    
    currentIndex = getFocusIndex();
    if (currentIndex == 0) return LB_ERR; // Already at top
    currentIndex--;    
    setFocusIndex(currentIndex);
  }
  scrollIndex = getFocusIndex() - 3 /* possible offset*/;
  if (scrollIndex < 0) scrollIndex = 0;
  if (!isVisible(scrollIndex, FALSE)) {
    SetTopIndex(scrollIndex);
  }
  else {
    RedrawWindow(0,0,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW);
  }
  return (0);
}

int CCaptainSLOGBox::ScrollDown(int Lines)
{
  int topIndex = GetTopIndex();
  int bottomIndex = GetCount()-1;
  int i, currentIndex, itemCount, scrollIndex;
  TraceEvent *e;

  for (i = 0; i<Lines; i++) {
    e = (TraceEvent *)GetItemDataPtr(getFocusIndex());     
    currentIndex = getFocusIndex();
    itemCount = e->getItemCount();
    if (currentIndex + itemCount-1 >= bottomIndex) return LB_ERR; // Already at bottom
    currentIndex += itemCount;    
    setFocusIndex(currentIndex);
  }
  itemCount = e->getItemCount();
  scrollIndex = (getFocusIndex() + itemCount-1) + 3 /* possible offset*/;
  if (scrollIndex > bottomIndex) scrollIndex = bottomIndex;
  if (!isVisible(scrollIndex, FALSE)) {
    SetTopIndex(scrollIndex - visibleCount() );
  }
  else {
    RedrawWindow(0,0,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW);
  }

  return (0);
}

void CCaptainSLOGBox::OnRButtonUp(UINT nFlags, CPoint point) {
}

void CCaptainSLOGBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  //SetRedraw(FALSE);
  //CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
  //SetRedraw(TRUE);

  switch (nChar) {
    case VK_PRIOR:
      this->SendMessage(WM_VSCROLL, SB_PAGEUP, NULL);
    break;
    case VK_NEXT:
      this->SendMessage(WM_VSCROLL, SB_PAGEDOWN, NULL);
    break;
    case VK_END:
      toEnd();
    break;
    case VK_HOME:
      toTop();
    break;
    case VK_LEFT:
      this->SendMessage(WM_HSCROLL, SB_LINELEFT, NULL);
    break;
    case VK_UP:
      ScrollUp(1);
    break;
    case VK_RIGHT:
      this->SendMessage(WM_HSCROLL, SB_LINERIGHT, NULL);
    break;
    case VK_DOWN:
        ScrollDown(1);
    break;
    default:
      ; // Do Nothing
  }

   
}


void CCaptainSLOGBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   SetRedraw(FALSE);
   CListBox::OnChar(nChar, nRepCnt, nFlags);
   SetRedraw(TRUE);
}

void CCaptainSLOGBox::OnMove(int x, int y)
{
   CListBox::OnMove(x, y);

   ShowWindow(SW_HIDE);
   m_HasBackGround = FALSE;
   Invalidate();
   UpdateWindow();
   ShowWindow(SW_SHOW);
   
}

void CCaptainSLOGBox::OnSize(UINT nType, int cx, int cy)
{
   CListBox::OnSize(nType, cx, cy);

#if 0
   CWnd *pParent = GetParent();
   if (pParent)
   {
      m_Bmp.DeleteObject();
      CRect Rect;
      GetClientRect(&Rect);
      ClientToScreen(&Rect);
      pParent->ScreenToClient(&Rect);
      CDC *pDC = pParent->GetDC();
      m_Bmp.CreateCompatibleBitmap(pDC,Rect.Width(),Rect.Height());
   }
   ShowWindow(SW_HIDE);
   m_HasBackGround = FALSE;
   Invalidate();
   UpdateWindow();
   ShowWindow(SW_SHOW);
#endif
}
