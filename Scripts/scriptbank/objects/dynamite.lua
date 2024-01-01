-- Dynamite v10 - By Necrym59
-- DESCRIPTION: Allows for pick and deployment of an exposive bomb device.
-- DESCRIPTION: [WEAPON_NAME$="Dynamite"]
-- DESCRIPTION: [PLACEMENT_TIME=1(1,3)]
-- DESCRIPTION: [EXPLOSION_DELAY=1(1,50)]
-- DESCRIPTION: [@BOMB_TYPE=1(1=Manual, 2=Timer, 3=Remote, 4=Switch)]
-- DESCRIPTION: [PLAYER_SAFE_DISTANCE=220(1,1000)]
-- DESCRIPTION: [ENEMY_HEAR_DISTANCE=1000(0,3000)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [PICKUP_TEXT$="E to collect"]
-- DESCRIPTION: <Sound0> Pickup Sound
-- DESCRIPTION: <Sound1> the primed/armed sound

local U = require "scriptbank\\utillib"
local rad = math.rad

g_dynamite = {}
local dynamite = {}
local weapon_name = {}
local placement_time = {}
local explosion_delay = {}
local bomb_type = {}
local player_safe_distance = {}
local enemy_hear_distance = {}
local bombs = {}
local bomb_state = {}
local timer_running = {}
local pressed = {}
local prime_time = {}
local armed_time = {}
local dynamite_armed = {}
local status = {}
local detonation_time = {}
local pickup_range = {}
local pickup_text = {}
local currentY = {}
local colobj = {}

function dynamite_properties(e, weapon_name, placement_time, explosion_delay, bomb_type, player_safe_distance, enemy_hear_distance, pickup_range, pickup_text)
	g_dynamite[e] = g_Entity[e]
	g_dynamite[e]['weapon_name'] = weapon_name						--Name of weapon
	g_dynamite[e]['placement_time'] = placement_time				--How long you need to hold key for before the bomb is placed (in seconds)
	g_dynamite[e]['explosion_delay'] = explosion_delay				--How long before the bomb explodes once triggered (in seconds)
	g_dynamite[e]['bomb_type'] = bomb_type							--The explosive bomb type
	g_dynamite[e]['player_safe_distance'] = player_safe_distance	--Minimum safe distance player needs to be before the explosive can be triggered
	g_dynamite[e]['enemy_hear_distance'] = enemy_hear_distance		--Distance nearby enemy can hear the explosion
	g_dynamite[e]['pickup_range'] = pickup_range					--Pickup range
	g_dynamite[e]['pickup_text'] = pickup_text						--Pickup prompt text
end

function dynamite_init(e)
	g_dynamite[e] = {}
	g_dynamite[e]['weapon_name'] = "Dynamite"
	g_dynamite[e]['placement_time'] = 1
	g_dynamite[e]['explosion_delay'] = 1
	g_dynamite[e]['bomb_type'] = 0
	g_dynamite[e]['player_safe_distance'] = 500
	g_dynamite[e]['enemy_hear_distance'] = 0
	g_dynamite[e]['pickup_range'] = 80
	g_dynamite[e]['pickup_text'] = "E to collect"
	bombs[e] = 0
	pressed[e] = 0
	bomb_state[e] = "collect"
	prime_time[e] = 0
	armed_time[e] = 0
	dynamite_armed[e] = 0
	detonation_time[e] = 0
	status[e] = "init"
	colobj[e] = 0
	SetEntityBaseAlpha(e,100)
	SetEntityTransparency(e,1)
end

function dynamite_main(e)
	g_dynamite[e] = g_Entity[e]
	if status[e] == "init" then
		if g_dynamite[e]['bomb_type'] == nil then g_dynamite[e]['bomb_type'] = 1 end
		bomb_state[e] = "collect"
		SetActivated(e,1)
		currentY[e] = g_Entity[e]['y']
		status[e] = "init_end"
	end
	local PlayerDist = GetPlayerDistance(e)

	if g_Entity[e]['activated'] == 1 and dynamite_armed[e] == 0 then
		if bomb_state[e] == "collect" and bombs[e] == 0 then
			if PlayerDist < g_dynamite[e]['pickup_range'] and g_PlayerHealth > 0 then
				local LookingAt = GetPlrLookingAtEx(e,1)
				if LookingAt == 1 then
					PromptLocal(e,g_dynamite[e]['pickup_text'])
					if g_KeyPressE == 1 and pressed[e] == 0 then
						pressed[e] = 1
						PlaySound(e,0)
						bombs[e] = bombs[e] + 1
						bomb_state[e] = "collected"
						currentY[e] = currentY[e]
						ActivateIfUsed(e)
						Hide(e)
						CollisionOff(e)
					end
				end
			end
		end

		if bomb_state[e] == "collected" then
			GravityOff(e)			
			local ox,oy,oz = U.Rotate3D(0,currentY[e],g_dynamite[e]['pickup_range'], math.rad(g_PlayerAngX), math.rad(g_PlayerAngY), math.rad(g_PlayerAngZ))
			local forwardposx, forwardposy, forwardposz = g_PlayerPosX + ox, g_PlayerPosY + oy, g_PlayerPosZ + oz
			SetPosition(e,forwardposx, forwardposy, forwardposz)
			ResetPosition(e,forwardposx, forwardposy, forwardposz)
			RotateObject(g_Entity[e]['obj'],0,g_Entity[e]['angley'],g_PlayerAngZ)
			SetEntityBaseAlpha(e,30)
			Show(e)
			PromptLocal(e,"Press P to place and prime the " ..g_dynamite[e]['weapon_name'].. " or Q to Drop")
			if GetInKey() == "p" or GetInKey() == "P" and pressed[e] == 0 then
				SetEntityBaseAlpha(e,100)
				PromptLocal(e,"")
				local ox,oy,oz = U.Rotate3D(0,currentY[e],g_dynamite[e]['pickup_range'], math.rad(g_PlayerAngX), math.rad(g_PlayerAngY), math.rad(g_PlayerAngZ))
				local forwardposx, forwardposy, forwardposz = g_PlayerPosX + ox, g_PlayerPosY + oy, g_PlayerPosZ + oz
				SetPosition(e,forwardposx, forwardposy, forwardposz)
				ResetPosition(e,forwardposx, forwardposy, forwardposz)
				bombs[e] = bombs[e] - 1
				dynamite_armed[e] = 0
				bomb_state[e] = "placed"
				GravityOn(e)
				CollisionOn(e)				
				Show(e)
				pressed[e] = 1
			end
			if GetInKey() == "q" or GetInKey() == "Q" and pressed[e] == 0 then
				SetEntityBaseAlpha(e,100)
				PromptLocal(e,"")
				local ox,oy,oz = U.Rotate3D(0,currentY[e],g_dynamite[e]['pickup_range'], math.rad(g_PlayerAngX), math.rad(g_PlayerAngY), math.rad(g_PlayerAngZ))
				local forwardposx, forwardposy, forwardposz = g_PlayerPosX + ox, g_PlayerPosY + oy, g_PlayerPosZ + oz
				SetPosition(e,forwardposx, forwardposy, forwardposz)
				ResetPosition(e,forwardposx, forwardposy, forwardposz)
				bombs[e] = bombs[e] - 1
				dynamite_armed[e] = 0
				bomb_state[e] = "collect"
				GravityOn(e)
				CollisionOn(e)
				Show(e)
				pressed[e] = 0
			end
		end
		if bomb_state[e] == "placed" then
			if prime_time[e] == 0 then
				PromptDuration(g_dynamite[e]['weapon_name'] .. " placed", 1000)
				prime_time[e] = GetTimer(e) + (g_dynamite[e]['placement_time'] * 1000)
				armed_time[e] = GetTimer(e) + (g_dynamite[e]['explosion_delay'] * 1000)
				dynamite_armed[e] = 0
			end
			if GetTimer(e) > prime_time[e] -500 then PromptDuration(g_dynamite[e]['weapon_name'] .. " primed", 1000) end
			if GetTimer(e) > prime_time[e] then
				bomb_state[e] = "primed"
				PlaySound(e,1)
			end
		end
		if bomb_state[e] == "primed" then
			if dynamite_armed[e] == 0 then
				if g_dynamite[e]['bomb_type'] == 1 then  --(Manual Detonation)
					armed_time[e] = GetTimer(e)+ (5000 * 10)
					dynamite_armed[e] = 1
					SetEntityHealth(e,0)
					MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,g_dynamite[e]['enemy_hear_distance'],1,e)
				end
				if g_dynamite[e]['bomb_type'] == 2 then  --(Timed Detonation)
					detonation_time[e] = math.floor(((armed_time[e] - GetTimer(e))/10)/60)
					Prompt(g_dynamite[e]['weapon_name'].. " will detonate in: " ..detonation_time[e]+1)
					if GetTimer(e) >= armed_time[e] then
						timer_running[e] = 1
						dynamite_armed[e] = 1
						SetEntityHealth(e,0)
						MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,g_dynamite[e]['enemy_hear_distance'],1,e)
					end
				end
				if g_dynamite[e]['bomb_type'] == 3 and GetPlayerDistance(e) > g_dynamite[e]['player_safe_distance'] then  --(Remote Detonation)
					PromptDuration("Press X To detonate the " ..g_dynamite[e]['weapon_name'],2000)
					if GetInKey() == "x" or GetInKey() == "X" then
						dynamite_armed[e] = 1
						SetEntityHealth(e,0)
						MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,g_dynamite[e]['enemy_hear_distance'],1,e)
					end
				end
				if g_dynamite[e]['bomb_type'] == 4 then  --(switched Detonation)
					PromptDuration("Use a switch to detonate the " ..g_dynamite[e]['weapon_name'],2000)
					dynamite_armed[e] = 1
					SetActivated(e,0)
				end				
			end
		end
	end
	
	if g_Entity[e]['activated'] == 1 and dynamite_armed[e] == 1 then
		SetEntityHealth(e,0)
		MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,g_dynamite[e]['enemy_hear_distance'],1,e)
	end

	if g_KeyPressE == 0 then
		timer_running[e] = 0
		pressed[e] = 0
	end
end