-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Winzone v3 by Necrym59
-- DESCRIPTION: When the player enters this zone, <Sound0> will play and the level is complete.
-- DESCRIPTION: Link to a trigger Zone.
-- DESCRIPTION: [PROMPT_TEXT$="Win Zone"]
-- DESCRIPTION: [ZONEHEIGHT=100(0,1000)]
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: [@GoToLevelMode=1(1=Use Storyboard Logic,2=Go to Specific Level)] controls whether the next level in the Storyboard, or another level is loaded after entry to the zone.
-- DESCRIPTION: <Sound0> -In zone Effect Sound

local winzone 			= {}
local prompt_text 		= {}
local zoneheight		= {}
local SpawnAtStart		= {}
local status			= {}
	
function winzone_properties(e, prompt_text, zoneheight, SpawnAtStart)
	winzone[e] = g_Entity[e]
	winzone[e].prompt_text = prompt_text
	winzone[e].zoneheight = zoneheight or 100
	winzone[e].SpawnAtStart = SpawnAtStart
end
 
function winzone_init(e)
	winzone[e] = g_Entity[e]
	winzone[e].prompt_text = "Win Zone"
	winzone[e].zoneheight = 100
	winzone[e].SpawnAtStart = 1
	status[e] = "init"
end
 
function winzone_main(e)	
	winzone[e] = g_Entity[e]	
	if status[e] == "init" then
		if winzone[e].SpawnAtStart == 1 then SetActivated(e,1) end
		if winzone[e].SpawnAtStart == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end
	if g_Entity[e]['activated'] == 1 then	
		if g_Entity[e]['plrinzone'] == 1 and g_PlayerHealth > 0 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+winzone[e].zoneheight then				
			Prompt(winzone[e].prompt_text)
			PerformLogicConnections(e)
			JumpToLevelIfUsed(e)		
		end	
	end
end
 
function winzone_exit(e)	
end

