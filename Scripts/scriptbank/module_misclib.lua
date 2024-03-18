-- DESCRIPTION: A global script that provides utility functions for common tasks.
-- Module_Misclib v12 - Necrym59 and Lee

g_module_misclib = {}
g_tEnt = {}

local module_misclib = {}
local U = require "scriptbank\\utillib"
local selectobj = {}

function module_misclib.pinpoint(e,pickuprange,highlight)
	--pinpoint select object--
	PlayerDist = GetPlayerDistance(e)
	if PlayerDist < pickuprange then
		selectobj[e]= U.ObjectPlayerLookingAt(pickuprange)
		if selectobj[e] ~= 0 or selectobj[e] ~= nil then
			if g_Entity[e]['obj'] == selectobj[e] then
				TextCenterOnXColor(50-0.01,50,3,"+",255,255,255) 	-- Cross
				--TextCenterOnXColor(50,50-0.4,3,".",255,255,255) 	-- Dot
				g_tEnt = e
				if highlight == 1 then SetEntityEmissiveStrength(g_tEnt,500) end
				if highlight == 2 then end -- for future outline option
			else
				if highlight == 1 then SetEntityEmissiveStrength(g_tEnt,0) end
				if highlight == 2 then end -- for future outline option
				g_tEnt = 0				
			end
		end
		if selectobj[e] == 0 or selectobj[e] == nil then
			g_tEnt = 0
			TextCenterOnXColor(50-0.01,50,3,"+",150,150,150) 	-- Cross
			--TextCenterOnXColor(50,50-0.4,3,".",150,150,150) 	-- Dot
		end					
	end
	--end pinpoint select object--
end

return module_misclib