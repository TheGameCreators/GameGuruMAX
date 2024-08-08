-- LUT_zone v6 by Necrym59 with thanks to Preben and Graphix
-- DESCRIPTION: LUT can be changed when the player enters the zone.
-- DESCRIPTION: [PROMPT_TEXT$="LUT Changed"] displayed when entering the zone.
-- DESCRIPTION: [LUT_GROUP$="editors\lut\ps_strongamber"] LUT group to change to (without ".png" file extension).
-- DESCRIPTION: [@TRANSITION=1(1=Instant, 2=Fade, 3=Incremental)] Type of LUT change
-- DESCRIPTION: [NUMBER_OF_INCREMENTS=0(0,100)] Number of incremental LUTS.
-- DESCRIPTION: [INCREMENT_DELAY=100(0,500)] milliseconds
-- DESCRIPTION: [ZONEHEIGHT=100] controls how far above the zone the player can be before the zone is not triggered.
-- DESCRIPTION: [SPAWN_AT_START!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: [LUT_RESET_ON_EXIT!=1] reset to previous LUT when leaving zone
-- DESCRIPTION: [USE_ONCE_ONLY!=0] to useonce only then zone is destroyed.
-- DESCRIPTION: <Sound0> when entering zone

local lutzone 				= {}
local prompt_text			= {}
local lut_group				= {}
local transition			= {}
local no_of_increments		= {}
local increment_delay		= {}
local zoneheight			= {}
local spawn_at_start		= {}
local lut_reset_on_exit		= {}
local use_once_only			= {}

local status		= {}
local newlut		= {}
local oldlut		= {}
local lutcount		= {}
local luttimer		= {}
local doonce		= {}
local fadeexposure	= {}
local fadeoutlut	= {}
local fadeinlut		= {}

function lut_zone_properties(e, prompt_text, lut_group, transition, no_of_increments, increment_delay, zoneheight, spawn_at_start, lut_reset_on_exit, use_once_only)
	lutzone[e].prompt_text = prompt_text
	lutzone[e].lut_group = lut_group
	lutzone[e].transition = transition	
	lutzone[e].no_of_increments = no_of_increments
	lutzone[e].increment_delay = increment_delay	
	lutzone[e].zoneheight = zoneheight
	lutzone[e].spawn_at_start = spawn_at_start or 0
	lutzone[e].lut_reset_on_exit = lut_reset_on_exit or 0
	lutzone[e].use_once_only = use_once_only or 0		
end

function lut_zone_init(e)
	lutzone[e] = {}
	lutzone[e].prompt_text = ""
	lutzone[e].lut_group = ""
	lutzone[e].transition = 1	
	lutzone[e].no_of_increments = 0
	lutzone[e].increment_delay = 100
	lutzone[e].zoneheight = 100
	lutzone[e].spawn_at_start = 1
	lutzone[e].lut_reset_on_exit = 1
	lutzone[e].use_once_only = 0

	status[e] = "init"
	doonce[e] = 0
	newlut[e] = ""
	oldlut[e] = ""
	lutcount[e] = 0
	luttimer[e] = math.huge
	fadeexposure[e] = 0
	fadeoutlut[e] = 0
	fadeinlut[e] = 9999
end

function lut_zone_main(e)

	if status[e] == "init" then
		oldlut[e] = GetLut()
		newlut[e] = lutzone[e].lut_group
		luttimer[e] = g_Time + 10
		if lutzone[e].spawn_at_start == 1 then SetActivated(e,1) end
		if lutzone[e].spawn_at_start == 0 then SetActivated(e,0) end
		fadeexposure[e] = GetExposure()
		fadeoutlut[e] = fadeexposure[e]
		status[e] = "endinit"
	end

	if g_Entity[e]['activated'] == 1 then
	
		if g_Entity[e]['plrinzone'] == 1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+lutzone[e].zoneheight then			
			if doonce[e] == 0 then
				Prompt(lutzone[e].prompt_text)
				PlaySound(e,0)
				PerformLogicConnections(e)
				ActivateIfUsed(e)
				doonce[e] = 1
			end
		end
		
		if doonce[e] == 1 then
			if lutzone[e].transition == 1 then  -- Instant Change
				SetLutTo(newlut[e].. ".png")
				doonce[e] = 2
			end			
			if lutzone[e].transition == 2 then	-- Quick Fadein		
				if fadeoutlut[e] > 0 then
					fadeoutlut[e] = fadeoutlut[e] - 0.03
					if fadeoutlut[e] <= 0 then
						fadeoutlut[e] = 0
						fadeinlut[e] = 0
						SetLutTo(newlut[e].. ".png")						
					end		
					SetExposure(fadeoutlut[e])
				end				
				if fadeinlut[e] < fadeexposure[e] then
					fadeinlut[e] = fadeinlut[e] + 0.03
					if fadeinlut[e] >= fadeexposure[e] then
						fadeinlut[e] = fadeexposure[e]
						doonce[e] = 2
					end
					SetExposure(fadeinlut[e])
				end				
			end				
			if lutzone[e].transition == 3 then  -- Incremental Transition
				if lutcount[e] < lutzone[e].no_of_increments and g_Time > luttimer[e] then
					SetLutTo(newlut[e].. "" ..lutcount[e].. ".png")
					lutcount[e] = lutcount[e] + 1
					luttimer[e] = g_Time + lutzone[e].increment_delay
					if lutcount[e] == lutzone[e].no_of_increments then
						SetLutTo(newlut[e].. ".png")					
						doonce[e] = 2
					end
				end
			end
		end			
		
		if g_Entity[e]['plrinzone'] == 0 and doonce[e] == 2 and lutzone[e].lut_reset_on_exit == 0 and lutzone[e].use_once_only == 1 then Destroy(e)	end	
		if g_Entity[e]['plrinzone'] == 0 and doonce[e] == 2 and lutzone[e].lut_reset_on_exit == 1 then
			if lutzone[e].lut_reset_on_exit == 1 and lutzone[e].transition == 1 or lutzone[e].transition == 2 and doonce[e] == 2 then
				SetLutTo(oldlut[e])
				doonce[e] = 3
			end
			if lutzone[e].lut_reset_on_exit == 1 and lutzone[e].transition == 3 and doonce[e] == 2 then  --FOR TRANSITION no_of_increments
				if lutcount[e] > 1 and g_Time > luttimer[e] then
					SetLutTo(newlut[e].. "" ..lutcount[e].. ".png")
					lutcount[e] = lutcount[e] - 1
					luttimer[e] = g_Time + lutzone[e].increment_delay
				end	
				if lutcount[e] == 1 then
					SetLutTo(oldlut[e])
					doonce[e] = 3					
				end	
			end
			if doonce[e] == 3 then
				fadeoutlut[e] = fadeexposure[e]
				lutcount[e] = 0
				doonce[e] = 0
				if lutzone[e].use_once_only == 1 then
					Destroy(e)
				end
			end	
		end
	end	
end

function lut_zone_preexit(e)
    SetExposure(fadeexposure[e])
end

function lut_zone_exit(e)
end