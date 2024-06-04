-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Player In Zone v12  by Necrym59 and Lee
-- DESCRIPTION: Re-triggerable zone to trigger an event.
-- DESCRIPTION: [ZONEHEIGHT=100] controls how far above the zone the player can be before the zone is not triggered.
-- DESCRIPTION: Set for [@MULTI_TRIGGER=2(1=Yes, 2=No)]
-- DESCRIPTION: Set [DELAY=0(0,100)] in seconds to delay triggered event.
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: [SoundVolume=100[1,100] adjust this sounds volume
-- DESCRIPTION: <Sound0> when entering zone

local plrinzone 	= {}
local zoneheight	= {}
local multi_trigger = {}
local delay			= {}
local spawnatstart	= {}
local soundvolume	= {}
local multi_dead	= {}
local multi_switch	= {}
local status		= {}
local doonce		= {}
local wait			= {}
local waittime 		= {}

function plrinzone_properties(e, zoneheight, multi_trigger, delay, spawnatstart, soundvolume)
	plrinzone[e].zoneheight = zoneheight
	plrinzone[e].multi_trigger = multi_trigger
	plrinzone[e].delay = delay
	plrinzone[e].spawnatstart = spawnatstart
	plrinzone[e].soundvolume = soundvolume or 1
end

function plrinzone_init(e)
	plrinzone[e] = {}
	plrinzone[e].zoneheight = 100
	plrinzone[e].multi_trigger = 2
	plrinzone[e].delay = 0
	plrinzone[e].spawnatstart = 1
	plrinzone[e].soundvolume = 100	
	status[e] = "init"
	doonce[e] = 0
	multi_dead[e] = 0
	multi_switch[e] = 0
	waittime[e] = 0
	wait[e] = math.huge
end

function plrinzone_main(e)
	if status[e] == "init" then
		if plrinzone[e].delay ~= nil then waittime[e] = plrinzone[e].delay * 1000 end
		if plrinzone[e].spawnatstart == 1 then SetActivated(e,1) end
		if plrinzone[e].spawnatstart == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end
	if g_Entity[e]['activated'] == 1 then
		if g_Entity[e]['plrinzone'] == 1 and multi_switch[e] == 0 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+plrinzone[e]['zoneheight'] then
			if doonce[e] == 0 then
				PlaySound(e,0)
				SetSound(e,0)
				SetSoundVolume(plrinzone[e].soundvolume)
				wait[e] = g_Time + waittime[e]
				doonce[e] = 1
			end
		end
		if g_Time > wait[e] then
			if plrinzone[e].multi_trigger == 1 and multi_switch[e] == 0 then
				multi_switch[e] = 1
				ActivateIfUsed(e)
				PerformLogicConnections(e)
			end
			if plrinzone[e].multi_trigger == 2 then
				if multi_dead[e] == 0 then
					ActivateIfUsed(e)
					PerformLogicConnections(e)
					Destroy(e)
					multi_dead[e] = 1
				end
			end
		end	
		if g_Entity[e]['plrinzone'] == 0 and multi_switch[e] == 1 then
			StopSound(e,0)
			doonce[e] = 0
			multi_switch[e] = 0			
		end		
	end
end