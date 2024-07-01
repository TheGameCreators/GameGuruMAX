-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Secret v3 by Necrym59
-- DESCRIPTION: When activated an will inform the player they have found a secret, and can increment a user global by a set amount and can activate other connected logic and/or Activate IfUsed links.
-- DESCRIPTION: Attach to an object and logic link from a switch or zone to activate.
-- DESCRIPTION: [PROMPT_TEXT$="You have found a secret"]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$=""] (eg: MySecretCounter)
-- DESCRIPTION: [USER_GLOBAL_VALUE=0(0,100)] value to apply.
-- DESCRIPTION: <Sound0> - Secret found Sound

local secret 				= {}
local prompt_text 			= {}
local user_global_affected	= {}
local user_global_value		= {}

local currentvalue		= {}
local doonce			= {}
local status			= {}

	
function secret_properties(e, prompt_text, user_global_affected, user_global_value)
	secret[e].prompt_text = prompt_text
	secret[e].user_global_affected = user_global_affected
	secret[e].user_global_value = user_global_value	
end
 
function secret_init(e)
	secret[e] = {}
	secret[e].prompt_text = "You have found a secret"
	secret[e].user_global_affected = ""
	secret[e].user_global_value = 0
	
	doonce[e] = 0
	currentvalue[e] = 0	
	status[e] = "init"
end
 
function secret_main(e)	
	
	if status[e] == "init" then
		status[e] = "endinit"
	end
	
	if g_Entity[e]['activated'] == 1 then		
		if doonce[e] == 0 then
			PromptDuration(secret[e].prompt_text,2000)
			PlaySound(e,0)
			ActivateIfUsed(e)
			PerformLogicConnections(e)
			if secret[e].user_global_affected > "" then
				if _G["g_UserGlobal['"..secret[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..secret[e].user_global_affected.."']"] end
				_G["g_UserGlobal['"..secret[e].user_global_affected.."']"] = currentvalue[e] + secret[e].user_global_value
			end
			doonce[e] = 1
			g_Entity[e]['activated'] = 0
			SwitchScript(e,"no_behavior_selected.lua")
		end			
	end	
end
 
function secret_exit(e)	
end