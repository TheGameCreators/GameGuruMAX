// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "eos_sdk.h"

/**
* Creates EOS SDK Platform
*/
class FPlatform
{
public:
	/**
	* Constructor
	*/
	FPlatform() noexcept(false) {};

	/**
	* No copying or copy assignment allowed for this class.
	*/
	FPlatform(FPlatform const&) = delete;
	FPlatform& operator=(FPlatform const&) = delete;

	/**
	* Destructor
	*/
	virtual ~FPlatform() {};

	/**
	* Creates EOS SDK platform
	*/
	static bool Create();

	/**
	* Releases EOS SDK platform
	*/
	static void Release();

	/**
	* Updates EOS SDK platform
	*/
	static void Update();
		
	/**
	* Accessor for EOS SDK platform handle
	*
	* @return EOS SDK platform handle
	*/
	static EOS_HPlatform const GetPlatformHandle() { return PlatformHandle; };

	/**
	* Accessor for Initialized flag
	*
	* @return True if Initialized is set
	*/
	static bool IsInitialized() { return bIsInit; }

private:
	/** Platform handle */
	static EOS_HPlatform PlatformHandle;

	/** True if EOS SDK has been initialized */
	static bool bIsInit;

	/** True if we're shutting down */
	static bool bIsShuttingDown;

	/** True if platform create failed and error has been shown */
	static bool bHasShownCreateFailedError;

	/** True if there are invalid param errors and error popup has been shown */
	static bool bHasShownInvalidParamsErrors;

	/** True if ProductId used for EOS_Platform_Create options is invlaid */
	static bool bHasInvalidParamProductId;

	/** True if SandboxId used for EOS_Platform_Create options is invlaid */
	static bool bHasInvalidParamSandboxId;

	/** True if DeploymentId used for EOS_Platform_Create options is invlaid */
	static bool bHasInvalidParamDeploymentId;

	/** True if Client Credentials used for EOS_Platform_Create options are invlaid */
	static bool bHasInvalidParamClientCreds;
};
