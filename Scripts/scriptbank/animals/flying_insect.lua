-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Flying Insect v6 by Necrym59
-- All flying insect animations are named 'fly' and 'attack'
-- DESCRIPTION: Applied to insect? Always active ON
-- DESCRIPTION: [@INSECT_TYPE=1(1=Butterfly, 2=Bee, 3=Wasp)]
-- DESCRIPTION: [@TEMPERAMENT=1(1=Benign, 2=Aggressive)]
-- DESCRIPTION: [BOUNDARY=500(1,2000)]
-- DESCRIPTION: <Sound0> Insect sound (use mono audio file)

	local insect = {}
	local insect_type = {}
	local temperament = {}
	local boundary = {}
	local insect_state = {}
	local rotation_movement = {}
	local vertical_movement = {}
	local attack_delay = {}
	local death = {}
	local start_timer1 = {}	
	local timer1 = {}
	local start_timer2 = {}	
	local timer2 = {}
	local random_time = {}
	local start_animation = {}
	local x_position = {}
	local y_position = {}
	local z_position = {}
	local insect_position = {}
	local insect_sound = {}	
	local terrainheight = {}
	local boundary_state = {}
	local doonce = {}
	local status = {}

function flying_insect_properties(e, insect_type, temperament, boundary)
	insect[e] = g_Entity[e]
	insect[e].insect_type = insect_type
	insect[e].temperament = temperament
	insect[e].boundary = boundary
end

function flying_insect_init_name(e)
	insect[e] = g_Entity[e]
	insect[e].insect_type = 1
	insect[e].temperament = 1
	insect[e].boundary = 500
	insect_state[e] = 1
	rotation_movement[e] = 0	
	vertical_movement[e] = 0
	start_timer1[e] = 0
	timer1[e] = 0
	start_timer2[e] = 0
	timer2[e] = 0
	random_time[e] = 0
	start_animation[e] = 0
	x_position[e] = 0
	z_position[e] = 0
	y_position[e] = 0
	insect_position[e] = 0
	insect_sound[e] = 0
	attack_delay[e] = 300
	death[e] = 0
	terrainheight[e] = nil
	boundary_state[e] = "in_bounds"
	doonce[e] = 0
	status[e] = "init"	
end
    
function flying_insect_main(e)
	insect[e] = g_Entity[e]
	if status[e] == "init" then	
		x_position[e] = g_Entity[e]['x']
		y_position[e] = g_Entity[e]['y']
		z_position[e] = g_Entity[e]['z']
		GravityOff(e)
		CollisionOff(e)
		terrainheight[e] = GetTerrainHeight(g_Entity[e]['x'],g_Entity[e]['z'])
		status[e] = "endinit"
	end	
	
	if insect[e].insect_type == 1 then -- Butterfly
		rotation_movement[e] = math.random(-160,200)
		vertical_movement[e] = math.random(-50,50)
		random_time[e] = math.random(1000,3000)
		ModulateSpeed(e,2.0)			
	end
	if insect[e].insect_type == 2 then -- Bee
		rotation_movement[e] = math.random(-170,200)
		vertical_movement[e] = math.random(-20,20)
		random_time[e] = math.random(1000,3000)
		ModulateSpeed(e,1.5)
	end
	if insect[e].insect_type == 3 then -- Wasp
		rotation_movement[e] = math.random(-160,220)
		vertical_movement[e] = math.random(-15,15)
		random_time[e] = math.random(1000,3000)
		ModulateSpeed(e,1.5)
	end
	-- Sound --------------------------------------
	if g_Entity[e]['health'] > 1 then LoopSound(e,0) end
	-- Timers -----------------------------------		
	if InsectDistance(e) > insect[e].boundary then
		boundary_state[e] = "out_of_bounds"
		timer2[e] = 200
	elseif InsectDistance(e) < insect[e].boundary then
		boundary_state[e] = "in_bounds"
	end
	if start_timer1[e] == 0 then
		timer1[e] = GetTimer(e)
		start_timer1[e] = 1
	end	
	if GetTimer(e) > timer1[e] + random_time[e] then
		start_animation[e] = 1
	end
	if g_Entity[e]['animating'] == 0 and start_animation[e] == 1 then
		SetAnimationName(e,"fly")
		PlayAnimation(e)
	g_Entity[e]['animating'] = 1
	end
	-- Flying -------------------------------------		
	if insect_state[e] == 1 and boundary_state[e] ~= "out_of_bounds" then
		MoveForward(e,50)
		MoveUp(e,vertical_movement[e])
		if g_Entity[e]['y'] < (terrainheight[e] + 100) and g_Entity[e]['y'] > (terrainheight[e] + 10) then	
			MoveUp(e,vertical_movement[e])
		elseif g_Entity[e]['y'] > (terrainheight[e] + 50) then
			MoveUp(e,-vertical_movement[e])
		elseif g_Entity[e]['y'] < (terrainheight[e] + 10) then
			MoveUp(e,20)
		end
		RotateY(e,rotation_movement[e])
	end	
	-- Attacking -------------------------------------	
	if insect_state[e] == 2 then
		RotateToPlayer(e)
		if g_Entity[e]['y'] > g_PlayerPosY then
			CollisionOff(e)
			MoveUp(e,-20) 
			CollisionOn(e)
		elseif g_Entity[e]['y'] < (terrainheight[e]+50) then
			CollisionOff(e)
			MoveUp(e,40)
			CollisionOn(e)
		end	
		RotateToPlayer(e)
		if GetPlayerDistance(e) > 10 then 
			CollisionOff(e)
			MoveForward(e,100)
			CollisionOn(e)
		end				
		if GetTimer(e) > attack_delay[e] then
			if doonce[e] == 0 then
				SetAnimationName(e,"attack")
				LoopAnimation(e)
				doonce[e] = 1
			end
			HurtPlayer(e,1)
			attack_delay[e] = GetTimer(e) + 250
		end
	end	
	-- Boundary -------------------------------------	
	if boundary_state[e] == "out_of_bounds" then
		if start_timer2[e] == 0 then	
			StartTimer(e)
			insect_position[e] = InsectDistance(e)
			start_timer2[e] = 1
		end
		if GetTimer(e) > timer2[e] then 
			if InsectDistance(e) > insect_position[e] then
				RotateY(e,250)
			end
			start_timer2[e] = 0
		end
		MoveForward(e,30)
		MoveUp(e,vertical_movement[e])
	end
	-- Temperament Check -----------------------------
	if GetPlayerDistance(e) > 80 and insect[e].temperament == 1 then insect_state[e] = 1 end
	if GetPlayerDistance(e) < 80 and insect[e].temperament == 2 then insect_state[e] = 2 end
	if GetPlayerDistance(e) > 50 and insect[e].temperament == 2 then				
		doonce[e] = 0
		if insect_state[e] == 2 and doonce[e] == 0 then
			SetAnimationName(e,"fly")
			LoopAnimation(e)
			doonce[e] = 1
		end
		doonce[e] = 0
		insect_state[e] = 1
		status[e] = "init"
	end	
	if g_Entity[e]['health'] < 2 then 
		doonce[e] = 0
		if insect_state[e] == 2 and doonce[e] == 0 then
			SetAnimationName(e,"fly")
			LoopAnimation(e)
			doonce[e] = 1
		end
		doonce[e] = 0		
		insect_state[e] = 1		
		status[e] = "init"
	end
	-- Dying -------------------------------------	
	if g_Entity[e]['health'] <= 1 then
		insect_state[e] = 0		
		StopAnimation(e)
		GravityOn(e)
		StopSound(e,0)
		g_Entity[e]['health'] = 0
		if g_Entity[e]['y'] > terrainheight[e] then
			MoveUp(e,-18)
		end
		status[e] = "init"
	end
end
	   
function InsectDistance(e)
	insectDX = (g_Entity[e]['x'] - x_position[e])
	insectDY = (g_Entity[e]['y'] - y_position[e])
	insectDZ = (g_Entity[e]['z'] - z_position[e])
	if math.abs(insectDY) > 100 then insectDY = insectDY * 4 end
	return math.sqrt(math.abs(insectDX*insectDX)+math.abs(insectDY*insectDY)+math.abs(insectDZ*insectDZ));
end   


