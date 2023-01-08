// SoundRecDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SoundRec.h"
#include "SoundRecDlg.h"
#include ".\soundrecdlg.h"

#pragma comment(lib,"winmm.lib")
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// externals
extern char g_pOutputWAVFile[2048];

// globals
UINT g_nDevices;
char g_devicelist[10][32];

// CSoundRecDlg dialog

CSoundRecDlg::CSoundRecDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSoundRecDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bRun=FALSE;
	m_hThread=NULL;
	m_hWaveIn=NULL;
	ZeroMemory(&m_stWFEX,sizeof(WAVEFORMATEX));
	ZeroMemory(m_stWHDR,MAX_BUFFERS*sizeof(WAVEHDR));
}

void CSoundRecDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSoundRecDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_REC, OnBnClickedRec)
END_MESSAGE_MAP()

UINT CSoundRecDlg::FillDevices()
{
	g_nDevices = waveInGetNumDevs();
	for(UINT nC1=0;nC1<g_nDevices;++nC1)
	{
		WAVEINCAPS stWIC={0};
		ZeroMemory(&stWIC,sizeof(WAVEINCAPS));
		MMRESULT mRes = waveInGetDevCaps(nC1,&stWIC,sizeof(WAVEINCAPS));
		strcpy(g_devicelist[nC1], stWIC.szPname);
	}
	return g_nDevices;
}

void CSoundRecDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CSoundRecDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

DWORD WINAPI ThFunc(LPVOID pDt)
{
	CSoundRecDlg *pOb=(CSoundRecDlg*)pDt;
	pOb->StartRecording();
	return 0;
}

void CALLBACK waveInProc(HWAVEIN hwi,UINT uMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2)
{
	WAVEHDR *pHdr=NULL;
	switch(uMsg)
	{
		case WIM_CLOSE:
			break;

		case WIM_DATA:
			{
				CSoundRecDlg *pDlg=(CSoundRecDlg*)dwInstance;
				pDlg->ProcessHeader((WAVEHDR *)dwParam1);
			}
			break;

		case WIM_OPEN:
			break;

		default:
			break;
	}
}

VOID CSoundRecDlg::ProcessHeader(WAVEHDR * pHdr)
{
	MMRESULT mRes=0;
	TRACE("%d",pHdr->dwUser);
	if(WHDR_DONE==(WHDR_DONE &pHdr->dwFlags))
	{
		mmioWrite(m_hOPFile,pHdr->lpData,pHdr->dwBytesRecorded);
		mRes=waveInAddBuffer(m_hWaveIn,pHdr,sizeof(WAVEHDR));
	}
}

VOID CSoundRecDlg::OpenDevice()
{
	int nT1=0;
	CString csT1;
	double dT1=0.0;
	MMRESULT mRes=0;

	m_stWFEX.nSamplesPerSec = 44100;// (int)dT1;
	m_stWFEX.nChannels=1;
	m_stWFEX.wBitsPerSample = 16;
	m_stWFEX.wFormatTag=WAVE_FORMAT_PCM;
	m_stWFEX.nBlockAlign=m_stWFEX.nChannels*m_stWFEX.wBitsPerSample/8;
	m_stWFEX.nAvgBytesPerSec=m_stWFEX.nSamplesPerSec*m_stWFEX.nBlockAlign;
	m_stWFEX.cbSize=sizeof(WAVEFORMATEX);
	UINT uDeviceID = 0;
	mRes=waveInOpen(&m_hWaveIn,uDeviceID,&m_stWFEX,(DWORD_PTR)waveInProc,(DWORD_PTR)this,CALLBACK_FUNCTION);
	if(mRes!=MMSYSERR_NOERROR)
	{
		throw m_csErrorText;
	}
	csT1.SetString(g_pOutputWAVFile);
	ZeroMemory(&m_stmmIF,sizeof(MMIOINFO));
	DeleteFile((PCHAR)(LPCTSTR)csT1);
	m_hOPFile=mmioOpen((PCHAR)(LPCTSTR)csT1,&m_stmmIF,MMIO_WRITE | MMIO_CREATE);
	if(m_hOPFile==NULL)
		throw "Can not open file...";

	ZeroMemory(&m_stckOutRIFF,sizeof(MMCKINFO));
	m_stckOutRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E'); 
	mRes=mmioCreateChunk(m_hOPFile, &m_stckOutRIFF, MMIO_CREATERIFF);
	if(mRes!=MMSYSERR_NOERROR)
	{
		throw m_csErrorText;
	}
	ZeroMemory(&m_stckOut,sizeof(MMCKINFO));
	m_stckOut.ckid = mmioFOURCC('f', 'm', 't', ' ');
	m_stckOut.cksize = sizeof(m_stWFEX);
	mRes=mmioCreateChunk(m_hOPFile, &m_stckOut, 0);
	if(mRes!=MMSYSERR_NOERROR)
	{
		throw m_csErrorText;
	}
	nT1=mmioWrite(m_hOPFile, (HPSTR) &m_stWFEX, sizeof(m_stWFEX));
	if(nT1!=sizeof(m_stWFEX))
	{
		m_csErrorText.Format("Can not write Wave Header..File: %s ,Line Number:%d",__FILE__,__LINE__);
		throw m_csErrorText;
	}
	mRes=mmioAscend(m_hOPFile, &m_stckOut, 0);
	if(mRes!=MMSYSERR_NOERROR)
	{
		throw m_csErrorText;
	}
	m_stckOut.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mRes=mmioCreateChunk(m_hOPFile, &m_stckOut, 0);
	if(mRes!=MMSYSERR_NOERROR)
	{
		throw m_csErrorText;
	}
}
VOID CSoundRecDlg::CloseDevice()
{
	MMRESULT mRes=0;
	
	if(m_hWaveIn)
	{
		UnPrepareBuffers();
		mRes=waveInClose(m_hWaveIn);
	}
	if(m_hOPFile)
	{
		mRes=mmioAscend(m_hOPFile, &m_stckOut, 0);
		mRes=mmioAscend(m_hOPFile, &m_stckOutRIFF, 0);
		mmioClose(m_hOPFile,0);
		m_hOPFile=NULL;
	}
	m_hWaveIn=NULL;
}

VOID CSoundRecDlg::PrepareBuffers()
{
	MMRESULT mRes=0;
	int nT1=0;

	for(nT1=0;nT1<MAX_BUFFERS;++nT1)
	{
		m_stWHDR[nT1].lpData=(LPSTR)HeapAlloc(GetProcessHeap(),8,m_stWFEX.nAvgBytesPerSec);
		m_stWHDR[nT1].dwBufferLength=m_stWFEX.nAvgBytesPerSec;
		m_stWHDR[nT1].dwUser=nT1;
		mRes=waveInPrepareHeader(m_hWaveIn,&m_stWHDR[nT1],sizeof(WAVEHDR));
		if(mRes!=0)
		{
			throw m_csErrorText;
		}
		mRes=waveInAddBuffer(m_hWaveIn,&m_stWHDR[nT1],sizeof(WAVEHDR));
		if(mRes!=0)
		{
			throw m_csErrorText;
		}
	}
}

VOID CSoundRecDlg::UnPrepareBuffers()
{
	MMRESULT mRes=0;
	int nT1=0;

	if(m_hWaveIn)
	{
		mRes=waveInStop(m_hWaveIn);
		for(nT1=0;nT1<3;++nT1)
		{
			if(m_stWHDR[nT1].lpData)
			{
				mRes=waveInUnprepareHeader(m_hWaveIn,&m_stWHDR[nT1],sizeof(WAVEHDR));
				HeapFree(GetProcessHeap(),0,m_stWHDR[nT1].lpData);
				ZeroMemory(&m_stWHDR[nT1],sizeof(WAVEHDR));
			}
		}
	}
}

VOID CSoundRecDlg::StartRecording()
{	
	MMRESULT mRes;
	try
	{
		OpenDevice();
		PrepareBuffers();
		mRes=waveInStart(m_hWaveIn);
		if(mRes!=0)
		{
			throw m_csErrorText;
		}
		while(m_bRun)
		{
			SleepEx(100,FALSE);
		}
	}
	catch(PCHAR pErrorMsg)
	{
		AfxMessageBox(pErrorMsg);
	}
	CloseDevice();
	CloseHandle(m_hThread);
	m_hThread=NULL;
	EndDialog(-1);
}

void CSoundRecDlg::OnBnClickedRec()
{
	m_bRun=FALSE;
}

BOOL CSoundRecDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	if(FillDevices()<=0)
	{
		AfxMessageBox("NO Input Devices Found..",MB_ICONERROR);
		CDialog::OnOK();
	}

	// start recording immediately
	m_hThread=CreateThread(NULL,0,ThFunc,this,0,NULL);
	m_bRun=TRUE;

	return TRUE;
}
