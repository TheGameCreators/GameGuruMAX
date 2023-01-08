// SoundRecDlg.h : header file
//

#pragma once
#include <MMSystem.h>
#define MAX_BUFFERS	3
// CSoundRecDlg dialog
class CSoundRecDlg : public CDialog
{
// Construction
public:
	CSoundRecDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SOUNDREC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRec();

public:
	VOID StartRecording();
	VOID ProcessHeader(WAVEHDR * pHdr);

private:
	VOID OpenDevice();
	VOID CloseDevice();
	VOID PrepareBuffers();
	VOID UnPrepareBuffers();
	UINT FillDevices();

	BOOL m_bRun;
	HWAVEIN m_hWaveIn;
	WAVEFORMATEX m_stWFEX;
	WAVEHDR m_stWHDR[MAX_BUFFERS];
	HANDLE m_hThread;
	CString m_csErrorText;
	HMMIO m_hOPFile;
	MMIOINFO m_stmmIF;
	MMCKINFO m_stckOut,m_stckOutRIFF; 
};
