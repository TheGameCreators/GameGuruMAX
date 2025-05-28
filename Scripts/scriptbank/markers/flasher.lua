-- Flasher v8 by Necrym59
-- DESCRIPTION: Will flash the light like a lightning effect.
-- DESCRIPTION: Attach to a light.
-- DESCRIPTION: [@ACTIVATION=1(1=Sensing, 2=Switch/Zone, 3=Always Active)]
-- DESCRIPTION: [SENSOR_RANGE=100(1,1000)]
-- DESCRIPTION: [FLASH_THRESHOLD=20] Flash Level Threshold 
-- DESCRIPTION: [FLASH_INTERVAL=6(1,60)] Randomised interval in seconds added to flash delay
-- DESCRIPTION: [FLASH_DELAY=8(1,60)] Seconds between flash sets
-- DESCRIPTION: [@ENVIRONMENT_FLASH=1(1=Off, 2=On)] Environmental flash effect
-- DESCRIPTION: <Sound0> for flash sound1
-- DESCRIPTION: <Sound1> for flash sound2

module_lightcontrol = require "scriptbank\\markers\\module_lightcontrol"

local lightflash 			= {}
local activation 			= {}
local sensor_range 			= {}
local flash_threshold		= {}
local flash_interval 		= {}
local flash_delay 			= {}
local environment_flash		= {}
local wait 					= {}
local status 				= {}
local played 				= {}
local doonce 				= {}
local randsound 			= {}
local nRandom 				= {}
local eRandom 				= {}
local nRange 				= {}
local default_Intensity 	= {}
local default_Exposure 		= {}
local flashcount			= {}

function flasher_properties(e, activation, sensor_range, flash_threshold, flash_interval, flash_delay, environment_flash)
	module_lightcontrol.init(e,1)
	lightflash[e].activation = activation
	lightflash[e].sensor_range = sensor_range
	lightflash[e].flash_threshold = flash_threshold
	lightflash[e].flash_interval = flash_interval
	lightflash[e].flash_delay = flash_delay
	lightflash[e].environment_flash = environment_flash or 1
end

function flasher_init(e)
	lightflash[e] = {}
	lightflash[e].activation = 1
	lightflash[e].sensor_range = 100
	lightflash[e].flash_threshold = 30
	lightflash[e].flash_interval = 3
	lightflash[e].flash_delay = 4
	lightflash[e].environment_flash = 1

	lightNum = GetEntityLightNumber(e)
	lightflash[e].range = GetLightRange(lightNum)
	wait[e] = math.huge
	played[e] = 0
	doonce[e] = 0
	randsound[e] = 0
	flashcount[e] = 0
	default_Intensity[e] = GetSunIntensity()/100
	default_Exposure[e] = GetExposure()
	SetLightRange(lightNum,0)
	SetExposure(default_Exposure[e])
	--SetSunIntensity(default_Intensity[e]*100)
	status[e] = "init"
end

function flasher_main(e)
	if status[e] == "init" then
		SetLightRange(lightNum,0)
		SetExposure(default_Exposure[e])		
		wait[e] = g_Time + (lightflash[e].flash_delay*1000)
		randsound[e] = math.random(0,50)
		flashcount[e] = 0
		played[e] = 0
		status[e] = "flash"
	end
	
	if lightflash[e].activation == 1 then
		if GetPlayerDistance(e) < lightflash[e].sensor_range and doonce[e] == 0 then
			SetActivated(e,1)
			status[e] = "init"
			doonce[e] = 1
		end
		if GetPlayerDistance(e) > lightflash[e].sensor_range then
			SetActivated(e,0)
			doonce[e] = 0
		end
	end
	if lightflash[e].activation == 3 then SetActivated(e,1)	end

	if g_Entity[e].activated == 1 then
		if status[e] == "flash" then
			if g_Time > wait[e] then
				if module_lightcontrol.control(e) == 1 then
					lightNum = GetEntityLightNumber(e)
					nRandom[e] = math.random(0,100)
					eRandom[e]= math.random(default_Exposure[e],default_Exposure[e]+0.1)
					nRange[e] = math.random(lightflash[e].range/3,lightflash[e].range)
					if flashcount[e] < lightflash[e].flash_threshold and nRandom[e] > lightflash[e].flash_threshold then
						SetLightRange(lightNum,nRange[e])
						if lightflash[e].environment_flash == 2 and nRandom[e] > 50 then
							SetExposure(eRandom[e])
							SetSunIntensity(eRandom[e]/20)
						end
						if played[e] == 0 and g_Time > wait[e] + 100 then
							if randsound[e] < 25 then PlaySound(e,0) end
							if randsound[e] > 25 then PlaySound(e,1) end
							played[e] = 1
						end
						flashcount[e] = flashcount[e] + math.random(0.1,0.3)
					else
						SetLightRange(lightNum,0)
						SetExposure(default_Exposure[e])
						SetSunIntensity(default_Intensity[e]/2)
					end
					if flashcount[e] >= lightflash[e].flash_threshold then
						wait[e] = g_Time + (lightflash[e].flash_delay*1000+(math.random(1000,lightflash[e].flash_interval*1000)))
						SetSunIntensity(default_Intensity[e]/2)						
						if lightflash[e].activation == 3 then SetActivated(e,0) end
						status[e] = "init"
					end
				end
			end
		end
	end
end