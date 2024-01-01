-- Hidden v1 by Necrym59
-- DESCRIPTION: A global behavior that will hide all the same named objects in-game.
-- DESCRIPTION: Attach to an object Physics=ON.
-- DESCRIPTION: [OBJECT_NAME$=""]
-- DESCRIPTION: Object [@COLLISION_MODE=1(1=On, 2=Off)] on or off.

local lower = string.lower
local hidden 			= {}
local object_name 		= {}
local collision_mode 	= {}
local hiddenlist 		= {}

local name 				= {}
local status			= {}
local objEnt			= {}

function hidden_properties(e, object_name, collision_mode)
	hidden[e] = g_Entity[e]
	hidden[e].object_name = lower(object_name)
	hidden[e].collision_mode = collision_mode
end 

function hidden_init_name(e,name)
	hidden[e] = {}
	hidden[e].object_name = name
	hidden[e].collision_mode = 1
	status[e] = "init"
end

function hidden_main(e)
	hidden[e] = g_Entity[e]
	
	if status[e] == "init" then
		for n = 1, g_EntityElementMax do
			if n ~= nil and g_Entity[n] ~= nil then
				if lower(GetEntityName(n)) == hidden[e].object_name then					
					objEnt[e] = n
					Hide(n)
					if hidden[e].collision_mode == 1 then CollisionOn(n) end
					if hidden[e].collision_mode == 2 then CollisionOff(n) end
				end
			end
		end
		status[e] = "endinit"
	end
	
end