-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Player Mode v3
-- DESCRIPTION: Will change a players game modes upon activation.
-- DESCRIPTION: Attach to an object and link to a switch or zone to activate.
-- DESCRIPTION: [PROMPT_TEXT$="Player modes activated"]
-- DESCRIPTION: [RUNNING!=1]
-- DESCRIPTION: [JUMPING!=1]
-- DESCRIPTION: [FLASHLIGHT!=1]
-- DESCRIPTION: [WEAPONS!=1]
-- DESCRIPTION: Unchecked options are disabled

local player_mode 			= {}
local prompt_text 			= {}
local running 				= {}
local jumping 				= {}
local flashlight 			= {}
local weapons 				= {}

local status 				= {}
local defaultjump			= {}
local doonce				= {}

function player_mode_properties(e, prompt_text, running, jumping, flashlight, weapons)
	player_mode[e].prompt_text = prompt_text
	player_mode[e].running = running
	player_mode[e].jumping = jumping
	player_mode[e].flashlight = flashlight
	player_mode[e].weapons = weapons
end

function player_mode_init(e)
	player_mode[e] = {}
	player_mode[e].prompt_text = ""
	player_mode[e].running = 1
	player_mode[e].jumping = 1
	player_mode[e].flashlight = 1
	player_mode[e].weapons = 1
	status[e] = "init"
	doonce[e] = 0
end

function player_mode_main(e)

	if status[e] == "init" then
		SetActivated(e,0)
		defaultjump[e] = GetGamePlayerControlJumpmax()
		status[e] = "endinit"
	end

	if g_Entity[e]['activated'] == 1 then
		SetGamePlayerControlCanRun(1)
		SetGamePlayerControlJumpmax(defaultjump[e])
		SetFlashLightKeyEnabled(1)
		SetPlayerWeapons(1)
		PromptDuration(player_mode[e].prompt_text,2000)					
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
		SetActivated(e,0)
	end
end

function player_mode_exit(e)
end

