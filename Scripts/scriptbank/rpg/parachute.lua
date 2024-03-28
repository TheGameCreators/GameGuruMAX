-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Parachute v7 by Necrym59 (from an idea created by cybernescence)

-- DESCRIPTION: Attached to the parachute backpack object. Physics Off, Collision = None, Gravity Off, Weight and Friction 0ff.
-- DESCRIPTION: [PICKUP_TEXT$="E to collect parachute"]
-- DESCRIPTION: [DEPLOY_TEXT$="Q to deploy parachute"]
-- DESCRIPTION: [FLIGHT_TEXT$="E to dismount parachute"]
-- DESCRIPTION: [#MOUNT_Z_ADJUSTMENT=0(-30,30)]
-- DESCRIPTION: [#MOUNT_Y_ADJUSTMENT=18(-30,30)]
-- DESCRIPTION: [MIN_DEPLOY_HEIGHT=1000(0,2000)]
-- DESCRIPTION: [DROP_SPEED=5(1,20)]
-- DESCRIPTION: [MAX_SPEED=3(1,20)]
-- DESCRIPTION: [MIN_SPEED=1(1,5)]
-- DESCRIPTION: [ACCELERATION=3(1,5)]
-- DESCRIPTION: [DECELERATION=8(0,50)]
-- DESCRIPTION: [TURN_SPEED=18(0,100)]
-- DESCRIPTION: [PARACHUTE_HEALTH=300(1,1000)]
-- DESCRIPTION: [PARACHUTE_NAME$="parachute"]
-- DESCRIPTION: [@HEIGHT_DISPLAY=1(1=Off, 2=On)]
-- DESCRIPTION: [@TEST_DEPLOY=1(1=Off, 2=On)]
-- DESCRIPTION: [@MODE=1(1=Use Once, 2=Re-useable)]
-- DESCRIPTION: <Sound0> Deployment Sound
-- DESCRIPTION: <Sound1> Flying Sound
-- DESCRIPTION: <Sound2> Landing Sound

	local U = require "scriptbank\\utillib"
	local lower = string.lower

	local parachute = {}
	local pickup_text = {}
	local deploy_text = {}	
	local flight_text = {}
	local mount_z_adjustment = {}
	local mount_z_adjustment = {}
	local min_deploy_height = {}	
	local drop_speed = {}
	local max_speed = {}
	local min_speed = {}
	local turn_speed = {}
	local acceleration = {}
	local deceleration = {}
	local parachute_health = {}
	local parachute_name = {}
	local parachute_object = {}
	local height_display = {}
	local test_deploy = {}
	local mode = {}
	local mindeploy = {}
	local landonobject = {}
	local old_health = nil 
	local vpressed = 0 
	local pos_x = nil 
	local pos_z = nil 
	local pos_y = nil
	local state = {}
	local speed = {}
	local have_parachute = {}
	local chute_no = {}
		
function parachute_properties(e, pickup_text, deploy_text, flight_text, mount_z_adjustment, mount_y_adjustment, min_deploy_height, drop_speed, max_speed, min_speed, acceleration, deceleration, turn_speed, parachute_health, parachute_name, height_display, test_deploy, mode)
	parachute[e] = g_Entity[e]	
	parachute[e].pickup_text		= pickup_text
	parachute[e].deploy_text		= deploy_text	
	parachute[e].flight_text		= flight_text	
	parachute[e].mount_z_adjustment	= mount_z_adjustment	--how far back/forward in the parachute the player will be (not yet practically functional for parachute model)
	parachute[e].mount_y_adjustment	= mount_y_adjustment	--how far up/down in the parachute the player will be
	parachute[e].min_deploy_height	= min_deploy_height		--minimum height to deploy the parachute	
	parachute[e].drop_speed			= drop_speed			--top speed for the parachute 
	parachute[e].max_speed			= max_speed				--top speed for the parachute 
	parachute[e].min_speed			= min_speed				--top reversing speed			
	parachute[e].acceleration		= acceleration			--how quickly the parachute gains speed (in reverse too)
	parachute[e].deceleration		= deceleration			--how quickly the parachute slows down if player not pressing W or S
	parachute[e].turn_speed			= turn_speed			--how quickly the parachute turns 	
	parachute[e].parachute_health	= parachute_health		--how much health to give parachute (adjusts player health while in the parachute)
	parachute[e].parachute_name		= lower(parachute_name) --parachute name
	parachute[e].parachute_object	= 0						--parachute (entity no)
	parachute[e].height_display		= height_display		--option to display current height
	parachute[e].test_deploy		= test_deploy			--option to test from any level the parachute
	parachute[e].mode				= mode					--option to re-use the parachute
	parachute[e].mindeploy			= min_deploy_height		--minimum height check
end

function parachute_init(e)
	parachute[e] = g_Entity[e]
	parachute[e].pickup_text		= "E to collect parachute"
	parachute[e].deploy_text		= "Q to deploy parachute"
	parachute[e].flight_text		= "E to dismount parachute"
	parachute[e].mount_z_adjustment = 0
	parachute[e].mount_y_adjustment	= 18
	parachute[e].min_deploy_height 	= 1000	
	parachute[e].drop_speed			= 5
	parachute[e].max_speed 			= 3 
	parachute[e].min_speed 			= 1
	parachute[e].acceleration 		= 3
	parachute[e].deceleration 		= 8
	parachute[e].turn_speed 		= 18	
	parachute[e].parachute_health	= 500
	parachute[e].parachute_name		= "parachute"
	parachute[e].parachute_object	= 0
	parachute[e].height_display		= 1
	parachute[e].test_deploy		= 1
	parachute[e].mode				= 1
	parachute[e].mindeploy			= 0
	speed[e] 						= 0 
	state[e] 						= "init"
	have_parachute[e] = 0
	landonobject[e] = 0	
end 

function parachute_main(e)
	parachute[e] = g_Entity[e]
	
	if state[e] == "init" then		
		if parachute[e].parachute_object == 0 or nil then
			for a = 1, g_EntityElementMax do			
				if a ~= nil and g_Entity[a] ~= nil then										
					if lower(GetEntityName(a)) == parachute[e].parachute_name then
						parachute[e].parachute_object = a						
						chute_no[e] = a
						Hide(chute_no[e])
						break
					end					
				end
			end
		end
		parachute[e].min_deploy_height = parachute[e].mindeploy + GetTerrainHeight(g_PlayerPosX,g_PlayerPosZ)		
		vpressed = 0
		GravityOff(e)
		state[e] = "collect"
	end
	
	if state[e] == "collect" then 
		if have_parachute[e] == 0 then
			PlayerDist = GetPlayerDistance(e)
			if PlayerDist < 80 and g_PlayerHealth > 0 then
				Prompt(parachute[e].pickup_text)
				if GetInKey() == "e" or GetInKey() == "E" then
					have_parachute[e] = 1
					Hide(e)
					ActivateIfUsed(e)
					PromptDuration(parachute[e].deploy_text,3000)
					state[e] = "deploy"
				end
			end
		end
	end	
	
	if state[e] == "deploy" then
		if parachute[e].height_display == 2 then
			TextCenterOnX(50,94,3,"Current Height: " ..math.ceil(g_PlayerPosY))		
		end
		
		ResetPosition(e,g_PlayerPosX,g_PlayerPosY+100,g_PlayerPosZ)
		ResetRotation(e,g_PlayerAngx,g_PlayerAngY,g_PlayerAngZ)		
		CollisionOn(e)	
		if parachute[e].test_deploy == 1 then
			if g_PlayerPosY <= parachute[e].min_deploy_height then
				if g_KeyPressQ == 1 and vpressed == 0 then
					Prompt("Too low to deploy")
				end
			end
			if g_PlayerPosY >= parachute[e].min_deploy_height then
				if g_KeyPressQ == 1 and vpressed == 0 then
					PlaySound(e,0)
					parachute[e].min_deploy_height = g_PlayerPosY
					vpressed = 1 
					old_health = g_PlayerHealth
					SetPlayerHealth(parachute[e].parachute_health)
					new_y = math.rad(g_Entity[e]['angley'])
					pos_x = g_Entity[e]['x'] + (math.sin(new_y) * parachute[e].mount_z_adjustment)
					pos_z = g_Entity[e]['z'] + (math.cos(new_y) * parachute[e].mount_z_adjustment)				
					SetFreezePosition(pos_x,g_Entity[e]['y']+parachute[e].min_deploy_height,pos_z)
					SetFreezeAngle(g_Entity[e]['anglex'],g_Entity[e]['angley'],g_Entity[e]['anglez'])			
					SetPriorityToTransporter(e,1)
					TransportToFreezePosition()				
					state[e] = "parachuting"				
				end
			end
		end
		if parachute[e].test_deploy == 2 then
			parachute[e].min_deploy_height = parachute[e].min_deploy_height
			if g_KeyPressQ == 1 and vpressed == 0 then 
				vpressed = 1 
				old_health = g_PlayerHealth
				SetPlayerHealth(parachute[e].parachute_health)
				new_y = math.rad(g_Entity[e]['angley'])
				pos_x = g_Entity[e]['x'] + (math.sin(new_y) * parachute[e].mount_z_adjustment)
				pos_z = g_Entity[e]['z'] + (math.cos(new_y) * parachute[e].mount_z_adjustment)
				SetFreezePosition(pos_x,g_Entity[e]['y']+parachute[e].min_deploy_height,pos_z)
				SetFreezeAngle(g_Entity[e]['anglex'],g_Entity[e]['angley'],g_Entity[e]['anglez'])
				SetPriorityToTransporter(e,1)
				TransportToFreezePosition()
				state[e] = "parachuting"		 
			end
		end
	end
	
	if state[e] == "parachuting" then
		new_y = math.rad(g_Entity[e]['angley'])
		pos_x = g_Entity[e]['x'] + (math.sin(new_y) * parachute[e].mount_z_adjustment)
		pos_z = g_Entity[e]['z'] + (math.cos(new_y) * parachute[e].mount_z_adjustment)
		pos_y = GetTerrainHeight(pos_x,pos_z)
		if parachute[e].min_deploy_height >= 35 then
			parachute[e].min_deploy_height = parachute[e].min_deploy_height - (parachute[e].drop_speed/10)
		end	
		if chute_no[e] > 0 then
			Show(chute_no[e])
			ResetPosition(chute_no[e],g_PlayerPosX,(g_Entity[e]['y']+parachute[e].min_deploy_height)+parachute[e].mount_y_adjustment,g_PlayerPosZ)					
			SetRotation(chute_no[e],0,g_Entity[e]['angley'],0)
		end
		SetFreezePosition(pos_x,(g_Entity[e]['y']+parachute[e].min_deploy_height),pos_z)
		SetFreezeAngle(g_PlayerAngx,g_PlayerAngY,g_PlayerAngZ)
		SetPriorityToTransporter(e,1)
		TransportToFreezePositionOnly()
		
		landonobject[e]=IntersectAll(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,g_PlayerPosX,g_PlayerPosY-10,g_PlayerPosZ,0)
		if landonobject[e] > 0 then state[e] = "landed" end
		
		if g_KeyPressW == 1 then
			LoopSound(e,1)				
			if speed[e] < parachute[e].max_speed then 
				speed[e] = speed[e] + parachute[e].acceleration 
			else 
				speed[e] = parachute[e].max_speed
			end 
		elseif g_KeyPressS == 1 then 
			if speed[e] > parachute[e].min_speed-10 then 
				speed[e] = speed[e] - parachute[e].acceleration
			else 
				speed[e] = parachute[e].min_speed-10 
			end 
		else 
			if speed[e] > 0 then 
				speed[e] = speed[e] - parachute[e].deceleration /10
			elseif speed[e] < 0 then 
				speed[e] = speed[e] + parachute[e].deceleration /10				
			end
			StopSound(e,1)
		end 
		CollisionOff(e)
		new_y = math.rad(g_Entity[e]['angley'])
		pos_x = g_Entity[e]['x'] + (math.sin(new_y) * (speed[e]/10))
		pos_z = g_Entity[e]['z'] + (math.cos(new_y) * (speed[e]/10))
		pos_y = GetTerrainHeight(pos_x,pos_z)		
		ResetPosition(e,pos_x,pos_y,pos_z)		
		if g_KeyPressA == 1 then
			SetRotation(e,g_Entity[e]['anglex'],g_Entity[e]['angley']-parachute[e].turn_speed/100,g_Entity[e]['anglez'])
		elseif g_KeyPressD == 1 then 
			SetRotation(e,g_Entity[e]['anglex'],g_Entity[e]['angley']+parachute[e].turn_speed/100,g_Entity[e]['anglez'])
		end 
		if speed[e] > 0 - (parachute[e].max_speed/10) and speed[e] < 0 + (parachute[e].max_speed/10) then
			Prompt(parachute[e].flight_text)
		end
		if g_PlayerPosY < GetTerrainHeight(g_PlayerPosX,g_PlayerPosZ)+80 then
			state[e] = "landed"
		end
		if g_KeyPressE == 1 and vpressed == 0 then 			
			vpressed = 1 
			speed[e] = 0
			StopSound(e,1)			
			parachute[e].parachute_health = g_PlayerHealth			
			CollisionOn(e)
			SetPlayerHealth(old_health)
			if g_PlayerPosY > GetTerrainHeight(g_PlayerPosX,g_PlayerPosZ)+1000 then
				state[e] = "deathfall"
			else
				state[e] = "landed"
			end
		end 		
		if g_PlayerHealth < 1 then 
			SetEntityHealth(e,0)
		end 

		if g_KeyPressE == 0 then 
			vpressed = 0 
		end
		--DeathFall-------------------------------------------------
		if state[e] == "deathfall" then
			HurtPlayer(-1,500)
			have_parachute[e] = 0
			Destroy(e)
			Destroy(chute_no[e])
		end
		--Landed-----------------------------------------------------
		if state[e] == "landed" then
			if parachute[e].mode == 1 then --On time use
				PlaySound(e,2)
				have_parachute[e] = 0
				Destroy(e)
				Destroy(chute_no[e])
			end
			if parachute[e].mode == 2 then--Re useable				
				PlaySound(e,2)
				have_parachute[e] = 0
				Hide(chute_no[e])
				if landonobject[e] > 0 then							
					ResetPosition(e,PlayerPosX,g_PlayerPosY,g_PlayerPosZ+3)					
				else					
					GravityOn(e)
					ResetPosition(e,PlayerPosX,g_PlayerPosY,g_PlayerPosZ+3)
					GravityOff(e)
				end
				Show(e)					
				state[e] = "init"
			end					
		end		
	end	
end	

function parachute_exit(e)
end 
