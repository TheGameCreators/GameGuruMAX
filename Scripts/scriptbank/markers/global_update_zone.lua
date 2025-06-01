-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Global Update Zone v3 by Necrym59
-- DESCRIPTION: Will update/replace a user global value when zone entered.
-- DESCRIPTION: [PROMPT_TEXT$="Global Updated"]
-- DESCRIPTION: [@@USER_GLOBAL$=""(0=globallist)] eg; MyLocation or MyValue
-- DESCRIPTION: [@GLOBAL_TYPE=1(1=Text,2=Numeric)] user global variable type
-- DESCRIPTION: [UPDATE_VALUE$=""] value to update user global.
-- DESCRIPTION: [ZoneHeight=100(1,1000)]
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: <Sound0> - When entering zone Sound

local glupdatezone 		= {}
local prompt_text		= {}
local user_global		= {}
local globa1_type		= {}
local update_value		= {}
local zoneheight		= {}
local spawnatstart 		= {}

local status = {}
local played = {}
local doonce = {}

function global_update_zone_properties(e, prompt_text, user_global, global_type, update_value, zoneheight, spawnatstart)
	glupdatezone[e].prompt_text = prompt_text
	glupdatezone[e].user_global = user_global
	glupdatezone[e].global_type = global_type or 1	
	glupdatezone[e].update_value = update_value
	glupdatezone[e].zoneheight = zoneheight or 100
	glupdatezone[e].spawnatstart = spawnatstart or 1
end 

function global_update_zone_init(e)
	glupdatezone[e] = {}
	glupdatezone[e].prompt_text = "Global Updated"
	glupdatezone[e].user_global = ""
	glupdatezone[e].global_type = 1
	glupdatezone[e].update_value = ""
	glupdatezone[e].zoneheight = 100
	glupdatezone[e].spawnatstart = 1	
	status[e] = "init"
	played[e] = 0
	doonce[e] = 0
end

function global_update_zone_main(e)
	if status[e] == "init" then
		if glupdatezone[e].spawnatstart == 1 then SetActivated(e,1) end
		if glupdatezone[e].spawnatstart == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end
	
	if g_Entity[e]['activated'] == 1 then		
		if g_Entity[e]['plrinzone'] == 1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y'] + glupdatezone[e].zoneheight then
			if played[e] == 0 then
				PlaySound(e,0)
				played[e] = 1
			end
			if doonce[e] == 0 then
				Prompt(glupdatezone[e].prompt_text)
				if glupdatezone[e].global_type == 1 then _G["g_UserGlobal['"..glupdatezone[e].user_global.."']"] = glupdatezone[e].update_value end
				if glupdatezone[e].global_type == 2 then _G["g_UserGlobal['"..glupdatezone[e].user_global.."']"] = tonumber(glupdatezone[e].update_value) end
				doonce[e] = 1
			end	
		end
		if g_Entity[e]['plrinzone'] == 0 then
			played[e] = 0
			doonce[e] = 0
		end
	end
end

