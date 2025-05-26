-- LUT_Changer v7 by Necrym59 with thanks to Preben and Graphix
-- DESCRIPTION: LUT will be changed when activated.
-- DESCRIPTION: Atach to an object and link to a switch or zone
-- DESCRIPTION: [PROMPT_TEXT$="LUT Changed"] displayed when activated.
-- DESCRIPTION: [LUT_GROUP$="editors\lut\ps_strongamber"] LUT group to change to (without ".png" file extension).
-- DESCRIPTION: [@TRANSITION=1(1=Instant, 2=Fade, 3=Incremental)] Type of LUT change
-- DESCRIPTION: [NUMBER_OF_INCREMENTS=0(0,100)] Number of incremental LUTS.
-- DESCRIPTION: [INCREMENT_DELAY=100(0,500)] milliseconds
-- DESCRIPTION: <Sound0> when LUT is changed

local lutchanger 			= {}
local prompt_text			= {}
local lut_group				= {}
local transition			= {}
local no_of_increments		= {}
local increment_delay		= {}

local status		= {}
local newlut		= {}
local oldlut		= {}
local lutswitch		= {}
local lutcount		= {}
local luttimer		= {}
local doonce		= {}
local fadeexposure	= {}
local fadeoutlut	= {}
local fadeinlut		= {}

function lut_changer_properties(e, prompt_text, lut_group, transition, no_of_increments, increment_delay)
	lutchanger[e].prompt_text = prompt_text
	lutchanger[e].lut_group = lut_group
	lutchanger[e].transition = transition	
	lutchanger[e].no_of_increments = no_of_increments
	lutchanger[e].increment_delay = increment_delay	
end

function lut_changer_init(e)
	lutchanger[e] = {}
	lutchanger[e].prompt_text = ""
	lutchanger[e].lut_group = ""
	lutchanger[e].transition = 1	
	lutchanger[e].no_of_increments = 0
	lutchanger[e].increment_delay = 100

	status[e] = "init"
	doonce[e] = 0
	newlut[e] = ""
	oldlut[e] = GetLut()
	lutswitch[e] = 0
	lutcount[e] = 0
	luttimer[e] = math.huge
	fadeexposure[e] = 0
	fadeoutlut[e] = 0
	fadeinlut[e] = 9999
end

function lut_changer_main(e)

	if status[e] == "init" then
		if lutswitch[e] == 0 then newlut[e] = lutchanger[e].lut_group end
		if lutswitch[e] == 1 then newlut[e] = oldlut[e]	end
		luttimer[e] = g_Time + 10
		fadeexposure[e] = GetExposure()
		fadeoutlut[e] = fadeexposure[e]
		status[e] = "endinit"
	end

	if g_Entity[e]['activated'] == 1 then
	
		if doonce[e] == 0 then
			Prompt(lutchanger[e].prompt_text)
			PlaySound(e,0)
			PerformLogicConnections(e)
			ActivateIfUsed(e)
			doonce[e] = 1
		end
		
		if doonce[e] == 1 then	
			if lutswitch[e] == 0 then
				lutswitch[e] = 1
			else
				lutswitch[e] = 0
			end
			if lutchanger[e].transition == 1 then  -- Instant Change
				SetLutTo(newlut[e].. ".png")
				doonce[e] = 2
			end			
			if lutchanger[e].transition == 2 then	-- Quick Fadein		
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
			if lutchanger[e].transition == 3 then  -- Incremental Plus
				if lutcount[e] < lutchanger[e].no_of_increments and g_Time > luttimer[e] then
					SetLutTo(newlut[e].. "" ..lutcount[e].. ".png")
					lutcount[e] = lutcount[e] + 1
					luttimer[e] = g_Time + lutchanger[e].increment_delay
					if lutcount[e] == lutchanger[e].no_of_increments then
						SetLutTo(newlut[e].. ".png")					
						doonce[e] = 2						
					end
				end
			end
		end
		if doonce[e] == 2 then
			SetActivated(e,0)
			fadeinlut[e] = 9999
			doonce[e] = 0			
			status[e] = "init"
		end		
	end	
end

function lut_changer_preexit(e)
    SetExposure(fadeexposure[e])
end

function lut_changer_exit(e)
end