-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Readiness Zone v5
-- DESCRIPTION: Will arm or disarm player when entering the zone.
-- DESCRIPTION: Attach to a trigger Zone
-- DESCRIPTION: [PROMPT_TEXT$="Lock and Load"]
-- DESCRIPTION: [@WEAPON_MODE$=1(1=Arm, 2=Disarm)]
-- DESCRIPTION: [ZONEHEIGHT=100(0,1000)]
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone

local readiness_zone 		= {}
local prompt_text 			= {}
local weapon_mode 			= {}
local zoneheight			= {}
local spawnatstart			= {}

local last_gun 				= {}
local status 				= {}
local gunstatus				= {}
local resettime				= {}

function readiness_zone_properties(e, prompt_text, weapon_mode, zoneheight, spawnatstart)
	readiness_zone[e] = g_Entity[e]
	readiness_zone[e].prompt_text = prompt_text
	readiness_zone[e].weapon_mode = weapon_mode
	readiness_zone[e].zoneheight = zoneheight or 100
	readiness_zone[e].spawnatstart = spawnatstart	
end

function readiness_zone_init(e)
	readiness_zone[e] = g_Entity[e]
	readiness_zone[e].prompt_text = ""
	readiness_zone[e].weapon_mode = 1
	readiness_zone[e].zoneheight = 100
	readiness_zone[e].spawnatstart = 1
	
	last_gun = g_PlayerGunName
	resettime[e] = 0
	status[e] = "init"
	gunstatus[e] = 0
end

function readiness_zone_main(e)
	readiness_zone[e] = g_Entity[e]
	if status[e] == "init" then
		if readiness_zone[e].spawnatstart == 1 then SetActivated(e,1) end
		if readiness_zone[e].spawnatstart == 0 then SetActivated(e,0) end
		SetGamePlayerStatePlrKeyForceKeystate(0)
		status[e] = "endinit"
	end

	if g_Entity[e]['activated'] == 1 then
		if g_Entity[e]['plrinzone'] == 1 and g_PlayerHealth > 0 and g_PlayerPosY < g_Entity[e]['y']+readiness_zone[e].zoneheight then
			Prompt(readiness_zone[e].prompt_text)
			----Show Weapon --------------
			if readiness_zone[e].weapon_mode == 1 then
				if g_PlayerGunID > 0 then end
				if g_PlayerGunID == 0 then
					ChangePlayerWeaponID(CurrentlyHeldWeaponID)
					SetGamePlayerStatePlrKeyForceKeystate(2)
					gunstatus[e] = 1
				end
			end
			----Hide Weapon --------------
			if readiness_zone[e].weapon_mode == 2 then
				if g_PlayerGunID > 0 then
					CurrentlyHeldWeaponID = GetPlayerWeaponID()
					SetGamePlayerStatePlrKeyForceKeystate(11)
					gunstatus[e] = 1
				end
			end
		end
		if g_Entity[e]['plrinzone'] == 0 and gunstatus[e] == 1 then
			SetGamePlayerStatePlrKeyForceKeystate(0)
			gunstatus[e] = 0
			Destroy(e)
		end
	end	
end

function readiness_zone_exit(e)
end