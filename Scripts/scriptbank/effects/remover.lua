-- Remover v2 by Necrym59
-- DESCRIPTION: A global behavior that will remove all the same named entities in-game when activated by a switch or zone.
-- DESCRIPTION: Attach to an object and link to a switch or zone. Set AlwaysActive=ON.
-- DESCRIPTION: [ENTITY_NAME$=""] Name of entities.

local lower = string.lower
local remover 			= {}
local entity_name 		= {}

local status			= {}

function remover_properties(e, entity_name)
	remover[e] = g_Entity[e]
	remover[e].entity_name = lower(entity_name)
end 

function remover_init_name(e)
	remover[e] = {}
	remover[e].entity_name = ""
	status[e] = "init"
end

function remover_main(e)
	
	if status[e] == "init" then
		status[e] = "endinit"	
	end
	
	if g_Entity[e]['activated'] == 1 then
		for n = 1, g_EntityElementMax do
			if n ~= nil and g_Entity[n] ~= nil then
				if lower(GetEntityName(n)) == remover[e].entity_name then					
					Destroy(n)
				end
			end
		end
		status[e] = "endinit"	
	end	
end