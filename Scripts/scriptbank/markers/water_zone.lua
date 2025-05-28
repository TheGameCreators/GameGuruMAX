-- Water Zone v5: by Necrym59
-- DESCRIPTION: Will enable a zone to be as if filled with water. 
-- DESCRIPTION: [PROMPT_TEXT$="In water"] Prompt text when enetering water zone
-- DESCRIPTION: [ZONEHEIGHT=300] Sets how far above the zone the water height is and player can be before the zone is triggered.
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: <Sound0> when entering zone

local waterzone 	= {}
local prompt_text	= {}
local zoneheight	= {}
local spawnatstart	= {}

local status		= {}
local water_height	= {}
local doonce		= {}
local inwater		= {}

function water_zone_properties(e, prompt_text, zoneheight, spawnatstart)
	waterzone[e].prompt_text = prompt_text
	waterzone[e].zoneheight = zoneheight
	waterzone[e].spawnatstart = spawnatstart
end

function water_zone_init(e)
	waterzone[e] = {}
	waterzone[e].prompt_text = "Underwater"
	waterzone[e].zoneheight = 100
	waterzone[e].spawnatstart = 1
	
	water_height[e]	= GetWaterHeight()	
	status[e] = "init"
	doonce[e] = 0
	inwater[e] = 0
	Show(e)
end

function water_zone_main(e)
	if status[e] == "init" then
		if waterzone[e].spawnatstart == 1 then SetActivated(e,1) end
		if waterzone[e].spawnatstart == 0 then SetActivated(e,0) end		
		status[e] = "endinit"
	end
	
	if g_Entity[e]['activated'] == 1 then
		if g_Entity[e]['plrinzone'] == 1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+waterzone[e]['zoneheight'] then
			if doonce[e] == 0 then
				PromptDuration(waterzone[e].prompt_text,1000)
				for a = 1, g_EntityElementMax do
					if a ~= nil and g_Entity[a] ~= nil then
						local allegiance = GetEntityAllegiance(a) -- (0-enemy, 1-ally, 2-neutral)
						if allegiance ~= -1 then
							SetEntityUnderwaterMode(a,1)
						end
					end
				end
				inwater[e] = 1
				doonce[e] = 1				
			end	
			SetWaterHeight(g_Entity[e]['y']+waterzone[e]['zoneheight'])			
		end
		if g_Entity[e]['plrinzone'] == 0 and doonce[e] == 1 then
			SetGamePlayerControlUnderwater(0)
			SetGamePlayerControlInWaterState(0)
			SetUnderwaterOff()
			SetWorldGravity(0, -20, 0, 0)
			local underwaterSound = GetGamePlayerControlSoundStartIndex()+34
			if (RawSoundExist(underwaterSound) == 1) then
				StopRawSound(underwaterSound)
			end
			if inwater[e] == 1 then
				for a = 1, g_EntityElementMax do
					if a ~= nil and g_Entity[a] ~= nil then
						local allegiance = GetEntityAllegiance(a) -- (0-enemy, 1-ally, 2-neutral)
						if allegiance ~= -1 then
							SetEntityUnderwaterMode(a,0)
						end
					end
				end
				inwater[e] = 0
				doonce[e] = 0
			end			
			SetWaterHeight(water_height[e])
		end
	end
end
	
function water_zone_exit(e)	
end