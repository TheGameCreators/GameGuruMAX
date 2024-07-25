//----------------------------------------------------
//--- GAMEGURU - M-AudioVolume
//----------------------------------------------------

#include "stdafx.h"
#include "gameguru.h"

// 
//  Character Sound
// 

bool g_bSoundIsMusic[65536];

void audio_volume_init ( void )
{
	t.audioVolume.music = 100;
	t.audioVolume.sound = 100;
	t.audioVolume.musicFloat = 1.0;
	t.audioVolume.soundFloat = 1.0;
	memset(g_bSoundIsMusic, 0, sizeof(g_bSoundIsMusic));
}

void audio_volume_update ( void )
{
	// convert to 0 - 1.0 to enable us to multiply volumes in game
	t.audioVolume.musicFloat = t.audioVolume.music;
	t.audioVolume.musicFloat = (( t.audioVolume.musicFloat * 0.70 ) + 30.0 ) * 0.01;
	if (t.audioVolume.music == 0) t.audioVolume.musicFloat = 0;
	t.audioVolume.soundFloat = t.audioVolume.sound;
	t.audioVolume.soundFloat = (( t.audioVolume.soundFloat * 0.30 ) + 70.0 ) * 0.01;

	//  change volume of all sounds and music
	for ( t.tSound = 1 ; t.tSound<=  65535; t.tSound++ )
	{
		if (  SoundExist(t.tSound) == 1 ) 
		{
			if (  t.tSound >= g.musicsoundoffset && t.tSound <= g.musicsoundoffsetend ) 
			{
				//  handled in music code	
			}
			else
			{
				if (t.tSound == 8802)
				{
					// Used for Ambient Music Track
					SetSoundVolume(t.tSound, (t.gamevisuals.iAmbientMusicTrackVolume * t.audioVolume.soundFloat));
				}
				else
				{
					// Used for Combat Music Track
					if (t.tSound == 8804)
					{
						SetSoundVolume(t.tSound, (t.gamevisuals.iAmbientMusicTrackVolume * t.audioVolume.soundFloat));
					}
					else
					{
						// detect for sound marked as used by ambientinzone (music zone)
						if (g_bSoundIsMusic[t.tSound] == 1)
						{
							SetSoundVolume(t.tSound, (100.0 * t.audioVolume.musicFloat));
						}
						else
						{
							// all others
							SetSoundVolume (t.tSound, (100.0 * t.audioVolume.soundFloat));
						}
					}
				}
			}
		}
	}
}

void audio_volume_free ( void )
{
	//  nothing needed
}
