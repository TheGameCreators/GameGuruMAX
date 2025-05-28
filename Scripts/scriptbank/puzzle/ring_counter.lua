-- Ring Counter v7 - by Necrym59
-- DESCRIPTION: Allows for a target rings to move through.
-- DESCRIPTION: [RING_RANGE=80(0,500)]
-- DESCRIPTION: [@RING_TYPE=1(1=Static Color, 2=Alternate Color)]
-- DESCRIPTION: [ALTERNATING_TIME=5(1,30))]Seconds
-- DESCRIPTION: [@TRIGGER_TYPE=1(1=None, 2=Lose Game, 3=Win Game, 4=Linked/IfUsed)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$=""] eg:(MyPoints)
-- DESCRIPTION: [AFFECT_AMOUNT=1(1,10)] Amount to add/Deduct
-- DESCRIPTION: [@PULSE_EFFECT=2(1=On, 2=Off)]
-- DESCRIPTION: [PULSE_STRENGTH=1000(1,2000)]
-- DESCRIPTION: [PULSE_SPEED=20(1,100)]
-- DESCRIPTION: [@SPIN_EFFECT=2(1=On, 2=Off)]
-- DESCRIPTION: [SPINSPEED=5(1,500)]
-- DESCRIPTION: [SPIN_X!=0]
-- DESCRIPTION: [SPIN_Y!=0]
-- DESCRIPTION: [SPIN_Z!=1]
-- DESCRIPTION: [@SPIN_DIRECTION=1(1=Clockwise, 2=Anti-Clockwise)]
-- DESCRIPTION: [@AUGMENTS=1(1=None, 2=Health, 3=Health+Repair)]
-- DESCRIPTION: <Sound0> for ring success
-- DESCRIPTION: <Sound1> for ring fail

local U = require "scriptbank\\utillib"

local ring 					= {}
local ring_range 			= {}
local ring_type 			= {}
local alternating_time		= {}
local trigger_type			= {}
local user_global_affected	= {}
local affect_amount 		= {}
local pulse_effect			= {}
local pulse_strength		= {}
local pulse_speed			= {}
local spin_effect			= {}
local spin_speed			= {}
local spin_x				= {}
local spin_y				= {}
local spin_z				= {}
local spin_direction		= {}
local augments				= {}

local alttimer = {}
local state	= {}
local currentvalue = {}
local current_level = {}
local emlevel = {}
local empulse = {}
local emr = {}
local emg = {}
local emb = {}
local emst = {}
local rspinspeed = {}
local closestent = {}
local played = {}
local doonce = {}
local repair = {}
local status = {}

function ring_counter_properties(e, ring_range, ring_type, alternating_time, trigger_type, user_global_affected, affect_amount, pulse_effect, pulse_strength, pulse_speed, spin_effect, spin_speed, spin_x, spin_y, spin_z, spin_direction, augments)
	ring[e].ring_range = ring_range
	ring[e].ring_type = ring_type
	ring[e].alternating_time = alternating_time
	ring[e].trigger_type = trigger_type
	ring[e].user_global_affected = user_global_affected
	ring[e].affect_amount = affect_amount
	ring[e].pulse_effect = pulse_effect
	ring[e].pulse_strength = pulse_strength
	ring[e].pulse_speed = pulse_speed
	ring[e].spin_effect	= spin_effect
	ring[e].spin_speed = spin_speed
	ring[e].spin_x = spin_x
	ring[e].spin_y = spin_y
	ring[e].spin_z = spin_z
	ring[e].spin_direction = spin_direction
	ring[e].augments = augments
end

function ring_counter_init(e)
	ring[e] = {}
	ring[e].ring_range = 100
	ring[e].ring_type = 1
	ring[e].alternating_time = 0
	ring[e].trigger_type = 1
	ring[e].user_global_affected = ""
	ring[e].affect_amount = 5
	ring[e].pulse_effect = 0
	ring[e].pulse_strength = 1000
	ring[e].pulse_speed = 20
	ring[e].spin_effect	= 0
	ring[e].spin_speed = 0
	ring[e].spin_x = 0
	ring[e].spin_y = 0
	ring[e].spin_z = 0
	ring[e].spin_direction = 1	
	ring[e].augments = 1

	alttimer[e] = math.huge
	played[e] = 0
	rspinspeed[e] = 0
	currentvalue[e] = 0
	doonce[e] = 0
	repair[e] = 0
	status[e] = "init"
	state[e] = "safe"
	emlevel[e] = ring[e].pulse_strength
	empulse[e] = "down"
	emr[e],emg[e],emb[e] = GetEntityEmissiveColor(e)
	SetEntityEmissiveColor(e,emr[e],emg[e],emb[e])
	emst[e] = GetEntityEmissiveStrength(e)
	SetEntityEmissiveStrength(e,emst[e])
end

function ring_counter_main(e)

	if status[e] == "init" then
		alttimer[e] = g_Time + (ring[e].alternating_time*1000)
		current_level[e] = GetEntityBaseAlpha(e)
		SetEntityTransparency(e,1)
		status[e] = "endinit"
	end

	local PlayerDist = GetPlayerDistance(e)

	if ring[e].ring_type == 1 then --Static
		SetEntityEmissiveColor(e,emr[e],emg[e],emb[e])
		SetEntityEmissiveStrength(e,emst[e])
		if PlayerDist < ring[e].ring_range then
			if played[e] == 0 then
				PlaySound(e,0)
				played[e] = 1
			end
			if ring[e].trigger_type == 2 then LoseGame() end
			if ring[e].trigger_type == 3 then WinGame() end
			if ring[e].trigger_type == 4 then
				ActivateIfUsed(e)
				PerformLogicConnections(e)
			end
			if  ring[e].user_global_affected > "" and doonce[e] == 0 then
				if _G["g_UserGlobal['"..ring[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..ring[e].user_global_affected.."']"] end
				_G["g_UserGlobal['"..ring[e].user_global_affected.."']"] = currentvalue[e] + ring[e].affect_amount
				doonce[e] = 1
			end
			if ring[e].augments == 1 then
				if repair[e] == 0 then
					SetPlayerHealth(g_PlayerHealth + ring[e].affect_amount)
					if g_PlayerHealth > g_PlayerStartStrength then g_PlayerHealth = g_PlayerStartStrength end
					SetPlayerHealthCore(g_PlayerHealth)
					repair[e] = 1
				end
			end
			if ring[e].augments == 2 then
				if repair[e] == 0 then
					closestent[e] = U.ClosestEntToPlayer(90)
					SetEntityHealth(closestent[e],g_Entity[closestent[e]]['health'] + ring[e].affect_amount)
					SetPlayerHealth(g_PlayerHealth + ring[e].affect_amount)
					if g_PlayerHealth > g_PlayerStartStrength then g_PlayerHealth = g_PlayerStartStrength end
					SetPlayerHealthCore(g_PlayerHealth)
					repair[e] = 1
				end
			end
			if current_level[e] > 0 then
				SetEntityBaseAlpha(e,current_level[e])
				current_level[e] = current_level[e] - 1
			end
			if current_level[e] <= 0 then
				CollisionOff(e)
				Hide(e)
				SetEntityEmissiveColor(e,emr[e],emg[e],emb[e])
				SetEntityEmissiveStrength(e,emst[e])
				Destroy(e)
			end

		end
	end

	if ring[e].ring_type == 2 then --Alternating
		if g_Time > alttimer[e] and state[e] == "safe" then
			state[e] = "unsafe"
			SetEntityEmissiveColor(e,255,0,0)
			alttimer[e] = g_Time + (ring[e].alternating_time*1000)
		end
		if g_Time > alttimer[e] and state[e] == "unsafe" then
			state[e] = "safe"
			SetEntityEmissiveColor(e,emr[e],emg[e],emb[e])
			alttimer[e] = g_Time + (ring[e].alternating_time*1000)
		end

		if PlayerDist < ring[e].ring_range and state[e] == "safe" then
			if played[e] == 0 then
				PlaySound(e,0)
				played[e] = 1
			end
			if ring[e].trigger_type == 2 then LoseGame() end
			if ring[e].trigger_type == 3 then WinGame() end
			if ring[e].trigger_type == 4 then
				ActivateIfUsed(e)
				PerformLogicConnections(e)
			end
			if ring[e].user_global_affected > "" and doonce[e] == 0 then
				if _G["g_UserGlobal['"..ring[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..ring[e].user_global_affected.."']"] end
				_G["g_UserGlobal['"..ring[e].user_global_affected.."']"] = currentvalue[e] + ring[e].affect_amount
				doonce[e] = 1
			end
			if ring[e].augments == 1 then
				if repair[e] == 0 then
					SetPlayerHealth(g_PlayerHealth + ring[e].affect_amount)
					if g_PlayerHealth > g_PlayerStartStrength then g_PlayerHealth = g_PlayerStartStrength end
					SetPlayerHealthCore(g_PlayerHealth)
					repair[e] = 1
				end
			end
			if ring[e].augments == 2 then
				if repair[e] == 0 then
					closestent[e] = U.ClosestEntToPlayer(90)
					SetEntityHealth(closestent[e],g_Entity[closestent[e]]['health'] + ring[e].affect_amount)
					SetPlayerHealth(g_PlayerHealth + ring[e].affect_amount)
					if g_PlayerHealth > g_PlayerStartStrength then g_PlayerHealth = g_PlayerStartStrength end
					SetPlayerHealthCore(g_PlayerHealth)
					repair[e] = 1
				end
			end
			if current_level[e] > 0 then
				SetEntityBaseAlpha(e,current_level[e])
				current_level[e] = current_level[e] - 1
			end
			if current_level[e] <= 0 then
				CollisionOff(e)
				Hide(e)
				SetEntityEmissiveColor(e,emr[e],emg[e],emb[e])
				SetEntityEmissiveStrength(e,emst[e])
				Destroy(e)
			end
		end

		if PlayerDist < ring[e].ring_range and state[e] == "unsafe" then
			if played[e] == 0 then
				PlaySound(e,1)
				played[e] = 1
			end
			if ring[e].trigger_type == 2 then LoseGame() end
			if ring[e].trigger_type == 3 then WinGame() end
			if ring[e].trigger_type == 4 then
				ActivateIfUsed(e)
				PerformLogicConnections(e)
			end
			if ring[e].user_global_affected > "" and doonce[e] == 0 then
				if _G["g_UserGlobal['"..ring[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..ring[e].user_global_affected.."']"] end
				_G["g_UserGlobal['"..ring[e].user_global_affected.."']"] = currentvalue[e] - ring[e].affect_amount
				doonce[e] = 1
			end
			if current_level[e] > 0 then
				SetEntityBaseAlpha(e,current_level[e])
				current_level[e] = current_level[e] - 1
			end
			if current_level[e] <= 0 then
				CollisionOff(e)
				Hide(e)
				SetEntityEmissiveColor(e,emr[e],emg[e],emb[e])
				SetEntityEmissiveStrength(e,emst[e])
				Destroy(e)
			end
		end
	end

	if ring[e].pulse_effect == 1 then
		if emlevel[e] > 10 and empulse[e] == "down" then
			SetEntityEmissiveStrength(e,emlevel[e])
			emlevel[e] = emlevel[e] - ring[e].pulse_speed
			if emlevel[e] <= 10 then empulse[e] = "climb" end
		end
		if emlevel[e] < ring[e].pulse_strength and empulse[e] == "climb" then
			SetEntityEmissiveStrength(e,emlevel[e])
			emlevel[e] = emlevel[e] + ring[e].pulse_speed
			if emlevel[e] >= ring[e].pulse_strength then empulse[e] = "down" end
		end
	end

	if ring[e].spin_effect == 1 then
		if ring[e].spin_direction == 1 then
			rspinspeed[e] = ring[e].spin_speed
		else
			rspinspeed[e] = ring[e].spin_speed * -1
		end
		if ring[e].spin_x == 1 then
			CollisionOff(e)
			RotateX(e,GetAnimationSpeed(e)*rspinspeed[e])
			CollisionOn(e)
		end
		if ring[e].spin_y == 1 then
			CollisionOff(e)
			RotateY(e,GetAnimationSpeed(e)*rspinspeed[e])
			CollisionOn(e)
		end
		if ring[e].spin_z == 1 then
			CollisionOff(e)
			RotateZ(e,GetAnimationSpeed(e)*rspinspeed[e])
			CollisionOn(e)
		end
	end
end