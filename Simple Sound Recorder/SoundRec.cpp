// SoundRec.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SoundRec.h"
#include "SoundRecDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Globals
char g_pOutputWAVFile[2048];

// CSoundRecApp

BEGIN_MESSAGE_MAP(CSoundRecApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

// CSoundRecApp construction

CSoundRecApp::CSoundRecApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CSoundRecApp object

CSoundRecApp theApp;


// CSoundRecApp initialization

BOOL CSoundRecApp::InitInstance()
{
	// get the location of where to save the WAV
	LPSTR pCmdLine = GetCommandLineA();
	char pAbsWAVPath[2048];
	int iSpeechCount = 0;
	strcpy(pAbsWAVPath, pCmdLine);
	for (int n = strlen(pCmdLine) - 1; n > 0; n--)
	{
		if (iSpeechCount == 1 && pCmdLine[n] == '"')
		{
			// found last parameter encased in speech marks
			strcpy(pAbsWAVPath, pCmdLine + n + 1);
			break;
		}
		if (iSpeechCount==0 && pCmdLine[n] == '"') iSpeechCount = 1;
	}
	if (pAbsWAVPath[strlen(pAbsWAVPath) - 1] == '"') pAbsWAVPath[strlen(pAbsWAVPath) - 1] = 0;
	strcpy(g_pOutputWAVFile, pAbsWAVPath);

	// MFC stuff
	InitCommonControls();
	CWinApp::InitInstance();
	//SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	CSoundRecDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	return FALSE;
}
