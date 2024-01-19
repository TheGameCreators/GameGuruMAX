-- Range Sensor v8 by Necrym59
-- DESCRIPTION: The attached object will be a range sense to detect player for activating other linked objects or IfUsed game elements or change lavel. Set Always Active = On
-- DESCRIPTION: [SENSOR_RANGE=180(1,10000)]
-- DESCRIPTION: [SENSED_TEXT$="Detected"]
-- DESCRIPTION: [@TRIGGER_MODE=1(1=Linked, 2=Video+ChangeLevel)]
-- DESCRIPTION: [@VIDEO_SKIP=1(1=Yes, 2=No)]
-- DESCRIPTION: [@GoToLevelMode=1(1=Use Storyboard Logic,2=Go to Specific Level)] controls whether to load the next level in the Storyboard, or a specific level.
-- DESCRIPTION: [ResetStates!=0] when entering the new level
-- DESCRIPTION: <Video Slot> for optional ending video


local rangesensor	= {}
local sensor_range	= {}
local sensed_text 	= {}
local trigger_mode	= {}
local video_skip	= {}
local resetstates	= {}

local doonce = {}
local status = {}
local endvid = {}


function range_sensor_properties(e,sensor_range, sensed_text, trigger_mode, video_skip, resetstates)
	rangesensor[e] = g_Entity[e]
	rangesensor[e].sensor_range = sensor_range
	rangesensor[e].sensed_text = sensed_text
	rangesensor[e].trigger_mode = trigger_mode
	rangesensor[e].video_skip = video_skip
	rangesensor[e].resetstates = resetstates	
end

function range_sensor_init(e)
	rangesensor[e] = {}
	rangesensor[e].sensor_range = 80
	rangesensor[e].sensed_text = "Detected"
	rangesensor[e].trigger_mode = 1	
	rangesensor[e].video_skip = 2
	rangesensor[e].resetstates = 0		
	doonce[e] = 0
	endvid[e] = 0
	status[e] = "init"
end

function range_sensor_main(e)

	if status[e] == "init" then
		status[e] = "sensing"
	end
	if status[e] == "sensing" then
		if GetPlayerDistance(e) < rangesensor[e].sensor_range then
			if doonce[e] == 0 then
				if rangesensor[e].trigger_mode == 1 then
					Prompt(rangesensor[e].sensed_text)
					PerformLogicConnections(e)
					ActivateIfUsed(e)
					doonce[e] = 1
				end
				if rangesensor[e].trigger_mode == 2 then
					if endvid[e] == 0 then
						if rangesensor[e].video_skip == 1 then PromptVideo(e,1) end
						if rangesensor[e].video_skip == 2 then PromptVideoNoSkip(e,1) end
						endvid[e] = 1
					end
					if endvid[e] == 1 then
						JumpToLevelIfUsedEx(e,rangesensor[e].resetstates)					
					end
					doonce[e] = 1
				end
			end
		end
		if GetPlayerDistance(e) > rangesensor[e].sensor_range then
			if doonce[e] == 1 then
				doonce[e] = 0
				endvid[e] = 0
			end
		end
	end
end