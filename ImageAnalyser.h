
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		

class CImageAnalyserApp : public CWinApp
{
public:
	CImageAnalyserApp();
public:
	virtual BOOL InitInstance();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
private:
	BOOL IsInstanceAlreadyExist();
private:
	HANDLE m_hAppMutex = NULL;
};

extern CImageAnalyserApp theApp;
