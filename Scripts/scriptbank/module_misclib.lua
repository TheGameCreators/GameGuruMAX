-- DESCRIPTION: A global script that provides utility functions for common tasks.

g_module_misclib = {}

local module_misclib = {}
local U = require "scriptbank\\utillib"
local tEnt = {}
local selectobj = {}

function module_misclib.pinpoint(e,pickuprange,maxrange)
	--pinpoint select object--
	PlayerDist = GetPlayerDistance(e)
	if PlayerDist < maxrange then
		local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
		local rayX, rayY, rayZ = 0, 0, pickuprange
		local paX, paY, paZ = math.rad(GetCameraAngleX(0)), math.rad(GetCameraAngleY(0)), math.rad(GetCameraAngleZ(0))
		rayX, rayY, rayZ = U.Rotate3D(rayX, rayY, rayZ, paX, paY, paZ)
		selectobj[e] = IntersectAll(px, py, pz, px+rayX, py+rayY, pz+rayZ, g_Entity[e]['obj'])
		if selectobj[e] ~= 0 or nil then
			if g_Entity[e]['obj'] == selectobj[e] then
				Text(50,50,3,"+")
				tEnt[e] = e
			end
		end
		if selectobj[e] == 0 or nil then tEnt[e] = 0 end
	end
	--end pinpoint select object--
end

return module_misclib
