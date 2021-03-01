#include "stdafx.h"
#include "Printer.h"

/****************************************************************************
*                              CPrinter::CPrinter
* Effect: 
*       Constructor
****************************************************************************/

CPrinter::CPrinter(CDC * usedc)
    {
     dc = usedc;
     pageVMargin = 0;
     pageHMargin = 0;
     pageHeight = 0;
     Y = 0;
     lineHeight = 0; 
     pageStarted = FALSE;
     docStarted = FALSE;
     pageNumber = 0;
    } // CPrinter::Printer

/****************************************************************************
*                             CPrinter::~CPrinter
* Effect: 
*       Destructor.
*	If the document is still active, calls the EndPrinting method
****************************************************************************/

CPrinter::~CPrinter()
    {
     if(docStarted)
	EndPrinting();
    } // CPrinter::~Printer

/****************************************************************************
*                           CPrinter::StartPrinting
* Result: BOOL
*       TRUE if printing can continue
*	FALSE if print startup failed
* Effect: 
*       Initiates printing
****************************************************************************/

BOOL CPrinter::StartPrinting()
    {
     DOCINFO info;
     ::ZeroMemory(&info, sizeof(info));
     info.lpszDocName = AfxGetAppName();

     SetPrinterFont();

     dc->StartDoc(&info);
     docStarted = TRUE;

     TEXTMETRIC tm;
     dc->GetTextMetrics(&tm);
     lineHeight  = tm.tmHeight + tm.tmInternalLeading;
     pageVMargin = dc->GetDeviceCaps(LOGPIXELSY) / 2;
     pageHMargin = dc->GetDeviceCaps(LOGPIXELSX) / 2;
     pageHeight  = dc->GetDeviceCaps(VERTRES);
     pageWidth   = dc->GetDeviceCaps(HORZRES);
     Y = pageVMargin;
     return TRUE;
    } // CPrinter::StartPrinting

/****************************************************************************
*                             CPrinter::PrintLine
* Inputs:
*       const CString & s: Line of text to print
* Result: void
*       
* Effect: 
*       Prints the line. Handles page breaks
****************************************************************************/

void CPrinter::PrintLine(const CString & s)
    {
     if(!pageStarted || Y > pageHeight - pageVMargin)
	{ /* new page */
	 if(pageStarted)
	    dc->EndPage();
	 dc->StartPage();
	 pageNumber++;
	 Y = pageVMargin;
	 pageStarted = TRUE;
	 PageHeading();
	} /* new page */
     dc->TextOut(pageHMargin, Y, s);
     Y += lineHeight;
    } // CPrinter::PrintLine

/****************************************************************************
*                            CPrinter::EndPrinting
* Result: void
*       
* Effect: 
*       Terminates printing
****************************************************************************/

void CPrinter::EndPrinting()
    {
     dc->EndPage();
     dc->EndDoc();
     docStarted = FALSE;
    } // CPrinter::EndPrinting

/****************************************************************************
*                          CPrinter::SetPrinterFont
* Result: void
*       
* Effect: 
*       Sets the printer font. Override for other than ANSI_FIXED_FONT
****************************************************************************/

void CPrinter::SetPrinterFont()
    {
     CFont f;
     f.CreateStockObject(ANSI_FIXED_FONT);
     dc->SelectObject(f);
    } // CPrinter::SetPrinterFont

/****************************************************************************
*                            CPrinter::PageHeading
* Result: void
*       
* Effect: 
*       Prints a page heading. This is the default method. Override this
*	in a subclass for fancier printing
****************************************************************************/

void CPrinter::PageHeading()
    {
     CString s(AfxGetAppName());
     int headingY = pageVMargin - (3 * lineHeight) / 2;
     int lineY = pageVMargin - lineHeight / 2;
     int saved = dc->SaveDC();
     dc->SetTextColor(RGB(0,0,0));
     CPen pen(PS_SOLID, dc->GetDeviceCaps(LOGPIXELSX) / 100, RGB(255, 0, 0));
     dc->SelectObject(pen);
     dc->TextOut(pageHMargin, headingY, s);
     s.Format(_T("%d"), pageNumber);
     int w = dc->GetTextExtent(s).cx;
     dc->TextOut(pageWidth - pageHMargin - w, headingY, s);
     dc->MoveTo(pageHMargin, lineY);
     dc->LineTo(pageWidth - pageHMargin, lineY);
     dc->RestoreDC(saved);
    } // CPrinter::PageHeading
