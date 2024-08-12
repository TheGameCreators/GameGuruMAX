-- Textinzone v9 by Necrym59 and Kasseyus
-- DESCRIPTION: While the player is within the zone the [PROMPT_TEXT$=""] is displayed.
-- DESCRIPTION: [ZONEHEIGHT=100] controls how far above the zone the player can be before the zone is not triggered.
-- DESCRIPTION: [!ONLYONCE=0] Set for the text zone to be destroyed after activation.
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: [DISPLAY_TIME=1(1,100)] in seconds to display the text
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Standard, 2=Positional)]
-- DESCRIPTION: [PROMPT_POSITION_X=50(1,100)] for display of the prompt text
-- DESCRIPTION: [PROMPT_POSITION_Y=90(1,100)] for display of the prompt text

local textinzone 		= {}
local prompt_text		= {}
local zoneheight		= {}
local onlyonce			= {}
local spawnatstart		= {}
local display_time		= {}
local prompt_position_x = {}
local prompt_position_y = {}

local status		= {}
local multiuse		= {}
local wait			= {}
local texttime		= {}
local doonce		= {}

function textinzone_properties(e, prompt_text, zoneheight, onlyonce, spawnatstart, display_time, prompt_display, prompt_position_x, prompt_position_y)
	textinzone[e].prompt_text = prompt_text
	textinzone[e].zoneheight = zoneheight
	textinzone[e].onlyonce = onlyonce
	textinzone[e].spawnatstart = spawnatstart
	textinzone[e].display_time = display_time or 1
	textinzone[e].prompt_display = prompt_display	
	textinzone[e].prompt_position_x = prompt_position_x
	textinzone[e].prompt_position_y = prompt_position_y
end

function textinzone_init(e)
	textinzone[e] = {}
	textinzone[e].prompt_text = ""
	textinzone[e].zoneheight = 100
	textinzone[e].onlyonce = 0
	textinzone[e].spawnatstart = 1
	textinzone[e].display_time = 1
	textinzone[e].prompt_display = 1	
	textinzone[e].prompt_position_x = 50
	textinzone[e].prompt_position_y = 90
	
	status[e] = "init"
	wait[e] = math.huge
	texttime[e] = math.huge	
	multiuse[e] = 0
	doonce[e] = 0
end

function textinzone_main(e)

	if status[e] == "init" then
		if textinzone[e].spawnatstart == 1 then SetActivated(e,1) end
		if textinzone[e].spawnatstart == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end

	if g_Entity[e]['activated'] == 1 then
		if g_Entity[e]['plrinzone'] == 1 and multiuse[e] == 0 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+textinzone[e].zoneheight then
			if doonce[e] == 0 then				
				if textinzone[e].prompt_display == 1 then PromptDuration(textinzone[e].prompt_text,textinzone[e].display_time * 1000) end				
				if textinzone[e].prompt_display == 2 then texttime[e] = g_Time + textinzone[e].display_time * 1000 end				
				wait[e] = g_Time + textinzone[e].display_time * 1000
				doonce[e] = 1
			end
			if textinzone[e].prompt_display == 2 and g_Time < texttime[e] then
				TextCenterOnX(textinzone[e].prompt_position_x, textinzone[e].prompt_position_y, 3, textinzone[e].prompt_text)
			end
		end	
			
		if g_Time > wait[e] then
			if textinzone[e].onlyonce == 0 and multiuse[e] == 0 then
				if doonce[e] == 1 then
					ActivateIfUsed(e)
					PerformLogicConnections(e)
					multiuse[e] = 1
					doonce[e] = 2
				end	
			end
			if textinzone[e].onlyonce == 1 then
				ActivateIfUsed(e)
				PerformLogicConnections(e)
				Destroy(e)
			end
		end
		
		if g_Entity[e]['plrinzone'] == 0 and multiuse[e] == 1 then
			doonce[e] = 0
			multiuse[e] = 0
		end
	end	
end
