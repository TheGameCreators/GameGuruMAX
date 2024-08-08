-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Light Mover v4 by Necrym59
-- DESCRIPTION: Allows a light to be attached to a moving object.
-- DESCRIPTION: Attach to a light and edit the settings.
-- DESCRIPTION: [OBJECT_NAME$=""] object name to attach to
-- DESCRIPTION: [ADJUST_X_OFFSET=0(-100,100)] ajdust x offset
-- DESCRIPTION: [ADJUST_Y_OFFSET=0(-100,100)] ajdust y offset
-- DESCRIPTION: [ADJUST_Z_OFFSET=0(-100,100)] ajdust z offset

module_lightcontrol = require "scriptbank\\markers\\module_lightcontrol"
local rad = math.rad
local lower = string.lower

local lightmover = {}
local light_range = {}
local object_name = {}
local adjust_x_offset = {}
local adjust_y_offset = {}
local adjust_z_offset = {}
local object_number = {}

local status = {}
local attachTo = {}
local lightattached = {}
local lightNum = {}
local tpositionx = {}
local tpositiony = {}
local tpositionz = {}
local offsetx = {}
local offsety = {}
local offsetz = {}

function light_mover_properties(e, object_name, adjust_x_offset, adjust_y_offset, adjust_z_offset)
	module_lightcontrol.init(e,1)
	lightmover[e].object_name = string.lower(object_name)	-- name of entity light attached to
	lightmover[e].adjust_x_offset = adjust_x_offset
	lightmover[e].adjust_y_offset = adjust_y_offset
	lightmover[e].adjust_z_offset = adjust_z_offset
end

function light_mover_init(e)
	lightmover[e] = {}
	lightmover[e].object_name = ""
	lightmover[e].adjust_x_offset = 0
	lightmover[e].adjust_y_offset = 0
	lightmover[e].adjust_z_offset = 0
	lightmover[e].object_number = 0
	lightNum[e] = GetEntityLightNumber(e)

	attachTo[e] = 0
	tpositionx[e] = 0
	tpositiony[e] = 0
	tpositionz[e] = 0
	offsetx[e] = 0
	offsety[e] = 0
	offsetz[e] = 0
	lightNum[e] = GetEntityLightNumber(e)
	lightattached[e] = 0
	status[e] = "init"
end

function light_mover_main(e)
	if status[e] == "init" then
		tpositionx[e], tpositiony[e], tpositionz[e] = GetLightPosition(lightNum[e])
		if lightmover[e].object_name > "" and lightmover[e].object_number == 0 then
			for a = 1, g_EntityElementMax do
				if a ~= nil and g_Entity[a] ~= nil then
					if string.lower(GetEntityName(a)) == lightmover[e].object_name then
						lightmover[e].object_number = a
						attachTo[e] = lightmover[e].object_number
						lightattached[e] = 1
						break
					end
				end
			end
		end
		local x,y,z,Ax,Ay,Az = GetEntityPosAng(attachTo[e])
		offsetx[e] = tpositionx[e] - x
		offsety[e] = tpositiony[e] - y
		offsetz[e] = tpositionz[e] - z
		status[e] = "initdone"
	end
	if attachTo[e] == -1 then return end

	if lightattached[e] == 1 then
		lightNum[e] = GetEntityLightNumber(e)
		local x,y,z,Ax,Ay,Az = GetEntityPosAng(attachTo[e])
		--Set Light Position
		SetLightPosition(lightNum[e], x+offsetx[e]+lightmover[e].adjust_x_offset, y+offsety[e]+lightmover[e].adjust_y_offset, z+offsetz[e]+lightmover[e].adjust_z_offset)
	end
	if g_Entity[attachTo[e]]['health'] <= 0 then
		SetLightRange(lightNum[e],0)
		Destroy(lightNum[e])
	end
end