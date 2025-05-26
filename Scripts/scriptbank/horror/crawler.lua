-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Crawler v18 by Necrym59
-- DESCRIPTION: Allows a character entity to walk/crawl, target a player or named object and climb a wall, or can ceiling jump or ceiling drop.
-- DESCRIPTION: when triggered by a zone or switched by control.
-- DESCRIPTION: Attach to object and set Physics ON/OFF, Always active ON.
-- DESCRIPTION: Change [PROMPT_TEXT$="What was that"] [@APPEARANCE_MODE=1(1=Target Player, 2=Target Object, 3=Target Object + Climb, 4=Ceiling Jump, 5=Ceiling Drop + Target Object)]
-- DESCRIPTION: [APPEARANCE_LEVEL=100(1,100)] [#APPEARANCE_SPEED=0.02(0,5)] [@DISAPPEARANCE_MODE=1(1=Fade, 2=Instant)]
-- DESCRIPTION: [#MOVEMENT_SPEED=50.0(1.0,5000)] [#CLIMB_HEIGHT=50.0(0,500)]
-- DESCRIPTION: [@IDLE_ANIMATION$=-1(0=AnimSetList)] [@MOVE_ANIMATION1$=-1(0=AnimSetList)]] [@MOVE_ANIMATION2$=-1(0=AnimSetList)]]
-- DESCRIPTION: [@ANIMATED_MODEL=2(1=Character Creator, 2=Legacy Character)]
-- DESCRIPTION: [OBJECT_NAME$=""] to target
-- DESCRIPTION: [@OBJECT_VISIBILITY=1(1=Visible, 2= Hidden)])
-- DESCRIPTION: [OBJECT_PROXIMITY=150(1,100)] to object endpoint
-- DESCRIPTION: <Sound0> upon apperance
-- DESCRIPTION: <Sound1> upon movement
-- DESCRIPTION: <Sound2> death scream.

	local lower = string.lower

	local crawler 				= {}
	local prompt_text 			= {}
	local appearance_level 		= {}
	local appearance_aura 		= {}
	local appearance_speed 		= {}
	local disappearance_mode	= {}
	local movement_speed 		= {}
	local climb_height			= {}
	local idle_animation		= {}
	local move_animation1		= {}
	local move_animation2		= {}
	local animated_model		= {}
	local object_name			= {}
	local object_no				= {}
	local object_proximity		= {}

	local current_level 		= {}
	local status 				= {}
	local doonce 				= {}
	local legacy		 		= {}
	local rotonce 				= {}
	local animonce				= {}
	local calconce				= {}
	local fullshow 				= {}
	local journey				= {}
	local startx				= {}
	local starty				= {}
	local startz				= {}
	local destx					= {}
	local desty					= {}
	local destz					= {}
	local destAngle				= {}
	local objdestx				= {}
	local objdesty				= {}
	local objdestz				= {}
	local objdestAngle			= {}	
	local pointcountp 			= {}
	local targetflag 			= {}
	local dropmode				= {}
	local wait					= {}

function crawler_properties(e, prompt_text, appearance_mode, appearance_level, appearance_speed, disappearance_mode, movement_speed, climb_height, idle_animation, move_animation1, move_animation2, animated_model, object_name, object_visibility, object_proximity)
	crawler[e].prompt_text = prompt_text
	crawler[e].appearance_mode	= appearance_mode
	crawler[e].appearance_level = appearance_level
	crawler[e].appearance_speed = appearance_speed
	crawler[e].disappearance_mode = disappearance_mode
	crawler[e].movement_speed = movement_speed/10
	crawler[e].climb_height = climb_height
	crawler[e].idle_animation = "=" .. tostring(idle_animation)
	crawler[e].move_animation1 = "=" .. tostring(move_animation1)
	crawler[e].move_animation2 = "=" .. tostring(move_animation2)
	crawler[e].animated_model = animated_model
	crawler[e].object_name =  lower(object_name)
	crawler[e].object_visibility = object_visibility
	crawler[e].object_proximity = object_proximity or 150
end

function crawler_init(e)
	crawler[e] = {}
	crawler[e].prompt_text 	= ""
	crawler[e].appearance_mode	= 1
	crawler[e].appearance_level = 80
	crawler[e].appearance_speed	= 1
	crawler[e].disappearance_mode = 1
	crawler[e].movement_speed = 5
	crawler[e].climb_height = 0
	crawler[e].idle_animation = ""
	crawler[e].move_animation1 = ""
	crawler[e].move_animation2 = ""
	crawler[e].animated_model = 1
	crawler[e].object_name = ""
	crawler[e].object_visibility = 1
	crawler[e].object_proximity = 150
	crawler[e].object_no = 0	
	status[e] = "init"
	doonce[e] = 0
	legacy[e] = 0
	rotonce[e] = 0
	animonce[e] = 0
	calconce[e] = 0
	fullshow[e]	= 0
	journey[e] = 0
	startx[e] = 0
	starty[e] = 0
	startz[e] = 0
	destx[e] = 0
	desty[e] = 0
	destz[e] = 0
	destAngle[e] = 0
	objdestx[e] = 0
	objdesty[e] = 0
	objdestz[e] = 0	
	objdestAngle[e] = 0	
	pointcountp[e] = 0
	targetflag[e] = 0
	dropmode[e] = 0
	wait[e] = math.huge
	current_level[e] = GetEntityBaseAlpha(e)
	SetEntityTransparency(e,1)
	GravityOff(e)
	if g_Entity[e]['health'] < 100 then SetEntityHealth(e,g_Entity[e]['health'] + 100) end
end

function crawler_main(e)
	
	if status[e] =="init" then
		startx[e] = g_Entity[e]['x']
		starty[e] = g_Entity[e]['y']
		startz[e] = g_Entity[e]['z']
		SetEntityBaseAlpha(e,0)
		current_level[e] = GetEntityBaseAlpha(e)
		StartTimer(e)
		if crawler[e].appearance_mode == 2 or crawler[e].appearance_mode == 3 or crawler[e].appearance_mode == 5 then
			if crawler[e].object_name ~= "" then
				for n = 1, g_EntityElementMax do
					if n ~= nil and g_Entity[n] ~= nil then
						if lower(GetEntityName(n)) == crawler[e].object_name then					
							crawler[e].object_no = n
							objdestx[e] = g_Entity[n]['x']
							objdesty[e] = g_Entity[n]['y']
							objdestz[e] = g_Entity[n]['z']
							objdestAngle[e] = g_Entity[n]['angley']
							if crawler[e].object_visibility == 2 then
								CollisionOff(n)
								Hide(n)
							end
						end
					end
				end
			end	
		end
		status[e] = "end"
	end

	local function GetFlatDistanceFromEntity(e,t)
		local x1,z1 = g_Entity[e].x,g_Entity[e].z
		local x2,z2 = g_Entity[t].x,g_Entity[t].z
		local dx = x2-x1
		local dz = z2-z1
		local ddist = dx*dx+dz*dz
		return ddist
	end

	if g_Entity[e]['activated'] == 1 then
		local PlayerDist = GetPlayerDistance(e)

		if doonce[e] == 0 then
			Prompt(crawler[e].prompt_text)
			PlaySound(e,0)
			doonce[e] = 1
			SetAnimationName(e,crawler[e].idle_animation)
			LoopAnimation(e)
		end
		--- Target Player -------------------------------------------------------------------
		if crawler[e].appearance_mode == 1 then
			GravityOff(e)
			if current_level[e] < crawler[e].appearance_level and fullshow[e] == 0 then
				SetEntityBaseAlpha(e,current_level[e])
				current_level[e] = current_level[e] + crawler[e].appearance_speed
			end
			if current_level[e] >= crawler[e].appearance_level then fullshow[e] = 1 end
			if current_level[e] >= crawler[e].appearance_level then				
				if animonce[e] == 0 then
					SetAnimationName(e,crawler[e].move_animation1)
					LoopAnimation(e)
					StopSound(e,0)
					PlaySound(e,1)
					animonce[e] = 1
				end
				if calconce[e] == 0 then
					destx[e] = g_PlayerPosX
					desty[e] = g_PlayerPosY
					destz[e] = g_PlayerPosZ
					RDFindPath(g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'],destx[e],desty[e],destz[e])
					destAngle[e] = g_PlayerAngY-180
					SetRotation(e,0,destAngle[e],0)
					calconce[e] = 1
				end
				if journey[e] == 0 then
					StartMoveAndRotateToXYZ(e,crawler[e].movement_speed,crawler[e].movement_speed,0,0)
					RDFindPath(g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'],destx[e],desty[e],destz[e])
					CollisionOff(e)
				end
				if crawler[e].animated_model == 1 then SetAnimationSpeedModulation (e,crawler[e].movement_speed/100) end
				if crawler[e].animated_model == 2 then
					if legacy[e] == 0 then
						crawler[e].movement_speed = crawler[e].movement_speed/100
						legacy[e] = 1
					end
				end
				if PlayerDist < 100 then journey[e] = 2 end
			end
		end

		--- Target Object -------------------------------------------------------------------
		if crawler[e].appearance_mode == 2 then
			GravityOff(e)
			if destAngle[e] ~= objdestAngle[e] then
				SetRotationYSlowly(e,objdestAngle[e],20)
			end	
			if current_level[e] < crawler[e].appearance_level and fullshow[e] == 0 then
				SetEntityBaseAlpha(e,current_level[e])
				current_level[e] = current_level[e] + crawler[e].appearance_speed
			end
			if current_level[e] >= crawler[e].appearance_level then fullshow[e] = 1 end
			if current_level[e] >= crawler[e].appearance_level then
				if animonce[e] == 0 then
					SetAnimationName(e,crawler[e].move_animation1)
					LoopAnimation(e)
					StopSound(e,0)
					PlaySound(e,1)
					animonce[e] = 1
				end
				if calconce[e] == 0 then
					destx[e] = objdestx[e]
					desty[e] = objdesty[e]
					destz[e] = objdestz[e]
					RDFindPath(g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'],destx[e],desty[e],destz[e])
					destAngle[e] = objdestAngle[e]
					SetRotation(e,0,destAngle[e],0)					
					calconce[e] = 1
					wait[e] = g_Time + 100
				end
				if journey[e] == 0 and g_Time > wait[e] then
					StartMoveAndRotateToXYZ(e,crawler[e].movement_speed,crawler[e].movement_speed,1,0)
					RDFindPath(g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'],destx[e],desty[e],destz[e])
					CollisionOff(e)
				end
				if crawler[e].animated_model == 1 then SetAnimationSpeedModulation (e,crawler[e].movement_speed/100) end
				if crawler[e].animated_model == 2 then
					if legacy[e] == 0 then
						crawler[e].movement_speed = crawler[e].movement_speed/100
						legacy[e] = 1
					end
				end
				if GetFlatDistanceFromEntity(e,crawler[e].object_no) < crawler[e].object_proximity*2 then CollisionOff(crawler[e].object_no) end				
				if GetFlatDistanceFromEntity(e,crawler[e].object_no) < crawler[e].object_proximity then
					if crawler[e].object_visibility == 1 then CollisionOn(crawler[e].object_no) end
					journey[e] = 2
				end
			end
		end

		--- Target Object then Vertical Climb ---------------------------------------------------
		if crawler[e].appearance_mode == 3 then
			GravityOff(e)
			if destAngle[e] ~= objdestAngle[e] then
				SetRotationYSlowly(e,objdestAngle[e],20)
			end	
			if current_level[e] < crawler[e].appearance_level and fullshow[e] == 0 then
				SetEntityBaseAlpha(e,current_level[e])
				current_level[e] = current_level[e] + crawler[e].appearance_speed
			end
			if current_level[e] >= crawler[e].appearance_level then fullshow[e] = 1 end
			if current_level[e] >= crawler[e].appearance_level then
				if animonce[e] == 0 then
					SetAnimationName(e,crawler[e].move_animation1)
					LoopAnimation(e)
					StopSound(e,0)
					PlaySound(e,1)
					animonce[e] = 1
				end
				if calconce[e] == 0 then
					destx[e] = objdestx[e]
					desty[e] = objdesty[e]
					destz[e] = objdestz[e]
					RDFindPath(g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'],destx[e],desty[e],destz[e])
					destAngle[e] = objdestAngle[e]
					SetRotation(e,0,destAngle[e],0)	
					calconce[e] = 1
					wait[e] = g_Time + 100
				end
				if journey[e] == 0 and g_Time > wait[e] then
					StartMoveAndRotateToXYZ(e,crawler[e].movement_speed,crawler[e].movement_speed,1,0)
					RDFindPath(g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'],destx[e],desty[e],destz[e])
					CollisionOff(e)
				end
				if crawler[e].animated_model == 1 then SetAnimationSpeedModulation (e,crawler[e].movement_speed/100) end
				if crawler[e].animated_model == 2 then
					if legacy[e] == 0 then
						crawler[e].movement_speed = crawler[e].movement_speed/100
						legacy[e] = 1
					end
				end
				if GetFlatDistanceFromEntity(e,crawler[e].object_no) < crawler[e].object_proximity then journey[e] = 1 end			
			end

			if journey[e] == 1 then
				if crawler[e].climb_height > 0 then
					SetCharacterMode(e,0)
					GravityOff(e)
					CollisionOff(e)
					if rotonce[e] == 0 then
						ResetRotation(e,g_Entity[e]['anglex']+90,g_Entity[e]['angley'],g_Entity[e]['anglez'])
						rotonce[e] = 1
					end
					if g_Entity[e]['y'] < crawler[e].climb_height then
						MoveUp(e,crawler[e].movement_speed*100)
					end
					if g_Entity[e]['y'] >= crawler[e].climb_height then journey[e] = 2 end
				end
				if crawler[e].climb_height == 0 then
					if crawler[e].object_visibility == 1 then CollisionOn(crawler[e].object_no) end
					journey[e] = 2
				end
			end
		end

		---Ceiling Jump -------------------------------------------------------------------
		if crawler[e].appearance_mode == 4 then
			GravityOff(e)
			if current_level[e] < crawler[e].appearance_level and fullshow[e] == 0 then
				SetEntityBaseAlpha(e,current_level[e])
				current_level[e] = current_level[e] + crawler[e].appearance_speed
			end
			if current_level[e] >= crawler[e].appearance_level then fullshow[e] = 1 end
			if current_level[e] >= crawler[e].appearance_level then
				if crawler[e].animated_model == 1 then SetAnimationSpeedModulation (e,crawler[e].movement_speed/100) end
				if crawler[e].animated_model == 2 then
					if legacy[e] == 0 then
						crawler[e].movement_speed = crawler[e].movement_speed/100
						legacy[e] = 1
					end
				end
				if crawler[e].climb_height > 0 then
					SetCharacterMode(e,0)
					GravityOff(e)
					CollisionOff(e)
					if rotonce[e] == 0 then
						ResetRotation(e,g_Entity[e]['anglex']+180,g_Entity[e]['angley'],g_Entity[e]['anglez'])
						if animonce[e] == 0 then
							SetAnimationName(e,crawler[e].move_animation1)
							PlayAnimation(e)
							StopAnimation(e)
							StopSound(e,0)
							PlaySound(e,1)
							animonce[e] = 1
						end
						rotonce[e] = 1
					end
					if g_Entity[e]['y'] < crawler[e].climb_height then
						MoveUp(e,crawler[e].movement_speed*100)
					end
					if g_Entity[e]['y'] >= crawler[e].climb_height then journey[e] = 2 end
				end
				if crawler[e].climb_height == 0 then journey[e] = 2 end
			end
		end

		---Ceiling Drop -------------------------------------------------------------------
		if crawler[e].appearance_mode == 5 then
			GravityOff(e)				
			if rotonce[e] == 0 then
				SetCharacterMode(e,0)
				destAngle[e] = objdestAngle[e]
				SetRotation(e,g_Entity[e]['anglex']+180,destAngle[e],g_Entity[e]['anglez'])
				SetPosition(e,g_Entity[e]['x'],crawler[e].climb_height,g_Entity[e]['z'])
				if animonce[e] == 0 then
					SetAnimationName(e,crawler[e].idle_animation)
					PlayAnimation(e)
					StopAnimation(e)
					StopSound(e,0)
					PlaySound(e,1)
					animonce[e] = 1
				end
				rotonce[e] = 1
			end
			if current_level[e] < crawler[e].appearance_level and fullshow[e] == 0 then
				SetEntityBaseAlpha(e,current_level[e])
				current_level[e] = current_level[e] + crawler[e].appearance_speed
			end
			if current_level[e] >= crawler[e].appearance_level then fullshow[e] = 1 end
			if current_level[e] >= crawler[e].appearance_level then
				if crawler[e].animated_model == 1 then SetAnimationSpeedModulation (e,crawler[e].movement_speed/100) end
				if crawler[e].animated_model == 2 then
					if legacy[e] == 0 then
						crawler[e].movement_speed = crawler[e].movement_speed/100
						legacy[e] = 1
					end
				end
				if g_Entity[e]['y'] > 0 then
					ResetPosition(e,g_Entity[e]['x'],g_Entity[e]['y']-crawler[e].movement_speed,g_Entity[e]['z'])
				end
				if g_Entity[e]['y'] <= crawler[e].climb_height-(crawler[e].climb_height-150) then
					if dropmode[e] == 0 then
						SetCharacterMode(e,1)
						GravityOn(e)
						CollisionOff(e)						
						ResetRotation(e,g_Entity[e]['anglex']+180,destAngle[e]-180,g_Entity[e]['anglez'])
						SetAnimationName(e,crawler[e].move_animation1)
						LoopAnimation(e)
						CollisionOn(e)
						dropmode[e] = 1
					end
				end
				if g_Entity[e]['y'] <= crawler[e].climb_height-(crawler[e].climb_height-30) then
					if dropmode[e] == 1 then
						SetAnimationName(e,crawler[e].move_animation2)
						LoopAnimation(e)
						dropmode[e] = 2
					end
					if dropmode[e] == 2 then crawler[e].appearance_mode = 2	end
				end
			end
		end
		-----------------------------------------------------------------------------------

		if fullshow[e] == 1 and journey[e] == 2 then
			CollisionOff(e)
			if crawler[e].disappearance_mode == 1 then
				StopAnimation(e)				
				if current_level[e] > 0 then
					SetEntityBaseAlpha(e,current_level[e])
					current_level[e] = current_level[e]-crawler[e].appearance_speed
				end
				if current_level[e] <= 0 then
					current_level[e] = 0
					Hide(e)
					SetEntityHealth(e,0)
					Destroy(e)					
					SetEntityBaseAlpha(e,100)					
				end
			end
			if crawler[e].disappearance_mode == 2 then
				CollisionOff(e)	
				Hide(e)
				SetEntityHealth(e,0)
				Destroy(e)				
			end
		end

		if g_Entity[e]['health'] < 99 then
			if crawler[e].animated_model == 1 then
				fullshow[e] = 1
				journey[e] = 2
				StopSound(e,0)
				StopSound(e,1)
				PlaySound(e,2)
			end
			if crawler[e].animated_model == 2 then
				SetEntityHealth(e,99)
				fullshow[e] = 1
				journey[e] = 2
				StopSound(e,0)
				StopSound(e,1)
				PlaySound(e,2)
			end
		end
	end	
end