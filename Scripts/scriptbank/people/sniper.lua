-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Sniper v6 by Necrym59
-- DESCRIPTION: Creates a sniper character that will attack player when within range
-- DESCRIPTION: Attach to a character.
-- DESCRIPTION: [SNIPER_RANGE=2000(0,2000)]
-- DESCRIPTION: [SNIPER_DELAY=4] seconds
-- DESCRIPTION: [HIT_DAMAGE=3]
-- DESCRIPTION: [HIT_CHANCE=60]
-- DESCRIPTION: [@IDLE_ANIMATION$=-1(0=AnimSetList)]
-- DESCRIPTION: [@AIM_ANIMATION$=-1(0=AnimSetList)]
-- DESCRIPTION: [@SHOOT_ANIMATION$=-1(0=AnimSetList)]
-- DESCRIPTION: [@FLINCH_ANIMATION$=-1(0=AnimSetList)]
-- DESCRIPTION: <Shooting Weapon>
-- DESCRIPTION: <Sound0> Gunshot
-- DESCRIPTION: <Sound1> Bullet Miss
-- DESCRIPTION: <Sound2> Bullet Hit

	local sniper 				= {}
	local sniper_range 			= {}
	local sniper_delay 			= {}	
	local hit_damage 			= {}	
	local hit_chance 			= {}	
	local idle_animation		= {}
	local aim_animation			= {}
	local shoot_animation		= {}
	local flinch_animation		= {}	

	local status 				= {}
	local shot_fired	 		= {}
	local anim_idle				= {}
	local anim_aim				= {}
	local anim_shoot			= {}
	local anim_flinch			= {}	
	local sx					= {}
	local sy					= {}
	local sz					= {}
	local px					= {}
	local py					= {}
	local pz					= {}
	local objecthit				= {}
	local shothit				= {}
	local snipertimer			= {}
	local hittimer				= {}	
	local sensed				= {}
	local start_health			= {}

function sniper_properties(e, sniper_range, sniper_delay, hit_damage, hit_chance, idle_animation, aim_animation, shoot_animation, flinch_animation)
	sniper[e] = g_Entity[e]
	sniper[e].sniper_mode = sniper_mode
	sniper[e].sniper_range = sniper_range
	sniper[e].sniper_delay = sniper_delay
	sniper[e].hit_damage = hit_damage
	sniper[e].hit_chance = hit_chance	
	sniper[e].idle_animation = "=" .. tostring(idle_animation)
	sniper[e].aim_animation = "=" .. tostring(aim_animation)
	sniper[e].shoot_animation = "=" .. tostring(shoot_animation)
	sniper[e].flinch_animation = "=" .. tostring(flinch_animation)		
end

function sniper_init(e)
	sniper[e] = {}
	sniper[e].sniper_range = 2000
	sniper[e].sniper_delay = 4
	sniper[e].hit_damage = 3
	sniper[e].hit_chance = 60
	sniper[e].idle_animation = ""
	sniper[e].aim_animation = ""
	sniper[e].shoot_animation = ""
	sniper[e].flinch_animation = ""
	
	status[e] = "init"
	anim_idle[e] = 0
	anim_aim[e] = 0
	anim_shoot[e] = 0
	anim_flinch[e] = 0	
	shot_fired[e] = 0
	objecthit[e] = 0
	shothit[e] = 0
	snipertimer[e] = 0
	hittimer[e] = 0
	start_health[e] = g_Entity[e]['health']
end

function sniper_main(e)
	
	if status[e] =="init" then
		local weaponid = GetEntityHasWeapon(e)
		status[e] ="endinit"
	end
	
	local PlayerDist = GetPlayerDistance(e)
	GetEntityPlayerVisibility(e)
	
	if PlayerDist > GetEntityViewRange(e) then
		--- Stationary -------------------------------------------------------------------
		if anim_idle[e] == 0 then	
			SetAnimationName(e,sniper[e].idle_animation)
			LoopAnimation(e)
			anim_idle[e] = 1
		end	
	end	
	
	if PlayerDist < GetEntityViewRange(e) and g_Entity[e]['plrvisible'] == 1 then
		RotateToPlayer(e)
		if anim_aim[e] == 0 then
			SetAnimationName(e,sniper[e].aim_animation)
			PlayAnimation(e)
			AimAtPlayer(e,10)
			anim_aim[e] = 1
		end
		if snipertimer[e] == 0 or nil then snipertimer[e] = g_Time + (sniper[e].sniper_delay*1000) end			
		if shot_fired[e] == 0 then
			if g_Time > snipertimer[e] then
				sx = g_Entity[e]['x']
				sy = g_Entity[e]['y'] + 100
				sz = g_Entity[e]['z']
				px = g_PlayerPosX
				py = g_PlayerPosY
				pz = g_PlayerPosZ
				shothit[e] = RayTerrain(sx,sy,sz,px,py+15,pz)
				if shothit[e] == 1 then
					shot_fired[e] = 1
					PlaySound(e,0)
					shothit[e] = 0
					snipertimer[e] = g_Time + sniper[e].sniper_delay*1000
				end
			end
		end
		if shot_fired[e] == 1 and PlayerDist < sniper[e].sniper_range then
			if anim_shoot[e] == 0 then 
				SetAnimationName(e,sniper[e].shoot_animation)
				PlayAnimation(e)
				FireWeapon(e)
				anim_shoot[e] = 1
			end	
			if math.random(1,100) <= sniper[e].hit_chance then
				PlaySound(e,2)
				HurtPlayer(e,sniper[e].hit_damage)
			else
				PlaySound(e,1)
			end
			anim_shoot[e] = 0
			shot_fired[e] = 0
		end
	end

	if g_Entity[e]['health'] < start_health[e] then
		if anim_flinch[e] == 0 then
			SetAnimationName(e,sniper[e].flinch_animation)
			PlayAnimation(e)
			RotateToPlayerSlowly(e,GetEntityTurnSpeed(e))
			anim_flinch[e] = 1
		end
		start_health[e] = g_Entity[e]['health']
		hittimer[e] = g_Time + 4000
	end
	if g_Entity[e]['health'] == 0 then				
		StopSound(e,0)
		StopSound(e,1)
		StopSound(e,2)
		SetEntityHealthSilent(e,-12345)
		SetEntityHealth(e,-12345)
		SetPreExitValue(e,2)
		SwitchScript(e,"no_behavior_selected.lua")			
	end
	if g_Time > hittimer[e] then
		anim_flinch[e] = 0
		anim_idle[e] = 0
	end	
end

function sniper_preexit(e)
end

function sniper_exit(e)
end
