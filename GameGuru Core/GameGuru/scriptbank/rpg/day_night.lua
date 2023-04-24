-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Day_Night v5 by Necrym59
-- DESCRIPTION: A Day/Night Time Cycler. Set ALLWAYS ON
-- DESCRIPTION: [#START_ANGLE=-95(-95,95)]
-- DESCRIPTION: [TIME_DIALATION=0(0,5000)]
-- DESCRIPTION: [DIAGNOSTICS!=0]
-- DESCRIPTION: [MIN_AMBIENCE_R=10(0,255)]
-- DESCRIPTION: [MIN_AMBIENCE_G=25(0,255)]
-- DESCRIPTION: [MIN_AMBIENCE_B=75(0,255)]
-- DESCRIPTION: [MIN_EXPOSURE#=0.45(0.01,1.00)]

local day_night = {}
local start_angle = {}
local time_dialation = {}
local diagnostics = {}
local min_ambience_r = {}
local min_ambience_g = {}
local min_ambience_b = {}
local min_exposure = {}


local suntimer = {}
local sunmoonangle = {}
local ambrvalue = {}
local ambgvalue = {}
local ambbvalue = {}
local expovalue = {}

local status = {}
local mode = {}

function day_night_properties(e, start_angle, time_dialation, diagnostics, min_ambience_r, min_ambience_g, min_ambience_b, min_exposure)
	day_night[e] = g_Entity[e]
	day_night[e].start_angle = start_angle
	day_night[e].time_dialation = time_dialation
	day_night[e].diagnostics = diagnostics
	day_night[e].min_ambience_r = min_ambience_r
	day_night[e].min_ambience_g = min_ambience_g	
	day_night[e].min_ambience_b = min_ambience_b
	day_night[e].min_exposure = min_exposure
end

function day_night_init(e)
	day_night[e] = g_Entity[e]
	day_night[e].start_angle = -95
	day_night[e].time_dialation = 0
	day_night[e].diagnostics = 1	
	day_night[e].min_ambience_r = 0
	day_night[e].min_ambience_g = 0
	day_night[e].min_ambience_b = 0
	day_night[e].min_exposure = 0.00
	status[e] = "init"
	sunmoonangle[e] = 0
	suntimer[e] = 0
	mode[e] = ""
end

function day_night_main(e)
	day_night[e] = g_Entity[e]

	if status[e] == "init" then
		ambrvalue[e] = day_night[e].min_ambience_r
		ambgvalue[e] = day_night[e].min_ambience_g
		ambbvalue[e] = day_night[e].min_ambience_b
		expovalue[e] = day_night[e].min_exposure
		if day_night[e].time_dialation > 1000 then day_night[e].time_dialation = 1000 end
		sunmoonangle[e] = day_night[e].start_angle
		if sunmoonangle[e] < 90 then mode[e] = "Day" end		
		if sunmoonangle[e] > 90 then mode[e] = "Night" end
		SetSunDirection(sunmoonangle[e],75,0)
		suntimer[e] = g_Time + 1000 - day_night[e].time_dialation
		status[e] = "endinit"
	end   
	
	if g_Time > suntimer[e] then		
		sunmoonangle[e] = (sunmoonangle[e] + 0.0042) --1 Sec = 0.0042 deg
		suntimer[e] = g_Time + 1000 - day_night[e].time_dialation
	end
	SetSunDirection(sunmoonangle[e],75,0)
	
	if sunmoonangle[e] > 100 then sunmoonangle[e] = -100 end  --Reset day restart sunposition
	
	if mode[e] == "Night" then
		SetSunLightingColor(2,15,75)
		SetSunIntensity(3.2)
		SetExposure(expovalue[e])
		SetAmbienceRed(ambrvalue[e])
		SetAmbienceGreen(ambgvalue[e])
		SetAmbienceBlue(ambbvalue[e])
		SetAmbienceIntensity(120)
		--Swap in Moon Image if possible
		if sunmoonangle[e] > 99 then
			mode[e] = "Day"
		end
	end
	if mode[e] == "Day" then
		SetSunLightingColor(255,255,255)
		SetSunIntensity(7.4)
		SetExposure(expovalue[e])
		SetAmbienceRed(ambrvalue[e])
		SetAmbienceGreen(ambgvalue[e])
		SetAmbienceBlue(ambbvalue[e])
		SetAmbienceIntensity(120)
		if sunmoonangle[e] < -20 and sunmoonangle[e] > -90 and ambrvalue[e] < 255 then ambrvalue[e] = ambrvalue[e] + 0.1 end
		if sunmoonangle[e] < -20 and sunmoonangle[e] > -90 and ambgvalue[e] < 255 then ambgvalue[e] = ambgvalue[e] + 0.1 end
		if sunmoonangle[e] < -20 and sunmoonangle[e] > -90 and ambbvalue[e] < 255 then ambbvalue[e] = ambbvalue[e] + 0.1 end
		if sunmoonangle[e] < -20 and sunmoonangle[e] > -90 and expovalue[e] < 1.00 then expovalue[e] = expovalue[e] + 0.0002 end
		if sunmoonangle[e] > 0 and sunmoonangle[e] < 85 then ambrvalue[e] = 255 end
		if sunmoonangle[e] > 0 and sunmoonangle[e] < 85 then ambgvalue[e] = 255 end
		if sunmoonangle[e] > 0 and sunmoonangle[e] < 85 then ambbvalue[e] = 255 end
		if sunmoonangle[e] > 0 and sunmoonangle[e] < 85 then expovalue[e] = 1.00 end
		if sunmoonangle[e] > 85 and ambrvalue[e] > day_night[e].min_ambience_r then ambrvalue[e] = ambrvalue[e] - 0.2 end
		if sunmoonangle[e] > 85 and ambgvalue[e] > day_night[e].min_ambience_g then ambgvalue[e] = ambgvalue[e] - 0.2 end
		if sunmoonangle[e] > 85 and ambbvalue[e] > day_night[e].min_ambience_b then ambbvalue[e] = ambbvalue[e] - 0.1 end
		if sunmoonangle[e] > 85 and expovalue[e] > day_night[e].min_exposure then expovalue[e] = expovalue[e] - 0.0002 end

		--Swap in Sun Image if possible
		if sunmoonangle[e] > 99 then
			mode[e] = "Night"
		end		
	end
	if day_night[e].diagnostics == 1 then
		Text(1,24,3,"Sun/Moon Angle: " ..math.floor(sunmoonangle[e]))
		Text(1,26,3,"Time Mode: " ..mode[e])	
		Text(1,28,3,"Dialation: " ..day_night[e].time_dialation/100)
		Text(1,30,3,"Ambience R: " ..math.floor(ambrvalue[e]))
		Text(1,32,3,"Ambience G: " ..math.floor(ambgvalue[e]))
		Text(1,34,3,"Ambience B: " ..math.floor(ambbvalue[e]))
		Text(1,36,3,"Exposure: " ..math.floor(expovalue[e]*100)/100)
	end
end

function day_night_exit(e)
end