#pragma once

class CImageAnalyserDlg : public CDialogEx
{
public:
	CImageAnalyserDlg(CWnd* pParent = nullptr);	

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IMAGEANALYSER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

protected:
	HICON m_hIcon;
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	void OnClose();
	void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	DECLARE_MESSAGE_MAP()
};
