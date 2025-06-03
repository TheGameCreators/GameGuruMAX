-- V5 by Lee and Necrym59
-- DESCRIPTION: When the player enters this zone any zones with the same script which are currently playing ambience/background music/sounds stops and audio from this zone is looped.
-- DESCRIPTION:[!OnlyPlayInZone=0] will cause the sound to stop playing when the player leaves the zone.
-- DESCRIPTION:[ZONEHEIGHT=100] controls how far above the zone the player can be before the zone is not triggered.
-- DESCRIPTION:[SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION:[SoundVolume=100[1,100] adjust this sounds volume
-- DESCRIPTION:[FadeInSpeed#=0.10(0.01,1.0)] adjust this zones fade-in volume 
-- DESCRIPTION:[FadeOutSpeed#=0.20(0.01,1.0)] adjust this zones fade-out volume 
-- DESCRIPTION:<Sound0> looped sound for this zone

g_ambienceinzone_soundvolume = 0
g_ambienceinzone_soundlooping = 0
g_ambienceinzone = {}
status = {}
sndvol = {}
played = {}
removed = {}

function ambienceinzone_properties(e, onlyplayinzone, zoneheight, SpawnAtStart, SoundVolume, FadeInSpeed, FadeOutSpeed)
	g_ambienceinzone[e]['onlyplayinzone'] = onlyplayinzone or 0
	g_ambienceinzone[e]['zoneheight'] = zoneheight
	g_ambienceinzone[e]['spawnatstart'] = SpawnAtStart or 1
	g_ambienceinzone[e]['volume'] = SoundVolume	
	g_ambienceinzone[e]['fadeinspeed'] = FadeInSpeed
	g_ambienceinzone[e]['fadeoutspeed'] = FadeOutSpeed
end 

function ambienceinzone_init(e)
	g_ambienceinzone[e] = {}
	g_ambienceinzone[e]['onlyplayinzone'] = 0
	g_ambienceinzone[e]['zoneheight'] = 100
	g_ambienceinzone[e]['spawnatstart'] = 1	
	g_ambienceinzone[e]['volume'] = 100	
	g_ambienceinzone[e]['fadeinspeed'] = 0.05
	g_ambienceinzone[e]['fadeoutspeed'] = 0.05	

	status[e] = "init"
	sndvol[e] = 0
	played[e] = 0
	removed[e] = 1
	SetEntityAlwaysActive(e,1)
end

function ambienceinzone_main(e)

	if status[e] == "init" then
		if g_ambienceinzone[e]['spawnatstart'] == 1 then SetActivated(e,1) end
		if g_ambienceinzone[e]['spawnatstart'] == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end

	if g_Entity[e]['activated'] == 1 then
		if g_PlayerHealth <= 0 then
			g_ambienceinzone_soundlooping = 0
		end
		if g_ambienceinzone[e]['zoneheight'] == nil then g_ambienceinzone[e]['zoneheight'] = 100 end
		if g_ambienceinzone[e]['zoneheight'] ~= nil then
			if g_Entity[e]['plrinzone']==1 and g_PlayerPosY+65 > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+g_ambienceinzone[e]['zoneheight'] then
				if g_ambienceinzone_soundlooping ~= e then  -- Remove Previous Sounds
					SetSoundMusicMode(GetEntityRawSound(g_ambienceinzone_soundlooping,0),1)
					SetRawSoundVolume(GetEntityRawSound(g_ambienceinzone_soundlooping,0),g_ambienceinzone_soundvolume)
					g_ambienceinzone_soundvolume = g_ambienceinzone_soundvolume - g_ambienceinzone[e]['fadeoutspeed']
					if g_ambienceinzone_soundvolume <= 0 then
						StopSound(g_ambienceinzone_soundlooping,0)
						g_ambienceinzone_soundvolume = 0
						removed[e] = 1
						PerformLogicConnections(e)
						g_ambienceinzone_soundlooping = e
					end
				end
				if played[e] == 0 and removed[e] == 1 then  -- Set New Sound					
					LoopNon3DSound(e,0)
					SetSoundMusicMode(GetEntityRawSound(e,0),1)	
					SetRawSoundVolume(GetEntityRawSound(e,0),sndvol[e])
					sndvol[e] = sndvol[e] + g_ambienceinzone[e]['fadeinspeed']
					if sndvol[e] >= g_ambienceinzone[e]['volume'] then
						sndvol[e] = g_ambienceinzone[e]['volume']
						g_ambienceinzone_soundvolume = g_ambienceinzone[e]['volume']
						played[e] = 1
						removed[e] = 0
					end
					if sndvol[e] < g_ambienceinzone[e]['volume'] then
						g_ambienceinzone_soundvolume = sndvol[e]
					end	
				end
			end				
		end
		if g_Entity[e]['plrinzone']==0 and g_ambienceinzone[e]['onlyplayinzone'] == 0 then			
			played[e] = 0
			sndvol[e] = 0
			removed[e] = 0			
		end	
		if g_Entity[e]['plrinzone']==0 and g_ambienceinzone[e]['onlyplayinzone'] == 1 then
			if g_ambienceinzone_soundlooping == e then  -- Remove Previous Sounds
				SetSoundMusicMode(GetEntityRawSound(g_ambienceinzone_soundlooping,0),1)
				SetRawSoundVolume(GetEntityRawSound(g_ambienceinzone_soundlooping,0),g_ambienceinzone_soundvolume)
				g_ambienceinzone_soundvolume = g_ambienceinzone_soundvolume - g_ambienceinzone[e]['fadeoutspeed']
				if g_ambienceinzone_soundvolume <= 0 then
					StopSound(g_ambienceinzone_soundlooping,0)
					g_ambienceinzone_soundvolume = 0
					g_ambienceinzone_soundlooping = 0
					removed[e] = 1
					played[e] = 0
					sndvol[e] = 0
				end
			end	
		end

		 -- restore logic
		if g_EntityExtra[e]['restoremenow'] ~= nil then
			if g_EntityExtra[e]['restoremenow'] == 1 then
				g_EntityExtra[e]['restoremenow'] = 0
				if g_ambienceinzone_soundlooping > 0 then
					LoopNon3DSound(g_ambienceinzone_soundlooping,0)
					SetSoundMusicMode(GetEntityRawSound(g_ambienceinzone_soundlooping,0),1)
				end
			end
		end
	end
end