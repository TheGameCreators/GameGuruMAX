-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Ambience v1 by Necrym59
-- DESCRIPTION: When activated will change ambience to the required settings. Secondary activation will return ambience to default settings.
-- DESCRIPTION: Add to an object then link to a switch or multi-trigger zone to activate/de-activate.
-- DESCRIPTION: [PROMPT_TEXT$="Ambience is changing"]
-- DESCRIPTION: [AMBIENCE_R=255(1,255)]
-- DESCRIPTION: [AMBIENCE_G=255(1,255)]
-- DESCRIPTION: [AMBIENCE_B=255(1,255)]
-- DESCRIPTION: [EXPOSURE#=1.00(0.01,1.00)]

local ambience 			= {}
local prompt_text 		= {}
local ambience_r		= {}
local ambience_g		= {}
local ambience_b		= {}
local exposure			= {}

local status 				= {}
local default_ambience_r	= {}
local default_ambience_g	= {}
local default_ambience_b	= {}
local default_exposure		= {}
local ambienceswitch		= {}
local doonce				= {}	
local played				= {}

function ambience_properties(e, prompt_text, ambience_r, ambience_g, ambience_b, exposure)
	ambience[e].prompt_text = prompt_text
	ambience[e].ambience_r = ambience_r
	ambience[e].ambience_g = ambience_g
	ambience[e].ambience_b = ambience_b	
	ambience[e].exposure = exposure
end

function ambience_init(e)
	ambience[e] = {}
	ambience[e].prompt_text = "Ambience changed"
	ambience[e].ambience_r = GetAmbienceRed()
	ambience[e].ambience_g = GetAmbienceGreen()
	ambience[e].ambience_b = GetAmbienceBlue()
	ambience[e].exposure = GetExposure()
	
	ambienceswitch[e] = 0
	status[e] = "init"
	doonce[e] = 0	
	played[e] = 0
end

function ambience_main(e)

	if status[e] == "init" then
		ambienceswitch[e] = 0
		default_ambience_r[e] = GetAmbienceRed()
		default_ambience_g[e] = GetAmbienceGreen()
		default_ambience_b[e] = GetAmbienceBlue()
		default_exposure[e]	= GetExposure()
		SetActivated(e,0)
		status[e] = "endinit"
	end
	
	if g_Entity[e]['activated'] == 1 and ambienceswitch[e] == 0 then
		PerformLogicConnections(e)
		Prompt(ambience[e].prompt_text)			
		SetAmbienceRed(ambience[e].ambience_r)
		SetAmbienceGreen(ambience[e].ambience_g)
		SetAmbienceBlue(ambience[e].ambience_b)
		SetExposure(ambience[e].exposure)	
		ambienceswitch[e] = 1
		SetActivated(e,0)
	end

	if g_Entity[e]['activated'] == 1 and ambienceswitch[e] == 1 then
		PerformLogicConnections(e)
		Prompt(ambience[e].prompt_text)
		SetAmbienceRed(default_ambience_r[e])
		SetAmbienceGreen(default_ambience_g[e])
		SetAmbienceBlue(default_ambience_b[e])
		SetExposure(default_exposure[e])	
		ambienceswitch[e] = 0
		SetActivated(e,0)
	end
end
