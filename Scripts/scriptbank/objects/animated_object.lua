-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Animated Object v1 by Necrym59
-- DESCRIPTION: Allows to run the inbuilt animation of an object
-- DESCRIPTION: [@RUN_ANIMATION$=-1(0=AnimSetList)]
-- DESCRIPTION: [#ANIMATION_SPEED=1.0(0.0,100.0)]
-- DESCRIPTION: [VARIABLE_SWITCH_USER_GLOBAL$=""] User global name for a variable switch (eg: Variable_Switch1)
-- DESCRIPTION: <Sound0> Animation sound

local animatedobject 	= {}
local run_animation		= {}
local animation_speed	= {}
local variable_switch_user_global	= {}	
	
local status			= {}
local currentvalue		= {}
	
function animated_object_properties(e, run_animation, animation_speed, variable_switch_user_global)
	animatedobject[e].run_animation = "=" .. tostring(run_animation)
	animatedobject[e].animation_speed = animation_speed
	animatedobject[e].variable_switch_user_global = variable_switch_user_global
end

function animated_object_init(e)
	animatedobject[e] = {}
	animatedobject[e].run_animation = ""
	animatedobject[e].animation_speed = 1
	animatedobject[e].variable_switch_user_global = ""
	
	status[e] = "init"
	currentvalue[e] = 0
end

function animated_object_main(e)

	if status[e] == "init" then
		SetAnimationName(e,animatedobject[e].run_animation)
		ModulateSpeed(e,animatedobject[e].animation_speed)
		LoopAnimation(e)
		LoopSound(e,0)
		status[e] = "endinit"
	end
	
	if animatedobject[e].variable_switch_user_global ~= "" then
		if _G["g_UserGlobal['"..animatedobject[e].variable_switch_user_global.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..animatedobject[e].variable_switch_user_global.."']"] end
		animatedobject[e].animation_speed = _G["g_UserGlobal['"..animatedobject[e].variable_switch_user_global.."']"]
		ModulateSpeed(e,animatedobject[e].animation_speed)
	end	
end
