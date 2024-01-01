-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Lurker v9 by Necrym 59 
-- DESCRIPTION: The attached character entity will be a lurker and watch, follow or attack when not looked at.
-- DESCRIPTION: [REACTION_TEXT$="What the.."]
-- DESCRIPTION: [SENSE_RANGE=1000(1,2000)]
-- DESCRIPTION: [SENSE_ANGLE=70(1,120)]
-- DESCRIPTION: [APPROACH_SPEED=180(1,500)]
-- DESCRIPTION: [ROTATION_SPEED=8(1,20)]
-- DESCRIPTION: [@MODE=1(1=Follow only, 2=Attack in range, 3=Watch 0nly, 4=Move then Disappear)]
-- DESCRIPTION: [ATTACK_RANGE=100(1,200)]
-- DESCRIPTION: [ATTACK_DELAY=500(1,2000)]
-- DESCRIPTION: [ATTACK_DAMAGE=10(1,100)]
-- DESCRIPTION: [TRANSPARENCY=100(0,100)]
-- DESCRIPTION: [EMISSIVENESS=0(0,5000)]
-- DESCRIPTION: [@IDLE_ANIMATION$=-1(0=AnimSetList)] [@ATTACK_ANIMATION$=-1(0=AnimSetList)]]
-- DESCRIPTION: <Sound0> for movement sound
-- DESCRIPTION: <Sound1> for lurking sound effect
-- DESCRIPTION: <Sound2> for attacking sound effect
-- DESCRIPTION: <Sound3> for disappearing sound effect


	local lurker 			= {}
	local reaction_text 	= {}
	local sense_range 		= {}
	local sense_angle 		= {}
	local approach_speed 	= {}
	local rotation_speed 	= {}
	local mode 				= {}	
	local attack_range 		= {}
	local attack_delay 		= {}
	local attack_damage 	= {}
	local status 			= {}
	local transparency 		= {}
	local emissiveness 		= {}
	local idle_animation	= {}
	local attack_animation	= {}
		
	local default_attack_delay 	= {}
	local default_sound_delay 	= {}
	local sound_delay 			= {}
	local start_x 				= {}
	local start_y 				= {}
	local start_z 				= {}
	local start_health 			= {}
	local scare 				= {}
	local movecount				= {}
	local doonce				= {}

function lurker_properties(e, reaction_text, sense_range, sense_angle, approach_speed, rotation_speed, mode, attack_range, attack_delay, attack_damage, transparency, emissiveness, idle_animation, attack_animation)
	lurker[e] = g_Entity[e]
	lurker[e].reaction_text = reaction_text
	lurker[e].sense_range = sense_range
	lurker[e].sense_angle = sense_angle
	lurker[e].approach_speed = approach_speed
	lurker[e].rotation_speed = rotation_speed
	lurker[e].mode = mode
	lurker[e].attack_range = attack_range
	lurker[e].attack_delay = attack_delay
	lurker[e].attack_damage = attack_damage
	lurker[e].transparency = transparency
	lurker[e].emissiveness = emissiveness
	lurker[e].idle_animation = "=" .. tostring(idle_animation)
	lurker[e].attack_animation = "=" .. tostring(attack_animation)
end

function lurker_init(e)
	lurker[e] = g_Entity[e]
	lurker[e].reaction_text = "What the.."
	lurker[e].sense_range = 500
	lurker[e].sense_angle = 70
	lurker[e].approach_speed = 280
	lurker[e].rotation_speed = 8
	lurker[e].mode = 1
	lurker[e].attack_range = 100
	lurker[e].attack_delay = 500
	lurker[e].attack_damage = 10
	lurker[e].transparency = 100
	lurker[e].emissiveness = 1
	sound_delay[e] = 1000
	SetEntityBaseAlpha(e,100)
	SetEntityTransparency(e,1)
	SetEntityEmissiveStrength(e,0)
	lurker[e].idle_animation = ""
	lurker[e].attack_animation = ""
	movecount[e] = 0
	doonce[e] = 0
	status[e] = "init"	
end

function lurker_main(e)
	lurker[e] = g_Entity[e]
	local PlayerDist = GetPlayerDistance(e)
	
	if status[e] == "init" then		
		SetEntityBaseAlpha(e,lurker[e].transparency)
		SetEntityEmissiveStrength(e,lurker[e].emissiveness)
		default_attack_delay[e] = lurker[e].attack_delay
		default_sound_delay[e] = sound_delay[e]
		start_x[e] = g_Entity[e]['x']
		start_y[e] = g_Entity[e]['y']
		start_z[e] = g_Entity[e]['z']
		start_health[e] = g_Entity[e]['health']
		scare[e] = 0
		status[e] = "endinit"
	end

	if PlayerDist > lurker[e].sense_range then
		SetAnimationName(e,"idle")
		PlayAnimation(e)
	end

	if PlayerDist <= lurker[e].sense_range then	
		if PlayerLooking(e,lurker[e].sense_range,lurker[e].sense_angle) == 1 then doonce[e] = 0 end
		if PlayerLooking(e,lurker[e].sense_range,lurker[e].sense_angle) ~= 1 then			
			LookAtPlayer(e)			
			if PlayerDist >= lurker[e].sense_range/2 then
				CollisionOff(e)
				RotateToPlayerSlowly(e,lurker[e].rotation_speed)
				CollisionOn(e)
			else
				CollisionOff(e)			
				RotateToPlayer(e)
				CollisionOn(e)
			end
			if PlayerDist <= lurker[e].attack_range then							
				if GetTimer(e) > lurker[e].attack_delay and PlayerLooking(e,lurker[e].sense_range,lurker[e].sense_angle) ~= 1 then
					if lurker[e].mode == 1 then
						SetAnimationName(e,lurker[e].idle_animation)
						PlayAnimation(e)						
					end
					if lurker[e].mode == 2 then
						SetAnimationName(e,lurker[e].attack_animation)
						PlayAnimation(e)
						HurtPlayer(-1,lurker[e].attack_damage)
						lurker[e].attack_delay = GetTimer(e) + default_attack_delay[e]
					end					
				end				
				if GetTimer(e) > sound_delay[e] then
					if lurker[e].mode == 1 then
						PlaySound(e,1)
						Prompt(lurker[e].reaction_text)
					end
					if lurker[e].mode == 2 then PlaySound(e,2) end
					sound_delay[e] = GetTimer(e) + default_sound_delay[e]
				end
			else
				GravityOff(e)
				CollisionOff(e)
				if lurker[e].mode == 1 then MoveForward(e,math.random(1,lurker[e].approach_speed)) end
				if lurker[e].mode == 2 then	MoveForward(e,lurker[e].approach_speed) end
				if lurker[e].mode == 3 then	LookAtPlayer(e) end
				if lurker[e].mode == 4 then 
					MoveForward(e,math.random(1,lurker[e].approach_speed))
					if doonce[e] == 0 then
						movecount[e] = movecount[e] + 1
						doonce[e] = 1
					end
				end
				LookAtPlayer(e)
				PlaySound(e,0)				
				sound_delay[e] = GetTimer(e) + (default_sound_delay[e] / 2)
				lurker[e].attack_delay = GetTimer(e) + default_attack_delay[e]
				if movecount[e] == 3 then
					PlaySound(e,3)
					Hide(e)
					CollisionOff(e)					
					Destroy(e)
				end
			end			
		end
	end	
	if g_Entity[e]['health'] < start_health[e] then
		CollisionOff(e)
		scare[e]=math.random(1,2)
		if scare[e] == 1 then
			SetPosition(e,g_Entity[e]['x']-math.random(-200,200),g_Entity[e]['y'],g_Entity[e]['z']-math.random(-200,200))
			ResetPosition(e,g_Entity[e]['x']-math.random(-200,200),g_Entity[e]['y'],g_Entity[e]['z']-math.random(-200,200))
		end
		if scare[e] == 2 then 
			SetPosition(e,start_x[e],g_Entity[e]['y'],start_z[e])
			ResetPosition(e,start_x[e],g_Entity[e]['y'],start_z[e])
		end
		RotateToPlayer(e)
		LookAtPlayer(e)
		start_health[e] = g_Entity[e]['health']
		CollisionOn(e)		
	end		
	CollisionOn(e)
	GravityOn(e)
end

function PlayerLooking(e,dis,v)
	if g_Entity[e] ~= nil then
		if dis == nil then
			dis = 3000
		end
		if v == nil then
			v = 0.5
		end
		if GetPlayerDistance(e) <= dis then
			local destx = g_Entity[e]['x'] - g_PlayerPosX
			local destz = g_Entity[e]['z'] - g_PlayerPosZ
			local angle = math.atan2(destx,destz)
			angle = angle * (180.0 / math.pi)
			if angle <= 0 then
				angle = 360 + angle
			elseif angle > 360 then
				angle = angle - 360
			end
			while g_PlayerAngY < 0 or g_PlayerAngY > 360 do
				if g_PlayerAngY <= 0 then
					g_PlayerAngY = 360 + g_PlayerAngY
				elseif g_PlayerAngY > 360 then
					g_PlayerAngY = g_PlayerAngY - 360
				end
			end
			local L = angle - v
			local R = angle + v
			if L <= 0 then
				L = 360 + L 
			elseif L > 360 then
				L = L - 360
			end
			if R <= 0 then
				R = 360 + R
			elseif R > 360 then
				R = R - 360
			end
			if (L < R and math.abs(g_PlayerAngY) > L and math.abs(g_PlayerAngY) < R) then
				return 1
			elseif (L > R and (math.abs(g_PlayerAngY) > L or math.abs(g_PlayerAngY) < R)) then
				return 1
			else
				return 0
			end
		else
			return 0
		end
	end
end