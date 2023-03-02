-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Boat v9 by Necrym59
-- DESCRIPTION: Creates a rideable boat object behavior: Set Physics=ON, Gravity=OFF, IsImobile=YES.
-- DESCRIPTION: Edit the[PROMPT_TEXT$="E to embark"], Activation [BOAT_RANGE=80] the [MIN_SPEED=2(0,10)], [MAX_SPEED=4(0,100)], [TURN_SPEED=2(1,10)], [DRAG=5]. Adjust [#PLAYER_Z_POSITION=0(-50,50)] and [#PLAYER_Y_POSITION=20(-50,50)]. Adjust [BOAT_ROTATION=0(0,180)], [BOAT_DRAFT=30], [BOAT_POWER=5(0,50)], [BOAT_BRAKE=5(0,50)]. <Sound0> - Entry/Exit <Sound1> - Moving Loop <Sound2> - Run Aground

	g_boat = {}
	local prompt_text = {}
	local boat_range = {}
	local min_speed = {}
	local max_speed = {}
	local turn_speed = {}
	local drag = {}
	local player_z_position = {}
	local player_y_position = {}
	local boat_rotation = {}
	local boat_draft = {}
	local boat_power = {}
	local boat_brake = {}
	local water_height = {}
	local playerdistance = {}
	local boat_active = {}
	local boat_release = {}
	local boat_aground = {}
	local onboatcheck= {}

function boat_properties(e, prompt_text, boat_range, min_speed, max_speed, turn_speed, drag, player_z_position, player_y_position, boat_rotation, boat_draft, boat_power, boat_brake)
	g_boat[e] = g_Entity[e]
	g_boat[e]['prompt_text'] = prompt_text
	g_boat[e]['boat_range'] = boat_range
	g_boat[e]['min_speed'] = min_speed
	g_boat[e]['max_speed'] = max_speed
	g_boat[e]['turn_speed'] = turn_speed
	g_boat[e]['drag'] = drag
	g_boat[e]['player_z_position'] = player_z_position
	g_boat[e]['player_y_position'] = player_y_position
	g_boat[e]['boat_rotation'] = boat_rotation
	g_boat[e]['boat_draft'] = boat_draft
	g_boat[e]['boat_power'] = boat_power
	g_boat[e]['boat_brake'] = boat_brake
	g_boat[e]['water_height'] = water_height
end -- End properties

function boat_init(e)
	g_boat[e] = g_Entity[e]
	g_boat[e]['prompt_text'] = "E to embark"
	g_boat[e]['boat_range'] = 80
	g_boat[e]['min_speed'] = 2
	g_boat[e]['max_speed'] = 4
	g_boat[e]['turn_speed'] = 2
	g_boat[e]['drag'] = 5
	g_boat[e]['player_z_position'] = 0
	g_boat[e]['player_y_position'] = 10
	g_boat[e]['boat_rotation'] = 0
	g_boat[e]['boat_draft'] = 30
	g_boat[e]['boat_power'] = 5
	g_boat[e]['boat_brake'] = 5
	boat_active[e]=0
	boat_release[e]=0
	boat_aground =0
	speed=0
	boatx=g_Entity[e]['x']
	boaty=g_Entity[e]['y']
	boatz=g_Entity[e]['z']
	turn=0
	onboatcheck[e]=0
end

function boat_main(e)
	g_boat[e] = g_Entity[e]
	if boat_release[e]==nil then boat_release[e]=0 end
	if boat_active[e]==nil then boat_active[e]=0 end

	boatangle=g_Entity[e]['angley']
	PlayerDX = g_Entity[e]['x'] - g_PlayerPosX;
	PlayerDZ = g_Entity[e]['z'] - g_PlayerPosZ;
	PlayerDist = math.sqrt(math.abs(PlayerDX*PlayerDX)+math.abs(PlayerDZ*PlayerDZ));
	playerdistance[e]=PlayerDist
	onboatcheck[e]=IntersectAll(g_PlayerPosX,0,g_PlayerPosZ+35,g_PlayerPosX,g_Entity[e]['y']+36,g_PlayerPosZ,0)
	if onboatcheck[e] == g_Entity[e]['obj'] or playerdistance[e] < g_boat[e]['boat_range'] and boat_active[e]==0 then
		Prompt(g_boat[e]['prompt_text'])
		if g_KeyPressE==1 and boat_release[e]==0 and boat_active[e]==0 then
			boat_active[e]=1
			boat_release[e]=1
			PlaySound(e,0)
			PromptDuration('Q to dismount',4000)
		end
	end

	if boat_active[e]==0 then
		GravityOff(e)
		CollisionOff(e)
		SetPosition(e,g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'])
		ResetPosition(e,g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'])
		GravityOn(e)
		CollisionOn(e)
		StopSound(e,1)
	end

	if boat_active[e]==1 then
		boatx=g_Entity[e]['x']
		boaty=g_Entity[e]['y']
		boatz=g_Entity[e]['z']
		SetFreezePosition(freezex,g_Entity[e]['y']+5,freezez)
		TriggerWaterRipple(g_Entity[e]['x'],g_Entity[e]['y']-5,g_Entity[e]['z'])
		-- Boat speed control
		if g_KeyPressW == 1 then  -- W key
			if speed<g_boat[e]['max_speed'] then
				speed=speed+(g_boat[e]['boat_power']/100)
				LoopSound(e,1)
			end
		end
		if g_KeyPressS == 1 then -- S key
			if speed>g_boat[e]['min_speed'] then
				speed=speed-(g_boat[e]['boat_brake']/100)
				LoopSound(e,1)
			end
			if boat_aground == 1 then
				speed=speed-0.5
				boat_aground = 0
				StopSound(e,1)
				PlaySound(e,2)
			end
		end
		-- Boat steering control
		if g_KeyPressA == 1 then -- A key
			boatangle=boatangle-(g_boat[e]['turn_speed']/10)
			turn=1
		end
		if g_KeyPressD == 1 then -- D key
			boatangle=boatangle+(g_boat[e]['turn_speed']/10)
			turn=1
		end
		if turn==1 then
			turn=0
			if g_PlayerAngX>0 and g_PlayerAngX<100 then
			SetFreezeAngle(g_PlayerAngX,boatangle+g_boat[e]['boat_rotation'],g_PlayerAngZ)
			TransportToFreezePosition()
			end
		end
		if g_KeyPressQ == 1 then -- Q key
			SetFreezePosition(freezex,(g_Entity[e]['y'] + 40),freezez)
			TransportToFreezePosition()
			boat_release[e]=0
			boat_active[e]=0
		end
		--Draft Collision Check and deactivate boat
		if  boat_active[e]==1 then
			local heightTerrain = GetTerrainHeight(g_Entity[e]['x'],g_Entity[e]['z'])
			if (g_Entity[e]['y'] - g_boat[e]['boat_draft']) <= heightTerrain then
				SetFreezePosition(freezex,(g_Entity[e]['y'] + 40),freezez)
				TransportToFreezePosition()
				boat_aground = 1
				speed = -1
				boat_release[e]=0
				boat_active[e]=0
			end
		end
		boatangledeg=math.rad(boatangle+g_boat[e]['boat_rotation'])
		boatangleleftdeg=math.rad(boatangle)
		boatanglerightdeg=math.rad(boatangle)
		-- Boat collsion
		boatx=boatx+(math.sin(boatangledeg)*speed)
		boaty=g_Entity[e]['y']
		boatz=boatz+(math.cos(boatangledeg)*speed)
		-- Update location
		if boat_active[e]==1 then
			freezex=g_Entity[e]['x']+(math.sin(boatangledeg)*(g_boat[e]['player_z_position']-speed)*-1)
			freezez=g_Entity[e]['z']+(math.cos(boatangledeg)*(g_boat[e]['player_z_position']-speed)*-1)
			SetFreezePosition(freezex,(g_Entity[e]['y'] + g_boat[e]['player_y_position']),freezez)
		end
		GravityOff(e)
		CollisionOff(e)
		SetPosition(e,boatx,boaty,boatz)
		SetRotation(e,g_Entity[e]['anglex'],boatangle,g_Entity[e]['anglez'])
		GravityOn(e)
		CollisionOn(e)
		TransportToFreezePositionOnly()
		if speed>0 and g_KeyPressW == 0 then speed=speed-(g_boat[e]['drag']/10000) end
		if speed<0 and g_KeyPressS == 0 then speed=speed/1.1 end
	end
end
