-- Textinzone v6
-- DESCRIPTION: While the player is within the zone the [PROMPT_TEXT$=""] is displayed.
-- DESCRIPTION: [ZONEHEIGHT=100] controls how far above the zone the player can be before the zone is not triggered.
-- DESCRIPTION: [!ONLYONCE=0] Set for the text zone to be destroyed after activation.
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: [DISPLAY_TIME=1(1,100)] in seconds to display the text

local textinzone 	= {}
local prompt_text	= {}
local zoneheight	= {}
local onlyonce		= {}
local spawnatstart	= {}
local display_time	= {}

local status		= {}
local multiuse		= {}
local wait			= {}
local doonce		= {}

function textinzone_properties(e, prompt_text, zoneheight, onlyonce, spawnatstart, display_time)
	textinzone[e].prompt_text = prompt_text
	textinzone[e].zoneheight = zoneheight
	textinzone[e].onlyonce = onlyonce
	textinzone[e].spawnatstart = spawnatstart
	textinzone[e].display_time = display_time or 1
end

function textinzone_init(e)
	textinzone[e] = {}
	textinzone[e].prompt_text = ""
	textinzone[e].zoneheight = 100
	textinzone[e].onlyonce = 0
	textinzone[e].spawnatstart = 1
	textinzone[e].display_time = 1

	status[e] = "init"
	wait[e] = math.huge
	multiuse[e] = 0
	doonce[e] = 0
end

function textinzone_main(e)

	if status[e] == "init" then
		if textinzone[e].spawnatstart == 1 then SetActivated(e,1) end
		if textinzone[e].spawnatstart == 0 then SetActivated(e,0) end
		status = "endinit"
	end

	if g_Entity[e]['activated'] == 1 then
		if g_Entity[e]['plrinzone'] == 1 and multiuse[e] == 0 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+textinzone[e].zoneheight then
			if doonce[e] == 0 then
				PromptDuration(textinzone[e].prompt_text,textinzone[e].display_time * 1000)
				wait[e] = g_Time + textinzone[e].display_time * 1000
				doonce[e] = 1
			end
		end
		if g_Entity[e]['plrinzone'] == 1 and g_Time > wait[e] and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+textinzone[e].zoneheight then
			if textinzone[e].onlyonce == 0 and multiuse[e] == 0 then
				ActivateIfUsed(e)
				PerformLogicConnections(e)
				multiuse[e] = 1
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
