#pragma once


// CBrkDlg dialog

class CBrkDlg : public CDialog  {
	DECLARE_DYNAMIC(CBrkDlg)

public:
	CBrkDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBrkDlg();

    void    InitBpListbox (void);

// Dialog Data
	enum { IDD = IDD_BRKDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedBrkcreate();
    afx_msg void OnLbnSelchangeBplist();
    afx_msg void OnBnClickedBpkill();
    afx_msg void OnBnClickedBpenable();
    afx_msg void OnBnClickedBpdisable();
};
