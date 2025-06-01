-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- RPG_Player State v12
-- DESCRIPTION: Incrementally adds or deducts the players globals over time.
-- DESCRIPTION: Attach to an object, Set always active On.
-- DESCRIPTION: [@@USER_GLOBAL_AFFECTED1$="MyGlobal1"(0=globallist)] eg: MyGlobal1
-- DESCRIPTION: [AMOUNT1=0] units
-- DESCRIPTION: [TIME_INTERVAL1=0] seconds
-- DESCRIPTION: [@EFFECT1=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [DAMAGE_LEVEL1=0] units above/below to attribute damage
-- DESCRIPTION: [DAMAGE_AMOUNT1=0] units of damage
-- DESCRIPTION: [@@USER_GLOBAL_AFFECTED2$="MyGlobal2"(0=globallist)] eg: MyGlobal2
-- DESCRIPTION: [AMOUNT2=0] units
-- DESCRIPTION: [TIME_INTERVAL2=0] seconds
-- DESCRIPTION: [@EFFECT2=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [DAMAGE_LEVEL2=0] units above/below to attribute damage
-- DESCRIPTION: [DAMAGE_AMOUNT2=0] units of damage
-- DESCRIPTION: [@@USER_GLOBAL_AFFECTED3$="MyGlobal3"(0=globallist)] eg: MyGlobal3
-- DESCRIPTION: [AMOUNT3=0] units
-- DESCRIPTION: [TIME_INTERVAL3=0] seconds
-- DESCRIPTION: [@EFFECT3=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [DAMAGE_LEVEL3=0] units above/below to attribute damage
-- DESCRIPTION: [DAMAGE_AMOUNT3=0] units of damage
-- DESCRIPTION: [@@USER_GLOBAL_AFFECTED4$="MyGlobal4"(0=globallist)] eg: MyGlobal4
-- DESCRIPTION: [AMOUNT4=0] units
-- DESCRIPTION: [TIME_INTERVAL4=0] seconds
-- DESCRIPTION: [@EFFECT4=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [DAMAGE_LEVEL4=0] units above/below to attribute damage
-- DESCRIPTION: [DAMAGE_AMOUNT4=0] units of damage
-- DESCRIPTION: [ActivateAtStart!=1] if unchecked use a switch or zone to activate

local rpg_player_state 		= {}
local user_global_affected1	= {}
local user_global_affected2	= {}
local user_global_affected3	= {}
local user_global_affected4	= {}
local amount1				= {}
local amount2				= {}
local amount3				= {}
local amount4				= {}
local time_interval1		= {}
local time_interval2		= {}
local time_interval3		= {}
local time_interval4		= {}
local effect1				= {}
local effect2				= {}
local effect3				= {}
local effect4				= {}
local damage_level1			= {}
local damage_level2			= {}
local damage_level3			= {}
local damage_level4			= {}
local damage_amount1		= {}
local damage_amount2		= {}
local damage_amount3		= {}
local damage_amount4		= {}
local ActivateAtStart		= {}

local currentvalue1			= {}
local currentvalue2			= {}
local currentvalue3			= {}
local currentvalue4			= {}
local timecheck1			= {}
local timecheck2			= {}
local timecheck3			= {}
local timecheck4			= {}
local initialvalue1			= {}
local initialvalue2			= {}
local initialvalue3			= {}
local initialvalue4			= {}
local status 				= {}
local doonce 				= {}

function rpg_player_state_properties(e, user_global_affected1, amount1, time_interval1, effect1, damage_level1, damage_amount1, user_global_affected2, amount2, time_interval2, effect2, damage_level2, damage_amount2, user_global_affected3, amount3, time_interval3, effect3, damage_level3, damage_amount3, user_global_affected4, amount4, time_interval4, effect4, damage_level4, damage_amount4, ActivateAtStart)
	rpg_player_state[e].user_global_affected1 = user_global_affected1
	rpg_player_state[e].amount1 = amount1
	rpg_player_state[e].time_interval1 = time_interval1
	rpg_player_state[e].effect1 = effect1
	rpg_player_state[e].damage_level1 = damage_level1
	rpg_player_state[e].damage_amount1 = damage_amount1	
	rpg_player_state[e].user_global_affected2 = user_global_affected2
	rpg_player_state[e].amount2 = amount2
	rpg_player_state[e].time_interval2 = time_interval2
	rpg_player_state[e].effect2 = effect2
	rpg_player_state[e].damage_level2 = damage_level2
	rpg_player_state[e].damage_amount2 = damage_amount2		
	rpg_player_state[e].user_global_affected3 = user_global_affected3
	rpg_player_state[e].amount3 = amount3
	rpg_player_state[e].time_interval3 = time_interval3
	rpg_player_state[e].effect3 = effect3
	rpg_player_state[e].damage_level3 = damage_level3
	rpg_player_state[e].damage_amount3 = damage_amount3		
	rpg_player_state[e].user_global_affected4 = user_global_affected4
	rpg_player_state[e].amount4 = amount4
	rpg_player_state[e].time_interval4 = time_interval4
	rpg_player_state[e].effect4 = effect4
	rpg_player_state[e].damage_level4 = damage_level4
	rpg_player_state[e].damage_amount4 = damage_amount4		
	rpg_player_state[e].ActivateAtStart = ActivateAtStart
end

function rpg_player_state_init(e)
	rpg_player_state[e] = {}
	rpg_player_state[e].user_global_affected1 = ""
	rpg_player_state[e].amount1 = 0
	rpg_player_state[e].time_interval1 = 0
	rpg_player_state[e].effect1 = 1
	rpg_player_state[e].damage_level1 = 0
	rpg_player_state[e].damage_amount1 = 0
	rpg_player_state[e].user_global_affected2 = ""
	rpg_player_state[e].amount2 = 0
	rpg_player_state[e].time_interval2 = 0
	rpg_player_state[e].effect2 = 1
	rpg_player_state[e].damage_level2 = 0
	rpg_player_state[e].damage_amount2 = 0	
	rpg_player_state[e].user_global_affected3 = ""
	rpg_player_state[e].amount3 = 0
	rpg_player_state[e].time_interval3 = 0
	rpg_player_state[e].effect3 = 1
	rpg_player_state[e].damage_level3 = 0
	rpg_player_state[e].damage_amount3 = 0	
	rpg_player_state[e].user_global_affected4 = ""
	rpg_player_state[e].amount4 = 0
	rpg_player_state[e].time_interval4 = 0
	rpg_player_state[e].effect4 = 1	
	rpg_player_state[e].damage_level4 = 0
	rpg_player_state[e].damage_amount4 = 0	
	rpg_player_state[e].ActivateAtStart = 1
	status[e] = "init"
	doonce[e] = 0
	currentvalue1[e] = 0
	currentvalue2[e] = 0
	currentvalue3[e] = 0
	currentvalue4[e] = 0
	initialvalue1[e] = 0
	initialvalue2[e] = 0	
	initialvalue3[e] = 0	
	initialvalue4[e] = 0	
	timecheck1[e] = 0
	timecheck2[e] = 0
	timecheck3[e] = 0
	timecheck4[e] = 0
end

function rpg_player_state_main(e)

	if status[e] == "init" then
		if rpg_player_state[e].ActivateAtStart == 1 then SetActivated(e,1) end
		if rpg_player_state[e].ActivateAtStart == 0 then SetActivated(e,0) end		
		status[e] = "endinit"
	end	

	if g_Entity[e]['activated'] == 1 then
		if doonce[e] == 0 then
			timecheck1[e] = g_Time + (rpg_player_state[e].time_interval1)*1000
			timecheck2[e] = g_Time + (rpg_player_state[e].time_interval2)*1000
			timecheck3[e] = g_Time + (rpg_player_state[e].time_interval3)*1000
			timecheck4[e] = g_Time + (rpg_player_state[e].time_interval4)*1000
			if _G["g_UserGlobal['"..rpg_player_state[e].user_global_affected1.."']"] ~= nil then initialvalue1[e] = _G["g_UserGlobal['"..rpg_player_state[e].user_global_affected1.."']"] end
			if _G["g_UserGlobal['"..rpg_player_state[e].user_global_affected2.."']"] ~= nil then initialvalue2[e] = _G["g_UserGlobal['"..rpg_player_state[e].user_global_affected2.."']"] end
			if _G["g_UserGlobal['"..rpg_player_state[e].user_global_affected3.."']"] ~= nil then initialvalue3[e] = _G["g_UserGlobal['"..rpg_player_state[e].user_global_affected3.."']"] end
			if _G["g_UserGlobal['"..rpg_player_state[e].user_global_affected4.."']"] ~= nil then initialvalue4[e] = _G["g_UserGlobal['"..rpg_player_state[e].user_global_affected4.."']"] end
			doonce[e] = 1
		end	
		if g_Time > timecheck1[e] and rpg_player_state[e].time_interval1 > 0 then		
			if _G["g_UserGlobal['"..rpg_player_state[e].user_global_affected1.."']"] ~= nil then currentvalue1[e] = _G["g_UserGlobal['"..rpg_player_state[e].user_global_affected1.."']"] end
			if rpg_player_state[e].effect1 == 1 then				
				_G["g_UserGlobal['"..rpg_player_state[e].user_global_affected1.."']"] = currentvalue1[e] + rpg_player_state[e].amount1
				if currentvalue1[e] >= 100 then _G["g_UserGlobal['"..rpg_player_state[e].user_global_affected1.."']"] = 100 end
				if currentvalue1[e] > rpg_player_state[e].damage_level1 then HurtPlayer(e,rpg_player_state[e].damage_amount1) end
				
			end
			if rpg_player_state[e].effect1 == 2 then 
				_G["g_UserGlobal['"..rpg_player_state[e].user_global_affected1.."']"] = currentvalue1[e] - rpg_player_state[e].amount1
				if currentvalue1[e] <= 0 then _G["g_UserGlobal['"..rpg_player_state[e].user_global_affected1.."']"] = 0 end
				if currentvalue1[e] < rpg_player_state[e].damage_level1 then HurtPlayer(e,rpg_player_state[e].damage_amount1) end
			end
			timecheck1[e] = g_Time + (rpg_player_state[e].time_interval1)*1000
		end
		if g_Time > timecheck2[e] and rpg_player_state[e].time_interval2 > 0 then
			if _G["g_UserGlobal['"..rpg_player_state[e].user_global_affected2.."']"] ~= nil then currentvalue2[e] = _G["g_UserGlobal['"..rpg_player_state[e].user_global_affected2.."']"] end
			if rpg_player_state[e].effect2 == 1 then
				_G["g_UserGlobal['"..rpg_player_state[e].user_global_affected2.."']"] = currentvalue2[e] + rpg_player_state[e].amount2
				if currentvalue2[e] >= 100 then _G["g_UserGlobal['"..rpg_player_state[e].user_global_affected2.."']"] = 100 end
				if currentvalue2[e] > rpg_player_state[e].damage_level2 then HurtPlayer(e,rpg_player_state[e].damage_amount2) end
			end
			if rpg_player_state[e].effect2 == 2 then
				_G["g_UserGlobal['"..rpg_player_state[e].user_global_affected2.."']"] = currentvalue2[e] - rpg_player_state[e].amount2
				if currentvalue2[e] <= 0 then _G["g_UserGlobal['"..rpg_player_state[e].user_global_affected2.."']"] = 0 end
				if currentvalue2[e] < rpg_player_state[e].damage_level2 then HurtPlayer(e,rpg_player_state[e].damage_amount2) end
			end
			timecheck2[e] = g_Time + (rpg_player_state[e].time_interval2)*1000
		end
		if g_Time > timecheck3[e] and rpg_player_state[e].time_interval3 > 0 then
			if _G["g_UserGlobal['"..rpg_player_state[e].user_global_affected3.."']"] ~= nil then currentvalue3[e] = _G["g_UserGlobal['"..rpg_player_state[e].user_global_affected3.."']"] end
			if rpg_player_state[e].effect3 == 1 then
				_G["g_UserGlobal['"..rpg_player_state[e].user_global_affected3.."']"] = currentvalue3[e] + rpg_player_state[e].amount3
				if currentvalue3[e] >= 100 then _G["g_UserGlobal['"..rpg_player_state[e].user_global_affected3.."']"] = 100 end
				if currentvalue3[e] > rpg_player_state[e].damage_level3 then HurtPlayer(e,rpg_player_state[e].damage_amount3) end
			end
			if rpg_player_state[e].effect3 == 2 then
				_G["g_UserGlobal['"..rpg_player_state[e].user_global_affected3.."']"] = currentvalue3[e] - rpg_player_state[e].amount3
				if currentvalue3[e] <= 0 then _G["g_UserGlobal['"..rpg_player_state[e].user_global_affected3.."']"] = 0 end
				if currentvalue3[e] < rpg_player_state[e].damage_level3 then HurtPlayer(e,rpg_player_state[e].damage_amount3) end
			end
			timecheck3[e] = g_Time + (rpg_player_state[e].time_interval3)*1000
		end
		if g_Time > timecheck4[e] and rpg_player_state[e].time_interval4 > 0 then
			if _G["g_UserGlobal['"..rpg_player_state[e].user_global_affected4.."']"] ~= nil then currentvalue4[e] = _G["g_UserGlobal['"..rpg_player_state[e].user_global_affected4.."']"] end
			if rpg_player_state[e].effect4 == 1 then
				_G["g_UserGlobal['"..rpg_player_state[e].user_global_affected4.."']"] = currentvalue4[e] + rpg_player_state[e].amount4
				if currentvalue4[e] >= 100 then _G["g_UserGlobal['"..rpg_player_state[e].user_global_affected4.."']"] = 100 end
				if currentvalue4[e] > rpg_player_state[e].damage_level4 then HurtPlayer(e,rpg_player_state[e].damage_amount4) end
			end
			if rpg_player_state[e].effect4 == 2 then
				_G["g_UserGlobal['"..rpg_player_state[e].user_global_affected4.."']"] = currentvalue4[e] - rpg_player_state[e].amount4
				if currentvalue4[e] <= 0 then _G["g_UserGlobal['"..rpg_player_state[e].user_global_affected4.."']"] = 0 end
				if currentvalue4[e] < rpg_player_state[e].damage_level4 then HurtPlayer(e,rpg_player_state[e].damage_amount4) end
			end
			timecheck4[e] = g_Time + (rpg_player_state[e].time_interval4)*1000
		end
		--player has expired so reset globals--
		if g_PlayerHealth <= 0 then  
			_G["g_UserGlobal['"..rpg_player_state[e].user_global_affected1.."']"] = initialvalue1[e]
			_G["g_UserGlobal['"..rpg_player_state[e].user_global_affected2.."']"] = initialvalue2[e]
			_G["g_UserGlobal['"..rpg_player_state[e].user_global_affected3.."']"] = initialvalue3[e]
			_G["g_UserGlobal['"..rpg_player_state[e].user_global_affected4.."']"] = initialvalue4[e]			
		end
	end
end