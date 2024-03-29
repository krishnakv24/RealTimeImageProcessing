#include "pch.h"

#include "framework.h"
#include "ImageAnalyser.h"
#include "ImageAnalyserDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;



BEGIN_MESSAGE_MAP(CImageAnalyserApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
	
END_MESSAGE_MAP()


CImageAnalyserApp::CImageAnalyserApp()
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

}

CImageAnalyserApp theApp;

BOOL CImageAnalyserApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


	AfxEnableControlContainer();
	CShellManager *pShellManager = new CShellManager;
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	SetRegistryKey(_T("Image Analyser"));

	CImageAnalyserDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	return FALSE;
}

BOOL CImageAnalyserApp::IsInstanceAlreadyExist()
{
	CString appname = m_pszAppName;
	m_hAppMutex = CreateMutex(NULL, FALSE, appname);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(m_hAppMutex);
		m_hAppMutex = NULL;
		return TRUE;
	}
	return FALSE;
}

int CImageAnalyserApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}

