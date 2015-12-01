
// PwdTool.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PwdTool.h"
#include "PwdToolDlg.h"
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void resetcwd()
{
    std::wstring temp(MAX_PATH, 0);
    GetModuleFileNameW(NULL, &temp[0], MAX_PATH);
    std::replace(temp.begin(), temp.end(), L'\\', L'/');
    size_t pos = temp.find_last_of(L'/');
    if(pos != temp.npos)
    {
        temp.erase(pos, temp.npos);
        SetCurrentDirectoryW(temp.c_str());
        std::wcout<<L"reset cur dir to: "<<temp<<std::endl;
    }
    else
    {
        std::wcout<<L"reset cur dir failed!"<<std::endl;
    }
}

// CPwdToolApp

BEGIN_MESSAGE_MAP(CPwdToolApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPwdToolApp construction

CPwdToolApp::CPwdToolApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CPwdToolApp object

CPwdToolApp theApp;


// CPwdToolApp initialization

BOOL CPwdToolApp::InitInstance()
{
    resetcwd();

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CPwdToolDlg dlg;
	m_pMainWnd = &dlg;

    if(!dlg.LoadPwdFile())
    {
        return FALSE;
    }

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

