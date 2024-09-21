-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Winzone v17 by Necrym59 and Lee
-- DESCRIPTION: When the player enters this zone, <Sound0> will play and the level is complete.
-- DESCRIPTION: [NOTES_TEXT$="This winzone takes the user to a new level"]
-- DESCRIPTION: [ZONEHEIGHT=100(0,1000)]
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: [ResetStates!=0] when entering the next level
-- DESCRIPTION: [@ENDING_MODE=1(1=None, 2=Image, 3=Video, 4=Video+Image)] Type of end level modes
-- DESCRIPTION: [ENDING_IMAGEFILE$=""] for optional ending image
-- DESCRIPTION: [SPAWN_MARKER_USER_GLOBAL$=""] user global required for using spawn markers (eg: MySpawnMarkers)
-- DESCRIPTION: [SPAWN_MARKER_NAME$=""] for optional spawning using spawn markers
-- DESCRIPTION: [@GoToLevelMode=1(1=Use Storyboard Logic,2=Go to Specific Level)] controls whether to load the next level in the Storyboard, or a specific level.

-- DESCRIPTION: <Video Slot> for optional ending video

local winzone 					= {}
local notes_text				= {}
local zoneheight				= {}
local spawnatstart				= {}
local ending_mode				= {}
local ending_imagefile			= {}
local spawn_marker_user_global	= {}
local spawn_marker_name			= {}
local resetstates				= {}

local status			= {}
local endimg			= {}
local endvid			= {}
	
function winzone_properties(e, notes_text, zoneheight, spawnatstart, resetstates, ending_mode, ending_imagefile, spawn_marker_user_global, spawn_marker_name)
	winzone[e].notes_text = notes_text
	winzone[e].zoneheight = zoneheight or 100
	winzone[e].spawnatstart = spawnatstart
	winzone[e].resetstates = resetstates		
	if ending_mode == nil then ending_mode = 1 end
	winzone[e].ending_mode = ending_mode
	if ending_imagefile == nil then ending_imagefile = "" end
	winzone[e].ending_imagefile = ending_imagefile
	winzone[e].spawn_marker_user_global = spawn_marker_user_global or ""
	winzone[e].spawn_marker_name = spawn_marker_name or ""
end
 
function winzone_init(e)
	winzone[e] = {}
	winzone[e].notes_text = ""
	winzone[e].zoneheight = 100
	winzone[e].spawnatstart = 1
	winzone[e].resetstates = 0		
	winzone[e].ending_mode = 1
	winzone[e].ending_imagefile = ""
	winzone[e].spawn_marker_user_global = ""
	winzone[e].spawn_marker_name = ""	
	status[e] = "init"
	endvid[e] = 0
end

function winzone_main(e)	
	if status[e] == "init" then
		if winzone[e].spawnatstart == 1 then SetActivated(e,1) end
		if winzone[e].spawnatstart == 0 then SetActivated(e,0) end
		if winzone[e].ending_imagefile ~= "" then 
			endimg[e] = CreateSprite(LoadImage(winzone[e].ending_imagefile))
			SetSpriteSize(endimg[e],100,100)
			SetSpriteDepth(endimg[e],1)
			SetSpritePosition(endimg[e],500,500)
		end			
		status[e] = "endinit"
	end

	if g_Entity[e]['activated'] == 1 then		
		if g_Entity[e]['plrinzone'] == 1 and g_PlayerHealth > 0 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+winzone[e].zoneheight then
			if _G["g_UserGlobal['"..winzone[e].spawn_marker_user_global.."']"] ~= nil then _G["g_UserGlobal['"..winzone[e].spawn_marker_user_global.."']"] = winzone[e].spawn_marker_name end
			if winzone[e].ending_mode == 1 then 
				JumpToLevelIfUsedEx(e,winzone[e].resetstates)
			end
			if winzone[e].ending_mode == 2 then 		
				if winzone[e].ending_imagefile > "" then
					PasteSpritePosition(endimg[e],0,0)
					FreezePlayer()
					Prompt ("Press E to Continue" )
					if g_KeyPressE == 1 then
						UnFreezePlayer()
						JumpToLevelIfUsedEx(e,winzone[e].resetstates)
					end	
				end
			end
			if winzone[e].ending_mode == 3 then
				if endvid[e] == 0 then
					PromptVideo(e,1)
					endvid[e] = 1
				end
				if endvid[e] == 1 then
					JumpToLevelIfUsedEx(e,winzone[e].resetstates)					
				end
			end
			if winzone[e].ending_mode == 4 then
				if endvid[e] == 0 then
					PromptVideo(e,1)
					endvid[e] = 1
				end
				if endvid[e] == 1 then
					if winzone[e].ending_imagefile > "" then
						PasteSpritePosition(endimg[e],0,0)
						FreezePlayer()
						Prompt ("Press E to Continue" )
						if g_KeyPressE == 1 then
							UnFreezePlayer()
							JumpToLevelIfUsedEx(e,winzone[e].resetstates)
						end
					else
						JumpToLevelIfUsedEx(e,winzone[e].resetstates)
					end
				end			
			end
		end
	end
end
 
function winzone_exit(e)	
end