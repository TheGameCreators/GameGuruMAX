// Copyright Epic Games, Inc. All Rights Reserved.

// Modified for leanness Win64
#include "Platform.h"
#include "Windows/eos_Windows.h"
#include <string>
#include "Common-Keys.h"

// Globals
EOS_HPlatform FPlatform::PlatformHandle = nullptr;
bool FPlatform::bIsInit = false;
bool FPlatform::bIsShuttingDown = false;

// Externs
extern char g_pStartingDirectory[260];

bool FPlatform::Create()
{
	bIsInit = false;

	// Create platform instance
	EOS_Platform_Options PlatformOptions = {};
	PlatformOptions.ApiVersion = EOS_PLATFORM_OPTIONS_API_LATEST;
	PlatformOptions.bIsServer = false;
	PlatformOptions.EncryptionKey = 0;
	PlatformOptions.OverrideCountryCode = nullptr;
	PlatformOptions.OverrideLocaleCode = nullptr;
	PlatformOptions.Flags = EOS_PF_WINDOWS_ENABLE_OVERLAY_D3D9 | EOS_PF_WINDOWS_ENABLE_OVERLAY_D3D10 | EOS_PF_WINDOWS_ENABLE_OVERLAY_OPENGL;
	PlatformOptions.CacheDirectory = NULL;

	// special app specific IDs
	PlatformOptions.ProductId = EOSPRODUCTID;
	PlatformOptions.SandboxId = EOSSANDBOXID;
	PlatformOptions.DeploymentId = EOSDEPLOYMENTID;
	PlatformOptions.ClientCredentials.ClientId = EOSCLIENTID;
	PlatformOptions.ClientCredentials.ClientSecret = EOSCLIENTSECRET;

	// options for RTC
	EOS_Platform_RTCOptions RtcOptions = { 0 };
	RtcOptions.ApiVersion = EOS_PLATFORM_RTCOPTIONS_API_LATEST;
	RtcOptions.BackgroundMode = EOS_ERTCBackgroundMode::EOS_RTCBM_LeaveRooms;
	std::string XAudio29DllPath = g_pStartingDirectory;// "D:\\DEV\\BUILDS\\GameGuru Wicked MAX Build Area\\Max";
	XAudio29DllPath.append("/xaudio2_9redist.dll");
	EOS_Windows_RTCOptions WindowsRtcOptions = { 0 };
	WindowsRtcOptions.ApiVersion = EOS_WINDOWS_RTCOPTIONS_API_LATEST;
	WindowsRtcOptions.XAudio29DllPath = XAudio29DllPath.c_str();
	RtcOptions.PlatformSpecificOptions = &WindowsRtcOptions;
	PlatformOptions.RTCOptions = &RtcOptions;
	if (!PlatformOptions.IntegratedPlatformOptionsContainerHandle)
	{
		EOS_IntegratedPlatform_CreateIntegratedPlatformOptionsContainerOptions CreateIntegratedPlatformOptionsContainerOptions = {};
		CreateIntegratedPlatformOptionsContainerOptions.ApiVersion = EOS_INTEGRATEDPLATFORM_CREATEINTEGRATEDPLATFORMOPTIONSCONTAINER_API_LATEST;
		EOS_EResult Result = EOS_IntegratedPlatform_CreateIntegratedPlatformOptionsContainer(&CreateIntegratedPlatformOptionsContainerOptions, &PlatformOptions.IntegratedPlatformOptionsContainerHandle);
		if (Result != EOS_EResult::EOS_Success)
		{
			// failed to create integrated platform options container
		}
	}
	PlatformOptions.Reserved = NULL;

	// finally create the platform
	PlatformHandle = EOS_Platform_Create(&PlatformOptions);
	if (PlatformOptions.IntegratedPlatformOptionsContainerHandle)
	{
		EOS_IntegratedPlatformOptionsContainer_Release(PlatformOptions.IntegratedPlatformOptionsContainerHandle);
	}
	if (PlatformHandle == nullptr)
	{
		return false;
	}

	// success
	bIsInit = true;
	return true;
}

void FPlatform::Release()
{
	bIsInit = false;
	PlatformHandle = nullptr;
	bIsShuttingDown = true;
}

void FPlatform::Update()
{
	if (PlatformHandle)
	{
		EOS_Platform_Tick(PlatformHandle);
	}
}
