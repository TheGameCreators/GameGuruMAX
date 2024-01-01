-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Player Mode v2
-- DESCRIPTION: Will change a players mode on or off upon activation.
-- DESCRIPTION: Attach to an object, Set always active On.
-- DESCRIPTION: [PROMPT_TEXT$="Player modes activated"]
-- DESCRIPTION: [RUNNING!=1]
-- DESCRIPTION: [JUMPING!=1]
-- DESCRIPTION: [FLASHLIGHT!=1]
-- DESCRIPTION: [WEAPONS!=1]
-- DESCRIPTION: [ZONEHEIGHT=100(0,1000)]
-- DESCRIPTION: [ActivateAtStart!=1] if unchecked use a switch or zone to activate

local player_mode 			= {}
local prompt_text 			= {}
local running 				= {}
local jumping 				= {}
local flashlight 			= {}
local weapons 				= {}
local zoneheight			= {}
local ActivateAtStart		= {}

local status 				= {}
local doonce				= {}

function player_mode_properties(e, prompt_text, running, jumping, flashlight, weapons, zoneheight, ActivateAtStart)
	player_mode[e] = g_Entity[e]
	player_mode[e].prompt_text = prompt_text
	player_mode[e].running = running
	player_mode[e].jumping = jumping
	player_mode[e].flashlight = flashlight
	player_mode[e].weapons = weapons
	player_mode[e].zoneheight = zoneheight or 100
	player_mode[e].ActivateAtStart = ActivateAtStart
end

function player_mode_init(e)
	player_mode[e] = g_Entity[e]
	player_mode[e].prompt_text = ""
	player_mode[e].running = 1
	player_mode[e].jumping = 1
	player_mode[e].flashlight = 1
	player_mode[e].weapons = 1
	player_mode[e].zoneheight = 100
	player_mode[e].ActivateAtStart = 1
	status[e] = "init"
	doonce[e] = 0
end

function player_mode_main(e)
	player_mode[e] = g_Entity[e]
	if status[e] == "init" then
		if player_mode[e].ActivateAtStart == 1 then SetActivated(e,1) end
		if player_mode[e].ActivateAtStart == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end

	if g_Entity[e]['activated'] == 1 then
		if doonce[e] == 0 then
			PromptDuration(player_mode[e].prompt_text,2000)
			doonce[e] = 1
		end
		----Disallow Running --------------
		if player_mode[e].running == 0 then SetGamePlayerControlCanRun(0) end
		----Disallow Jumping --------------
		if player_mode[e].jumping == 0 then SetGamePlayerControlJumpmax(0) end
		----Disallow Flashlight --------------
		if player_mode[e].flashlight == 0 then
			SetFlashLight(0)
			SetFlashLightKeyEnabled(0)
		end
		----Disallow weapons --------------
		if player_mode[e].weapons == 0 then SetPlayerWeapons(0) end
	end
	if g_Entity[e]['activated'] == 0 then
		SetGamePlayerControlCanRun(1)
		SetGamePlayerControlJumpmax(1)
		SetFlashLightKeyEnabled(1)
		SetPlayerWeapons(1)
	end
end

function player_mode_exit(e)
end

