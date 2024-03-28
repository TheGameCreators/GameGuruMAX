--LUA Script - precede every function and global member with lowercase name of script + '_main'
--Platform Script V14  by Necrym59 with thanks to smallg
--DESCRIPTION: Attach to an object. With Physics ON, Gravity OFF, Weight and Friction 0, IsImobile ON
--DESCRIPTION: Change [PROMPT_TEXT$="E to start"]
--DESCRIPTION: The direction 
--DESCRIPTION: [DIRECTION_X!=0]
--DESCRIPTION: [DIRECTION_Y!=0]
--DESCRIPTION: [DIRECTION_Z!=0]
--DESCRIPTION: The distance to travel
--DESCRIPTION: [DISTANCE_X=0(0,3000)]
--DESCRIPTION: [DISTANCE_Y=0(0,3000)]
--DESCRIPTION: [DISTANCE_Z=500(0,3000)]
--DESCRIPTION: The platform speed
--DESCRIPTION: [SPEED_X=0(0,100)]
--DESCRIPTION: [SPEED_Y=0(0,100)]
--DESCRIPTION: [SPEED_Z=100(0,100)]
--DESCRIPTION: Lock any positions or rotation angles
--DESCRIPTION: [LOCK_X_POSITION!=0]
--DESCRIPTION: [LOCK_Y_POSITION!=0]
--DESCRIPTION: [LOCK_Z_POSITION!=0]
--DESCRIPTION: [LOCK_X_ROTATION!=1]
--DESCRIPTION: [LOCK_Y_ROTATION!=1]
--DESCRIPTION: [LOCK_Z_ROTATION!=1]
--DESCRIPTION: [@AUTO_EJECT=1(1=On, 2=Off)]. 
--DESCRIPTION: Change its [@VISIBILITY=1(1=Visible, 2=Invisible)]
--DESCRIPTION: [@RIDING=1(1=Locked, 2=Unlocked)]
--DESCRIPTION: [@MODE=1(1=Manual, 2=Auto)]
--DESCRIPTION: [EXTENDED!=0] to start at extended position
--DESCRIPTION: <Sound0> - for platform starting
--DESCRIPTION: <Sound1> - for platform running
--DESCRIPTION: <Sound2> - for platform stopping

	local platform 			= {}
	local prompt_text 		= {}
	local direction_x 		= {}
	local direction_y 		= {}
	local direction_z 		= {}
	local distance_x 		= {}
	local distance_y 		= {}
	local distance_z 		= {}
	local speed_x 			= {}
	local speed_y 			= {}
	local speed_z 			= {}
	local lock_x_position 	= {}
	local lock_y_position 	= {}
	local lock_z_position 	= {}
	local lock_x_rotation 	= {}
	local lock_y_rotation 	= {}
	local lock_z_rotation 	= {}
	local auto_eject 		= {}
	local visibility 		= {}	
	local riding 			= {}
	local mode 				= {}
	local extended	= {}
	
	local wait_time			= {}
	local direction 		= {}
	local startx 			= {}
	local starty 			= {}
	local startz 			= {}
	local startax 			= {}
	local startay 			= {}
	local startaz 			= {}
	local state 			= {}
	local onplatform		= {}

function platform_properties(e, prompt_text, direction_x, direction_y, direction_z, distance_x, distance_y, distance_z, speed_x, speed_y, speed_z, lock_x_position, lock_y_position, lock_z_position, lock_x_rotation, lock_y_rotation, lock_z_rotation, auto_eject, visibility, riding, mode, extended)
	platform[e] = g_Entity[e]
	platform[e].prompt_text 			= prompt_text
	platform[e].direction_x 			= direction_x
	platform[e].direction_y 			= direction_y
	platform[e].direction_z 			= direction_z
	platform[e].distance_x 				= distance_x
	platform[e].distance_y 				= distance_y
	platform[e].distance_z 				= distance_z
	platform[e].speed_x 				= speed_x
	platform[e].speed_y 				= speed_y
	platform[e].speed_z 				= speed_z
	platform[e].lock_x_position 		= lock_x_position
	platform[e].lock_y_position 		= lock_y_position
	platform[e].lock_z_position 		= lock_z_position
	platform[e].lock_x_rotation 		= lock_x_rotation
	platform[e].lock_y_rotation 		= lock_y_rotation
	platform[e].lock_z_rotation 		= lock_z_rotation
	platform[e].auto_eject 				= auto_eject or 1
	platform[e].visibility 				= visibility or 1
	platform[e].riding 					= riding or 1
	platform[e].mode 					= mode or 1
	platform[e].extended				= extended
end 

function platform_init(e)
	platform[e] = g_Entity[e]
	platform[e].prompt_text 			= ""
	platform[e].direction_x 			= 0
	platform[e].direction_y 			= 0
	platform[e].direction_z 			= 0
	platform[e].distance_x 				= 0
	platform[e].distance_y 				= 0
	platform[e].distance_z 				= 500
	platform[e].speed_x 				= 0
	platform[e].speed_y 				= 0
	platform[e].speed_z 				= 100
	platform[e].lock_x_position 		= 0
	platform[e].lock_y_position 		= 0
	platform[e].lock_z_position 		= 0
	platform[e].lock_x_rotation 		= 1
	platform[e].lock_y_rotation 		= 1
	platform[e].lock_z_rotation 		= 1
	platform[e].auto_eject 				= 1
	platform[e].visibility 				= 1
	platform[e].riding					= 1
	platform[e].mode 					= 1
	platform[e].extended				= 0
	riding[e] = 1
	onplatform[e] = 0
	state[e] = "init"
	direction[e] = {}
	wait_time[e] = 0
	for a = 1, 3 do 
		direction[e][a] = 0
	end
end 

function platform_main(e)
	platform[e] = g_Entity[e]
	
	if g_Entity[e]['activated'] == 1 then
		state[e] = "active"
		SetActivated(e,0)
	end
	
	if state[e] == "init" then 
		if platform[e].direction_x == 0 then 
			platform[e].speed_x = 0
		else
			direction[e][1] = platform[e].speed_x / 100
		end 
		if platform[e].direction_y == 0 then 
			platform[e].speed_y = 0			
		else 
			direction[e][2] = platform[e].speed_y / 100
		end 
		if platform[e].direction_z == 0 then 
			platform[e].speed_z = 0
		else 			
			direction[e][3] = platform[e].speed_z / 100
		end 
		if platform[e].direction_x == 0 and platform[e].direction_y == 0 and platform[e].direction_z == 0 then return end
		startx[e], starty[e], startz[e], startax[e], startay[e], startaz[e] = GetEntityPosAng(e)
		if direction[e][1] < 0 then 
			startx[e] = startx[e] - platform[e].distance_x
		end 
		if direction[e][2] < 0 then 
			starty[e] = starty[e] - platform[e].distance_y
		end 
		if direction[e][3] < 0 then 
			startz[e] = startz[e] - platform[e].distance_z
		end 
		if platform[e].visibility == 1 then Show(e) end
		if platform[e].visibility == 2 then Hide(e) end	
		wait_time[e] = GetTimer(e) + 3000
		if platform[e].extended == 1 and platform[e].mode == 1 then
			platform[e].mode = 2
		else
			platform[e].extended = 0
		end
		state[e] = "idle"
	end
	
	if state[e] == "idle" then
		
		local PlayerDist = GetPlayerDistance(e)		
		if PlayerDist <= 200 then			
			onplatform[e]=IntersectAll(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,g_PlayerPosX,-1,g_PlayerPosZ,0)		
			if onplatform[e] ~= g_Entity[e]['obj'] then onplatform[e] = 0 end
			if g_Entity[e]['obj'] == onplatform[e] then
				riding[e] = 1
			else
				riding[e] = 2
			end
		else
			riding[e] = 2
		end	
		if riding[e] == 1 and platform[e].mode == 1 then Prompt(platform[e].prompt_text) end
		if g_Scancode == 18 and riding[e] == 1 and platform[e].mode == 1 then
			PlaySound(e,0)			
			LoopSound(e,1)
			state[e] = "active"	
		end
		if GetTimer(e) >= wait_time[e] and platform[e].mode == 2 then
			state[e] = "active"
		end		
	end
	
	if state[e] == "active" then
		local x1,y1,z1,ax,ay,az = GetEntityPosAng(e)
		if platform[e].direction_x == 1 then 
			if direction[e][1] > 0 then 
				if x1 < startx[e] + platform[e].distance_x then 
					x1 = x1 + direction[e][1]
					if x1 >= startx[e] + platform[e].distance_x then
						if platform[e].auto_eject == 1 and riding[e] == 1 then ForcePlayer(g_PlayerAngY + 1, 2) end
						StopSound(e,1)
						PlaySound(e,2)
						wait_time[e] = GetTimer(e) + 3000
						state[e] = "idle"
					end
				else 
					direction[e][1] = direction[e][1] * -1
					x1 = startx[e] + platform[e].distance_x
				end 
			elseif direction[e][1] < 0 then 
				if x1 > startx[e] then 
					x1 = x1 + direction[e][1]
				else 
					direction[e][1] = direction[e][1] * -1
					x1 = startx[e]
					if x1 <= startx[e] then
						if platform[e].auto_eject == 1 and riding[e] == 1 then ForcePlayer(g_PlayerAngY + 1, 2) end
						StopSound(e,1)
						PlaySound(e,2)
						wait_time[e] = GetTimer(e) + 3000
						state[e] = "idle"
					end	
				end 
			end
			if platform[e].extended == 1 and x1 >= startx[e] + platform[e].distance_x then
				platform[e].mode = 1
				platform[e].extended = 0
			end
		end 
		if platform[e].direction_y == 1 then 
			if direction[e][2] > 0 then
				if y1 < starty[e] + platform[e].distance_y then 
					y1 = y1 + direction[e][2]
					if y1 >= starty[e] + platform[e].distance_y then
						if platform[e].auto_eject == 1 and riding[e] == 1 then ForcePlayer(g_PlayerAngY + 1, 2) end
						StopSound(e,1)
						PlaySound(e,2)
						wait_time[e] = GetTimer(e) + 3000
						state[e] = "idle"
					end
				else 
					direction[e][2] = direction[e][2] * -1
					y1 = starty[e] + platform[e].distance_y
				end 
			elseif direction[e][2] < 0 then 
				if y1 > starty[e] then 
					y1 = y1 + direction[e][2]
				else 
					direction[e][2] = direction[e][2] * -1
					y1 = starty[e]
					if y1 <= starty[e] then
						if platform[e].auto_eject == 1 and riding[e] == 1 then ForcePlayer(g_PlayerAngY + 1, 2) end
						StopSound(e,1)
						PlaySound(e,2)
						wait_time[e] = GetTimer(e) + 3000
						state[e] = "idle"
					end	
				end 
			end 
			if platform[e].extended == 1 and y1 >= starty[e] + platform[e].distance_y then
				platform[e].mode = 1
				platform[e].extended = 0
			end
		end 
		if platform[e].direction_z == 1 then 
			if direction[e][3] > 0 then 
				if z1 < startz[e] + platform[e].distance_z then 
					z1 = z1 + direction[e][3]
					if z1 >= startz[e] + platform[e].distance_z then
						if platform[e].auto_eject == 1 and riding[e] == 1 then ForcePlayer(g_PlayerAngY + 1, 2) end
						StopSound(e,1)
						PlaySound(e,2)
						wait_time[e] = GetTimer(e) + 3000
						state[e] = "idle"
					end
				else 
					direction[e][3] = direction[e][3] * -1
					z1 = startz[e] + platform[e].distance_z									
				end 
			elseif direction[e][3] < 0 then				
				if z1 > startz[e] then 
					z1 = z1 + direction[e][3]
				else 
					direction[e][3] = direction[e][3] * -1
					z1 = startz[e]
					if z1 <= startz[e] then
						if platform[e].auto_eject == 1 and riding[e] == 1 then ForcePlayer(g_PlayerAngY + 1, 2) end
						StopSound(e,1)
						PlaySound(e,2)
						wait_time[e] = GetTimer(e) + 3000
						state[e] = "idle"
					end					
				end 
			end
			if platform[e].extended == 1 and z1 >= startz[e] + platform[e].distance_z then
				platform[e].mode = 1
				platform[e].extended = 0
			end
		end 
		if platform[e].lock_x_position == 1 then 
			x1 = startx[e]
		end 
		if platform[e].lock_y_position == 1 then 
			y1 = starty[e]
		end 
		if platform[e].lock_z_position == 1 then 
			z1 = startz[e]
		end
		GravityOff(e)
		CollisionOff(e)		
		SetPosition(e, x1, y1, z1)
		ResetPosition(e, x1, y1, z1)		
		if riding[e] == 1 then	
			if platform[e].riding == 1 then
				SetFreezePosition(g_Entity[e]['x'],(g_Entity[e]['y'] + 38), g_Entity[e]['z'])					
				SetPriorityToTransporter(e,1)
				TransportToFreezePositionOnly()
			end	
		end
		CollisionOn(e)
		GravityOn(e)
		if platform[e].lock_x_rotation == 1 then 
			ax = startax[e]
		end 
		if platform[e].lock_y_rotation == 1 then 
			ay = startay[e]
		end 
		if platform[e].lock_z_rotation == 1 then 
			az = startaz[e] 
		end

		if platform[e].mode == 2 then
			GravityOff(e)
			CollisionOff(e)			
			SetRotation(e, ax, ay-180, az)
			ResetRotation(e, ax, ay-180, az)			
			CollisionOn(e)
			GravityOn(e)
		else
			GravityOff(e)
			CollisionOff(e)			
			SetRotation(e, ax, ay, az)
			ResetRotation(e, ax, ay, az)			
			CollisionOn(e)
			GravityOn(e)
		end	
	end
end 

