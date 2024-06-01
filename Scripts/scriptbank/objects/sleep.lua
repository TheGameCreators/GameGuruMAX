-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Sleep v3 by Necrym59
-- DESCRIPTION: Allows player to sleep for the time set
-- DESCRIPTION: [USE_RANGE=80(0,300)]
-- DESCRIPTION: [PROMPT_TEXT$="E to sleep"]
-- DESCRIPTION: [USE_TEXT$="1-8 to set Hours, Press ENTER to confirm"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: <Sound0> Sleeping Sound

g_sunrollposition = {}
g_updatedposition = {}

local sleep					= {}
local use_range				= {}
local prompt_text			= {}
local use_text				= {}
local time_period 			= {}
local prompt_display		= {}
	
local status			= {}
local pressed			= {}
local sleep_sprite 		= {}
local current_level 	= {}
local timeshift			= {}
local timecheck			= {}
	
function sleep_properties(e, use_range, prompt_text, use_text, prompt_display, time_period )
	sleep[e].use_range = use_range
	sleep[e].prompt_text = prompt_text
	sleep[e].use_text = use_text
	sleep[e].prompt_display = prompt_display	
	sleep[e].time_period = 0
	
end

function sleep_init(e)
	sleep[e] = {}
	sleep[e].use_range = 80
	sleep[e].prompt_text = "E to sleep"
	sleep[e].use_text = "1-8 to set Hours, Press ENTER to confirm"
	sleep[e].prompt_display = 1
	sleep[e].time_period = 0	
	
	status[e] = "init"
	pressed[e] = 0
	current_level[e] = 0
	timeshift[e] = 0
	timecheck[e] = 0
	g_updatedposition = 0
end

function sleep_main(e)

	if status[e] == "init" then
		timecheck[e] = g_Time + 2000
		status[e] = "start"
	end
	
	local PlayerDist = GetPlayerDistance(e)
	local LookingAt = GetPlrLookingAtEx(e,1)
	
	if status[e] == "start" then		
		if PlayerDist < sleep[e].use_range and LookingAt == 1 then
			if sleep[e].prompt_display == 1 then PromptLocal(e,sleep[e].prompt_text) end
			if sleep[e].prompt_display == 2 then Prompt(sleep[e].prompt_text) end
			if g_KeyPressE == 1 and pressed[e] == 0 then pressed[e] = 1 end	
			if pressed[e] == 1 then
				local key = GetKeyPressed(e,g_InKey)
				if key == "1" or key == "2" or key == "3" or key == "4" or key == "1" or key == "5" or key == "6" or key == "7" or key == "8" then sleep[e].time_period = key end
				if sleep[e].prompt_display == 1 then
					PromptLocal(e,"Sleep for " ..sleep[e].time_period.. " hours.")
					TextCenterOnX(50,95,1,sleep[e].use_text)	
				end
				if sleep[e].prompt_display == 2 then
					Prompt("Sleep for " ..sleep[e].time_period.. " hours.")
					TextCenterOnX(50,95,3,sleep[e].use_text)
				end				
				if g_Scancode == 28 then
					SetCameraOverride(3)
					status[e] = "sleep"
				end
			end	
		end
	end

	if status[e] == "sleep" then
		LoopSound(e,0)
		if sleep[e].prompt_display == 1 then PromptLocal(e,"Sleeping for " ..timeshift[e].. " hours") end
		if sleep[e].prompt_display == 2 then Prompt("Sleeping for " ..timeshift[e].. " hours") end
		if timeshift[e] < tonumber(sleep[e].time_period) and g_Time > timecheck[e] then
			g_updatedposition = g_updatedposition + 15
			timeshift[e] = timeshift[e] + 1
			if timeshift[e] >= tonumber(sleep[e].time_period) then timeshift[e] = tonumber(sleep[e].time_period) end			
			timecheck[e] = g_Time + 1000
		end
		if timeshift[e] == tonumber(sleep[e].time_period) then
			SetCameraOverride(0)
			StopSound(e,0)
			pressed[e] = 0
			status[e] = "start"
		end	
	end		
end

function GetKeyPressed(e, key, ignorecase)
    key = key or ""
    ignorecase = ignorecase or false 
    lastpressed = lastpressed or {}
    e = e or 0
    lastpressed[e] = lastpressed[e] or {}
    local inkey = g_InKey
    if ignorecase then
        key = string.lower(key)
        inkey = string.lower(g_InKey)
    end
    local waspressed
    if inkey == key and lastpressed[e] ~= g_InKey then
        waspressed = g_InKey 
    else
        waspressed = "false"
    end
    lastpressed[e] = g_InKey
    return waspressed
end
