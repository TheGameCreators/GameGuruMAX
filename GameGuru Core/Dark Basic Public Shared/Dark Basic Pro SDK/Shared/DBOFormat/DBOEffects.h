// DBOEffects Functions Header
#ifndef _DBOEFFECTS_H_
#define _DBOEFFECTS_H_

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include "directx-macros.h"
#include "DBOFormat.h"

#include "preprocessor-flags.h"
#ifdef WICKEDENGINE
#else
#include "..\global.h"
#endif

class cExternalEffect : public cSpecialEffect {};

#endif _DBOEFFECTS_H_

