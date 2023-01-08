//----------------------------------------------------
//--- GAMEGURU - M-Sky
//----------------------------------------------------

#include "cstr.h"

void sky_init ( void );
#ifdef WICKEDENGINE
void sky_skyspec_init(bool bResetVisuals = true );
#else
void sky_skyspec_init(void);
#endif
void sky_hide ( void );
void sky_show ( void );
void sky_free ( void );
void sky_loop ( void );
