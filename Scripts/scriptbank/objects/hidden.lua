-- Hidden v2 by Necrym59
-- DESCRIPTION: A global behavior that will hide all the same named (dynamic) objects in-game.
-- DESCRIPTION: All objects to be hidden must be dynamic eg: Physics ON, isimobile ON
-- DESCRIPTION: Attach to any object. Physics=ON.
-- DESCRIPTION: [OBJECT_NAME$=""]
-- DESCRIPTION: Object [@COLLISION_MODE=1(1=On, 2=Off)] on or off.

local lower = string.lower
local hidden 			= {}
local object_name 		= {}
local collision_mode 	= {}

local status			= {}
local objEnt			= {}

function hidden_properties(e, object_name, collision_mode)
	hidden[e] = g_Entity[e]
	hidden[e].object_name = lower(object_name)
	hidden[e].collision_mode = collision_mode
end 

function hidden_init_name(e)
	hidden[e] = {}
	hidden[e].object_name = ""
	hidden[e].collision_mode = 1
	status[e] = "init"
end

function hidden_main(e)
	
	if status[e] == "init" then
		for n = 1, g_EntityElementMax do
			if n ~= nil and g_Entity[n] ~= nil then
				if lower(GetEntityName(n)) == hidden[e].object_name then					
					Hide(n)
					if hidden[e].collision_mode == 1 then CollisionOn(n) end
					if hidden[e].collision_mode == 2 then CollisionOff(n) end
				end
			end
		end
		status[e] = "endinit"	
	end
end