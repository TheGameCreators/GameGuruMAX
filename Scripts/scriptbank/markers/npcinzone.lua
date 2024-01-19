-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Npcinzone v8 by Necrym59
-- DESCRIPTION: If npc of [@NPC_TYPE=0(0=Enemy, 1=Ally, 2=Neutral, 3=Any)] type enters the zone, displays a 
-- DESCRIPTION: [NOTIFICATION$="NPC in zone"] and can activate an object or zone, then destroys this zone.
-- DESCRIPTION: [ZONEHEIGHT=100(0,1000)]
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: Plays <Sound0> when triggered.

g_NpcActivated 			= {}
local npcinzone 		= {}
local npc_type 			= {}
local zoneheight		= {}
local spawnatstart		= {}

local EntityID			= {}
local EntityAL			= {}
local wait				= {}
local played 			= {}
local doonce			= {}
local status			= {}
	
function npcinzone_properties(e, npc_type, notification, zoneheight, spawnatstart)
	npcinzone[e] = g_Entity[e]
	npcinzone[e].npc_type = npc_type
	npcinzone[e].notification = notification
	npcinzone[e].zoneheight = zoneheight
	npcinzone[e].spawnatstart = spawnatstart
end
 
function npcinzone_init(e)
	npcinzone[e] = {}
	npcinzone[e].npc_type = 0
	npcinzone[e].notification = "NPC in zone"
	npcinzone[e].zoneheight = 100
	npcinzone[e].spawnatstart = 1
	status[e] = "init"
	played[e] = 0
	doonce[e] = 0
	wait[e] = math.huge
	EntityID[e] = 0
	EntityAL[e] = 0
	g_NpcActivated = 0
end
 
function npcinzone_main(e)	
	if status[e] == "init" then
		if npcinzone[e].spawnatstart == 1 then SetActivated(e,1) end
		if npcinzone[e].spawnatstart == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end
	if g_Entity[e]['activated'] == 1 then		
		GetEntityInZone(e)
		EntityID[e] = g_Entity[e]['entityinzone']
		EntityAL[e] = GetEntityAllegiance(EntityID[e])
		if g_Entity[e]['entityinzone'] > 0 and EntityID[e] > 0 and EntityAL[e] ~= -1 and g_Entity[EntityID[e]]['y'] > g_Entity[e]['y']-10 and g_Entity[EntityID[e]]['y'] < g_Entity[e]['y']+npcinzone[e].zoneheight then
			if npcinzone[e].npc_type ~= 3 then
				if EntityAL[e] == npcinzone[e].npc_type then
					Prompt(npcinzone[e].notification)
					if played[e] == 0 then
						PlaySound(e,0)
						played[e] = 1
					end	
					if doonce[e] == 0 then
						g_NpcActivated = 1
						ActivateIfUsed(e)
						PerformLogicConnections(e)
						doonce[e] = 1
						wait[e] = g_Time + 2000
					end
				end
			end
			if npcinzone[e].npc_type == 3 then
				if EntityID[e] > 0 then
					Prompt(npcinzone[e].notification)
					if played[e] == 0 then
						PlaySound(e,0)
						played[e] = 1
					end	
					if doonce[e] == 0 then
						g_NpcActivated = 1
						ActivateIfUsed(e)
						SetActivatedWithMP(e,201)
						PerformLogicConnections(e)
						doonce[e] = 1
						wait[e] = g_Time + 2000
					end
				end
			end			
		end	
	end
	if g_Time > wait[e] then		
		g_NpcActivated = 0
		Destroy(e)
	end
end
 
function npcinzone_exit(e)
end
