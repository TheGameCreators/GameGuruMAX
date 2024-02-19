-- DESCRIPTION: A global script that provides utility functions for common tasks.
-- Module_Misclib v11 - Necrym and Lee

g_module_misclib = {}
g_tEnt = {}

local module_misclib = {}
local U = require "scriptbank\\utillib"
local selectobj = {}

function module_misclib.pinpoint(e,pickuprange,maxrange)
	--pinpoint select object--
	PlayerDist = GetPlayerDistance(e)
	if PlayerDist < pickuprange then
		selectobj[e]= U.ObjectPlayerLookingAt(pickuprange)
		if selectobj[e] ~= 0 or selectobj[e] ~= nil then
			if g_Entity[e]['obj'] == selectobj[e] then
				TextCenterOnXColor(50-0.01,50,3,"+",255,255,255) -- Compensated -0.01 position for offset of character font
				g_tEnt = e
			else
				g_tEnt = 0
			end
		end
		if selectobj[e] == 0 or selectobj[e] == nil then
			g_tEnt = 0
			TextCenterOnXColor(50-0.01,50,3,"+",155,155,155) -- Compensated -0.01 position for offset of character font
		end
	end
	--end pinpoint select object--
end

return module_misclib
