-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Phantom v15 by Necrym59
-- DESCRIPTION: Fades in an object and can approach player when in zone or switched by control.
-- DESCRIPTION: Attach to object and set Physics ON/OFF, Always active ON. 
-- DESCRIPTION: Change [PROMPT_TEXT$="What was that"] [@APPEARANCE_MODE=1(1=Show-Fade/Disappear, 2=Show-Approach, 3=Show-Face-Approach, 4=Show-Move-Fade/Disappear, 5=Character Move-Once-No Collision)]
-- DESCRIPTION: [APPEARANCE_LEVEL=80(1,100)] [#APPEARANCE_AURA=0.0(0.0,50)] [#APPEARANCE_SPEED=0.02(0,5)] [#APPEARANCE_TIME=3.0(0,10)] [@DISAPPEARANCE_MODE=1(1=Fade, 2=Instant)]
-- DESCRIPTION: [#MOVEMENT_SPEED=50.0(0,500)] [#MOVEMENT_X=-0.0(-1000,1000)] [#MOVEMENT_Z=-0.0(-1000,1000)]
-- DESCRIPTION: [@IDLE_ANIMATION$=-1(0=AnimSetList)] [@MOVE_ANIMATION$=-1(0=AnimSetList)]]
-- DESCRIPTION: [@ANIMATED_MODEL=3(1=Character Creator, 2=Legacy Character, 3=Non Animated)]
-- DESCRIPTION: <Sound0> upon apperance
-- DESCRIPTION: <Sound1> upon approach
-- DESCRIPTION: <Sound2> upon disapperance

	local V = require "scriptbank\\vectlib"
	local U = require "scriptbank\\utillib"
	
	local phantom 				= {}
	local prompt_text 			= {}
	local appearance_mode 		= {}
	local appearance_level 		= {}
	local appearance_aura 		= {}
	local appearance_speed 		= {}
	local appearance_time		= {}
	local disappearance_mode	= {}
	local movement_speed 		= {}
	local movement_x 			= {}
	local movement_z 			= {}
	local idle_animation		= {}
	local move_animation		= {}
	local animated_model		= {}
	
	local current_level 		= {}	
	local status 				= {}
	local doonce 				= {}
	local legacy		 		= {}	
	local rotonce 				= {}
	local animonce				= {}
	local calconce				= {}
	local showtime 				= {}
	local fullshow 				= {}

function phantom_properties(e, prompt_text, appearance_mode, appearance_level, appearance_aura, appearance_speed, appearance_time, disappearance_mode, movement_speed, movement_x, movement_z, idle_animation, move_animation, animated_model)
	phantom[e] = g_Entity[e]
	phantom[e].prompt_text = prompt_text
	phantom[e].appearance_mode	= appearance_mode
	phantom[e].appearance_level = appearance_level
	phantom[e].appearance_aura = appearance_aura
	phantom[e].appearance_speed = appearance_speed
	phantom[e].appearance_time = appearance_time
	phantom[e].disappearance_mode = disappearance_mode
	phantom[e].movement_speed = movement_speed
	phantom[e].movement_x = movement_x
	phantom[e].movement_z = movement_z
	phantom[e].idle_animation = "=" .. tostring(idle_animation)
	phantom[e].move_animation = "=" .. tostring(move_animation)
	phantom[e].animated_model = animated_model
end 

function phantom_init(e)
	phantom[e] = g_Entity[e]
	phantom[e].prompt_text 	= ""
	phantom[e].appearance_mode	= 1
	phantom[e].appearance_level = 80
	phantom[e].appearance_aura = 0
	phantom[e].appearance_speed	= 1
	phantom[e].appearance_time = 3.0
	phantom[e].disappearance_mode = 1
	phantom[e].movement_speed = 5
	phantom[e].movement_x = 0
	phantom[e].movement_z = 0
	phantom[e].idle_animation = ""
	phantom[e].move_animation = ""
	phantom[e].animated_model = 1
	status[e] = "init"
	doonce[e] = 0
	legacy[e] = 0
	rotonce[e] = 0
	animonce[e] = 0
	calconce[e] = 0
	showtime[e] = 0
	fullshow[e]	= 0
	current_level[e] = GetEntityBaseAlpha(e)
	SetEntityTransparency(e,1)
	SetEntityEmissiveStrength(e,0)
end

function phantom_main(e)
	phantom[e] = g_Entity[e]
	if status[e] =="init" then	
		SetEntityBaseAlpha(e,0)
		current_level[e] = GetEntityBaseAlpha(e)
		StartTimer(e)
		status[e] = "end"
	end

	if g_Entity[e]['activated'] == 1 then
		PlayerDist = GetPlayerDistance(e)
		
		if showtime[e] == 0 then
			showtime[e] = GetTimer(e) + (phantom[e].appearance_time*1000)
		end			
		if doonce[e] == 0 then
			Prompt(phantom[e].prompt_text)		
			PlaySound(e,0)
			doonce[e] = 1
			SetAnimationName(e,phantom[e].idle_animation)					
			LoopAnimation(e)			
		end
		
		if phantom[e].appearance_mode == 1 then -- Show				
			if current_level[e] < phantom[e].appearance_level and fullshow[e] == 0 then			
				SetEntityBaseAlpha(e,current_level[e])
				if phantom[e].appearance_aura > 0 then SetEntityEmissiveStrength(e,current_level[e]*phantom[e].appearance_aura) end
				current_level[e] = current_level[e] + phantom[e].appearance_speed				
			end			
			if current_level[e] >= phantom[e].appearance_level then fullshow[e] = 1 end				
			if fullshow[e] == 1 then 
				if GetTimer(e) >= showtime[e] then
					CollisionOff(e)
					if phantom[e].disappearance_mode == 1 then 
						if current_level[e] > 0 then
							SetEntityEmissiveStrength(e,GetEntityEmissiveStrength(e)-phantom[e].appearance_speed)
							SetEntityBaseAlpha(e,current_level[e])
							current_level[e] = current_level[e] - phantom[e].appearance_speed
						end							
						if current_level[e] <= 0 then
							current_level[e] = 0
							Destroy(e)
						end
					end
					if phantom[e].disappearance_mode == 2 then
						CollisionOff(e)			
						Destroy(e)
					end					
				end
			end	
		end
		
		if phantom[e].appearance_mode == 2 then -- Show and Approach
			GravityOff(e)
			CollisionOff(e)	
			if current_level[e] < phantom[e].appearance_level then				
				SetEntityBaseAlpha(e,current_level[e])
				if phantom[e].appearance_aura > 0 then SetEntityEmissiveStrength(e,current_level[e]*phantom[e].appearance_aura) end
				current_level[e] = current_level[e] + phantom[e].appearance_speed				
			end
			if current_level[e] >= phantom[e].appearance_level then	
				if animonce[e] == 0 then
					SetAnimationName(e,phantom[e].move_animation)
					LoopAnimation(e)
					animonce[e] = 1
				end				
				local plrpos = V.Create(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
				local objpos = V.Create(g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'])
				newvec = V.Mul(V.Norm(V.Sub(plrpos,objpos)),phantom[e].movement_speed)
				PushObject(g_Entity[e]['obj'],newvec.x, 0, newvec.z, 8, 1, 0 )
				RDFindPath(g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'],g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
				StartMoveAndRotateToXYZ(e,phantom[e].movement_speed,phantom[e].movement_speed,0,0)
				if phantom[e].animated_model == 1 then SetAnimationSpeedModulation (e,phantom[e].movement_speed/100) end
				if phantom[e].animated_model == 2 then
					if legacy[e] == 0 then
						phantom[e].movement_speed = phantom[e].movement_speed/100
						legacy[e] = 1 
					end
				end
				if phantom[e].animated_model == 3 then SetAnimationSpeedModulation (e,phantom[e].movement_speed/100) end
				StopSound(e,0)
				PlaySound(e,1)
				if PlayerDist < 50 then
					CollisionOff(e)					
					Destroy(e)
				end			
			end
		end
		if phantom[e].appearance_mode == 3 then -- Show-Face and Approach	
			GravityOff(e)
			if current_level[e] < phantom[e].appearance_level then			
				SetEntityBaseAlpha(e,current_level[e])
				if phantom[e].appearance_aura > 0 then SetEntityEmissiveStrength(e,current_level[e]*phantom[e].appearance_aura) end
				current_level[e] = current_level[e] + phantom[e].appearance_speed				
			end
			if current_level[e] >= phantom[e].appearance_level and rotonce[e] == 0 then
				CollisionOff(e)
				if phantom[e].animated_model == 2 then SetRotation(e,0,g_PlayerAngY-180,0) end		
				RotateToPlayer(e)				
				CollisionOn(e)
				rotonce[e] = 1
			end			
			if current_level[e] >= phantom[e].appearance_level and rotonce[e] == 1 then
				if animonce[e] == 0 then					
					SetAnimationName(e,phantom[e].move_animation)
					LoopAnimation(e)
					animonce[e] = 1
				end
				local plrpos = V.Create(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
				local objpos = V.Create(g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'])
				newvec = V.Mul(V.Norm(V.Sub(plrpos,objpos)),phantom[e].movement_speed)
				PushObject(g_Entity[e]['obj'],newvec.x, 0, newvec.z)				
				RDFindPath(g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'],g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
				StartMoveAndRotateToXYZ(e,phantom[e].movement_speed,phantom[e].movement_speed,0,0)							
				if phantom[e].animated_model == 1 then SetAnimationSpeedModulation (e,phantom[e].movement_speed/100) end
				if phantom[e].animated_model == 2 then
					if legacy[e] == 0 then
						phantom[e].movement_speed = phantom[e].movement_speed/100
						legacy[e] = 1 
					end
				end
				if phantom[e].animated_model == 3 then SetAnimationSpeedModulation (e,phantom[e].movement_speed/100) end
				StopSound(e,0)
				PlaySound(e,1)
				if PlayerDist < 50 then
					CollisionOff(e)			
					Destroy(e)
				end
			end
		end
		if phantom[e].appearance_mode == 4 then -- Show-Move-Disappear
			GravityOff(e)
			if current_level[e] < phantom[e].appearance_level and fullshow[e] == 0 then	
				SetEntityBaseAlpha(e,current_level[e])
				if phantom[e].appearance_aura > 0 then SetEntityEmissiveStrength(e,current_level[e]*phantom[e].appearance_aura) end
				current_level[e] = current_level[e] + phantom[e].appearance_speed				
			end	
			if current_level[e] >= phantom[e].appearance_level then fullshow[e] = 1 end
			if current_level[e] >= phantom[e].appearance_level then	
				if animonce[e] == 0 then				
					SetAnimationName(e,phantom[e].move_animation)
					LoopAnimation(e)					
					animonce[e] = 1
				end
				local plrpos = V.Create(g_PlayerPosX+400,g_PlayerPosY,g_PlayerPosZ)
				local objpos = V.Create(g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'])
				newvec = V.Mul(V.Norm(V.Sub(plrpos,objpos)),phantom[e].movement_speed)
				PushObject(g_Entity[e]['obj'],newvec.x, 0, newvec.z)
				if calconce[e] == 0 then					
					RDFindPath(g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'],g_Entity[e]['x']+phantom[e].movement_x,g_Entity[e]['y'], g_Entity[e]['z']+phantom[e].movement_z)					
					calconce[e] = 1
				end
				StartMoveAndRotateToXYZ(e,phantom[e].movement_speed,phantom[e].movement_speed,0,0)
				RDFindPath(g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'],g_Entity[e]['x']+phantom[e].movement_x,g_Entity[e]['y'], g_Entity[e]['z']+phantom[e].movement_z)
				if phantom[e].animated_model == 1 then SetAnimationSpeedModulation (e,phantom[e].movement_speed/100) end
				if phantom[e].animated_model == 2 then
					if legacy[e] == 0 then
						phantom[e].movement_speed = phantom[e].movement_speed/100
						legacy[e] = 1 
					end
				end
				if phantom[e].animated_model == 3 then SetAnimationSpeedModulation (e,phantom[e].movement_speed/100) end
				StopSound(e,0)
			end
			if fullshow[e] == 1 then 
				if GetTimer(e) >= showtime[e] then
					CollisionOff(e)
					if phantom[e].disappearance_mode == 1 then
						StopAnimation(e)
						if current_level[e] > 0 then
							SetEntityEmissiveStrength(e,GetEntityEmissiveStrength(e)-phantom[e].appearance_speed)
							SetEntityBaseAlpha(e,current_level[e])
							current_level[e] = current_level[e]-phantom[e].appearance_speed
						end							
						if current_level[e] <= 0 then
							current_level[e] = 0
							Destroy(e)
						end
					end
					if phantom[e].disappearance_mode == 2 then
						CollisionOff(e)			
						Destroy(e)
					end					
				end	
			end
		end
		if phantom[e].appearance_mode == 5 then -- Character Move-Once-No Collision
			GravityOff(e)
			if current_level[e] < phantom[e].appearance_level then				
				SetEntityBaseAlpha(e,current_level[e])
				if phantom[e].appearance_aura > 0 then SetEntityEmissiveStrength(e,current_level[e]*phantom[e].appearance_aura) end
				current_level[e] = current_level[e] + phantom[e].appearance_speed				
			end	
			if current_level[e] >= phantom[e].appearance_level then	
				if animonce[e] == 0 then				
					SetAnimationName(e,phantom[e].move_animation)					
					PlayAnimation(e)
					animonce[e] = 1
				end				
				if phantom[e].animated_model == 1 or 3 then SetAnimationSpeedModulation (e,phantom[e].movement_speed/100) end
				if phantom[e].animated_model == 2 then
					if legacy[e] == 0 then
						phantom[e].movement_speed = phantom[e].movement_speed/100
						legacy[e] = 1 
					end
				end
				StopSound(e,0)
				PlaySound(e,2)
				if GetTimer(e) >= showtime[e] or PlayerDist < 50 then					
					CollisionOff(e)				
					Destroy(e)
				end
			end
		end
	end	
	PerformLogicConnections(e)
end
