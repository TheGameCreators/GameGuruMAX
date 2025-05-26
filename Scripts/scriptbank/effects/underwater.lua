-- Underwater v3 by Necrym59
-- DESCRIPTION: Will change to the designated LUT group when player is submerged
-- DESCRIPTION: Attach to an object. Set Always Active ON.
-- DESCRIPTION: [LUT_GROUP$="editors\lut\crispwintertable"] to change to (without ".png" file extension)
-- DESCRIPTION: [CHANGE_COLOR!=0] If set on will change water to rgb values set.
-- DESCRIPTION: [WATER_COLOR_R=0(0,255)]
-- DESCRIPTION: [WATER_COLOR_G=0(0,255)]
-- DESCRIPTION: [WATER_COLOR_B=0(0,255)]

local underwater 	= {}
local lut_group		= {}
local change_color 	= {}
local water_color_r = {}
local water_color_g = {}
local water_color_b = {}

local doonce 	= {}
local oldlut 	= {}
local newlut 	= {}
local status 	= {}
local default_color_r = {}
local default_color_g = {}
local default_color_b = {} 

function underwater_properties(e, lut_group, change_color, water_color_r, water_color_g, water_color_b)
    underwater[e].lut_group = lut_group
	underwater[e].change_color 	= change_color or 0
    underwater[e].water_color_r = water_color_r
    underwater[e].water_color_g = water_color_g
    underwater[e].water_color_b = water_color_b
end

function underwater_init(e)
	underwater[e] = {}
	underwater[e].lut_group = ""
	underwater[e].change_color 	= change_color or 0
    underwater[e].water_color_r = 0
    underwater[e].water_color_g = 0
    underwater[e].water_color_b = 0	
	
	oldlut[e] = ""
	newlut[e] = ""
	doonce[e] = 0
	default_color_r[e] = GetWaterShaderColorRed()
	default_color_g[e] = GetWaterShaderColorGreen()
	default_color_b[e] = GetWaterShaderColorBlue()		
	status[e] = "init"
end

function underwater_main(e)
    
	if status[e] == "init" then
		newlut[e] = underwater[e].lut_group		
		oldlut[e] = GetLut()
		status[e] = "endinit"
	end	
	
    if GetGamePlayerStateUnderwater() == 1 then
		if doonce[e] == 0 then
			SetLutTo(newlut[e].. ".png")
			if underwater[e].change_color == 1 then
				SetWaterColor(underwater[e].water_color_r, underwater[e].water_color_g, underwater[e].water_color_b)
			end
			doonce[e] = 1
		end
    end	
	if GetGamePlayerStateUnderwater() == 0 then 
		if doonce[e] == 1 then
			SetLutTo(oldlut[e].. ".png")
			if underwater[e].change_color == 1 then
				SetWaterColor(default_color_r[e], default_color_g[e], default_color_b[e])
			end
			doonce[e] = 0
		end		
    end	
end