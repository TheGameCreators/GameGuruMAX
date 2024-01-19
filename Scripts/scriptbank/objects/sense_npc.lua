-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Sense Npc v2 
-- DESCRIPTION: Allows any linked object(s) to be activated once when a named entity is within range.
-- DESCRIPTION: Attach to an object/entity and set AlwaysActive=ON, IsImobile=YES
-- DESCRIPTION: [SENSE_RANGE=250(0,1000)]
-- DESCRIPTION: [NPC_NAME$=""] to monitor.

local lower = string.lower

local sense_npc 	= {}
local sense_range  	= {}
local npc_name  	= {}
local npc_number  	= {}

local entrange  	= {}
local status  		= {}
local doonce		= {}

function sense_npc_properties(e, sense_range, npc_name)
	sense_npc[e] = g_Entity[e]
	sense_npc[e].sense_range = sense_range
	sense_npc[e].npc_name = lower(npc_name) or ""
	sense_npc[e].npc_number = 0
end

function sense_npc_init(e)
	sense_npc[e] = {}
	sense_npc[e].sense_range = 200
	sense_npc[e].npc_name = ""
	sense_npc[e].npc_number = 0
	
	entrange[e] = 0
	doonce[e] = 0
	status[e] = "init"
end

function sense_npc_main(e)

	if status[e] == "init" then
		if sense_npc[e].npc_number == 0 then
			for n = 1, g_EntityElementMax do
				if n ~= nil and g_Entity[n] ~= nil then
					if lower(GetEntityName(n)) == sense_npc[e].npc_name then 
						sense_npc[e].npc_number = n
						status[e] = "monitor"
						break
					end
				end
			end
		end
	end
	if status[e] == "monitor" then
		entrange[e] = math.ceil(GetFlatDistance(e,sense_npc[e].npc_number))
		if entrange[e] <= sense_npc[e].sense_range then
			if doonce[e] == 0 then
				PerformLogicConnections(e)
				ActivateIfUsed(e)
				doonce[e] = 1
				SwitchScript(e,"no_behavior_selected.lua")
			end 
		end
	end
end

function GetFlatDistance(e,v)
    if g_Entity[e] ~= nil and g_Entity[v] ~= nil then
        local distDX = g_Entity[e]['x'] - g_Entity[v]['x']
        local distDZ = g_Entity[e]['z'] - g_Entity[v]['z']
        return math.sqrt((distDX*distDX)+(distDZ*distDZ));
    end
end