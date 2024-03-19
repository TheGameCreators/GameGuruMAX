-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Level Zone v14 by Necrym59
-- DESCRIPTION: When the player enters this zone it will launch a designated level.
-- DESCRIPTION: [@LAUNCH_MODE=1(1=None, 2=Keystroke, 3=Image+Keystroke)]
-- DESCRIPTION: [IMAGEFILE$=""] for optional image
-- DESCRIPTION: [PROMPT_TEXT$="Press E to Continue"]
-- DESCRIPTION: [ZONEHEIGHT=100(0,1000)]
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: [@GoToLevelMode=1(1=Use Storyboard Logic,2=Go to Specific Level)] controls whether to load the next level in the Storyboard, or a specific level.
-- DESCRIPTION: [ResetStates!=0] when entering the new level
-- DESCRIPTION: <Sound0> - In zone Effect Sound

local level_zone 		= {}
local launch_mode		= {}
local imagefile			= {}
local prompt_text		= {}
local zoneheight		= {}
local spawnatstart		= {}
local resetstates		= {}
local status			= {}
local played			= {}
local endimg			= {}

function level_zone_properties(e, launch_mode, imagefile, prompt_text, zoneheight, spawnatstart, resetstates)
	level_zone[e].launch_mode = launch_mode or 1
	level_zone[e].imagefile = imagefile
	level_zone[e].prompt_text = prompt_text
	level_zone[e].zoneheight = zoneheight or 100
	level_zone[e].spawnatstart = spawnatstart
	level_zone[e].resetstates = resetstates
end

function level_zone_init(e)
	level_zone[e] = {}
	level_zone[e].launch_mode = 1
	level_zone[e].imagefile = ""
	level_zone[e].prompt_text = "Press E to Continue"
	level_zone[e].zoneheight = 100
	level_zone[e].spawnatstart = 1
	level_zone[e].resetstates = 0

	status[e] = "init"
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
			if played[e] == 0 then
				PlaySound(e,0)
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
		end
	end
end

function level_zone_exit(e)
end