//#pragma optimize("", off)
 
//PE: Really slow , switched to wicked xaudio2.
#define WICKEDAUDIO

//PE: reverB works.
//#define REVERBTEST

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable : 4996)

#include "csoundc.h"
#include ".\..\error\cerror.h"
#include "globstruct.h"

//#pragma comment ( lib, "dxerr8.lib" )
#pragma comment ( lib, "winmm.lib" )
#pragma comment ( lib, "dsound.lib" )

#include <windows.h>
#include <basetsd.h>
#include <commctrl.h>
#include <commdlg.h>
#include <mmreg.h>
#include <mmsystem.h>
//#include <dxerr8.h>
#include <dsound.h>
#include <math.h>
#include <stdio.h>
#include "DSUtil.h"
#include "DXUtil.h"
#include "direct.h"
#include ".\..\Core\SteamCheckForWorkshop.h"

#include "CFileC.h"

#ifdef WICKEDAUDIO
#include "..\..\..\..\WickedRepo\WickedEngine\WickedEngine.h"
#include <xaudio2.h>
using namespace wiGraphics;
using namespace wiScene;
using namespace wiECS;
using namespace wiAudio;
std::string realname = "";
float GetCurveDistanceScaler(void);
#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// GLOBALS ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DBPRO_GLOBAL CSDKSoundManager			m_SDKSoundManager;		// stores all sound data
DBPRO_GLOBAL sSoundData*				m_ptr;					// pointer to current sound
HWND									m_hWndSOUND;			// handle to window

DBPRO_GLOBAL CSoundManager*				g_pSoundManager			= NULL;
DBPRO_GLOBAL IDirectSound3DListener*	pDSListener				= NULL;
DBPRO_GLOBAL GGVECTOR3					vecListenerPosition		= GGVECTOR3 ( 0.0f, 0.0f, 0.0f );
DBPRO_GLOBAL GGVECTOR3					vecListenerFront		= GGVECTOR3 ( 0.0f, 0.0f, 1.0f );
DBPRO_GLOBAL GGVECTOR3					vecListenerTop			= GGVECTOR3 ( 0.0f, 1.0f, 0.0f );
DBPRO_GLOBAL GGVECTOR3					vecListenerAngle		= GGVECTOR3 ( 0.0f, 0.0f, 0.0f );
DBPRO_GLOBAL UINT						g_wDeviceID				= 0;
DBPRO_GLOBAL int						g_iSoundToRecordOver	= 0;
DBPRO_GLOBAL int						g_iReduce3DSoundCalc	= 0;
extern GlobStruct*						g_pGlob;
HWND									g_currentSoundHWND		= NULL;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef WICKEDENGINE
//PE: We cant disable a single sound if it fails the setup, everything else will also fail.
//PE: So we need to disable everything, and add the error messages to the log instead.
#define IGNOREALLSOUNDERRORS
void timestampactivity(int i, char* desc_s);
#endif

//PE: This also solves problems with Classic so:
#ifdef PRODUCTCLASSIC
#define IGNOREALLSOUNDERRORS
void timestampactivity(int i, char* desc_s);
#endif


//////////////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void SoundConstructor ( void )
{
#ifdef WICKEDAUDIO
#else
	// NOTE: No hardware mixing for Direct X 11 DirectSound!

	// store the window handle
	m_hWndSOUND = g_pGlob->hWnd;

    // Create a static IDirectSound in the CSound class.  
    g_pSoundManager = new CSoundManager();

    // Set coop level to DSSCL_PRIORITY, and set primary buffer format to stereo, 22kHz and 16-bit output.
	HRESULT hr = g_pSoundManager->Initialize( g_pGlob->hWnd , DSSCL_PRIORITY, 2, 22050, 16 );
    if( FAILED(hr) )
	{
		return;
	}

	
	//DSCAPS dsCaps;
	//ZeroMemory(&dsCaps, sizeof(DSCAPS));
	//dsCaps.dwSize = sizeof(DSCAPS);
	//if (FAILED(g_pSoundManager->m_pDS->GetCaps(&dsCaps))) {
	//}
	//else {
	//	if (dsCaps.dwFlags & DSCAPS_HARDWARE) {
	//	}
	//	else {
	//	}
	//}


    // Get the 3D listener, so we can control its params
    hr |= g_pSoundManager->Get3DListenerInterface( &pDSListener );
    if( FAILED(hr) )
	{
		return;
	}

	// setup listener
	pDSListener->SetDistanceFactor(1.0f, DS3D_DEFERRED );
	pDSListener->SetPosition(0.0f, 0.0f, 0.0f, DS3D_DEFERRED );
	pDSListener->SetOrientation(0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, DS3D_DEFERRED );
	pDSListener->SetRolloffFactor(1.0f, DS3D_DEFERRED );
	pDSListener->SetVelocity(0.0f, 0.0f, 0.0f, DS3D_DEFERRED );
	pDSListener->SetDopplerFactor(DS3D_MINDOPPLERFACTOR, DS3D_DEFERRED );
#endif
}

DARKSDK void SoundDestructor ( void )
{
#ifdef WICKEDAUDIO
#else

	// reduce to non priority
	if ( g_pSoundManager )
		if ( g_pSoundManager->m_pDS )
			g_pSoundManager->m_pDS->SetCooperativeLevel ( g_pGlob->hWnd, DSSCL_NORMAL );

	// clean up the sound library
	m_SDKSoundManager.ShutDown();

	// release listener
	SAFE_RELEASE ( pDSListener );

	// release ds
    SAFE_DELETE( g_pSoundManager );
#endif
}

DARKSDK bool UpdateSoundPtr ( int iID )
{
	// update internal pointer

	// set the pointer to null
	//m_ptr = NULL;  //PE: not thread safe, as others can use it. moved to if failed.
	
	// attempt to retrive the data
	if (!(m_ptr = m_SDKSoundManager.GetData(iID)))
	{
		m_ptr = NULL;
		return false;
	}

	// if we got it ok then return true
	return true;
}

DARKSDK sSoundData* GetSound( int iID )
{
    return m_SDKSoundManager.GetData( iID );
}

DARKSDK void SetSoundErrorHandler ( LPVOID pErrorHandlerPtr )
{
#ifdef WICKEDAUDIO
#else
	// Update error handler pointer
	g_pErrorHandler = (CRuntimeErrorHandler*)pErrorHandlerPtr;
#endif
}

DARKSDK void PassSoundCoreData( LPVOID pGlobPtr )
{
#ifdef WICKEDAUDIO
#else
	g_pGlob = (GlobStruct*)pGlobPtr;
#endif
}

DARKSDK void SoundRefreshGRAFIX ( int iMode )
{
}

DARKSDK float GetSoundPosition(int iID)
{
#ifdef WICKEDAUDIO

	if (iID < 1 || iID > MAXIMUMVALUE)
	{
		return 0;
	}
	if (!UpdateSoundPtr(iID))
	{
		return 0;
	}
	SoundComponent* sound = GetScene().sounds.GetComponent(m_ptr->wickedEntity);
	if (sound != nullptr)
	{
		float percent = wiAudio::PlayedPercent(&sound->soundinstance);
		return(percent);
	}
#else

	// if no sound card, leave now
	if (!g_pSoundManager) return 0;

	if (!g_pSoundManager->GetExists())
		return 0.0f;

	sSoundData* ptr = NULL;
	ptr = m_SDKSoundManager.GetData(iID);
	if (!ptr)
	{
		return 0.0f;
	}
	DWORD dwSoundSize = ptr->pSound->GetBufferSize();

	if (ptr->bPlaying && dwSoundSize > 0)
	{
		DWORD pos, wpos;
		ptr->pSound->GetBuffer(0)->GetCurrentPosition(&pos, &wpos);
		if (pos > dwSoundSize)
			return(100.0f);
		return(( (float) pos/ (float) dwSoundSize)*100.0f);
	}
#endif
	return 0.0f;
}

DARKSDK void UpdateSound ( void )
{
#ifdef WICKEDAUDIO
#else
	// if no sound card, leave now
	if (!g_pSoundManager) return;

	// LB:silent return if sound card does not exist
	if (!g_pSoundManager->GetExists())
		return;

	// check all sounds for forced looping boundaries
	link* check = NULL;
	check = m_SDKSoundManager.GetList();

	while(check)
	{
		sSoundData* ptr = NULL;
		//ptr = m_SDKSoundManager.GetData ( check->id ); // U74 - 080509 - faster!
		ptr = (sSoundData*)check->data;
		if (ptr == NULL || !ptr->bPlaying) {
			//PE: This would produce a endless loop.
			check = check->next;
			continue;
		}

		// detect OGG finish
		DWORD dwSoundSize = ptr->pSound->GetBufferSize();
		if ( ptr->bPlaying && dwSoundSize>0 )
		{
			// detect OGG finish
			DWORD pos, wpos;
			ptr->pSound->GetBuffer(0)->GetCurrentPosition(&pos, &wpos);
			if ( pos>dwSoundSize )
				ptr->bPlaying = false;
		}

		// if sound is looping and active, check forced looping
		if(ptr->bPlaying && ptr->bLoop)
		{
			DWORD pos, wpos;
			ptr->pSound->GetBuffer(0)->GetCurrentPosition(&pos, &wpos);
			if ( ptr->iLoopStartPos>=0 )
			{
				if(pos<(DWORD)ptr->iLoopStartPos)
				{
					// bring to start of loop
					ptr->pSound->GetBuffer(0)->SetCurrentPosition(ptr->iLoopStartPos);
				}
			}
			if ( ptr->iLoopEndPos>=0 )
			{
				if(pos>(DWORD)ptr->iLoopEndPos)
				{
					// bring to start of loop
					ptr->pSound->GetBuffer(0)->SetCurrentPosition(ptr->iLoopStartPos);
				}
			}
		}
		if(ptr->bPlaying && ptr->bLoop==false)
		{
			DWORD status;
			ptr->pSound->GetBuffer(0)->GetStatus(&status);
			if( !(status & DSBSTATUS_PLAYING) )
				if (ptr->bPause==false) // Issue 292 - changed to pause and resume instead of stop when in gane menu
					ptr->bPlaying = false;
		}
		check = check->next;
	}

	// Only update 3DDEFERRED (once every fifth/tenth call causes large slowdown when used)
	if ( pDSListener )
	{
		// leefix - 180705 - We need 3D quick and up to date, the trick is to use fewer 3D sounds!
		g_iReduce3DSoundCalc++;
		if ( g_iReduce3DSoundCalc > 1 )
		{
			// Calculate 3D sounds only once per odd cycle
			pDSListener->CommitDeferredSettings();
			g_iReduce3DSoundCalc=0;
		}
	}

	// Set cooperative level of original HWND if available
	HWND hWnd = GetForegroundWindow();
	if ( g_currentSoundHWND != hWnd )
	{
		g_currentSoundHWND = hWnd;
		if (g_pSoundManager->m_pDS)
		{
			HRESULT hr = g_pSoundManager->m_pDS->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
		}
	}
#endif
}

//
// SDK Functions
//

DARKSDK float SoundPositionX ( int iID )
{
#ifdef WICKEDAUDIO
#else
	// if no sound card, leave now
	if (!g_pSoundManager) return 0;

	// get sound x position
	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return 0.0f;

	// get data
	if ( !UpdateSoundPtr ( iID ) )
	{
		Error1 ( "GetSoundPositionX - sound does not exist" );
		return 0.0f;
	}
#endif

	// return x position
	return m_ptr->vecPosition.x;
}

DARKSDK float SoundPositionY ( int iID )
{
#ifdef WICKEDAUDIO
#else

	// get sound y position

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return 0.0f;

	// get data
	if ( !UpdateSoundPtr ( iID ) )
	{
		Error1 ( "GetSoundPositionY - sound does not exist" );
		return 0.0f;
	}

	// return y position
#endif
	return m_ptr->vecPosition.y;
}

DARKSDK float SoundPositionZ ( int iID )
{
#ifdef WICKEDAUDIO
#else

	// get sound z position

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return 0.0f;

	// get data
	if ( !UpdateSoundPtr ( iID ) )
	{
		Error1 ( "GetSoundPositionZ - sound does not exist" );
		return 0.0f;
	}

	// return z position
#endif
	return m_ptr->vecPosition.z;
}

DARKSDK float ListenerPositionX ( void )
{
#ifdef WICKEDAUDIO
	const wiScene::CameraComponent& camera = wiScene::GetCamera();
	return(camera.Eye.x);
#else

	// get listener x position
	return vecListenerPosition.x;
#endif
}

DARKSDK float ListenerPositionY ( void )
{
#ifdef WICKEDAUDIO
	const wiScene::CameraComponent& camera = wiScene::GetCamera();
	return(camera.Eye.y);
#else

	// get listener y position
	return vecListenerPosition.y;
#endif
}

DARKSDK float ListenerPositionZ ( void )
{
#ifdef WICKEDAUDIO
	const wiScene::CameraComponent& camera = wiScene::GetCamera();
	return(camera.Eye.z);
#else
	// get listener z position
	return vecListenerPosition.z;
#endif
}

DARKSDK float ListenerAngleX ( void )
{
	// get angle x
	return vecListenerAngle.x;
}

DARKSDK float ListenerAngleY ( void )
{
	// get angle y
	return vecListenerAngle.y;
}

DARKSDK float ListenerAngleZ ( void )
{
	// get angle z
	return vecListenerAngle.z;
}

/*DARKSDK BOOL DB_FileExist(char* Filename)
{
	// Open BASIC Script
	HANDLE hfile = GG_CreateFile(Filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hfile==INVALID_HANDLE_VALUE)
		return FALSE;

	CloseHandle(hfile);
	return TRUE;
}*/

DARKSDK void DB_GetWinTemp(LPSTR pWindowsTempDirectory)
{
	// Current dir
	char CurrentDirectory[_MAX_PATH];
	getcwd(CurrentDirectory, _MAX_PATH);

	// Find temporary directory (C:\WINDOWS\Temp)
	GetTempPath(_MAX_PATH, pWindowsTempDirectory);
	if(stricmp(pWindowsTempDirectory, CurrentDirectory)!=NULL)
	{
		// XP Temp Folder
		_chdir(pWindowsTempDirectory);
		mkdir("dbpdata");
		// U69 - 050608 - add seperator onyl if not present
		if ( strncmp ( pWindowsTempDirectory + strlen ( pWindowsTempDirectory ) - 1, "\\", 1 )!=NULL ) strcat ( pWindowsTempDirectory, "\\" );
		strcat(pWindowsTempDirectory, "dbpdata");
		_chdir(CurrentDirectory);
	}
	else
	{
		// Pre-XP Temp Folder
		GetWindowsDirectory(pWindowsTempDirectory, _MAX_PATH);
		_chdir(pWindowsTempDirectory);
		mkdir("temp");
		_chdir("temp");
		mkdir("dbpdata");
		strcat(pWindowsTempDirectory, "\\temp\\dbpdata");
		_chdir(CurrentDirectory);
	}
}

#ifdef WICKEDAUDIO
bool bLoadWickedSound(LPSTR szFilename, int iID, bool b3DSound, int iSilentFail, std::vector<uint8_t>& data)
{
#ifdef WICKEDAUDIO
	std::string fileName = szFilename;
	std::string soundName = wiHelper::GetFileNameFromPath(realname);

	Scene& myscene = GetScene();
	Entity entity = myscene.Entity_CreateSound_GG(soundName, fileName, XMFLOAT3(0, 0, 0),realname, data);

	DBPRO_GLOBAL sSoundData* mptr;
	if ((mptr = m_SDKSoundManager.GetData(iID)))
	{
		mptr->wickedEntity = entity;
		strcpy(&mptr->wickedFilename[0], fileName.c_str());
	}

	SoundComponent* sound = GetScene().sounds.GetComponent(entity);
	if (sound != nullptr)
	{
		//PE: Make it work like old GGM , but 3D stereo is supported.
		uint32_t channels = GetSoundChannels(&sound->soundinstance);
		if (channels > 1)
		{
			b3DSound = false;
		}

		//PE: Defaults. 
		sound->SetDisable3D(1 - b3DSound);
		sound->SetLooped(false);
#ifdef REVERBTEST
		static bool breverbinit = true;
		if (breverbinit)
		{
			wiAudio::SetReverb(REVERB_PRESET_CAVE);
			breverbinit = false;
		}
		sound->soundinstance.SetEnableReverb(true);
#else
		sound->soundinstance.SetEnableReverb(false);
#endif
		sound->volume = 1.0f; //0-1
		if(b3DSound)
			sound->soundinstance.type = wiAudio::SUBMIX_TYPE::SUBMIX_TYPE_SOUNDEFFECT; //SUBMIX_TYPE_SOUNDEFFECT, SUBMIX_TYPE_MUSIC,
		else
			sound->soundinstance.type = wiAudio::SUBMIX_TYPE::SUBMIX_TYPE_MUSIC; //SUBMIX_TYPE_SOUNDEFFECT, SUBMIX_TYPE_MUSIC,
		if (sound && sound->soundResource && sound->soundResource->sound.IsValid() && sound->soundinstance.IsValid())
		{
			wiAudio::CreateSoundInstance(&sound->soundResource->sound, &sound->soundinstance);
			sound->Stop();
			return true;
		}
	}
#else
#endif
	return false;
}
#endif
DARKSDK void LoadRawSoundCoreOgg ( LPSTR szFilename, int iID, bool b3DSound, int iSilentFail )
{
#ifdef WICKEDAUDIO
	std::vector<uint8_t> emptydata;
	bLoadWickedSound(szFilename, iID,b3DSound,iSilentFail, emptydata);
	return;
#else

	// load a sound into a buffer
	
	GUID    guid3DAlgorithm = SOUND_GUID_NULL;
    HRESULT hr; 

    if( m_ptr->pSound )
    {
        m_ptr->pSound->Stop();
        m_ptr->pSound->Reset();
    }

    // Free any previous sound, and make a new one
    SAFE_DELETE( m_ptr->pSound );

    
	DWORD dwFlags=DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	if(b3DSound)
	{
		// Get the software DirectSound3D emulation algorithm to use
		guid3DAlgorithm = DS3DALG_DEFAULT;
		dwFlags = DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	}

	m_ptr->pSound = new CSound;
	DWORD dwSizeOfSound = 0; // 070111 - OGG needs size to detect end!
	m_ptr->pSound->m_apDSBuffer = g_pSoundManager->LoadOggVorbis ( szFilename, &dwSizeOfSound );
	m_ptr->pSound->m_dwDSBufferSize = dwSizeOfSound;

	// LEEFIX - 191102 - fill with default sound data
	m_ptr->iVolume	= 100;
	m_ptr->iPan		= 100;

    // Get the 3D buffer from the secondary buffer
	if(b3DSound)
	{
		if( FAILED( hr = m_ptr->pSound->Get3DBufferInterface( 0, &m_ptr->pDSBuffer3D ) ) )
		{
			// no 3d buffer
			m_ptr->pDSBuffer3D=NULL;
		}
		else
		{
			// Get the 3D buffer parameters
			DS3DBUFFER dsBufferParams;
			dsBufferParams.dwSize = sizeof(DS3DBUFFER);
			m_ptr->pDSBuffer3D->GetAllParameters( &dsBufferParams );

			// Set new 3D buffer parameters
			#ifndef _DEBUG
			m_ptr->pDSBuffer3D->SetAllParameters( &dsBufferParams, DS3D_DEFERRED );
			#endif
		}
	}
	else
	{
		m_ptr->pDSBuffer3D=NULL;
	}
#endif
}

DARKSDK void LoadRawSoundCore(LPSTR szPassedInFilename, int iID, bool b3DSound, int iSilentFail, int iGlobalSound)
{
#ifdef WICKEDAUDIO
#else
	// if no sound card, leave now
	if (!g_pSoundManager) return;
#endif
	// first check if this file is actually in writable area
	char szFilename[MAX_PATH];
	strcpy(szFilename, szPassedInFilename);
	GG_GetRealPath(szFilename, 0);

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	g_pGlob->dwInternalFunctionCode = 15001;
#ifdef WICKEDAUDIO
#else
	if (!g_pSoundManager->GetExists())
		return;
#endif
	// checks
	if (iID < 1 || iID > MAXIMUMVALUE)
	{
#ifndef IGNOREALLSOUNDERRORS
		if (iSilentFail == 0) RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
#endif
		return;
	}
	if (UpdateSoundPtr(iID))
	{
#ifndef IGNOREALLSOUNDERRORS
		if (iSilentFail == 0) RunTimeError(RUNTIMEERROR_SOUNDALREADYEXISTS);
#else
		char cError[MAX_PATH + 20];
		sprintf(cError, "RUNTIMEERROR_SOUNDALREADYEXISTS: ID %d , file %s", iID, szFilename);
		timestampactivity(0, cError);
#endif	
		return;
	}
	//PE: Added additional info for silence.wav missing Max bug.
	if (strlen(szFilename) < 1 || !szFilename)
	{
		char cError[MAX_PATH + 20];
		sprintf(cError, "(flen) %s", szFilename);
#ifndef IGNOREALLSOUNDERRORS
		if (iSilentFail == 0) RunTimeError(RUNTIMEERROR_SOUNDLOADFAILED, cError);
#else
		timestampactivity(0, cError);
#endif
		return;
	}

	// add structure to sound manager
	g_pGlob->dwInternalFunctionCode = 15002;
	sSoundData		m_Data;							// all sound data
	memset(&m_Data, 0, sizeof(m_Data));
	m_SDKSoundManager.Add(&m_Data, iID);

	// update list
	g_pGlob->dwInternalFunctionCode = 15003;
	if (!UpdateSoundPtr(iID))
	{
		m_SDKSoundManager.Delete(iID);
		char cError[MAX_PATH + 20];
		sprintf(cError, "(updatesoundptr) %s", szFilename);
#ifndef IGNOREALLSOUNDERRORS
		if (iSilentFail == 0) RunTimeError(RUNTIMEERROR_SOUNDLOADFAILED, cError);
#else
		timestampactivity(0, cError);
#endif
		return;
	}

	// save the 3D flag
	m_ptr->b3D = b3DSound;

	// mike - 260705
	g_pGlob->dwInternalFunctionCode = 15004;
	if (strnicmp((char*)szFilename + strlen((char*)szFilename) - 4, ".ogg", 4) == NULL)
	{
#ifdef WICKEDAUDIO
		std::vector<uint8_t> emptydata;
		bool bOK = bLoadWickedSound(szFilename, iID, b3DSound, iSilentFail,emptydata);
		if (!bOK)
#else
		//char pLookSee[1024];
		//GetCurrentDirectory ( 1024, pLookSee );
		LoadRawSoundCoreOgg(szFilename, iID, b3DSound, iSilentFail);
		if (m_ptr->pDSBuffer3D == NULL && m_ptr->pSound->m_apDSBuffer == NULL)
#endif
		{
			m_SDKSoundManager.Delete(iID);
			char cError[MAX_PATH + 20];
			sprintf(cError, "OGG-RUNTIMEERROR_SOUNDLOADFAILED: %s", szFilename);
#ifndef IGNOREALLSOUNDERRORS
			if (iSilentFail == 0) RunTimeError(RUNTIMEERROR_SOUNDLOADFAILED, szFilename);
#else
			timestampactivity(0, cError);
#endif
			return;
		}
		return;
	}

	// load a sound into a buffer
	g_pGlob->dwInternalFunctionCode = 15005;
	GUID    guid3DAlgorithm = SOUND_GUID_NULL;
	HRESULT hr;

	// Free any previous sound, and make a new one
#ifdef WICKEDAUDIO
	std::vector<uint8_t> emptydata;
	bool bOK = bLoadWickedSound(szFilename, iID, b3DSound, iSilentFail, emptydata);
	if (!bOK)
	{
		// Invalid WAV format
		m_SDKSoundManager.Delete(iID);
		char cError[MAX_PATH + 20];
		sprintf(cError, "Failed: %s", szFilename);
		#ifndef IGNOREALLSOUNDERRORS
		if (iSilentFail == 0) RunTimeError(RUNTIMEERROR_SOUNDLOADFAILED, cError);
		#else
		timestampactivity(0, cError);
		#endif
		return;
	}
#else
    if( m_ptr->pSound )
    {
        m_ptr->pSound->Stop();
        m_ptr->pSound->Reset();
    }
    SAFE_DELETE( m_ptr->pSound );

	g_pGlob->dwInternalFunctionCode=15006;
    CWaveFile waveFile;
	HRESULT hre = waveFile.Open( szFilename, NULL, WAVEFILE_READ );
    WAVEFORMATEX* pwfx = waveFile.GetFormat();
    if( pwfx == NULL )
    {
		// Invalid WAV format
		m_SDKSoundManager.Delete ( iID );
		char cError[MAX_PATH + 20];
		sprintf(cError, "(hr=%ld) %s", hre, szFilename);
		#ifndef IGNOREALLSOUNDERRORS
		if ( iSilentFail==0 ) RunTimeError(RUNTIMEERROR_SOUNDLOADFAILED, cError);
		#else
		timestampactivity(0, cError);
		#endif
        return;
    }

	g_pGlob->dwInternalFunctionCode=15007;
    if( pwfx->nChannels > 1 && b3DSound==true)
    {
        // Too many channels in wave.  Sound must be mono when using DSBCAPS_CTRL3D
		//PE: This is not really a error, as we always first try 3D and then if failed load as normal sound.
		m_SDKSoundManager.Delete ( iID );
		#ifndef IGNOREALLSOUNDERRORS
		char cError[MAX_PATH + 20];
		sprintf(cError, "(3D sound failed, Too many channels, load normal) %s", szFilename);
		if ( iSilentFail==0 ) RunTimeError(RUNTIMEERROR_SOUNDLOADFAILED, cError);
		#else
		//timestampactivity(0, cError);
		#endif
        return;
    }
	DWORD dwFlags=DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	if(b3DSound)
	{
		// Get the software DirectSound3D emulation algorithm to use
		guid3DAlgorithm = DS3DALG_DEFAULT;
		dwFlags = DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	}

	// leeadd - 011208 - U71 - global sound flag
	if ( iGlobalSound==1 ) dwFlags = dwFlags | DSBCAPS_GLOBALFOCUS;
	//dwFlags = dwFlags | DSBCAPS_GLOBALFOCUS; // 050214 - always use global sound now!

    // Load the wave file into a DirectSound buffer
	g_pGlob->dwInternalFunctionCode=15008;
	//PE: Pri. hardware buffers.
	//PE: Getting E_NOTIMPL Not implemented. ?

    //hr = g_pSoundManager->Create( &m_ptr->pSound, szFilename, dwFlags | DSBCAPS_LOCHARDWARE, guid3DAlgorithm);
	//if (FAILED(hr))
	//{
	//	hr = g_pSoundManager->Create(&m_ptr->pSound, szFilename, dwFlags, guid3DAlgorithm);
	//}

	hr = g_pSoundManager->Create(&m_ptr->pSound, szFilename, dwFlags, guid3DAlgorithm);

    if( FAILED( hr ) || hr == DS_NO_VIRTUALIZATION )
    {
		// no sound buffer
		m_SDKSoundManager.Delete ( iID );
		char cError[MAX_PATH + 20];
		sprintf(cError, "(No sound buffer,hr=%ld) %s", hr, szFilename);
		#ifndef IGNOREALLSOUNDERRORS
		if ( iSilentFail==0 ) RunTimeError(RUNTIMEERROR_SOUNDLOADFAILED, cError);
		#else
		timestampactivity(0, cError);
		#endif
        return; 
    }

#endif

	// LEEFIX - 191102 - fill with default sound data
	g_pGlob->dwInternalFunctionCode=15022;
	m_ptr->iVolume	= 100;
	m_ptr->iPan		= 100;
	m_ptr->b3D = b3DSound;

#ifdef WICKEDAUDIO
#else
    // Get the 3D buffer from the secondary buffer
	if(b3DSound)
	{
		g_pGlob->dwInternalFunctionCode=15023;
		if( FAILED( hr = m_ptr->pSound->Get3DBufferInterface( 0, &m_ptr->pDSBuffer3D ) ) )
		{
			// no 3d buffer
			m_ptr->pDSBuffer3D=NULL;
		}
		else
		{
			// Get the 3D buffer parameters
			g_pGlob->dwInternalFunctionCode=15024;
			DS3DBUFFER dsBufferParams;
			dsBufferParams.dwSize = sizeof(DS3DBUFFER);
			m_ptr->pDSBuffer3D->GetAllParameters( &dsBufferParams );

			// Set new 3D buffer parameters
			#ifndef _DEBUG
			m_ptr->pDSBuffer3D->SetAllParameters( &dsBufferParams, DS3D_DEFERRED );
			#endif
		}
	}
	else
	{
		m_ptr->pDSBuffer3D=NULL;
	}
#endif
	// got to end of command without crash
	g_pGlob->dwInternalFunctionCode=15031;
}

DARKSDK BOOL SaveWAVDataToFile(HANDLE hfile, char* wavedata, DWORD sizeofdata, DWORD nSamplesPerSec, WAVEFORMATEX wfx  )
{
#ifdef WICKEDAUDIO
	return false;
#else
	DWORD bw;

	// Prepare header
	DWORD dwRiff = ('R') + ('I'<<8) + ('F'<<16) + ('F'<<24);
	DWORD dwType = ('W') + ('A'<<8) + ('V'<<16) + ('E'<<24);

	//WAVEFORMATEX wfx;

	/*
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 1;
	wfx.nSamplesPerSec = nSamplesPerSec;
	wfx.nAvgBytesPerSec = nSamplesPerSec;
	wfx.nBlockAlign = 1;
	wfx.wBitsPerSample = 8;
	wfx.cbSize = 0;
	*/

	/*
	
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 1;
	wfx.nSamplesPerSec = 32000;
	wfx.nAvgBytesPerSec = 64000;
	wfx.nBlockAlign = 2;
	wfx.wBitsPerSample = 16;
	wfx.cbSize = 0;
	*/

	// Write out WAV file format
	DWORD dwLength = sizeofdata + 36;
	WriteFile(hfile, &dwRiff, 4, &bw, FALSE);//OFFSET 0-3 (should be 'RIFF')
	WriteFile(hfile, &dwLength, 4, &bw, FALSE);//OFFSET 4-7 size of Data Chunk
	WriteFile(hfile, &dwType, 4, &bw, FALSE);//OFFSET 8-11 (should be start of Data Chunk 'WAVE')

	// Write Out Chunk FMT
	dwType = ('f') + ('m'<<8) + ('t'<<16) + (' '<<24);
	dwLength = sizeof(WAVEFORMATEX)-2;
	WriteFile(hfile, &dwType, 4, &bw, FALSE);
	WriteFile(hfile, &dwLength, 4, &bw, FALSE);
	WriteFile(hfile, &wfx, dwLength, &bw, FALSE);

	// Write Out Chunk DATA
	dwType = ('d') + ('a'<<8) + ('t'<<16) + ('a'<<24);
	dwLength = sizeofdata;
	WriteFile(hfile, &dwType, 4, &bw, FALSE);
	WriteFile(hfile, &dwLength, 4, &bw, FALSE);
	WriteFile(hfile, wavedata, dwLength, &bw, FALSE);

	return TRUE;
#endif
}

DARKSDK BOOL DB_SaveWAVFile(char* filename, LPSTR pData, DWORD dwDataSize, DWORD nSamplesPerSec, WAVEFORMATEX wfx  )
{
#ifdef WICKEDAUDIO
	return false;
#else
	BOOL		bResult		= FALSE;
	HANDLE		hFile;

	// Open File
	hFile=GG_CreateFile(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile==INVALID_HANDLE_VALUE)
		return bResult;

	// Write File
	if(SaveWAVDataToFile(hFile, pData, dwDataSize, nSamplesPerSec, wfx ))
		bResult=TRUE;

	// Close File
	CloseHandle(hFile);

	return bResult;
#endif
}

//
// DBV1 Commands
//

DARKSDK void LoadRawSound ( LPSTR szFilename, int iID, bool bFlag, int iSilentFail, int iGlobalSound )
{
	// Uses actual or virtual file..
	char VirtualFilename[_MAX_PATH];
	#ifdef WICKEDAUDIO
	realname = szFilename;
	#endif
	strcpy(VirtualFilename, szFilename);
	//g_pGlob->UpdateFilenameFromVirtualTable( VirtualFilename);

	// replace with encrypted/customcontent if not found in regular location
	CheckForWorkshopFile ( VirtualFilename );

	// Decrypt and use media, re-encrypt
	if ( VirtualFilename && strlen(VirtualFilename) > 1 )
	{
		g_pGlob->Decrypt( VirtualFilename );
		LoadRawSoundCore ( VirtualFilename, iID, bFlag, iSilentFail, iGlobalSound );
		g_pGlob->Encrypt( VirtualFilename );
	}
}

DARKSDK void LoadSound ( LPSTR szFilename, int iID )
{
	// Sound is 2D
	LoadRawSound ( szFilename, iID, false, 0, 0 );
}

DARKSDK void LoadSound ( LPSTR szFilename, int iID, int iFlag )
{
	// Sound is 2D
	LoadRawSound ( szFilename, iID, false, 0, 0 );
}

DARKSDK void LoadSound ( LPSTR szFilename, int iID, int iFlag, int iSilentFail )
{
	// Sound is 2D
	LoadRawSound ( szFilename, iID, false, iSilentFail, 0 );
}

DARKSDK void LoadSound ( LPSTR szFilename, int iID, int iFlag, int iSilentFail, int iGlobalSound )
{
	// leeadd - 120108 - U71 - Sound is Global Sound (sound even if app not have focus)
	LoadRawSound ( szFilename, iID, false, iSilentFail, iGlobalSound );
}

DARKSDK void PlaySound ( int iID )
{
#ifdef WICKEDAUDIO
#else
	// if no sound card, leave now
	if (!g_pSoundManager) return;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return;
#endif

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return;
	}
	if ( !UpdateSoundPtr ( iID ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		#endif
		return;
	}

	// plays a sound

	// set pause to false
	m_ptr->bPause = false;
	m_ptr->bPlaying = true;
	m_ptr->bLoop = false;

#ifdef WICKEDAUDIO
	Scene&  myscene = GetScene();
	SoundComponent* sound = myscene.sounds.GetComponent(m_ptr->wickedEntity);
	if (sound != nullptr)
	{
		sound->CurveDistanceScaler = GetCurveDistanceScaler();
		sound->SetLooped(false);
		if (sound->soundinstance.IsLooped())
		{
			sound->soundinstance.SetLooped(false);
			if (sound && sound->soundResource && sound->soundResource->sound.IsValid() && sound->soundinstance.IsValid())
			{
				if (!sound->IsDisable3D())
					sound->soundinstance.type = wiAudio::SUBMIX_TYPE::SUBMIX_TYPE_SOUNDEFFECT; //SUBMIX_TYPE_SOUNDEFFECT, SUBMIX_TYPE_MUSIC,
				else
					sound->soundinstance.type = wiAudio::SUBMIX_TYPE::SUBMIX_TYPE_MUSIC; //SUBMIX_TYPE_SOUNDEFFECT, SUBMIX_TYPE_MUSIC,
				wiAudio::CreateSoundInstance(&sound->soundResource->sound, &sound->soundinstance);
			}
		}
		wiAudio::Stop(&sound->soundinstance); //Flush so restart.
		wiAudio::Play(&sound->soundinstance); //PE: Start immediately.
		sound->Play();
	}
	else
	{
		m_ptr->bPlaying = false;
	}
#else

	// mike - 2320604 - added in these 2 extra lines
	m_ptr->pSound->Stop();
	m_ptr->pSound->Reset();

	// plays a sound from start
	m_ptr->pSound->GetBuffer(0)->SetCurrentPosition(0);

	// now play the sound
	m_ptr->pSound->Play(0, NULL );
#endif
}

DARKSDK void PlaySoundOffset ( int iID, int iOffset )
{
#ifdef WICKEDAUDIO
	//PE: MAX always use iOffset=0 , so just use normal playsound.
	PlaySound(iID);
#else

	// if no sound card, leave now
	if (!g_pSoundManager) return;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return;

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return;
	}
	if ( !UpdateSoundPtr ( iID ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		#endif
		return;
	}
	if ( iOffset < 0 || iOffset > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDERROR);
		#endif
		return;
	}

	// set pause to false
	m_ptr->bPause = false;
	m_ptr->bPlaying = true;
	m_ptr->bLoop = false;

	// now play the sound
	m_ptr->pSound->Stop();
	m_ptr->pSound->Reset();

	// plays a sound from a position
	m_ptr->pSound->GetBuffer(0)->SetCurrentPosition(iOffset);

	// now play the sound
	m_ptr->pSound->Play(0, NULL );
#endif
}
uint32_t Unique_Clones = 0;
DARKSDK void CloneSound ( int iDestination, int iSource )
{
#ifdef WICKEDAUDIO
#else
	// if no sound card, leave now
	if (!g_pSoundManager) return;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return;
#endif

	if ( iSource < 1 || iSource > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return;
	}
	if ( !UpdateSoundPtr ( iSource ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		#endif
		return;
	}
	sSoundData* pSrcPtr = m_ptr;
	if ( iDestination < 1 || iDestination > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return;
	}
	if ( UpdateSoundPtr ( iDestination ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDALREADYEXISTS);
		#endif
		return;
	}

	// add structure to sound manager
	sSoundData		m_Data;							// all sound data
	memset ( &m_Data, 0, sizeof ( m_Data ) );
	memcpy ( &m_Data, pSrcPtr, sizeof ( sSoundData ) );
	m_Data.pDSBuffer3D=NULL;
	m_Data.pSound=NULL;
	m_SDKSoundManager.Add ( &m_Data, iDestination );

	// update list
	if ( !UpdateSoundPtr ( iDestination ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDCANNOTBECLONED);
		#endif
		return;
	}

	// to prevent my source data being freed!
	memset ( &m_Data, 0, sizeof ( sSoundData ) );

#ifdef WICKEDAUDIO
	//PE: Check if we can do a clone.
	bool bCloneFailed = true;
	if(pSrcPtr->wickedEntity > 0)
	{
		Scene& myscene = GetScene();
		SoundComponent* soundsrc = myscene.sounds.GetComponent(pSrcPtr->wickedEntity);
		if (soundsrc)
		{
			//soundsrc->soundResource->filedata.data()
			if (soundsrc->soundResource->filedata.size() > 0)
			{
				//PE: In MAX filename is crypted.wav all the time , so need a unique name, or wicked will reuse the crypted.wav resource.
				std::string ext = wiHelper::toUpper(wiHelper::GetExtensionFromFileName(pSrcPtr->wickedFilename));
				realname = "Clone" + std::to_string(iSource) + " " + std::to_string(Unique_Clones++) + "." + ext;
				bLoadWickedSound(pSrcPtr->wickedFilename, iDestination, pSrcPtr->b3D, 1, soundsrc->soundResource->filedata);
				return;
			}
		}
		const Entity entity = GetScene().Entity_Duplicate(pSrcPtr->wickedEntity);
		if (entity > 0)
		{
			m_ptr->wickedEntity = entity;
			strcpy(&m_ptr->wickedFilename[0], pSrcPtr->wickedFilename);
			m_ptr->b3D = pSrcPtr->b3D;
			SoundComponent* sound = GetScene().sounds.GetComponent(entity);
			if (sound != nullptr)
			{
				//PE: Make it work like old GGM , but 3D stereo is supported.
				uint32_t channels = GetSoundChannels(&sound->soundinstance);
				if (channels > 1)
				{
					m_ptr->b3D = false;
				}

				//PE: Defaults. 
				sound->SetDisable3D(1 - m_ptr->b3D);
				sound->SetLooped(false);
#ifdef REVERBTEST
				sound->soundinstance.SetEnableReverb(true);
#else
				sound->soundinstance.SetEnableReverb(false);
#endif
				sound->volume = 1.0f; //0-1
				if (pSrcPtr->b3D)
					sound->soundinstance.type = wiAudio::SUBMIX_TYPE::SUBMIX_TYPE_SOUNDEFFECT; //SUBMIX_TYPE_SOUNDEFFECT, SUBMIX_TYPE_MUSIC,
				else
					sound->soundinstance.type = wiAudio::SUBMIX_TYPE::SUBMIX_TYPE_MUSIC; //SUBMIX_TYPE_SOUNDEFFECT, SUBMIX_TYPE_MUSIC,
				if (sound && sound->soundResource && sound->soundResource->sound.IsValid() && sound->soundinstance.IsValid())
				{
					wiAudio::CreateSoundInstance(&sound->soundResource->sound, &sound->soundinstance);
					sound->Stop();
					bCloneFailed = false;
				}
			}
		}
	}
	if(bCloneFailed)
	{
		std::vector<uint8_t> emptydata;
		bLoadWickedSound(pSrcPtr->wickedFilename, iDestination, pSrcPtr->b3D, 1, emptydata);
	}
#else

	// For the copy process
    if( pSrcPtr->pSound ) pSrcPtr->pSound->Stop();

	// Duplicate sound buffer
	m_ptr->pSound = g_pSoundManager->CloneSound( pSrcPtr->pSound );

    // Get the 3D buffer from the secondary buffer
	if(pSrcPtr->b3D && m_ptr->pSound)
	{
		HRESULT hr;
		if( FAILED( hr = m_ptr->pSound->Get3DBufferInterface( 0, &m_ptr->pDSBuffer3D ) ) )
		{
			// no 3d buffer
			m_ptr->pDSBuffer3D=NULL;
		}
	}
	else
	{
		m_ptr->pDSBuffer3D=NULL;
	}
#endif
}

#ifdef WICKEDAUDIO
void setReverB(int iID, uint32_t index)
{
	if (iID < 1 || iID > MAXIMUMVALUE)
	{
		return;
	}
	if (!UpdateSoundPtr(iID))
	{
		return;
	}
	SoundComponent* sound = GetScene().sounds.GetComponent(m_ptr->wickedEntity);
	if (sound != nullptr)
	{
		if (index > 0)
		{
			sound->soundinstance.SetEnableReverb(true);
			wiAudio::SetReverb(REVERB_PRESET_CAVE);
		}
		else
		{
			sound->soundinstance.SetEnableReverb(false);
			wiAudio::SetReverb(REVERB_PRESET_DEFAULT);
		}
	}
	return;
}
#endif
DARKSDK void DeleteSound ( int iID )
{
#ifdef WICKEDAUDIO
#else
	// if no sound card, leave now
	if (!g_pSoundManager) return;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return;
#endif

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return;
	}
	if ( !UpdateSoundPtr ( iID ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		#endif
		return;
	}
#ifdef WICKEDAUDIO
	StopSound(iID);
	//PE: Delete wicked entity.
	GetScene().Entity_Remove(m_ptr->wickedEntity);
	m_ptr->wickedEntity = 0;
#endif

	// deletes a sound
	m_SDKSoundManager.Delete ( iID );
}

DARKSDK void LoopSound ( int iID, int iStart, int iEnd, int iInitialPos )
{
#ifdef WICKEDAUDIO
#else
	// if no sound card, leave now
	if (!g_pSoundManager) return;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return;
#endif

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return;
	}
	if ( !UpdateSoundPtr ( iID ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		#endif
		return;
	}

	// loops with boundaries and an initail start position

	// setup properties
	m_ptr->bPause = false; 
	m_ptr->bPlaying  = true;
	m_ptr->bLoop  = true;

#ifdef WICKEDAUDIO
	Scene& myscene = GetScene();
	SoundComponent* sound = myscene.sounds.GetComponent(m_ptr->wickedEntity);
	if (sound != nullptr)
	{
		sound->CurveDistanceScaler = GetCurveDistanceScaler();
		const bool looped = true; //true
		sound->SetLooped(looped);
		if (!sound->soundinstance.IsLooped())
		{
			sound->soundinstance.SetLooped(looped);
			if (sound && sound->soundResource && sound->soundResource->sound.IsValid() && sound->soundinstance.IsValid())
			{
				wiAudio::CreateSoundInstance(&sound->soundResource->sound, &sound->soundinstance);
			}
		}
		bool bPlaying = wiAudio::bIsReallyPlaying(&sound->soundinstance); //Flush so restart.
		if (!bPlaying || !sound->IsPlaying() || m_ptr->bLoopRestart)
		{
			wiAudio::Stop(&sound->soundinstance); //Flush and restart.
			m_ptr->bLoopRestart = false;
		}
		sound->Play();
	}
	else
	{
		m_ptr->bPlaying = false;
		m_ptr->bLoop = false;
	}
#else

	// Values used in controlling loop (in update function)
	m_ptr->iLoopStartPos = iStart;
	m_ptr->iLoopEndPos = iEnd;

	// mike - 2320604 - added in these 2 extra lines
	m_ptr->pSound->Stop();
	m_ptr->pSound->Reset();

	m_ptr->pSound->Stop();
	if(iInitialPos!=-1) m_ptr->pSound->GetBuffer(0)->SetCurrentPosition(iInitialPos);

	m_ptr->pSound->Play(0, DSBPLAY_LOOPING );
#endif
}

DARKSDK void LoopSound ( int iID )
{
	// loop
	LoopSound ( iID, -1, -1, -1 );
}

DARKSDK void LoopSound ( int iID, int iStart )
{
	// loop but return to advance start position
	LoopSound ( iID, iStart, -1, -1 );
}

DARKSDK void LoopSound ( int iID, int iStart, int iEnd )
{
	// loop and contain within boundaries
	LoopSound ( iID, iStart, iEnd, -1 );
}

DARKSDK void StopSound ( int iID )
{
#ifdef WICKEDAUDIO
#else
	// if no sound card, leave now
	if (!g_pSoundManager) return;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return;
#endif

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return;
	}
	if ( !UpdateSoundPtr ( iID ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		#endif
		return;
	}

	// stop the sound from playing
	m_ptr->bPlaying = false;
	m_ptr->bLoop = false;
	m_ptr->bPause = false;

#ifdef WICKEDAUDIO
	SoundComponent* sound = GetScene().sounds.GetComponent(m_ptr->wickedEntity);
	if (sound != nullptr)
	{
		sound->Stop();
	}
#else
	// mike - 230604 - added in these 2 extra lines
	m_ptr->pSound->Stop();
	m_ptr->pSound->Reset();

	// mike - 230604 - reset loop and pause states
	m_ptr->bLoop = false;
	m_ptr->bPause = false;

	// call the stop commands
	m_ptr->pSound->Stop();
#endif
}

DARKSDK void ResumeSound ( int iID )
{
#ifdef WICKEDAUDIO
#else
	// if no sound card, leave now
	if (!g_pSoundManager) return;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return;
#endif

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return;
	}
	if ( !UpdateSoundPtr ( iID ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		#endif
		return;
	}

	// resume a sound after it has been paused

	// set pause to false
	m_ptr->bPause = false;

#ifdef WICKEDAUDIO
	Scene& myscene = GetScene();
	SoundComponent* sound = myscene.sounds.GetComponent(m_ptr->wickedEntity);
	if (sound != nullptr)
	{
		sound->CurveDistanceScaler = GetCurveDistanceScaler();
		sound->Play();
	}
#else
	// resume playing
    if (m_ptr->bLoop)
    	m_ptr->pSound->Play(0, DSBPLAY_LOOPING );
    else
    	m_ptr->pSound->Play(0, NULL );
#endif
}

DARKSDK void PauseSound	( int iID )
{
#ifdef WICKEDAUDIO
#else
	// if no sound card, leave now
	if (!g_pSoundManager) return;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return;
#endif

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return;
	}
	if ( !UpdateSoundPtr ( iID ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		#endif
		return;
	}

	// pause a sound

	// save the pause state
	m_ptr->bPause = true;
#ifdef WICKEDAUDIO
	//PE: Pause sound.
	SoundComponent* sound = GetScene().sounds.GetComponent(m_ptr->wickedEntity);
	if (sound != nullptr)
	{
		wiAudio::Pause(&sound->soundinstance); //PE: preserves position
	}
#else

	// get the current play position
	m_ptr->pSound->Stop();
#endif
}

DARKSDK void SetSoundSpeed ( int iID, int iFrequency )
{
#ifdef WICKEDAUDIO
	//PE: Need doodler.
	//printf("tmp");
#else

	// if no sound card, leave now
	if (!g_pSoundManager) return;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return;

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return;
	}
	if ( !UpdateSoundPtr ( iID ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		#endif
		return;

	}
	// Rather than throw a runtime, if a freq is below the min or above, lets set it to the min/max instead
	/*if ( iFrequency < 100 || iFrequency > 100000 )
	{
		RunTimeError(RUNTIMEERROR_SOUNDFREQILLEGAL);
		return;
	}*/

	if ( iFrequency < 100 ) iFrequency = 100;
	if ( iFrequency > 100000 ) iFrequency = 100000;

	// set the speed of a sound

	// set the speed
	m_ptr->pSound->GetBuffer(0)->SetFrequency ( iFrequency );
#endif
}

DARKSDK void SetSoundVolume ( int iID, int iVolume )
{
#ifdef WICKEDAUDIO
#else

	// if no sound card, leave now
	if (!g_pSoundManager) return;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return;
#endif

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return;
	}
	if ( !UpdateSoundPtr ( iID ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		#endif
		return;
	}
	//PE: Not needed, we limit the range later.
	//if ( iVolume < 0 || iVolume > 100 )
	//{
	//	RunTimeError(RUNTIMEERROR_SOUNDVOLUMEILLEGAL);
	//	return;
	//}

	// set the volume property
	m_ptr->iVolume=iVolume;

#ifdef WICKEDAUDIO
	SoundComponent* sound = GetScene().sounds.GetComponent(m_ptr->wickedEntity);
	if (sound != nullptr)
	{
		float fVol = (float) iVolume;
		if (fVol < 0) fVol = 0;
		if (fVol > 100) fVol = 100;
		sound->volume = fVol / 100.0f; //0-1 range.
		//wiAudio::SetVolume(sound->volume,&sound->soundinstance);
	}
#else
	float newvolume = (int)( (float) -9600.0f * (float) ( (float) (100.0f - iVolume) / 100.0f));

	if (newvolume < DSBVOLUME_MIN) newvolume = DSBVOLUME_MIN;
	if (newvolume > DSBVOLUME_MAX) newvolume = DSBVOLUME_MAX;

	if (iVolume <= 0) {
		newvolume = DSBVOLUME_MIN;
	}
	else if (iVolume >= 100) {
		newvolume = DSBVOLUME_MAX;
	}
	else {
		newvolume *= 0.75; //Get it closer to zero.
	}
	m_ptr->pSound->GetBuffer(0)->SetVolume (newvolume);
#endif
}

/*
void dbSetSoundVolumeEx ( int iID, int iDecibels )
{
#ifdef WICKEDAUDIO
#else
	// if no sound card, leave now
	if (!g_pSoundManager) return;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return;

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return;
	}
	if ( !UpdateSoundPtr ( iID ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		#endif
		return;
	}
	
	// set the volume

	// set the volume property
	m_ptr->iVolume=iDecibels;
	m_ptr->pSound->GetBuffer(0)->SetVolume ( iDecibels );
#endif
}
*/

DARKSDK void RecordSound ( int iID, int iCaptureDuration )
{
#ifdef WICKEDAUDIO
#else

	// if no sound card, leave now
	if (!g_pSoundManager) return;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return;
#endif

	// lee, this should eventually be replaced with soundcapture code!!

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return;
	}

    // u74b7 - If currently recording, stop it so we can start a new one
    //         Basically, if the sound id is set, we ARE recording.
    if (g_iSoundToRecordOver > 0)
    {
        StopRecordingSound();
    }

	// delete any existing sound
	m_SDKSoundManager.Delete ( iID );

	// start recording file to temp wav
    MCI_OPEN_PARMS mciOpenParms;

    // Open a waveform-audio device with a new file for recording.
    mciOpenParms.lpstrElementName = "";
    mciOpenParms.lpstrDeviceType = "waveaudio";
    if (mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE, (DWORD)(LPVOID) &mciOpenParms))
    {
        // Failed to open device; don't close it, just return error.
		return;
    }

    // The device opened successfully; get the device ID.
    g_wDeviceID = mciOpenParms.wDeviceID;

    // Begin recording
    MCI_RECORD_PARMS mciRecordParms;
    mciRecordParms.dwTo = iCaptureDuration;
    if (mciSendCommand(g_wDeviceID, MCI_RECORD, MCI_TO, (DWORD)(LPVOID) &mciRecordParms) == 0)
    {
        // u74b7 - Successfully sent the record message, so record the ID
	    g_iSoundToRecordOver=iID;
    }
    else
    {
        // u74b7 - Failed to send the record message, so shut everything down
        mciSendCommand(g_wDeviceID, MCI_CLOSE, 0, NULL);
    }
}

DARKSDK void RecordSound ( int iID )
{
	RecordSound ( iID, 5000 );
}

DARKSDK void StopRecordingSound ( void )
{
#ifdef WICKEDAUDIO
#else
	// if no sound card, leave now
	if (!g_pSoundManager) return;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return;
#endif

    // u74b7 - if not currently recording, silently fail
    if (g_iSoundToRecordOver == 0)
        return;

    // Generate a temporary filename for loading
	char path[_MAX_PATH];
	DB_GetWinTemp(path);
	char file[_MAX_PATH];
	wsprintf(file, "%s\\tempfile%d.wav", path, g_iSoundToRecordOver);

	// end recording of temp wav
	mciSendCommand(g_wDeviceID, MCI_STOP, 0, NULL);

    // Save the recording to a file named TEMPFILE.WAV. Wait for the operation to complete before continuing.
    MCI_SAVE_PARMS mciSaveParms;
    mciSaveParms.lpfilename = file;
    if (mciSendCommand(g_wDeviceID, MCI_SAVE, MCI_SAVE_FILE | MCI_WAIT, (DWORD)(LPVOID) &mciSaveParms) == 0)
    {
	    // delete any existing sound
	    m_SDKSoundManager.Delete ( g_iSoundToRecordOver );

	    // if file exists
	    if(DB_FileExist(file))
	    {
		    // load temp wav to sound
		    LoadRawSound( file, g_iSoundToRecordOver, false, 0, 0 );
		    m_ptr->bRecorded=true;
	    }
    }

    // finish MCI operation
    mciSendCommand(g_wDeviceID, MCI_CLOSE, 0, NULL);

    // u74b7 - Reset ID to signal that recording has completed.
    g_iSoundToRecordOver = 0;
}

DARKSDK void SaveSound ( LPSTR szFilename, int iID )
{
#ifdef WICKEDAUDIO
#else
	// if no sound card, leave now
	if (!g_pSoundManager) return;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return;
#endif

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return;
	}
	if ( !UpdateSoundPtr ( iID ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		#endif
		return;
	}
	if(m_ptr->bRecorded)
	{
		// Was it recorded?
		char path[_MAX_PATH];
		DB_GetWinTemp(path);
		char file[_MAX_PATH];
		wsprintf(file, "%s\\tempfile%d.wav", path, iID);
		if(DB_FileExist(file)==false)
		{
			m_ptr->bRecorded=false;
		}

		// Copy if sound was recorded
		if(m_ptr->bRecorded)
		{
			//CopyFile ( file,  szFilename, FALSE ); // lee - 270715 - replaced OS copy with DB copy
			CopyAFile ( file,  szFilename );
		}
	}
	if(m_ptr->bRecorded==false)
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDSAVEFAILED);
		#endif
		return;
	}
}

DARKSDK void Load3DSound ( LPSTR szFilename, int iID )
{
	// Sound is 3D
	LoadRawSound ( szFilename, iID, true, 0, 0 );
}

DARKSDK void Load3DSound ( LPSTR szFilename, int iID, int iSilentFail )
{
	// Sound is 3D
	LoadRawSound ( szFilename, iID, true, iSilentFail, 0 );
}

DARKSDK void Load3DSound ( LPSTR szFilename, int iID, int iSilentFail, int iGlobalSound )
{
	// leeadd - 120108 - U&1 - Sound is 3D
	LoadRawSound ( szFilename, iID, true, iSilentFail, iGlobalSound );
}

DARKSDK void PositionSound ( int iID, float fX, float fY, float fZ )
{
#ifdef WICKEDAUDIO
#else

	//return;

	// if no sound card, leave now
	if (!g_pSoundManager) return;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return;
#endif

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return;
	}
	if ( !UpdateSoundPtr ( iID ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		#endif
		return;
	}
#ifdef WICKEDAUDIO
	m_ptr->vecPosition = GGVECTOR3(fX / 100.0f, fY / 100.0f, fZ / 100.0f);
	SoundComponent* sound = GetScene().sounds.GetComponent(m_ptr->wickedEntity);
	if (sound != nullptr)
	{
		Entity entity = m_ptr->wickedEntity;
		TransformComponent* transform = GetScene().transforms.GetComponent(entity);
		if (transform != nullptr)
		{
			transform->scale_local = XMFLOAT3(1, 1, 1);
			transform->rotation_local = XMFLOAT4(0, 0, 0, 0);
			transform->translation_local = XMFLOAT3(fX, fY, fZ);
			transform->SetDirty();
			transform->UpdateTransform();
		}
	}
#else
	if ( m_ptr->pDSBuffer3D==NULL )
		return;

	if (!m_ptr->bPlaying)
		return;

	// Adjust distance
	fX/=100.0f;
	fY/=100.0f;
	fZ/=100.0f;

	// positions a sound in 3D space
	m_ptr->vecPosition = GGVECTOR3 ( fX, fY, fZ );
	if ( fabs ( GGVec3Length ( &(m_ptr->vecLast - m_ptr->vecPosition) ) ) > 0.09f )
	{
		#ifndef _DEBUG
		m_ptr->pDSBuffer3D->SetPosition ( fX, fY, fZ, DS3D_DEFERRED );
		#endif
		m_ptr->vecLast = m_ptr->vecPosition;
	}
#endif
}

// listener is a hog - only update if changes
float g_fListenQuickScale = 0.0f;
GGVECTOR3 g_vListenQuickPos = GGVECTOR3(-1, -1, -1);
GGVECTOR3 g_vListenQuickAngle = GGVECTOR3(-1, -1, -1);

DARKSDK void ResetListener ( void )
{
	g_fListenQuickScale = 0.0f;
	g_vListenQuickPos = GGVECTOR3(-1, -1, -1);
	g_vListenQuickAngle = GGVECTOR3(-1, -1, -1);
}

DARKSDK void PositionListener ( float fX, float fY, float fZ )
{
#ifdef WICKEDAUDIO
	//PE: Always camera for now.
	const wiScene::CameraComponent& camera = wiScene::GetCamera();
	vecListenerPosition = GGVECTOR3(fX / 100.0f, fY / 100.0f, fZ / 100.0f);
#else
	//return;
	if ( pDSListener )
	{
		// Adjust distance
		fX/=100.0f;
		fY/=100.0f;
		fZ/=100.0f;

		// store the position and set it
		vecListenerPosition = GGVECTOR3 ( fX, fY, fZ );
		//PE: xyz / 100 so 5.0 = 500 units, think we need to update faster :)
		if (GGVec3Length(&(vecListenerPosition - g_vListenQuickPos)) > 0.5f) //5.0f
		{
			g_vListenQuickPos = vecListenerPosition;
			#ifndef _DEBUG
			pDSListener->SetPosition (fX, fY, fZ, DS3D_DEFERRED);
			#endif
		}
	}
#endif
}

DARKSDK void RotateListener ( float fX, float fY, float fZ )
{
#ifdef WICKEDAUDIO
	//PE: Always camera for now.
	vecListenerAngle.x = fX;
	vecListenerAngle.y = fY;
	vecListenerAngle.z = fZ;
#else
	//return;
	if ( pDSListener )
	{
		// for reference
		vecListenerAngle.x = fX;
		vecListenerAngle.y = fY;
		vecListenerAngle.z = fZ;

		// quicker method
		float fOldAngle = fabs(vecListenerAngle.x) + fabs(vecListenerAngle.y) + fabs(vecListenerAngle.z);
		float fNewAngle = fabs(g_vListenQuickAngle.x) + fabs(g_vListenQuickAngle.y) + fabs(g_vListenQuickAngle.z);
		if (fabs(fabs(fNewAngle) - fabs(fOldAngle)) > 5.0f || g_vListenQuickAngle.x == -1)
		{
			// record new angle
			g_vListenQuickAngle = vecListenerAngle;

			// Set Angle Rotation
			GGMATRIX matFront, matTop, matTemp, matRot;

			// Convert angle formats
			fX = GGToRadian (fX);
			fY = GGToRadian (fY);
			fZ = GGToRadian (fZ);

			// Set Front and Top
			ZeroMemory(&matFront, sizeof(GGMATRIX));
			matFront._41 = 0.0f;
			matFront._42 = 0.0f;
			matFront._43 = 1.0f;
			ZeroMemory(&matTop, sizeof(GGMATRIX));
			matTop._41 = 0.0f;
			matTop._42 = 1.0f;
			matTop._43 = 0.0f;

			// Produce and combine the rotation matrices.
			GGMatrixIdentity(&matRot);
			GGMatrixRotationX(&matTemp, fX);
			GGMatrixMultiply(&matRot, &matTemp, &matRot);
			GGMatrixRotationY(&matTemp, fY);
			GGMatrixMultiply(&matRot, &matTemp, &matRot);
			GGMatrixRotationZ(&matTemp, fZ);
			GGMatrixMultiply(&matRot, &matTemp, &matRot);

			// Apply the rotation matrices to complete the matrix.
			GGMatrixMultiply(&matFront, &matFront, &matRot);
			GGMatrixMultiply(&matTop, &matTop, &matRot);

			// rotate the listener data
			#ifndef _DEBUG
			pDSListener->SetOrientation(matFront._41, matFront._42, matFront._43, matTop._41, matTop._42, matTop._43, DS3D_DEFERRED);
			#endif
		}
	}
#endif
}

DARKSDK void ScaleListener ( float fScale )
{
#ifdef WICKEDAUDIO
	//PE: Always camera for now.
#else

	if ( pDSListener )
	{
		if (fScale != g_fListenQuickScale )
		{
			// check all sounds and alter min/max defaults
			g_fListenQuickScale = fScale;
			link* check = m_SDKSoundManager.GetList();
			while (check)
			{
				sSoundData* ptr = NULL;
				ptr = m_SDKSoundManager.GetData (check->id);
				if (ptr == NULL) continue;

				// scale min and max (3000 is the default scene camera range)
				if (ptr->pDSBuffer3D)
				{
					#ifndef _DEBUG
					ptr->pDSBuffer3D->SetMinDistance(DS3D_DEFAULTMINDISTANCE * fScale, DS3D_DEFERRED);
					ptr->pDSBuffer3D->SetMaxDistance(3000.0f * fScale, DS3D_DEFERRED);
					#endif
				}

				// Next sound
				check = check->next;
			}
		}
	}
#endif
}

DARKSDK void SetEAX ( int iEffect ) 
{
	// Not Implemented in DBPRO V1 RELEASE
	#ifndef IGNOREALLSOUNDERRORS
	RunTimeError(RUNTIMEERROR_COMMANDNOWOBSOLETE);
	#endif
}

DARKSDK int SoundExist ( int iID )
{
#ifdef WICKEDAUDIO
#else
	// if no sound card, leave now
	if (!g_pSoundManager) return 0;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return 0;
#endif

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return 0;
	}

	// returns true if the sound exists

	// get data
	if ( !UpdateSoundPtr ( iID ) )
		return 0;
	
	return 1;
}

DARKSDK int SoundType ( int iID )
{
#ifdef WICKEDAUDIO
#else
	// if no sound card, leave now
	if (!g_pSoundManager) return 0;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return 0;
#endif

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return 0;
	}
	if ( !UpdateSoundPtr ( iID ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		#endif
		return 0;
	}

	// get the sound type
	
	// check the 3D flag
	if ( m_ptr->b3D )
		return 1;

	return 0;
}

DARKSDK int SoundPlaying ( int iID )
{
#ifdef WICKEDAUDIO
#else
	// if no sound card, leave now
	if (!g_pSoundManager) return 0;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return 0;
#endif

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return 0;
	}
	if ( !UpdateSoundPtr ( iID ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		#endif
		return 0;
	}

#ifdef WICKEDAUDIO
	SoundComponent* sound = GetScene().sounds.GetComponent(m_ptr->wickedEntity);
	if (sound != nullptr)
	{ 
		//PE: This is not correct need to check if it has finish.
		//PE: OnStreamEnd fixed it :)
		//uint32_t cf = wiAudio::GetCallBackF(&sound->soundinstance);
		//uint32_t cs = wiAudio::GetCallBackS(&sound->soundinstance);
		bool bPlaying = wiAudio::bIsReallyPlaying(&sound->soundinstance); //Flush so restart.
		if (!m_ptr->bPlaying)
			bPlaying = false;
		return(bPlaying);
	}
#else
	// is the sound playing
	if (m_ptr->bPlaying)
		return 1;
	else
		return 0;
#endif
}

DARKSDK int SoundLooping ( int iID )
{
#ifdef WICKEDAUDIO
#else
	// if no sound card, leave now
	if (!g_pSoundManager) return 0;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return 0;
#endif

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return 0;
	}
	if ( !UpdateSoundPtr ( iID ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		#endif
		return 0;
	}

	// is the sound looping

	// return loop flag
	return m_ptr->bLoop;
}

DARKSDK int SoundPaused ( int iID )
{
#ifdef WICKEDAUDIO
#else
	// if no sound card, leave now
	if (!g_pSoundManager) return 0;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return 0;
#endif

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return 0;
	}
	if ( !UpdateSoundPtr ( iID ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		#endif
		return 0;
	}

	// is the sound paused

	// return pause flag
	return m_ptr->bPause;
}

DARKSDK int SoundPan ( int iID )
{
#ifdef WICKEDAUDIO
#else
	// if no sound card, leave now
	if (!g_pSoundManager) return 0;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return 0;
#endif

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return 0;
	}
	if ( !UpdateSoundPtr ( iID ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		#endif
		return 0;
	}

	// get the pan of a sound
	if ( m_ptr->b3D )
		return 0;

#ifdef WICKEDAUDIO
	return 0;
#else

	long pan;
	
	// get the pan
	m_ptr->pSound->GetBuffer(0)->GetPan ( &pan );

	// return
	return ( int ) pan;
#endif
}

DARKSDK int SoundSpeed ( int iID )
{
#ifdef WICKEDAUDIO
#else
	// if no sound card, leave now
	if (!g_pSoundManager) return 0;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return 0;
#endif
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return 0;
	}
	if ( !UpdateSoundPtr ( iID ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		#endif
		return 0;
	}

	// get speed of a sound
	DWORD dwSpeed;
#ifdef WICKEDAUDIO
	return(0);
#else
	// get the speed
	m_ptr->pSound->GetBuffer(0)->GetFrequency ( &dwSpeed );

	// return
	return ( int ) dwSpeed;
#endif
}

DARKSDK int SoundVolume ( int iID )
{
#ifdef WICKEDAUDIO
#else
	// if no sound card, leave now
	if (!g_pSoundManager) return 0;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return 0;
#endif

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return 0;
	}
	if ( !UpdateSoundPtr ( iID ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		#endif
		return 0;
	}

	// get sound volume

	// return volume
	return m_ptr->iVolume;
}

DARKSDK DWORD SoundPositionXEx ( int iID )
{
#ifdef WICKEDAUDIO
#else
	// if no sound card, leave now
	if (!g_pSoundManager) return 0;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return 0;
#endif

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return 0;
	}
	if ( !UpdateSoundPtr ( iID ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		#endif
		return 0;
	}

	// get sound x position

	// return x position
	float fResult = m_ptr->vecPosition.x*100.0f;
	return *(DWORD*)&fResult;
}

DARKSDK DWORD SoundPositionYEx ( int iID )
{
#ifdef WICKEDAUDIO
#else
	// if no sound card, leave now
	if (!g_pSoundManager) return 0;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return 0;
#endif

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return 0;
	}
	if ( !UpdateSoundPtr ( iID ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		#endif
		return 0;
	}

	// get sound y position

	// return y position
	float fResult = m_ptr->vecPosition.y*100.0f;
	return *(DWORD*)&fResult;
}

DARKSDK DWORD SoundPositionZEx ( int iID )
{
#ifdef WICKEDAUDIO
#else
	// if no sound card, leave now
	if (!g_pSoundManager) return 0;

	// mike - 010904 - 5.7 - silent return if sound card does not exist
	if ( !g_pSoundManager->GetExists ( ) )
		return 0;
#endif

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return 0;
	}
	if ( !UpdateSoundPtr ( iID ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		#endif
		return 0;
	}

	// get sound z position

	// return z position
	float fResult = m_ptr->vecPosition.z*100.0f;
	return *(DWORD*)&fResult;
}

DARKSDK DWORD ListenerPositionXEx ( void )
{
	// get listener x position
	float fResult = vecListenerPosition.x*100.0f;
	return *(DWORD*)&fResult;
}

DARKSDK DWORD ListenerPositionYEx ( void )
{
	// get listener y position
	float fResult = vecListenerPosition.y*100.0f;
	return *(DWORD*)&fResult;
}

DARKSDK DWORD ListenerPositionZEx ( void )
{
	// get listener z position
	float fResult = vecListenerPosition.z*100.0f;
	return *(DWORD*)&fResult;
}

DARKSDK DWORD ListenerAngleXEx ( void )
{
	// get angle x
	return *(DWORD*)&vecListenerAngle.x;
}

DARKSDK DWORD ListenerAngleYEx ( void )
{
	// get angle y
	return *(DWORD*)&vecListenerAngle.y;
}

DARKSDK DWORD ListenerAngleZEx ( void )
{
	// get angle z
	return *(DWORD*)&vecListenerAngle.z;
}

//
// Data Access Functions
//

DARKSDK void GetSoundData( int iID, DWORD* dwBitsPerSecond, DWORD* Frequency, DWORD* Duration, LPSTR* pData, DWORD* dwDataSize, bool bLockData, WAVEFORMATEX* wfx )
{
#ifdef WICKEDAUDIO
	//PE: Only used by memblocks that we do not use in MAX.
#else

	// mike - 300305 - new param for waveformat

	// Read Data
	if(bLockData==true)
	{
		// get ptr to sound
		if ( !UpdateSoundPtr ( iID ) )
			return;

		// may need to extract data from a clones real sound

		// Sound Buffer Data
		DWORD dwSourceSize = m_ptr->pSound->GetBufferSize();
		LPSTR pSourcePtr = m_ptr->pSound->GetBufferData();

		// data
	    WAVEFORMATEX* pwfx = m_ptr->pSound->m_pWaveFile->GetFormat();

		// mike - 300305 - need to store extra wave data
		memcpy ( wfx, pwfx, sizeof ( WAVEFORMATEX ) );

		*Frequency = pwfx->nAvgBytesPerSec/(pwfx->wBitsPerSample/8);
		*dwBitsPerSecond = pwfx->nAvgBytesPerSec*8;
		*Duration = dwSourceSize/pwfx->nAvgBytesPerSec;

		// create memory
		*pData = new char[dwSourceSize];
		*dwDataSize = dwSourceSize;

		// copy sound data
		LPSTR pPtr = *pData;
		memcpy(pPtr, pSourcePtr, dwSourceSize);
	}
	else
	{
		// free memory
		delete *pData;
	}
#endif
}

DARKSDK void SetSoundData( int iID, DWORD dwBitsPerSecond, DWORD Frequency, DWORD Duration, LPSTR pData, DWORD dwDataSize, WAVEFORMATEX wfx )
{
#ifdef WICKEDAUDIO
	//PE: Only used by memblocks that we do not use in MAX.
#else

	// mike - 300305 - new param for waveformat

	// Delete if exists
	if ( UpdateSoundPtr ( iID ) ) DeleteSound ( iID );

	// add structure to sound manager
	sSoundData m_Data;	
	memset ( &m_Data, 0, sizeof ( m_Data ) );
	m_SDKSoundManager.Add ( &m_Data, iID );
	if ( !UpdateSoundPtr ( iID ) ) return;

	// fill with sound data
	m_ptr->iVolume=100;
	m_ptr->iPan=100;

	// temo sound file
	char path[_MAX_PATH];
	DB_GetWinTemp(path);
	char szFilename[_MAX_PATH];

	// U69 - 050608 - add folder seperator only if required
	strcpy ( szFilename, path );
	if ( strncmp ( path + strlen ( path ) - 1, "\\", 1 )!=NULL ) strcat ( szFilename, "\\" );
	strcat ( szFilename, "tempfile.wav" );

	// sample quality
	DWORD nSamplesPerSec=Frequency;

	// save sound
	DeleteFile(szFilename);
	DB_SaveWAVFile(szFilename, pData, dwDataSize, nSamplesPerSec, wfx );

    // Load the wave file into a DirectSound buffer
    HRESULT hr = g_pSoundManager->Create( &m_ptr->pSound, szFilename, DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY, SOUND_GUID_NULL );  
    if( FAILED( hr ) )
	{
		// no sound buffer
		m_SDKSoundManager.Delete ( iID );
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDLOADFAILED,szFilename);
		#endif
        return;
	}

	// Delete temo sound file
	DeleteFile(szFilename);
#endif
}

// MIKE - 090104
//PE: Not used.
/*
DARKSDK LPDIRECTSOUND8 GetSoundInterface ( void )
{
	// if no sound card, leave now
	if (!g_pSoundManager) return NULL;

	return g_pSoundManager->m_pDS;
}
*/
// MIKE - 100204
//PE: Not used.
/*
DARKSDK IDirectSound3DBuffer8* GetSoundBuffer ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNUMBERILLEGAL);
		#endif
		return 0;
	}
	if ( !UpdateSoundPtr ( iID ) )
	{
		#ifndef IGNOREALLSOUNDERRORS
		RunTimeError(RUNTIMEERROR_SOUNDNOTEXIST);
		#endif
		return 0;
	}

	return m_ptr->pDSBuffer3D;
}
*/

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// DARK SDK SECTION //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE

void ConstructorSound ( HWND hWnd )
{
	Constructor ( hWnd );
}

void DestructorSound ( void )
{
	Destructor ( );
}

void SetErrorHandlerSound	( LPVOID pErrorHandlerPtr )
{
	SetErrorHandler ( pErrorHandlerPtr );
}

void PassCoreDataSound ( LPVOID pGlobPtr )
{
	PassCoreData ( pGlobPtr );
}

void RefreshGRAFIXSound ( int iMode )
{
	RefreshGRAFIX ( iMode );
}

void UpdateSound ( void )
{
	Update ( );
}

void dbLoadSound ( LPSTR szFilename, int iID )
{
	LoadSound ( szFilename, iID );
}

void dbLoadSound ( LPSTR szFilename, int iID, int iFlag )
{
	LoadSound ( szFilename, iID, iFlag );
}

void dbCloneSound ( int iDestination, int iSource )
{
	CloneSound ( iDestination, iSource );
}

void dbDeleteSound ( int iID )
{
	DeleteSound ( iID );
}

void dbPlaySound ( int iID )
{
	PlaySound ( iID );
}

void dbPlaySound ( int iID, int iOffset )
{
	PlaySoundOffset ( iID, iOffset );
}

void dbLoopSound ( int iID )
{
	LoopSound ( iID );
}

void dbLoopSound ( int iID, int iStart )
{
	LoopSound ( iID, iStart );
}

void dbLoopSound ( int iID, int iStart, int iEnd )
{
	LoopSound ( iID, iStart, iEnd );
}

void dbLoopSound ( int iID, int iStart, int iEnd, int iInitialPos )
{
	LoopSound ( iID, iStart, iEnd, iInitialPos );
}

void dbStopSound ( int iID )
{
	StopSound ( iID );
}

void dbResumeSound ( int iID )
{
	ResumeSound ( iID );
}

void dbPauseSound ( int iID )
{
	PauseSound ( iID );
}

void dbSetSoundPan ( int iID, int iPan )
{
	SetSoundPan ( iID, iPan );
}

void dbSetSoundSpeed ( int iID, int iFrequency )
{
	SetSoundSpeed ( iID, iFrequency );
}

void dbSetSoundVolume ( int iID, int iVolume )
{
	SetSoundVolume ( iID, iVolume );
}

void dbRecordSound ( int iID )
{
	RecordSound ( iID );
}

void dbRecordSound ( int iID, int iCaptureDuration )
{
	RecordSound ( iID, iCaptureDuration );
}

void dbStopRecordingSound ( void )
{
	StopRecordingSound ( );
}

void dbSaveSound ( LPSTR szFilename, int iID )
{
	SaveSound ( szFilename, iID );
}

void dbLoad3DSound ( LPSTR szFilename, int iID )
{
	Load3DSound ( szFilename, iID );
}

void dbLoad3DSound ( LPSTR szFilename, int iID, int iSilentFail )
{
    Load3DSound ( szFilename, iID, iSilentFail );
}

void dbLoad3DSound ( LPSTR szFilename, int iID, int iSilentFail, int iGlobalSound )
{
    Load3DSound ( szFilename, iID, iSilentFail, iGlobalSound );
}

void dbPositionSound ( int iID, float fX, float fY, float fZ )
{
	PositionSound (  iID,  fX,  fY,  fZ );
}

void dbPositionListener ( float fX, float fY, float fZ )
{
	PositionListener (  fX,  fY,  fZ );
}

void dbRotateListener ( float fX, float fY, float fZ )
{
	RotateListener (  fX,  fY,  fZ );
}

void dbScaleListener ( float fScale )
{
	ScaleListener (  fScale );
}

void dbSetEAX ( int iEffect )
{
	SetEAX ( iEffect );
}

int dbSoundExist ( int iID )
{
	return GetSoundExist ( iID );
}

int dbSoundType ( int iID )
{
	return GetSoundType ( iID );
}

int dbSoundPlaying ( int iID )
{
	return GetSoundPlaying ( iID );
}

int dbSoundLooping ( int iID )
{
	return GetSoundLooping ( iID );
}

int dbSoundPaused ( int iID )
{
	return GetSoundPaused ( iID );
}

int dbSoundPan ( int iID )
{
	return GetSoundPan ( iID );
}

int dbSoundSpeed ( int iID )
{
	return GetSoundSpeed ( iID );
}

int dbSoundVolume ( int iID )
{
	return GetSoundVolume ( iID );
}

float dbSoundPositionX ( int iID )
{
	DWORD dwReturn = GetSoundPositionXEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbSoundPositionY ( int iID )
{
	DWORD dwReturn = GetSoundPositionYEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbSoundPositionZ ( int iID )
{
	DWORD dwReturn = GetSoundPositionZEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbListenerPositionX ( void )
{
	DWORD dwReturn = GetListenerPositionXEx ( );
	
	return *( float* ) &dwReturn;
}

float dbListenerPositionY ( void )
{
	DWORD dwReturn = GetListenerPositionYEx ( );
	
	return *( float* ) &dwReturn;
}

float dbListenerPositionZ ( void )
{
	DWORD dwReturn = GetListenerPositionZEx ( );
	
	return *( float* ) &dwReturn;
}

float dbListenerAngleX ( void )
{
	DWORD dwReturn = GetListenerAngleXEx ( );
	
	return *( float* ) &dwReturn;
}

float dbListenerAngleY ( void )
{
	DWORD dwReturn = GetListenerAngleYEx ( );
	
	return *( float* ) &dwReturn;
}

float dbListenerAngleZ ( void )
{
	DWORD dwReturn = GetListenerAngleZEx ( );
	
	return *( float* ) &dwReturn;
}

float dbGetSoundPositionX ( int iID )
{
	return GetSoundPositionX ( iID );
}

float dbGetSoundPositionY ( int iID )
{
	return GetSoundPositionY ( iID );
}

float dbGetSoundPositionZ ( int iID )
{
	return GetSoundPositionZ ( iID );
}

float dbGetListenerPositionX ( int iID )
{
	return 0.0f;

	//return GetListenerPositionX ( iID );
}

float dbGetListenerPositionY ( int iID )
{
	return 0.0f;

	//return GetListenerPositionY ( iID );
}

float dbGetListenerPositionZ ( int iID )
{
	return 0.0f;

	//return GetListenerPositionZ ( iID );
}

float dbGetListenerAngleX ( int iID )
{
	return 0.0f;
	//return GetListenerAngleX ( iID );
}

float dbGetListenerAngleY ( int iID )
{
	return 0.0f;
	//return GetListenerAngleY ( iID );
}

float dbGetListenerAngleZ ( int iID )
{
	return 0.0f;
	//return GetListenerAngleZ ( iID );
}

void dbGetSoundData	( int iID, DWORD* dwBitsPerSecond, DWORD* Frequency, DWORD* Duration, LPSTR* pData, DWORD* dwDataSize, bool bLockData, WAVEFORMATEX* wfx )
{
	GetSoundData ( iID, dwBitsPerSecond, Frequency, Duration, pData, dwDataSize, bLockData, wfx );
}

void dbSetSoundData	( int iID, DWORD dwBitsPerSecond, DWORD Frequency, DWORD Duration, LPSTR pData, DWORD dwDataSize, WAVEFORMATEX wfx )
{
	SetSoundData ( iID, dwBitsPerSecond, Frequency, Duration, pData, dwDataSize, wfx );
}

#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
