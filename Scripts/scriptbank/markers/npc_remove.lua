-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Npc Remove v6 by Necrym59
-- DESCRIPTION: If npc enters the zone, displays a [NOTIFICATION$="NPC removed"] and will
-- DESCRIPTION: remove/destroy the NPC, activates any logic links, then destroys this zone.
-- DESCRIPTION: NPC [@REMOVE_STYLE=1 (1=Instant, 2=Fade)]
-- DESCRIPTION: Plays <Sound0> when triggered.

local npc_remove 	= {}	
local notification 	= {}
local remove_style 	= {}
local zoneheight	= {}
local played 		= {}
local wait			= {}
local fade_level	= {}
local doonce		= {}
local EntityID		= {}
local status		= {}
	
function npc_remove_properties(e, notification, remove_style, zoneheight)
	npc_remove[e].notification = notification
	npc_remove[e].remove_style = remove_style
	npc_remove[e].zoneheight = zoneheight or 100
end 

function npc_remove_init(e)
	npc_remove[e] = {}
	npc_remove[e].notification = "NPC removed"
	npc_remove[e].remove_style = 1
	npc_remove[e].zoneheight = 100
	played[e] = 0
	EntityID[e] = 0
	doonce[e] = 0
	wait[e] = math.huge		
end

function npc_remove_main(e)
	
	GetEntityInZoneWithFilter(e,2)
	EntityID[e] = g_Entity[e]['entityinzone']
	if EntityID[e] ~= 0 or nil and g_Entity[EntityID[e]]['y'] > g_Entity[e]['y']-1 and g_Entity[EntityID[e]]['y'] < g_Entity[e]['y'] + npc_remove[e].zoneheight then
		if doonce[e] == 0 then
			SetEntityBaseAlpha(EntityID[e],100)
			SetEntityTransparency(EntityID[e],1)
			fade_level[e] = GetEntityBaseAlpha(EntityID[e])
			doonce[e] = 1
		end		
		if g_Entity[e]['activated'] == 0 then			
			if played[e] == 0 then
				PromptDuration(npc_remove[e].notification,1000)
				PlaySound(e,0)
				played[e] = 1				
			end
			SetEntityActive(EntityID[e],0)
			if npc_remove[e].remove_style == 1 then
				Hide(EntityID[e])
				CollisionOff(EntityID[e])
				Destroy(EntityID[e])
				ActivateIfUsed(e)
				PerformLogicConnections(e)
				wait[e] = g_Time + 2000
			end
			if npc_remove[e].remove_style == 2 then
				StopAnimation(EntityID[e])
				if fade_level[e] > 0 then
					SetEntityBaseAlpha(EntityID[e],fade_level[e])
					fade_level[e] = fade_level[e]-0.5
				end
				if fade_level[e] <= 0 then
					Hide(EntityID[e])
					CollisionOff(EntityID[e])
					Destroy(EntityID[e])
					ActivateIfUsed(e)
					PerformLogicConnections(e)
					wait[e] = g_Time + 1000
				end
			end
		end	
	end
	if g_Time > wait[e] then
		Destroy(e)
	end
end

function npc_remove_exit(e)
end
