-- Affected v4 by Necrym59
-- DESCRIPTION: When activated, player will be affected with the selected effect type
-- DESCRIPTION: Apply to an object and set AlwaysActive
-- DESCRIPTION: [PROMPT_TEXT$="You are affected"]
-- DESCRIPTION: [@EFFECT_TYPE=1(1=Drunk,2=Poisoned,3=Shudder)]
-- DESCRIPTION: [EFFECT_TIME=5] Seconds
-- DESCRIPTION: [@@USER_GLOBAL_AFFECTED$=""(0=globallist)] eg: MyGlobal
-- DESCRIPTION: [@GLOBAL_AFFECT=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [AFFECT_AMOUNT=0(1,100)]
-- DESCRIPTION: [AFFECT_HEALTH!=0] if ON will affect players health by Affect Amount
-- DESCRIPTION: [ACTIVATE_LOGIC!=0] if ON will activate logic links or IfUsed
-- DESCRIPTION: <Sound0> when starting effect.

local affected 				= {}
local prompt_text 			= {}
local effect_type			= {}
local effect_time			= {}
local user_global_affected 	= {}
local global_affect 		= {}
local affect_amount 		= {}
local affect_health 		= {}
local activate_logic 		= {}

local status 				= {}
local effectperiod			= {}
local currentval1			= {}
local currentval2			= {}
local currentvalue			= {}
local calchealth			= {}
local doonce				= {}
local effectdone			= {}

function affected_properties(e, prompt_text, effect_type, effect_time, user_global_affected, global_affect, affect_amount, affect_health, activate_logic)
	affected[e].prompt_text = prompt_text
	affected[e].effect_type = effect_type
	affected[e].effect_time = effect_time
	affected[e].user_global_affected = user_global_affected
	affected[e].global_affect = global_affect	
	affected[e].affect_amount = affect_amount
	affected[e].affect_health = affect_health or 0	
	affected[e].activate_logic = activate_logic or 0
end

function affected_init(e)
	affected[e] = {}
	affected[e].prompt_text = "You are affected"
	affected[e].effect_type = effect_type
	affected[e].effect_time = effect_time
	affected[e].user_global_affected = ""
	affected[e].global_affect = 1
	affected[e].affect_amount = 0
	affected[e].affect_health = 0
	affected[e].activate_logic = 0
	
	effectperiod[e] = math.huge
	effectdone[e] = 0
	currentvalue[e] = 0
	currentval1[e] = 0
	currentval2[e] = 0
	doonce[e] = 0
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
			calchealth[e] = g_PlayerHealth
			effectperiod[e] = g_Time + (affected[e].effect_time * 1000)
			PlaySound(e,0)
			effectdone[e] = 0
			status[e] = "do_effect"
		end	
	end
	if status[e] == "do_effect" then
		if doonce[e] == 0 then	
			if affected[e].activate_logic == 1 then					
				PerformLogicConnections(e)
				ActivateIfUsed(e)
				doonce[e] = 1
			end
		end	
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
			if affected[e].effect_type == 3 then --(Shudder)
				GamePlayerControlSetShakeTrauma(20.0)
				GamePlayerControlSetShakePeriod(60.00)
				currentval1[e] = 20.0
				currentval2[e] = 60.0				
			end
			if affected[e].user_global_affected > "" then 		
				if _G["g_UserGlobal['"..affected[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..affected[e].user_global_affected.."']"] end
				if affected[e].global_affect == 1 then
					_G["g_UserGlobal['"..affected[e].user_global_affected.."']"] = currentvalue[e] + affected[e].affect_amount
					if _G["g_UserGlobal['"..affected[e].user_global_affected.."']"] >= 100 then _G["g_UserGlobal['"..affected[e].user_global_affected.."']"] = 100 end
				end		
				if affected[e].global_affect == 2 then
					_G["g_UserGlobal['"..affected[e].user_global_affected.."']"] = currentvalue[e] - affected[e].affect_amount
					if _G["g_UserGlobal['"..affected[e].user_global_affected.."']"] <= 0 then _G["g_UserGlobal['"..affected[e].user_global_affected.."']"] = 0 end
				end
			end
			
			if affected[e].global_affect == 1 then
				if affected[e].affect_health == 1 then
					calchealth[e] = calchealth[e] + (affected[e].affect_amount/1000)
					if calchealth[e] > g_gameloop_StartHealth then calchealth[e] = g_gameloop_StartHealth end
					SetPlayerHealth(calchealth[e])
				end
			end
			if affected[e].global_affect == 2 then
				if affected[e].affect_health == 1 then
					calchealth[e] = calchealth[e] - (affected[e].affect_amount/1000)
					if calchealth[e] > g_gameloop_StartHealth then calchealth[e] = g_gameloop_StartHealth end
					SetPlayerHealth(calchealth[e])
				end
			end	
			
		end
		if g_Time > effectperiod[e] and effectdone[e] == 0 then
			if currentval1[e] > 0 then
				GamePlayerControlSetShakeTrauma(currentval1[e])
				GamePlayerControlSetShakePeriod(currentval2[e])
				if affected[e].effect_type == 1 then currentval1[e] = currentval1[e] - 0.1 end
				if affected[e].effect_type == 2 then currentval1[e] = currentval1[e] - 1 end
				if affected[e].effect_type == 3 then currentval1[e] = currentval1[e] - 1 end
				if currentval1[e] <= 0 then currentval1[e] = 0 end
			end
			if doonce[e] == 1 then	
				if affected[e].activate_logic == 1 then					
					PerformLogicConnections(e)
					ActivateIfUsed(e)
					doonce[e] = 2
				end
			end	
			if currentval1[e] == 0 then
				currentval2[e] = 0
				SetActivated(e,0)
				effectdone[e] = 1
				doonce[e] = 0
				status[e] = "waiting"
			end	
		end		
	end	
end