-- DESCRIPTION: A global script that provides utility functions for common tasks.
-- Module_Misclib v18 - Necrym59 and Lee

g_module_misclib = {}
g_tEnt = {}

local module_misclib = {}
local U = require "scriptbank\\utillib"

local selectobj = {}
local rgbtemp = 0
local emvalue = 0

function module_misclib.pinpoint(e,pickuprange,highlight,iconimage)
	--pinpoint select object--
	selectobj[e]=U.ObjectPlayerLookingAt(pickuprange)
	if selectobj[e] ~= 0 or selectobj[e] ~= nil and U.PlayerLookingAtObj(selectobj[e],pickuprange) == true then
		if g_Entity[e]['obj'] == selectobj[e] then
			g_tEnt = e				
			if highlight == 1 then -- shape option
				local r,g,b = GetEntityEmissiveColor(g_tEnt)
				emvalue = GetEntityEmissiveStrength(g_tEnt)
				if emvalue > 0 then						
					SetEntityEmissiveStrength(g_tEnt,emvalue)
				end
				if r==0 and g==0 and b==0 then
					SetEntityEmissiveColor(g_tEnt,0,80,0)
					SetEntityEmissiveStrength(g_tEnt,500)
					rgbtemp = 1
				end								
			end 	
			if highlight == 2 then -- outline option
				SetEntityOutline(g_tEnt,1)
			end
			if highlight == 3 then -- icon or none
				PasteSpritePosition(iconimage,50,50)
			else
				TextCenterOnXColor(50-0.01,50,3,"+",255,255,255) -- Cross pointer
			end
		else
			if highlight == 1 then -- end shape option
				if emvalue > 0 then	
					SetEntityEmissiveStrength(g_tEnt,emvalue)
				end
				if rgbtemp == 1	then
					SetEntityEmissiveColor(g_tEnt,0,0,0)
					SetEntityEmissiveStrength(g_tEnt,0)
					rgbtemp = 0
				end
			end
			if highlight == 2 then -- end outline option
				SetEntityOutline(g_tEnt,0)
			end
			if highlight == 3 then -- end icon or none option
			end			
		end
	end
	if selectobj[e] == 0 or selectobj[e] == nil then		
		if highlight == 1 then -- end shape option
			if emvalue > 0 then	
				SetEntityEmissiveStrength(g_tEnt,emvalue)
			end
			if rgbtemp == 1	then
				SetEntityEmissiveColor(g_tEnt,0,0,0)
				SetEntityEmissiveStrength(g_tEnt,0)
				rgbtemp = 0
			end
		end
		if highlight == 2 then -- end outline option
			SetEntityOutline(g_tEnt,0)
		end
		if highlight == 3 then -- end icon or none option
		end
		g_tEnt = 0
		if U.PlayerLookingNear(e,pickuprange,60) then
			TextCenterOnXColor(50,50-0.4,3,".",180,180,180) -- Dot pointer
		end			
	end
	--end pinpoint select object--
end

return module_misclib