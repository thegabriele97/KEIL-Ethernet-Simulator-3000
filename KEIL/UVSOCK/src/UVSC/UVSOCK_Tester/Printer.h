class CPrinter {
    public:
       CPrinter(CDC * dc);
       virtual ~CPrinter();
       BOOL StartPrinting();
       void EndPrinting();
       void PrintLine(const CString & s);
       virtual void PageHeading();
       virtual void SetPrinterFont();
       int GetPageNumber() { return pageNumber; }
    protected:
       CDC * dc;
       int pageVMargin;
       int pageHMargin;
       int pageHeight;
       int pageWidth;
       int Y;
       int lineHeight;
       BOOL pageStarted;
       BOOL docStarted;
       int pageNumber;
};
