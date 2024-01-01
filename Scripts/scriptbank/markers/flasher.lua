-- Flasher v7
-- DESCRIPTION: Flashes the light like a lightning flash.
-- DESCRIPTION: Attach to a light. Trigger by range, if No then activate by switch or zone.
-- DESCRIPTION: [@RANGE_SENSING=1(1=Yes, 2=No)]
-- DESCRIPTION: [SENSOR_RANGE=100(1,1000)]
-- DESCRIPTION: Specify [FLASHES=30]
-- DESCRIPTION: Set the [FLASH_TIME=2]
-- DESCRIPTION: Set the [FLASH_DELAY=4]
-- DESCRIPTION: <Sound0> for flash sound1
-- DESCRIPTION: <Sound1> for flash sound2

module_lightcontrol = require "scriptbank\\markers\\module_lightcontrol"

g_lightflash = {}

local range_sensing 		= {}
local sensor_range 			= {}
local wait 					= {}
local status 				= {}
local played 				= {}
local doonce 				= {}
local randsound 			= {}

function flasher_properties(e, range_sensing, sensor_range, flashes, flash_time, flash_delay, lighton)
	module_lightcontrol.init(e,lighton)
	g_lightflash[e]['range_sensing'] = range_sensing
	g_lightflash[e]['sensor_range'] = sensor_range
	g_lightflash[e]['flashes'] = flashes
	g_lightflash[e]['flash_time'] = flash_time
	g_lightflash[e]['flash_delay'] = flash_delay
end 

function flasher_init(e)
	g_lightflash[e] = {}
	lightNum = GetEntityLightNumber(e)
	g_lightflash[e]['range'] = GetLightRange(lightNum)
	flasher_properties(e,1,100,30,3,4,0)
	wait[e] = math.huge
	played[e] = 0
	doonce[e] = 0
	randsound[e] = 0
	status[e] = "init"	
end

function flasher_main(e)
	if status[e] == "init" then		
		SetLightRange(lightNum,0)
		wait[e] = g_Time + (g_lightflash[e]['flash_delay']*1000)
		randsound[e] = math.random(0,50)
		status[e] = "flash"
		played[e] = 0
	end
	
	if g_lightflash[e]['range_sensing'] == 1 then		
		if GetPlayerDistance(e) < g_lightflash[e]['sensor_range'] and doonce[e] == 0 then
			SetActivated(e,1)
			status[e] = "init"
			doonce[e] = 1
		end
		if GetPlayerDistance(e) > g_lightflash[e]['sensor_range'] then
			SetActivated(e,0)
			doonce[e] = 0
		end		
	end
	
	if g_Entity[e]['activated'] == 1 then
		if status[e] == "flash" then
			if g_Time > wait[e] then
				if module_lightcontrol.control(e) == 1 then	
					lightNum = GetEntityLightNumber(e)		
					local nRandom = math.random(0,800)
					local nRange = math.random(g_lightflash[e]['range']/3,g_lightflash[e]['range'])		
					if nRandom > g_lightflash[e]['flashes'] then
						SetLightRange(lightNum,0)
						if played[e] == 0 and g_Time > wait[e] + 100 then							
							if randsound[e] < 25 then PlaySound(e,0) end
							if randsound[e] > 25 then PlaySound(e,1) end
							played[e] = 1
						end
					else
						SetLightRange(lightNum,nRange)				
					end
				end
				if g_Time > wait[e] + (g_lightflash[e]['flash_delay']*(g_lightflash[e]['flash_time']*1000)) then
					status[e] = "init"
				end
			end
		end
	end
end	