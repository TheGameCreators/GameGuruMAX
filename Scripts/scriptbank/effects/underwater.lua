-- Underwater v2 by Necrym59
-- DESCRIPTION: Will change to the designated LUT group when player is submerged.
-- DESCRIPTION: Attach to an object. Set Always Active ON.
-- DESCRIPTION: [LUT_GROUP$="editors\lut\crispwintertable"] to change to (without ".png" file extension)

local underwater 	= {}
local lut_group		= {}

local doonce 	= {}
local oldlut 	= {}
local newlut 	= {}
local status 	= {} 

function underwater_properties(e, lut_group)
    underwater[e].lut_group = lut_group
end

function underwater_init(e)
	underwater[e] = {}
	underwater[e].lut_group = ""
	
	oldlut[e] = ""
	newlut[e] = ""
	doonce[e] = 0
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
			doonce[e] = 1
		end
    end	
	if GetGamePlayerStateUnderwater() == 0 then 
		if doonce[e] == 1 then
			SetLutTo(oldlut[e].. ".png")
			doonce[e] = 0
		end			
    end	
end