-- Textinzone v3
-- DESCRIPTION: While the player is within the zone the [PROMPTTEXT$=""] is displayed on screen. 
-- DESCRIPTION: [ZONEHEIGHT=100] controls how far above the zone the player can be before the zone is not triggered.
-- DESCRIPTION: Set[!ONLYONCE=0] for the text zone to be destroyed after activation.
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone

local textinzone = {}
local status = {}
local wait = {}
local doonce = {}

function textinzone_properties(e, prompttext, zoneheight, onlyonce, spawnatstart)
	textinzone[e].prompttext = prompttext
	textinzone[e].zoneheight = zoneheight
	textinzone[e].onlyonce = onlyonce
	textinzone[e].spawnatstart = spawnatstart
end 

function textinzone_init(e)
	textinzone[e] = {}
	textinzone[e].prompttext = ""
	textinzone[e].zoneheight = 100
	textinzone[e].onlyonce = 0
	textinzone[e].spawnatstart = 1
	status[e] = "init"
	wait[e] = math.huge
	doonce[e] = 0
end

function textinzone_main(e)

	if status[e] == "init" then
		if textinzone[e].spawnatstart == 1 then SetActivated(e,1) end
		if textinzone[e].spawnatstart == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end

	if g_Entity[e]['activated'] == 1 then
		if textinzone[e].zoneheight == nil then textinzone[e].zoneheight = 100 end
		if textinzone[e].zoneheight ~= nil then
			if g_Entity[e].plrinzone ==1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+textinzone[e].zoneheight then
				if doonce[e] == 0 then
					wait[e] = g_Time + 1000
					doonce[e] = 1
				end
				PerformLogicConnections(e)
				Prompt(textinzone[e].prompttext)
				if g_Time > wait[e] then
					if textinzone[e]['onlyonce'] == 1 then Destroy(e) end
				end
			end
		end
	end
end
