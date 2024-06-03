-- Sound In Zone v6
-- DESCRIPTION: Plays the audio from <Sound0> when the player enters this zone, and stop other zone audio from playing.
-- DESCRIPTION: You can optionally instruct the sound to play [ONLYONCE!=1].
-- DESCRIPTION: [ZONEHEIGHT=100] controls how far above the zone the player can be before the zone is not triggered.
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: [SoundVolume=100[1,100] adjust this zones sounds volume
-- DESCRIPTION: [OverlaySound!=0)]

g_currentlyplayinge = {}

local soundinzone = {}
local onlyonce = {}
local zoneheight = {}
local SpawnAtStart = {}
local SoundVolume = {}
local currentlyplayingactive = {}
local status = {}

function soundinzone_properties(e, onlyonce, zoneheight, SpawnAtStart, SoundVolume, OverlaySound)
	soundinzone[e].onlyonce = onlyonce
	soundinzone[e].zoneheight = zoneheight
	soundinzone[e].SpawnAtStart = SpawnAtStart or 1
	soundinzone[e].SoundVolume = SoundVolume or 1
	soundinzone[e].OverlaySound = OverlaySound or 0	
 end

function soundinzone_init(e)
	soundinzone[e] = {}
	soundinzone[e].onlyonce = 1
	soundinzone[e].zoneheight = 100
	soundinzone[e].SpawnAtStart = 1
	soundinzone[e].SoundVolume = 50	
	soundinzone[e].OverlaySound = 0	
	currentlyplayingactive[e] = 0
	status[e] = "init"
end

function soundinzone_main(e)

	if status[e] == "init" then
		if soundinzone[e].zoneheight == nil then soundinzone[e].zoneheight = 100 end
		if soundinzone[e].SpawnAtStart == 1 then SetActivated(e,1) end
		if soundinzone[e].SpawnAtStart == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end
	
	if g_Entity[e]['activated'] == 1 then
		if g_Entity[e]['plrinzone']==1 then
			if currentlyplayingactive[e] == 0 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y'] + soundinzone[e].zoneheight then
				if g_currentlyplayinge ~= e then
					if soundinzone[e].OverlaySound == 0 then
						StopSound(g_currentlyplayinge,0)
						StopSound(g_currentlyplayinge,1)
						StopSound(g_currentlyplayinge,2)
						StopSound(g_currentlyplayinge,3)
					end	
					g_currentlyplayinge = 0
				end
				if soundinzone[e].onlyonce == 0 or soundinzone[e].onlyonce == 1 then
					if soundinzone[e].onlyonce == 1 then soundinzone[e].onlyonce = 2 end
					g_currentlyplayinge = e
					PlaySoundIfSilent(e,0)
					SetSound(e,0)
					SetSoundVolume(soundinzone[e].SoundVolume)
				end
				PerformLogicConnections(e)
				currentlyplayingactive[e] = 1
			end
		else
			if currentlyplayingactive[e] == 1 then
				currentlyplayingactive[e] = 0
			end
		end
	end
end
