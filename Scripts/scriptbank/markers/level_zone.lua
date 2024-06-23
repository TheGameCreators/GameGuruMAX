-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Level Zone v17 by Necrym59
-- DESCRIPTION: When the player enters this zone it will launch a designated level after the designated mode.
-- DESCRIPTION: [@LAUNCH_MODE=1(1=None, 2=Keystroke, 3=Image+Keystroke, 4=Video)]
-- DESCRIPTION: [IMAGEFILE$=""] for optional image
-- DESCRIPTION: [PROMPT_TEXT$="Press E to Continue"]
-- DESCRIPTION: [ZONEHEIGHT=100(0,1000)]
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: [@GoToLevelMode=1(1=Use Storyboard Logic,2=Go to Specific Level)] controls whether to load the next level in the Storyboard, or a specific level.
-- DESCRIPTION: [ResetStates!=0] when entering the new level
-- DESCRIPTION: [SPAWN_MARKER_USER_GLOBAL$=""] user global required for using spawn markers (eg: MySpawnMarkers)
-- DESCRIPTION: [SPAWN_MARKER_NAME$=""] name of spawn marker to relocate spawn on next map
-- DESCRIPTION: <Video Slot> for optional ending video
-- DESCRIPTION: <Sound1> - In zone Effect Sound

local level_zone 				= {}
local launch_mode				= {}
local imagefile					= {}
local prompt_text				= {}
local zoneheight				= {}
local spawnatstart				= {}
local resetstates				= {}
local spawn_marker_user_global	= {}
local spawn_marker_name			= {}

local status = {}
local played = {}
local endimg = {}
local endvid = {}

function level_zone_properties(e, launch_mode, imagefile, prompt_text, zoneheight, spawnatstart, resetstates, spawn_marker_user_global, spawn_marker_name)
	level_zone[e].launch_mode = launch_mode or 1
	level_zone[e].imagefile = imagefile
	level_zone[e].prompt_text = prompt_text
	level_zone[e].zoneheight = zoneheight or 100
	level_zone[e].spawnatstart = spawnatstart
	level_zone[e].resetstates = resetstates
	level_zone[e].spawn_marker_user_global = spawn_marker_user_global
	level_zone[e].spawn_marker_name = spawn_marker_name
end

function level_zone_init(e)
	level_zone[e] = {}
	level_zone[e].launch_mode = 1
	level_zone[e].imagefile = ""
	level_zone[e].prompt_text = "Press E to Continue"
	level_zone[e].zoneheight = 100
	level_zone[e].spawnatstart = 1
	level_zone[e].resetstates = 0
	level_zone[e].spawn_marker_user_global = ""
	level_zone[e].spawn_marker_name = ""

	status[e] = "init"
	endvid[e] = 0
	played[e] = 0
end

function level_zone_main(e)
	if status[e] == "init" then
		if level_zone[e].spawnatstart == 1 then SetActivated(e,1) end
		if level_zone[e].spawnatstart == 0 then SetActivated(e,0) end
		if level_zone[e].imagefile ~= "" then
			endimg[e] = CreateSprite(LoadImage(level_zone[e].imagefile))
			SetSpriteSize(endimg[e],100,100)
			SetSpriteDepth(endimg[e],1)
			SetSpritePosition(endimg[e],500,500)
		end
		status[e] = "endinit"
	end
	if g_Entity[e]['activated'] == 1 then
		if g_Entity[e]['plrinzone'] == 1 and g_PlayerHealth > 0 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+level_zone[e].zoneheight then
			if _G["g_UserGlobal['"..level_zone[e].spawn_marker_user_global.."']"] ~= nil then _G["g_UserGlobal['"..level_zone[e].spawn_marker_user_global.."']"] = level_zone[e].spawn_marker_name end
			if played[e] == 0 then
				PlaySound(e,1)
				played[e] = 1
			end
			if level_zone[e].launch_mode == 1 then
				JumpToLevelIfUsedEx(e,level_zone[e].resetstates)
			end
			if level_zone[e].launch_mode == 2 then
				Prompt(level_zone[e].prompt_text)
				if g_KeyPressE == 1 then
					JumpToLevelIfUsedEx(e,level_zone[e].resetstates)
				end
			end
			if level_zone[e].launch_mode == 3 then
				if level_zone[e].imagefile > "" then
					PasteSpritePosition(endimg[e],0,0)
					FreezePlayer()
					Prompt(level_zone[e].prompt_text)
					if g_KeyPressE == 1 then
						UnFreezePlayer()
						JumpToLevelIfUsedEx(e,level_zone[e].resetstates)
					end
				end				
			end
			if level_zone[e].launch_mode == 4 then
				if endvid[e] == 0 then
					PromptVideo(e,1)
					endvid[e] = 1
				end
				if endvid[e] == 1 then
					JumpToLevelIfUsedEx(e,level_zone[e].resetstates)					
				end
			end
		end
	end
end

function level_zone_exit(e)
end