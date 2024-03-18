#pragma once
#include "uiCamImageControl.h"
#include "hwFetchCameraDetails.h"
#include "hwCameraManger.h"

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
	void OnSize(UINT nType, int cx, int cy);
	void AlignUIControls();
	void OnClose();
	void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStartGrab();
	static UINT ImageGrabberThread(LPVOID Param);
	void AddImageToQueue(const cv::Mat& image);
	bool GetNextImageFromQueue(cv::Mat& image);
	afx_msg LRESULT DoUpdate(WPARAM wParam, LPARAM lParam);
private:
	shared_ptr<ChwFetchCameraDetails> m_ptrFetchCameraDetils;
	shared_ptr<ChwCameraManger> m_ptrCameraManager;
	CuiCamImageControl m_ImageControl;
	CButton m_btnLiveCpature;
	CButton m_btnStopCapture;
	std::queue<cv::Mat> imageQueue;
	std::mutex queueMutex;
	std::atomic<bool> m_bstopThreadFlag
	{
		false
	};
public:
	afx_msg void OnBnClickedButtonStopGrab2();
};
