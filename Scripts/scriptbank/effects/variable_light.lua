-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Variable Light v2
-- DESCRIPTION: A light for use with a variable switch. Attach to a light.
-- DESCRIPTION: Enter the name of the [VARIABLE_SWITCH_USER_GLOBAL$="Variable_Switch1"]

module_lightcontrol = require "scriptbank\\markers\\module_lightcontrol"
local rad = math.rad

local varlight 						= {}
local variable_switch_user_global	= {}

local status 						= {}
local current_level					= {}
local currentvalue					= {}
local lightNum = GetEntityLightNumber(e)

function variable_light_properties(e,variable_switch_user_global)	
	module_lightcontrol.init(e,1)
	varlight[e].variable_switch_user_global = variable_switch_user_global
end

function variable_light_init(e)
	varlight[e] = {}
	varlight[e].variable_switch_user_global = ""
	
	lightNum = GetEntityLightNumber(e)
	SetActivated(e,1)
	currentvalue[e] = 0
	status[e] = "init"
end
	
function variable_light_main(e)	

	if status[e] == "init" then
		status[e] = "endinit"
	end
	
	current_level[e] = g_vswitchvalue
	
	if g_Entity[e]['activated'] == 1 then		
		lightNum = GetEntityLightNumber(e)
		if varlight[e].variable_switch_user_global ~= "" then
			if _G["g_UserGlobal['"..varlight[e].variable_switch_user_global.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..varlight[e].variable_switch_user_global.."']"] end
			current_level[e] = _G["g_UserGlobal['"..varlight[e].variable_switch_user_global.."']"]
		end
		SetLightRange(lightNum,current_level[e])
	end
end