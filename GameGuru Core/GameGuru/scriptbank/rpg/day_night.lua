-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Day_Night v3 by Necrym59
-- DESCRIPTION: A Day/Night Time Cycler. Set ALLWAYS ON
-- DESCRIPTION: [#START_ANGLE=-95(-95,95)]
-- DESCRIPTION: [TIME_DIALATION=0(0,1000)]
-- DESCRIPTION: [DIAGNOSTICS!=0]

local day_night = {}
local start_angle = {}
local time_dialation = {}
local diagnostics = {}

local suntimer = {}
local sunangle = {}
local status = {}
local mode = {}

function day_night_properties(e, start_angle, time_dialation, diagnostics)
	day_night[e] = g_Entity[e]
	day_night[e].start_angle = start_angle
	day_night[e].time_dialation = time_dialation
	day_night[e].diagnostics = diagnostics
end

function day_night_init(e)
	day_night[e] = g_Entity[e]
	day_night[e].start_angle = -95
	day_night[e].time_dialation = 0
	day_night[e].diagnostics = 0
	status[e] = "init"
	sunangle[e] = 0
	suntimer[e] = 0
	mode[e] = ""
end

function day_night_main(e)
	day_night[e] = g_Entity[e]

	if status[e] == "init" then
		if day_night[e].time_dialation > 1000 then day_night[e].time_dialation = 1000 end
		sunangle[e] = day_night[e].start_angle
		SetSunDirection(sunangle[e],75,0)
		suntimer[e] = g_Time + 1000 - day_night[e].time_dialation
		status[e] = "endinit"
	end   
	
	if g_Time > suntimer[e] then		
		sunangle[e] = (sunangle[e] + 0.0042) --1 Sec = 0.0042 deg
		suntimer[e] = g_Time + 1000 - day_night[e].time_dialation
	end
	SetSunDirection(sunangle[e],75,0)
	
	if sunangle[e] > -95 and sunangle[e] < 95 then mode[e] = "Day" end
	if sunangle[e] > 100 then mode[e] = "Night" end
	if sunangle[e] > 100 then sunangle[e] = -100 end  --Reset day restart sunposition
	if mode[e] == "Night" then
		SetSunLightingColor(25,255,255)
		SetSunIntensity(75)
		--SetSunExposure(75)
		SetAmbienceRed(25)
		SetAmbienceBlue(30)
		SetAmbienceGreen(40)
		--Swap in Moon Image if possible
	end
	if mode[e] == "Day" then
		SetSunLightingColor(255,255,255)
		SetSunIntensity(75)
		--SetSunExposure(75)
		SetAmbienceRed(255)
		SetAmbienceBlue(255)
		SetAmbienceGreen(255)
		--Swap in Sun Image if possible
	end
	if day_night[e].diagnostics == 1 then
		Text(1,24,3,"Sun Angle: " ..sunangle[e])
		Text(1,26,3,"Time Mode: " ..mode[e])	
		Text(1,28,3,"Dialation: " ..day_night[e].time_dialation/1000)
	end
end

function day_night_exit(e)
end