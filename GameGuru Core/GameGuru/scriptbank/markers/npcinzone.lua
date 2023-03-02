-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Npcinzone v5 by Necrym59
-- DESCRIPTION: If npc of [@NPC_TYPE=0(0=Enemy, 1=Ally, 2=Neutral)] type enters the zone, displays a [NOTIFICATION$="NPC in zone"] and can activate an object or zone, then destroys this zone.
-- DESCRIPTION: Plays <Sound0> when triggered.

	local npcinzone 	= {}	
	local npc_type 		= {}
	local notification 	= {}
	local zoneheight	= {}
	local played 		= {}
	local npcallegiance	= {}
	local wait			= {}
	
function npcinzone_properties(e, npc_type, notification, zoneheight)
	npcinzone[e] = g_Entity[e]
	npcinzone[e].npc_type = npc_type
	npcinzone[e].notification = notification
	npcinzone[e].zoneheight = zoneheight or 100
end 

function npcinzone_init(e)
	npcinzone[e] = g_Entity[e]
	npcinzone[e].npc_type = 0
	npcinzone[e].notification = "NPC in zone"
	npcinzone[e].zoneheight = 100
	played = 0
	wait[e] = math.huge
	npcallegiance[e] = 0
end

function npcinzone_main(e)
	npcinzone[e] = g_Entity[e]
		
	GetEntityInZone(e)
	local EntityID = g_Entity[e]['entityinzone']
	local allegiance = GetEntityAllegiance(EntityID) 	
	if EntityID > 0 and g_Entity[e]['entityinzone'] < g_Entity[e]['y'] + npcinzone[e].zoneheight then		
		npcallegiance[e] = npcinzone[e].npc_type
		if allegiance == npcallegiance[e] then -- (0-enemy, 1-ally, 2-neutral)
			PromptDuration(npcinzone[e].notification,2000)			
			if g_Entity[e]['activated'] == 0 then
				if played == 0 then
					PlaySound(e,0)
					played = 1
					wait[e] = g_Time + 2 * 1000
				end								
				ActivateIfUsed(e)
				PerformLogicConnections(e)
			end	
		end		
	end
	if g_Time > wait[e] then
		Destroy(e)
	end
end
