
#include "pch.h"
#include "framework.h"
#include "ImageAnalyser.h"
#include "ImageAnalyserDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);  
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


CImageAnalyserDlg::CImageAnalyserDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IMAGEANALYSER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_ptrFetchCameraDetils = make_shared<ChwFetchCameraDetails>();
}

void CImageAnalyserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ST_IMAGE_CTRL, m_ImageControl);
	DDX_Control(pDX, IDC_BUTTON_START_GRAB, m_btnLiveCpature);
	DDX_Control(pDX, IDC_BUTTON_STOP_GRAB2, m_btnStopCapture);
}

BEGIN_MESSAGE_MAP(CImageAnalyserDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_GETMINMAXINFO()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDOK, &CImageAnalyserDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CImageAnalyserDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_START_GRAB, &CImageAnalyserDlg::OnBnClickedButtonStartGrab)
END_MESSAGE_MAP()


BOOL CImageAnalyserDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	SetWindowText(_T("Real Time Object Detections"));
	AlignUIControls();
	auto ConnectedCameraCount = m_ptrFetchCameraDetils->FetchCameraHardwareData();
	if (ConnectedCameraCount)
	{
		m_ptrCameraManager = make_shared<ChwCameraManger>();
		m_ptrCameraManager->IntiCameraModules(ConnectedCameraCount);
	}

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);	

	return TRUE; 
}

void CImageAnalyserDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CImageAnalyserDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); 

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CImageAnalyserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CImageAnalyserDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CDialogEx::OnGetMinMaxInfo(lpMMI);

	lpMMI->ptMinTrackSize.x = lpMMI->ptMaxTrackSize.x = 950;
	lpMMI->ptMinTrackSize.y = lpMMI->ptMaxTrackSize.y = 650;
}

void CImageAnalyserDlg::OnBnClickedOk()
{
}

void CImageAnalyserDlg::OnBnClickedCancel()
{
}

void CImageAnalyserDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

}

void CImageAnalyserDlg::AlignUIControls()
{
	CRect rct;
	GetClientRect(rct);
	if (m_ImageControl.GetSafeHwnd())
	{
		long startY = rct.bottom = rct.bottom - 35;
		m_ImageControl.MoveWindow(rct, TRUE);

		CRect rctLiveBtnRect(rct.left + 2, startY + 2, rct.left + 150, startY + 33);
		m_btnLiveCpature.MoveWindow(rctLiveBtnRect, TRUE);


		CRect rctDrwROIImage(rctLiveBtnRect.right + 10, rctLiveBtnRect.top, rctLiveBtnRect.right + 10 + 150, startY + 33);
		m_btnStopCapture.MoveWindow(rctDrwROIImage, TRUE);	
	}
}



void CImageAnalyserDlg::OnClose()
{
	int result = AfxMessageBox(_T("Are you want to Close the Application?"), MB_OKCANCEL | MB_ICONQUESTION);
	if (result == IDOK)
	{
		CDialogEx::OnCancel();
	}
	else if (result == IDCANCEL)
	{
	}
}


void CImageAnalyserDlg::OnBnClickedButtonStartGrab()
{
	AfxBeginThread(ImageGrabberThread, (LPVOID*)this, THREAD_PRIORITY_NORMAL);
}

UINT CImageAnalyserDlg::ImageGrabberThread(LPVOID Param)
{
	auto ptrOutSide = (CImageAnalyserDlg*)Param;

	//Want to add Post Message and Handle UI Related action in Post Message
	cv::Mat SpecReadMat;
	ptrOutSide->m_ptrCameraManager->SnapGrab(0, SpecReadMat);
	ptrOutSide->m_ImageControl.SetMatImage(SpecReadMat);

	return TRUE;
}