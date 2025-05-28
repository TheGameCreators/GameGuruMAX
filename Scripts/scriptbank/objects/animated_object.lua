-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Animated Object v4 by Necrym59 and Blood Moon Interactive
-- DESCRIPTION: Allows to run the inbuilt animation of an object
-- DESCRIPTION: [@RUN_ANIMATION$=-1(0=AnimSetList)]
-- DESCRIPTION: [#ANIMATION_SPEED=1.0(0.0,100.0)]
-- DESCRIPTION: [@@VARIABLE_SWITCH_USER_GLOBAL$=""(0=globallist)] User global name for a variable switch (eg: Variable_Switch1)
-- DESCRIPTION: <Sound0> Animation sound

local animatedobject 	= {}
local run_animation		= {}
local animation_speed	= {}
local variable_switch_user_global	= {}	
	
local status			= {}
local currentvalue		= {}
local doonce			= {}
	
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
	doonce[e] = 0
	currentvalue[e] = 0
end

function animated_object_main(e)

	if status[e] == "init" then
		SetAnimationName(e,animatedobject[e].run_animation)
		ModulateSpeed(e,animatedobject[e].animation_speed)
		LoopAnimation(e)
		-- Only play sound if animation speed is greater than 0
		if animatedobject[e].animation_speed > 0 then
			LoopSound(e,0)
		end
		status[e] = "endinit"
	end
	
	if animatedobject[e].variable_switch_user_global ~= "" then
		if _G["g_UserGlobal['"..animatedobject[e].variable_switch_user_global.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..animatedobject[e].variable_switch_user_global.."']"] end
		animatedobject[e].animation_speed = _G["g_UserGlobal['"..animatedobject[e].variable_switch_user_global.."']"]
		ModulateSpeed(e,animatedobject[e].animation_speed)
	end	

	-- Control sound based on animation --
	if animatedobject[e].animation_speed > 0 and doonce[e] == 0 then
		LoopSound(e,0)
		doonce[e] = 1
	end
	if animatedobject[e].animation_speed == 0 and doonce[e] == 1 then
		StopSound(e,0)
		doonce[e] = 0
	end
end
