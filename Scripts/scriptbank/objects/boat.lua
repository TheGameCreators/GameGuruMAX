-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Boat v14 by Necrym59
-- DESCRIPTION: Creates a rideable boat object behavior: Set Physics=ON, Gravity=OFF, IsImobile=YES.
-- DESCRIPTION: Edit the[PROMPT_TEXT$="E to embark"], Activation [BOAT_RANGE=80] the [MIN_SPEED=2(0,10)], [MAX_SPEED=4(0,100)], [TURN_SPEED=2(1,10)], [DRAG=5].
-- DESCRIPTION: Adjust [#PLAYER_Z_POSITION=0(-100,100)] and [#PLAYER_Y_POSITION=20(-100,100)].
-- DESCRIPTION: Adjust [BOAT_ROTATION=0(0,180)], [BOAT_DRAFT=30], [BOAT_POWER=5(0,50)], [BOAT_BRAKE=5(0,50)], [BOAT_BUOYANCY=1(1,5)].
-- DESCRIPTION: <Sound0> - Entry/Exit 
-- DESCRIPTION: <Sound1> - Moving Loop
-- DESCRIPTION: <Sound2> - Run Aground

local U = require "scriptbank\\utillib"

local boat = {}
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
local boat_buoyancy = {}
local water_height = {}
local playerdistance = {}
local boat_active = {}
local boat_release = {}
local boat_aground = {}
local onboatcheck  = {}
local floatangle = {}
local colobj = {}
local boatlength = {}
local heightTerrain = {}
local heightSurface = {}
local status = {}

function boat_properties(e, prompt_text, boat_range, min_speed, max_speed, turn_speed, drag, player_z_position, player_y_position, boat_rotation, boat_draft, boat_power, boat_brake, boat_buoyancy)
	boat[e] = g_Entity[e]
	boat[e].prompt_text = prompt_text
	boat[e].boat_range = boat_range
	boat[e].min_speed = min_speed
	boat[e].max_speed = max_speed
	boat[e].turn_speed = turn_speed
	boat[e].drag = drag
	boat[e].player_z_position = player_z_position
	boat[e].player_y_position = player_y_position
	boat[e].boat_rotation = boat_rotation
	boat[e].boat_draft = boat_draft
	boat[e].boat_power = boat_power
	boat[e].boat_brake = boat_brake
	boat[e].boat_buoyancy = boat_buoyancy or 1
	boat[e].water_height = water_height
end

function boat_init(e)
	boat[e] = {}
	boat[e].prompt_text = "E to embark"
	boat[e].boat_range = 80
	boat[e].min_speed = 2
	boat[e].max_speed = 4
	boat[e].turn_speed = 2
	boat[e].drag = 5
	boat[e].player_z_position = 0
	boat[e].player_y_position = 10
	boat[e].boat_rotation = 0
	boat[e].boat_draft = 30
	boat[e].boat_power = 5
	boat[e].boat_brake = 5
	boat[e].boat_buoyancy = 1
	boat_active[e] = 0
	boat_release[e] = 0
	boat_aground =0
	speed=0
	boatx=g_Entity[e].x
	boaty=g_Entity[e].y
	boatz=g_Entity[e].z
	boatlength[e] = 0
	turn=0
	onboatcheck[e]=0
	floatangle[e]=0
	colobj[e]=0
	heightTerrain[e] = 0
	heightSurface[e] = 0
	status[e] = 'init'
end

function boat_main(e)
	boat[e] = g_Entity[e]
	if status[e] == 'init' then
		local xmin, ymin, zmin, xmax, ymax, zmax = GetObjectColBox(g_Entity[e]['obj'])
		local sx,sy,sz = GetObjectScales(g_Entity[e]['obj'])
		boatlength[e] = ((zmax - zmin) * sz)/2
		status[e] = 'boating'
	end	

	if status[e] == 'boating' then		
		if boat_release[e]==nil then boat_release[e]=0 end
		if boat_active[e]==nil then boat_active[e]=0 end
		boatangle=g_Entity[e].angley
		PlayerDX = g_Entity[e].x - g_PlayerPosX;
		PlayerDZ = g_Entity[e].z - g_PlayerPosZ;
		PlayerDist = math.sqrt(math.abs(PlayerDX*PlayerDX)+math.abs(PlayerDZ*PlayerDZ));
		playerdistance[e]=PlayerDist
		onboatcheck[e]=IntersectAll(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ+35,g_PlayerPosX,g_Entity[e].y+36,g_PlayerPosZ,0)
		if onboatcheck[e] == g_Entity[e].obj or playerdistance[e] < boat[e].boat_range and boat_active[e]==0 then
			if boat_active[e]==0 then Prompt(boat[e].prompt_text) end
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
			local nfloatheight = boat[e].boat_buoyancy
			floatangle[e] = floatangle[e] + (GetAnimationSpeed(e)/100.0)
			local fFinalY = (GetWaterHeight()-1) + nfloatheight + (math.cos(floatangle[e])*nfloatheight)
			local fSwayXZ = nfloatheight/2 + (math.cos(floatangle[e])*nfloatheight/2)
			SetPosition(e,g_Entity[e].x,fFinalY,g_Entity[e].z)
			ResetPosition(e,g_Entity[e].x,fFinalY,g_Entity[e].z)
			SetRotation(e,fSwayXZ/3,boatangle,fSwayXZ)		
			GravityOn(e)
			CollisionOn(e)
			StopSound(e,1)
		end  

		if boat_active[e]==1 then			
			local nfloatheight = boat[e].boat_buoyancy
			floatangle[e] = floatangle[e] + (GetAnimationSpeed(e)/100.0)
			local fFinalY = GetWaterHeight() + nfloatheight + (math.cos(floatangle[e])*nfloatheight)
			local fSwayXZ = nfloatheight/2 + (math.cos(floatangle[e])*nfloatheight/2)
			boatx=g_Entity[e].x
			boaty=g_Entity[e].y
			boatz=g_Entity[e].z
			SetFreezePosition(freezex,g_Entity[e].y+5,freezez)
			TriggerWaterRipple(g_Entity[e].x,g_Entity[e].y-5,g_Entity[e].z)
			-- Boat speed control
			if g_KeyPressW == 1 then  -- W key
				if speed<boat[e].max_speed then
					speed=speed+(boat[e].boat_power/100)
					LoopSound(e,1)
				end
			end
			if g_KeyPressS == 1 then -- S key
				if speed>boat[e].min_speed then
					speed=speed-(boat[e].boat_brake/100)
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
				boatangle=boatangle-(boat[e].turn_speed/10)
			end
			if g_KeyPressD == 1 then -- D key
				boatangle=boatangle+(boat[e].turn_speed/10)
			end
			if g_KeyPressQ == 1 then -- Q key
				SetFreezePosition(freezex,(g_Entity[e].y + 40),freezez)
				TransportToFreezePosition()
				boat_release[e]=0
				boat_active[e]=0
			end
			if  boat_active[e]==1 then
				--Draft Check-------------------------------------------------------------------------------
				heightTerrain[e] = GetTerrainHeight(g_Entity[e].x,g_Entity[e].z)
				if (g_Entity[e].y - boat[e].boat_draft) <= heightTerrain[e] then
					SetFreezePosition(freezex,(g_Entity[e].y + 40),freezez)
					TransportToFreezePosition()
					boat_aground = 1
					speed = -1
					boat_release[e]=0
					boat_active[e]=0
				end
				--Collision Check--------------------------------------------------------------------------
				local ex,ey,ez,eax,eay,eaz = GetEntityPosAng(e)
				local ox,oy,oz = U.Rotate3D(0,0,boatlength[e],0,math.rad(eay),0)
				colobj[e] = IntersectAllIncludeTerrain(ex,ey,ez,ex+ox,ey+oy,ez+oz,g_Entity[e].obj,0,0,1,0)				
				if colobj[e] > 0 or colobj[e] == -1 then
					SetFreezePosition(freezex,(g_Entity[e].y + 40),freezez)
					TransportToFreezePosition()
					boat_aground = 1
					speed = -1
					boat_release[e]=0
					boat_active[e]=0
				end	
				-------------------------------------------------------------------------------------------				
			end
			boatangledeg=math.rad(boatangle+boat[e].boat_rotation)
			boatangleleftdeg=math.rad(boatangle)
			boatanglerightdeg=math.rad(boatangle)
			-- Boat collision
			boatx=boatx+(math.sin(boatangledeg)*speed)
			boaty=g_Entity[e].y
			boatz=boatz+(math.cos(boatangledeg)*speed)
			-- Update location
			if boat_active[e]==1 then
				freezex=g_Entity[e].x+(math.sin(boatangledeg)*(boat[e].player_z_position-speed)*-1)
				freezez=g_Entity[e].z+(math.cos(boatangledeg)*(boat[e].player_z_position-speed)*-1)
				SetFreezePosition(freezex,g_Entity[e].y+boat[e].player_y_position,freezez)
			end
			GravityOff(e)
			CollisionOff(e)
			SetPosition(e,boatx,boaty,boatz)
			SetRotation(e,fSwayXZ/3,boatangle,fSwayXZ)
			GravityOn(e)
			CollisionOn(e)
			TransportToFreezePositionOnly()
			if speed>0 and g_KeyPressW == 0 then speed=speed-(boat[e].drag/10000) end
			if speed<0 and g_KeyPressS == 0 then speed=speed/1.1 end
		end
	end
end