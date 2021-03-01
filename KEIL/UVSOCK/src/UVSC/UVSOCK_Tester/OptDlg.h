#pragma once


// COptDlg dialog

class COptDlg : public CDialog  {
	DECLARE_DYNAMIC(COptDlg)

public:
	COptDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COptDlg();
    int   CheckTname (char *pB);
    int  CheckOptStr (char *pB);
    int     GetGname (char *pB);
    int     GetFname (char *pB);

// Dialog Data
	enum { IDD = IDD_PRJOPT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedOptread();
    afx_msg void OnBnClickedOptwrite();
};
