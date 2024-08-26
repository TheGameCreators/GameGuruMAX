-- Affected v2 by Necrym59
-- DESCRIPTION: When activated, player will be affected with the selected effect type
-- DESCRIPTION: Apply to an object and set AlwaysActive
-- DESCRIPTION: [PROMPT_TEXT$="You are affected"]
-- DESCRIPTION: [@EFFECT_TYPE=1(1=Drunk,2=Poisoned)]
-- DESCRIPTION: [EFFECT_TIME=5] Seconds
-- DESCRIPTION: <Sound0> when starting effect.

local affected 				= {}
local prompt_text 			= {}
local effect_time			= {}

local status 				= {}
local effectperiod			= {}
local currentval1			= {}
local currentval2			= {}

function affected_properties(e, prompt_text, effect_type, effect_time)
	affected[e].prompt_text = prompt_text
	affected[e].effect_type = effect_type
	affected[e].effect_time = effect_time
end

function affected_init(e)
	affected[e] = {}
	affected[e].prompt_text = "You are affected"
	affected[e].effect_type = effect_type
	affected[e].effect_time = effect_time
	
	effectperiod[e] = math.huge
	currentval1[e] = 0
	currentval2[e] = 0	
	status[e] = "init"
	SetActivated(e,0)
end

function affected_main(e)

	if status[e] == "init" then
		effectperiod[e] = g_Time + (affected[e].effect_time * 1000)
		status[e] = "waiting"
	end
	
	if status[e] == "waiting" then
		if g_Entity[e]['activated'] == 1 then
			effectperiod[e] = g_Time + (affected[e].effect_time * 1000)
			PlaySound(e,0)
			status[e] = "do_effect"
		end	
	end
	
	if status[e] == "do_effect" then
		if g_Time < effectperiod[e] then				
			if affected[e].effect_type == 1 then --(Drunk)
				GamePlayerControlSetShakeTrauma(65.0)
				GamePlayerControlSetShakePeriod(960.0)
				currentval1[e] = 65.0
				currentval2[e] = 960.0				
			end
			if affected[e].effect_type == 2 then --(Poisoned)
				GamePlayerControlSetShakeTrauma(565.0)
				GamePlayerControlSetShakePeriod(960.00)
				currentval1[e] = 565.0
				currentval2[e] = 960.0				
			end
		end	
		if g_Time > effectperiod[e] then
			if currentval1[e] > 0 then
				GamePlayerControlSetShakeTrauma(currentval1[e])
				GamePlayerControlSetShakePeriod(currentval2[e])
				if affected[e].effect_type == 1 then currentval1[e] = currentval1[e] - 0.1 end
				if affected[e].effect_type == 2 then currentval1[e] = currentval1[e] - 1 end
				if currentval1[e] <= 0 then currentval1[e] = 0 end
			end
			if currentval1[e] == 0 then
				currentval2[e] = 0
				SetActivated(e,0)
				status[e] = "waiting"
			end	
		end
	end
end