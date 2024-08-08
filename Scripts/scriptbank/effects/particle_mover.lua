-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Particle Mover v6 by Necrym59
-- DESCRIPTION: Allows a particle to be attached to a moving object.
-- DESCRIPTION: Attach to an object. Set Always Active ON.
-- DESCRIPTION: [PARTICLE_NAME$=""] particle name
-- DESCRIPTION: [OBJECT_NAME$=""] name of object to attach to
-- DESCRIPTION: [ADJUST_X_OFFSET=0(-100,100)] ajdust x offset
-- DESCRIPTION: [ADJUST_Y_OFFSET=0(-100,100)] ajdust y offset
-- DESCRIPTION: [ADJUST_Z_OFFSET=0(-100,100)] ajdust z offset
-- DESCRIPTION: [!HIDE_OBJECT=0] hide the attached object

local lower = string.lower
local partmover = {}
local particle_name = {}	
local object_name = {}
local adjust_x_offset = {}
local adjust_y_offset = {}
local adjust_z_offset = {}
local hide_object = {}

local particle_number = {}	
local object_number = {}

local status = {}
local attachTo = {}
local partattached = {}
local tpositionx = {}
local tpositiony = {}
local tpositionz = {}
local offsetx = {}
local offsety = {}
local offsetz = {}

function particle_mover_properties(e, particle_name, object_name, adjust_x_offset, adjust_y_offset, adjust_z_offset, hide_object)
	partmover[e].particle_name = string.lower(particle_name)	-- name of particle
	partmover[e].object_name = string.lower(object_name)		-- name of entity
	partmover[e].adjust_x_offset = adjust_x_offset
	partmover[e].adjust_y_offset = adjust_y_offset
	partmover[e].adjust_z_offset = adjust_z_offset
	partmover[e].hide_object = hide_object or 0
end

function particle_mover_init(e)
	partmover[e] = {}
	partmover[e].particle_name = ""
	partmover[e].object_name = ""
	partmover[e].adjust_x_offset = 0
	partmover[e].adjust_y_offset = 0
	partmover[e].adjust_z_offset = 0
	partmover[e].hide_object = 0
	partmover[e].particle_number = 0
	partmover[e].object_number = 0

	attachTo[e] = 0
	tpositionx[e] = 0
	tpositiony[e] = 0
	tpositionz[e] = 0
	offsetx[e] = 0
	offsety[e] = 0
	offsetz[e] = 0
	partattached[e] = 0
	status[e] = "init"
end

function particle_mover_main(e)
	if status[e] == "init" then
		status[e] = "endinit"
	end
	
	if partmover[e].object_name > "" and partmover[e].object_number == 0 then
		for a = 1, g_EntityElementMax do
			if a ~= nil and g_Entity[a] ~= nil then
				if string.lower(GetEntityName(a)) == partmover[e].object_name then
					partmover[e].object_number = a
					attachTo[e] = partmover[e].object_number
					if partmover[e].hide_object == 1 then Hide(a) end
					break
				end
			end
		end
	end
	if partmover[e].particle_name > "" and partmover[e].particle_number == 0then
		for p = 1, g_EntityElementMax do
			if p ~= nil and g_Entity[p] ~= nil then
				if string.lower(GetEntityName(p)) == partmover[e].particle_name then
					partmover[e].particle_number = p					
					tpositionx[e], tpositiony[e], tpositionz[e] = GetEntityPosAng(partmover[e].particle_number)					
					partattached[e] = 1
					break
				end
			end
		end
	end
	
	if attachTo[e] == -1 then return end
	
	if partattached[e] == 1 then
		local x,y,z,Ax,Ay,Az = GetEntityPosAng(attachTo[e])
		--Set Particle Position
		ResetPosition(partmover[e].particle_number, x+offsetx[e]+partmover[e].adjust_x_offset, y+offsety[e]+partmover[e].adjust_y_offset, z+offsetz[e]+partmover[e].adjust_z_offset)
		Show(partmover[e].particle_number)
	end
	if g_Entity[attachTo[e]]['health'] <= 0 then
		Hide(partmover[e].particle_number)
		Destroy(partmover[e].particle_number)
	end
end