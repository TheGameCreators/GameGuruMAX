-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Mines v6  by Necrym 59
-- DESCRIPTION: Allows pickup and deployment of an anti-personnel mine device. Physics=ON, Always Active=ON, IsImobile=ON, Explodable=ON
-- DESCRIPTION: The [WEAPON_NAME$="Anti-Personel Mine"], [PICKUP_RANGE=50(1,200)], [TRIGGER_DISTANCE=80(1,300)] and the [ENEMY_HEAR_DISTANCE=1000(0,3000)]
-- DESCRIPTION: [PICKUP_TEXT$="Press E To pick up mine"] [PLACING_TEXT$="Press R To place mine"] [ARMING_TEXT$="Mine is primed/armed"], [ARMING_DELAY=3(1,5)] seconds, [@HARM_PLAYER=1(1=No, 2=Yes)]
-- DESCRIPTION: <Sound0> Pickup Sound <Sound1> the primed/armed sound.

local U = require "scriptbank\\utillib"
local rad = math.rad

g_mines = {}
local mine = {}
local weapon_name = {}
local pickup_range = {}
local trigger_distance = {}
local enemy_hear_distance = {}
local pickup_text = {}
local placing_text = {}
local arming_text = {}
local harm_player = {}
local mine_state = {}
local mine_armed = {}
local arming_delay = {}
local pressed = {}
local status = {}
local prime_time = {}
local enemy_distance = {}

function mines_properties(e, weapon_name, pickup_range, trigger_distance, enemy_hear_distance, pickup_text, placing_text, arming_text, arming_delay, harm_player)
	mine[e] = g_Entity[e]
	mine[e].weapon_name = weapon_name						--weapon name
	mine[e].pickup_range = pickup_range						--pickup range
	mine[e].trigger_distance = trigger_distance				--distance before the mine is triggered
	mine[e].enemy_hear_distance = enemy_hear_distance		--distance nearby enemy can hear the explosion
	mine[e].pickup_text = pickup_text						--pickup text
	mine[e].placing_text = placing_text						--placing text
	mine[e].arming_text = arming_text						--arming text
	mine[e].arming_delay = arming_delay						--arming delay
	mine[e].harm_player = harm_player						--player can be harmed
end	
	
function mines_init(e)
	mine[e] = g_Entity[e]
	mine[e].weapon_name = "Anti-Personel Mine"
	mine[e].pickup_range = 50
	mine[e].trigger_distance = 80
	mine[e].enemy_hear_distance = 0
	mine[e].pickup_text = "Press E To pick up mine"
	mine[e].placing_text = "Press R To place mine"
	mine[e].arming_text = "Mine is primed/armed"
	mine[e].arming_delay = 3
	mine[e].harm_player = 1
	pressed[e] = 0
	mine_state[e] = "collect"	
	mine_armed[e] = 0
	status[e] = "init"
	g_mines = 0
	prime_time[e] = 0
end

function mines_main(e)
	mine[e] = g_Entity[e]
	if status[e] == "init" then
		mine_state[e] = "collect"		
		status[e] = "init_end"
	end
	PlayerDist = GetPlayerDistance(e)	
	
	if mine_state[e] == "collect" and g_mines == 0 then	
		if PlayerDist < mine[e].pickup_range then
			local LookingAt = GetPlrLookingAtEx(e,1)	
			if LookingAt == 1 then
				Prompt(mine[e].pickup_text)
				if GetInKey() == "e" or GetInKey() == "E" and pressed == 0 then				
					PromptDuration("Collected the " ..mine[e].weapon_name,3000)
					pressed[e] = 1
					PlaySound(e,0)
					mine_state[e] = "collected"
					g_mines = 1
					ActivateIfUsed(e)
					Hide(e)
					CollisionOff(e)
				end					
			end		
		end		
	end
	
	if mine_state[e] == "collected" and g_mines == 1 then
		PromptDuration(mine[e].placing_text,3000)		
		if GetInKey() == "r" or GetInKey() == "R" and pressed == 0 then	
			local grheighty = GetGroundHeight(g_PlayerPosX,g_PlayerPosZ)
			SetPosition(e,g_PlayerPosX,grheighty-1,g_PlayerPosZ)
			ResetPosition(e,g_PlayerPosX,grheighty-1,g_PlayerPosZ)
			SetRotation(e,g_Entity[e]['anglex'] ,g_PlayerAngY,g_Entity[e]['anglez'])
			CollisionOn(e)
			mine_armed[e] = 0
			mine_state[e] = "placed"
			g_mines = 0
			Show(e)
			pressed[e] = 1		
			ForcePlayer(g_PlayerAngY,-8)			
		end
	end
	if mine_state[e] == "placed" then			
		PromptDuration(mine[e].arming_text,3000)
		if prime_time[e] == 0 then 
			prime_time[e] = GetTimer(e) + (mine[e].arming_delay*1000)
		end
		if GetTimer(e) > prime_time[e] then		
			mine_armed[e] = 1
			PlaySound(e,1)
			SetAnimationName(e,"on")
			PlayAnimation(e)
			mine_state[e] = "primed"
			prime_time[e] = 0
		end
	end
	if mine_state[e] == "primed" then
		if mine_armed[e] == 1 then 		
			for a = 1, g_EntityElementMax do
				if g_Entity[a] ~= nil and g_Entity[a]['health'] > 1 then					
					local mineDX = g_Entity[a]['x'] - g_Entity[e]['x']
					local mineDY = g_Entity[a]['y'] - g_Entity[e]['y']
					local mineDZ = g_Entity[a]['z'] - g_Entity[e]['z']
					enemy_distance[e] = math.sqrt(math.abs(mineDX*mineDX)+math.abs(mineDY*mineDY)+math.abs(mineDZ*mineDZ))
					if enemy_distance[e] < mine[e].trigger_distance and mine_armed[e] == 1 then												
						SetEntityHealth(e,0)						
						MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,mine[e].enemy_hear_distance,1,e) -- other nearby enemies hear explosion
						Destroy(e)						
					end
				end
			end
			if mine[e].harm_player == 2 then 
				PlayerDist = GetPlayerDistance(e)
				if PlayerDist < mine[e].trigger_distance then
					SetEntityHealth(e,0)
				end
			end
		end
	end	
end
