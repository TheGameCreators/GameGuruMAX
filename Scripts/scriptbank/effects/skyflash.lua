-- Sky Flash v2 by Necrym59
-- DESCRIPTION: Will create a sky sheet lightning flash effect.
-- DESCRIPTION: Attach to a object set, activate by switch or zone.
-- DESCRIPTION: [FLASH_THRESHOLD=20] Flash Level Threshold 
-- DESCRIPTION: [FLASH_INTERVAL=6(1,60)] Randomised interval in seconds added to flash delay
-- DESCRIPTION: [FLASH_DELAY=8(1,60)] Seconds between flash sets
-- DESCRIPTION: <Sound0> for flash sound1
-- DESCRIPTION: <Sound1> for flash sound2

local skyflash 				= {}
local flash_threshold		= {}
local flash_interval 		= {}
local flash_delay 			= {}

local wait 					= {}
local status 				= {}
local played 				= {}
local doonce 				= {}
local randsound 			= {}
local nRandom 				= {}
local eRandom 				= {}
local default_Exposure 		= {}
local flashcount			= {}

function skyflash_properties(e, flash_threshold, flash_interval, flash_delay)
	skyflash[e].flash_threshold = flash_threshold
	skyflash[e].flash_interval = flash_interval
	skyflash[e].flash_delay = flash_delay
end

function skyflash_init(e)
	skyflash[e] = {}
	skyflash[e].flash_threshold = 30
	skyflash[e].flash_interval = 3
	skyflash[e].flash_delay = 4

	wait[e] = math.huge
	played[e] = 0
	doonce[e] = 0
	randsound[e] = 0
	flashcount[e] = 0
	status[e] = "init"
end

function skyflash_main(e)
	
	if status[e] == "init" then
		wait[e] = g_Time + (skyflash[e].flash_delay*1000)
		randsound[e] = math.random(0,50)
		flashcount[e] = 0
		played[e] = 0
		status[e] = "skyflash"
	end
	
	if g_Entity[e].activated == 1 then
		if status[e] == "skyflash" then
			if doonce[e] == 0 then
				default_Exposure[e] = GetExposure()
				doonce[e] = 1
			end		
			if g_Time > wait[e] then
				nRandom[e] = math.random(0,100)
				eRandom[e] = math.random(default_Exposure[e],default_Exposure[e]+0.1)
				if flashcount[e] < skyflash[e].flash_threshold and nRandom[e] > skyflash[e].flash_threshold then
					if nRandom[e] > 50 then
						SetExposure(eRandom[e])						
					end
					if played[e] == 0 and g_Time > wait[e] + 100 then
						if randsound[e] < 25 then PlaySound(e,0) end
						if randsound[e] > 25 then PlaySound(e,1) end
						played[e] = 1
					end
					flashcount[e] = flashcount[e] + math.random(0.1,0.3)
				else
					SetExposure(default_Exposure[e])
				end
				if flashcount[e] >= skyflash[e].flash_threshold then
					wait[e] = g_Time + (skyflash[e].flash_delay*1000+(math.random(1000,skyflash[e].flash_interval*1000)))
					SetExposure(default_Exposure[e])
					randsound[e] = math.random(0,50)
					flashcount[e] = 0
					played[e] = 0
				end
			end			
		end
	end	
end