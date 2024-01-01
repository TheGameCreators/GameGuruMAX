-- Range Sensor v5 by Necrym59
-- DESCRIPTION: The attached object will be a range sense detector for activating other linked objects or game elements. Set Always Active = On
-- DESCRIPTION: [SENSOR_RANGE=180(1,2000)]
-- DESCRIPTION: [SENSED_TEXT$="Detected"]

local rangesensor = {}
local sensor_range = {}
local sensed_text = {}
local activate_entity_name = {}	
local activate_entity_number = {}
local doonce = {}
local status = {}
	
function range_sensor_properties(e,sensor_range, sensed_text)
	rangesensor[e] = g_Entity[e]
	rangesensor[e].sensor_range = sensor_range
	rangesensor[e].sensed_text = sensed_text
end

function range_sensor_init(e)	
	rangesensor[e] = g_Entity[e]
	rangesensor[e].sensor_range = 80
	rangesensor[e].sensed_text = "Detected"
	doonce[e] = 0
	status[e] = "init"	
end

function range_sensor_main(e)
	rangesensor[e] = g_Entity[e]
	if status[e] == "init" then		
		status[e] = "sensing"
	end	
	if status[e] == "sensing" then		
		if GetPlayerDistance(e) < rangesensor[e].sensor_range then			
			if doonce[e] == 0 then
				Prompt(rangesensor[e].sensed_text)
				PerformLogicConnections(e)
				ActivateIfUsed(e)
				doonce[e] = 1
			end
		end
		if GetPlayerDistance(e) > rangesensor[e].sensor_range then
			if doonce[e] == 1 then				
				doonce[e] = 0
			end			
		end	
	end
end