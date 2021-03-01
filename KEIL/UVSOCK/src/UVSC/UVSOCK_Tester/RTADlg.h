#pragma once


#include "resource.h"
#include "afxwin.h"
#include "Captain_SLOG_ListBox.h"

// CRTADlg dialog

class CRTADlg : public CDialog
{
	DECLARE_DYNAMIC(CRTADlg)

public:
  void ShowRTAMsg (BYTE *msgBuf);

private:
  CFont RTATraceFont;
  void OutLine (char *fmt, ...);
  void OutArray (unsigned char *data, unsigned int len);

public:
	CRTADlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRTADlg();

// Dialog Data
	enum { IDD = IDD_RTA };

protected:
  virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  // RTA trace window
  CButton m_CheckRaw;
  CButton m_showCustom[16];
  CButton m_CheckTaskSwitch;
  afx_msg void OnBnClickedButtonclearwindow();
  CCaptainSLOGBox m_listRxRTA;
  afx_msg void OnBnClickedCheckcust1();
};
