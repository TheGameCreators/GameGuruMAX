-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Speeder v5 by Necrym59 and smallg
-- DESCRIPTION: Will create a speeder vehicle object. Set IsImmobile ON.
-- DESCRIPTION: [PROMPT_TEXT$="E to mount speeder"]
-- DESCRIPTION: [ENTER_RANGE=150]
-- DESCRIPTION: [ENTER_ANGLE=35]
-- DESCRIPTION: [PLAYER_XZ_ADJUSTMENT=-8(-100,100)]
-- DESCRIPTION: [PLAYER_Y_ADJUSTMENT=40(0,100)]
-- DESCRIPTION: [PLAYER_ANGLE_ADJUSTMENT=0(0,360)]
-- DESCRIPTION: [MAXIMUM_SPEED=40(1,100)]
-- DESCRIPTION: [MINIMUM_SPEED=-40(-100,100)]
-- DESCRIPTION: [TURN_SPEED=1(1,100)]
-- DESCRIPTION: [#ACCELERATION=3.0(0.0,10.0)]
-- DESCRIPTION: [#DECCELERATION=0.15(0.0,10.0)]
-- DESCRIPTION: [IMPACT_RANGE=150(1,500)]
-- DESCRIPTION: [IMPACT_ANGLE=40(1,360)]
-- DESCRIPTION: [SPEEDER_HEALTH=1000(100,1000)]
-- DESCRIPTION: [MAXIMUM_SLOPE=19(1,100)]
-- DESCRIPTION: <Sound0> Moving
-- DESCRIPTION: <Sound1> Idle Loop
-- DESCRIPTION: <Sound2> Impact

local speeder = {}
local prompt_text = {}
local enter_range = {}
local enter_angle = {}
local player_xz_adjustment = {}
local player_y_adjustment = {}
local player_angle_adjustment = {}
local maximum_speed = {}
local minimum_speed = {}
local turn_speed = {}
local acceleration = {}
local decceleration = {}
local impact_range = {}
local impact_angle = {}
local speeder_health = {}
local maximum_slope = {}

local old_health = {}
local pos_x = {}
local pos_z = {} 
local pos_y = {}
local plrpos_x = {}
local plrpos_y = {}
local plrpos_z = {}
local status = {}
local speed = {}
local shealth = {}
local vpressed = {}
local heightdiff = {}
local heightstop = {}
local oldy = {}

function speeder_properties(e, prompt_text, enter_range, enter_angle, player_xz_adjustment, player_y_adjustment, player_angle_adjustment, maximum_speed, minimum_speed, turn_speed, acceleration, decceleration, impact_range, impact_angle, speeder_health, maximum_slope)
	speeder[e].prompt_text = prompt_text
	speeder[e].enter_range = enter_range
	speeder[e].enter_angle = enter_angle
	speeder[e].player_xz_adjustment = player_xz_adjustment
	speeder[e].player_y_adjustment = player_y_adjustment
	speeder[e].player_angle_adjustment = player_angle_adjustment
	speeder[e].maximum_speed = maximum_speed
	speeder[e].minimum_speed = minimum_speed
	speeder[e].turn_speed = turn_speed
	speeder[e].acceleration = acceleration
	speeder[e].decceleration = decceleration	
	speeder[e].impact_range = impact_range
	speeder[e].impact_angle = impact_angle	
	speeder[e].speeder_health = speeder_health
	speeder[e].maximum_slope = maximum_slope
end

function speeder_init(e)
	speeder[e] = {}
	speeder[e].enter_range = 150
	speeder[e].enter_angle = 35
	speeder[e].player_xz_adjustment = -8
	speeder[e].player_y_adjustment = 40
	speeder[e].player_angle_adjustment = 0
	speeder[e].maximum_speed = 150
	speeder[e].minimum_speed = -5
	speeder[e].turn_speed = 1
	speeder[e].acceleration = 3 
	speeder[e].decceleration = 0.15
	speeder[e].impact_range = 150
	speeder[e].impact_angle = 40
	speeder[e].speeder_health = 1000
	speeder[e].maximum_slope = 19

	status[e] = "init"
	speed[e] = 0
	shealth[e] = 0
	old_health[e] = 0 
	pos_x[e] = 0 
	pos_z[e] = 0
	pos_y[e] = 0
	plrpos_x[e] = 0
	plrpos_y[e] = 0
	plrpos_z[e] = 0
	vpressed[e] = 0
	heightdiff[e] = 1
	oldy[e] = 0 
end 

function speeder_main(e)
	if status[e] == "init" then
		shealth[e] = speeder[e].speeder_health
		status[e] = "wait"
	end
	
	if status[e] == "wait" then 
		StopSound(e,0)
		StopSound(e,1)
		StopSound(e,2)
		CollisionOn(e)
		GetInspeeder(e)
	end
	if status[e] == "drive" then 
		UpdatePosition(e)
		UpdatePlayerPosition(e)
		Controlspeeder(e)				
		if speed[e] > 0 - (speeder[e].maximum_speed/10) and speed[e] < 0 + (speeder[e].maximum_speed/10) then 
			Prompt("E to exit speeder")
		else 
			CheckForHittingNPC(e)
			CheckForHittingTerrainOrSlopes(e)
		end
		GetOutspeeder(e)
		if g_PlayerHealth < 1 then 
			SetEntityHealth(e,0)
		end
		CheckForHittingTerrainOrSlopes(e)
	end
	if g_KeyPressE == 0 then 
		vpressed[e] = 0 
	end
end

function speeder_exit(e)
end 

function GetInspeeder(e)
	if PlayerLooking(e,speeder[e].enter_range,speeder[e].enter_angle) == 1 then
		if shealth[e] <= 0 then	PromptLocal(e," Vehicle inoperative") end
		if shealth[e] > 0 then PromptLocal(e,speeder[e].prompt_text) end
		if g_KeyPressE == 1 and vpressed[e] == 0 and shealth[e] > 0 then 
			vpressed[e] = 1 
			old_health[e] = g_PlayerHealth
			SetEntityHealth(e,shealth[e])
			new_y = math.rad(g_Entity[e]['angley'])
			pos_x[e] = g_Entity[e]['x'] + (math.sin(new_y) * speeder[e].player_xz_adjustment)
			pos_z[e] = g_Entity[e]['z'] + (math.cos(new_y) * speeder[e].player_xz_adjustment)
			SetFreezePosition(pos_x[e],g_Entity[e]['y']+speeder[e].player_y_adjustment,pos_z[e])				
			SetFreezeAngle(g_Entity[e]['anglex'],g_Entity[e]['angley']+speeder[e].player_angle_adjustment,g_Entity[e]['anglez'])
			TransportToFreezePosition()
			oldy[e] = g_Entity[e]['y']+speeder[e].player_y_adjustment			
			ChangePlayerWeaponID(0)
			LoopSound(e,1) -- idle sound
			StopSound(e,0)
			status[e] = "drive"
		end 
	end 
end 

function UpdatePosition(e)
	CollisionOff(e)
	new_y = math.rad(g_Entity[e]['angley'])
	pos_x[e] = g_Entity[e]['x'] + (math.sin(new_y) * (speed[e]/10))
	pos_z[e] = g_Entity[e]['z'] + (math.cos(new_y) * (speed[e]/10))
	pos_y[e] = GetTerrainHeight(pos_x[e],pos_z[e])	
	ResetPosition(e,pos_x[e],pos_y[e],pos_z[e])
end

function UpdatePlayerPosition(e)
	new_y = math.rad(g_Entity[e]['angley'])
	plrpos_x[e] = g_Entity[e]['x'] + (math.sin(new_y) * speeder[e].player_xz_adjustment)
	plrpos_z[e] = g_Entity[e]['z'] + (math.cos(new_y) * speeder[e].player_xz_adjustment)
	plrpos_y[e] = g_Entity[e]['y'] + speeder[e].player_y_adjustment
	SetCameraPosition(0,plrpos_x[e],plrpos_y[e]+ speeder[e].player_y_adjustment,plrpos_z[e])
	SetFreezePosition(plrpos_x[e],plrpos_y[e],plrpos_z[e])
	TransportToFreezePositionOnly()
end 

function Controlspeeder(e)
	if shealth[e] > 0 then
		if g_KeyPressW == 1 then 
			LoopSound(e,0) -- drive sound
			StopSound(e,1) 
			if speed[e] < speeder[e].maximum_speed then 
				speed[e] = speed[e] + speeder[e].acceleration
			else 
				speed[e] = speeder[e].maximum_speed 
				LoopSound(e,0)
				StopSound(e,1)
			end 
		elseif g_KeyPressS == 1 then 
			if speed[e] > speeder[e].minimum_speed then 
				speed[e] = speed[e] - speeder[e].acceleration 
				LoopSound(e,1) 
				StopSound(e,0)
			else 
				speed[e] = speeder[e].minimum_speed 
			end 
		else 
			if speed[e] > 0 then 
				speed[e] = speed[e] - speeder[e].decceleration 
			elseif speed[e] < 0 then 
				speed[e] = speed[e] + speeder[e].decceleration 
				StopSound(e,0)
				LoopSound(e,1)
			end 
		end 
		if g_KeyPressA == 1 then 
			SetRotation(e,g_Entity[e]['anglex'],g_Entity[e]['angley']-speeder[e].turn_speed,g_Entity[e]['anglez'])
		elseif g_KeyPressD == 1 then 
			SetRotation(e,g_Entity[e]['anglex'],g_Entity[e]['angley']+speeder[e].turn_speed,g_Entity[e]['anglez'])
		end 
	end
end

function CheckForHittingTerrainOrSlopes(e)
	local x1 = GetEntityPositionX(e) +6 	-- fine tune for collision with entities
	local y1 = GetEntityPositionY(e) +4		-- fine tune for collision with entities 
	local z1 = GetEntityPositionZ(e) +28	-- fine tune for collision with entities 
	local obstructionhit = IntersectAllIncludeTerrain(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,x1,y1,z1,g_Entity[e]['obj'])
	if obstructionhit == nil and (y1(e,a,speeder[e].impact_range,speeder[e].impact_angle) == 1 and speed[e] > 0) or (EntityLooking(e,a,speeder[e].impact_range,360-speeder[e].impact_angle) == 0 and GetDistance(e,a) < speeder[e].impact_range and speed[e] < 0) then 	
		obstructionhit = 0 
	end
	if obstructionhit > 0 then
		speed[e] = speeder[e].minimum_speed
		shealth[e] = (shealth[e] - 20)
		if shealth[e] < g_PlayerHealth then old_health[e] = (old_health[e] - 3) end
		SetPlayerHealth(old_health[e])
		PlaySoundIfSilent(e,2) -- crash sound
	elseif heightdiff[e] == 1  then	
		local slopeH = g_PlayerPosY - oldy[e]
		if slopeH > speeder[e].maximum_slope then   
			speed[e] = -1
		end
		if slopeH > speeder[e].maximum_slope + 2 then   
			speed[e] = -2
		end
		if slopeH > speeder[e].maximum_slope + 5 then   
			speed[e] = -5
		end
	end
	if shealth[e] <= 0 then	speed[e] = -1 end		
end

function CheckForHittingNPC(e)
	if GetTimer(e) > 50 then 
		StartTimer(e)
		for a,b in pairs (g_Entity) do 
			local alleg = GetEntityAllegiance(b)
			if alleg > -1 then 
				if b.health > 0 then 	
					if (EntityLooking(e,a,speeder[e].impact_range,speeder[e].impact_angle) == 1 and speed[e] > 0) or (EntityLooking(e,a,speeder[e].impact_range,360-speeder[e].impact_angle) == 0 and GetDistance(e,a) < speeder[e].impact_range and speed[e] < 0) then  
						SetEntityHealth(a,0)            
						SetEntityRagdollForce(a,"head",g_Entity[a]['x']-g_Entity[e]['x'],10,g_Entity[a]['z']-g_Entity[e]['z'],math.random(math.abs(speed[e])*100,math.abs(speed[e])*150)) 
						SetEntityHealth(e,speeder[e].speeder_health-20)
					end
				end 
			end
		end
	end 
end

function GetOutspeeder(e)
	if shealth[e] <= 0 then		
		PromptLocal(e," Vehicle inoperative")
		g_KeyPressE = 1		
	end
	if g_KeyPressE == 1 and speed[e] > 0 - (speeder[e].maximum_speed/10) and speed[e] < 0 + (speeder[e].maximum_speed/10) and vpressed[e] == 0 then 
		vpressed[e] = 1 
		speed[e] = 0 
		new_y = math.rad(g_Entity[e]['y'])		
		pos_x[e] = g_Entity[e]['x'] + (math.sin(new_y) * speeder[e].enter_range)
		pos_z[e] = g_Entity[e]['z'] + (math.cos(new_y) * speeder[e].enter_range)
		local exitheight = GetTerrainHeight(pos_x[e],pos_z[e])
		SetFreezePosition(pos_x[e],exitheight+35,pos_z[e])
		TransportToFreezePositionOnly()
		CollisionOn(e)
		SetPlayerHealth(old_health[e])
		status[e] = "wait" 
	end 
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

function EntityLooking(e,t,dis,v)
	if g_Entity[e] ~= nil and g_Entity[t] ~= nil then
		if dis == nil then
			dis = 3000
		end
		if v == nil then
			v = 0.5
		end
		local destx = g_Entity[t]['x'] - g_Entity[e]['x']
		local destz = g_Entity[t]['z'] - g_Entity[e]['z']
		if math.sqrt((destx*destx)+(destz*destz)) <= dis then
			local angle = math.atan2(destx,destz)
			angle = angle * (180.0 / math.pi)   
			if angle <= 0 then
				angle = 360 + angle
			elseif angle > 360 then
				angle = angle - 360
			end
			while g_Entity[e]['angley'] < 0 or g_Entity[e]['angley'] > 360 do
				if g_Entity[e]['angley'] <= 0 then
					g_Entity[e]['angley'] = 360 + g_Entity[e]['angley']
				elseif g_Entity[e]['angley'] > 360 then
					g_Entity[e]['angley'] = g_Entity[e]['angley'] - 360
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
			if (L < R and math.abs(g_Entity[e]['angley']) > L and math.abs(g_Entity[e]['angley']) < R) then
				return 1
			elseif (L > R and (math.abs(g_Entity[e]['angley']) > L or math.abs(g_Entity[e]['angley']) < R)) then
				return 1
			else
				return 0
			end
		else
			return 0
		end
	end
end

function GetDistance(e,v)
	if g_Entity[e] ~= nil and g_Entity[e] ~= 0 and g_Entity[v] ~= nil and g_Entity[v] ~= 0 then
		local disx = g_Entity[e]['x'] - g_Entity[v]['x']
		local disz = g_Entity[e]['z'] - g_Entity[v]['z']
		local disy = g_Entity[e]['y'] - g_Entity[v]['y']
		return math.sqrt(disx^2 + disz^2 + disy^2)
	end
end

function GetPlayerFlatDistance(e)
	tPlayerDX = (g_Entity[e]['x'] - g_PlayerPosX)
	tPlayerDZ = (g_Entity[e]['z'] - g_PlayerPosZ)
	return math.sqrt(math.abs(tPlayerDX*tPlayerDX)+math.abs(tPlayerDZ*tPlayerDZ));
end