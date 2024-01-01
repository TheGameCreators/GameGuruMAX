-- Aircraft v31 by Necrym59
-- DESCRIPTION: Creates a controllable aircraft from an object.
-- DESCRIPTION: Attach to an object. Set Physics On, Polygon Collision.
-- DESCRIPTION: [@VEHICLE_TYPE=1(1=Plane,2=Helicopter,3=VTOL-Vehicle)]
-- DESCRIPTION: [USE_RANGE=100(1,500)]
-- DESCRIPTION: [PROMPT_TEXT$="E to Fly"]
-- DESCRIPTION: [USE_TEXT1$="WASD=Control, Mouse=Pitch,Roll, Q=Exit craft"]
-- DESCRIPTION: [USE_TEXT2$="Space=UP, Shift=DN"]
-- DESCRIPTION: [LZ_TEXT$="Landing Zone detected - Press X to commence Auto-landing"]
-- DESCRIPTION: [DISPLAY_X=50]
-- DESCRIPTION: [DISPLAY_Y=88]
-- DESCRIPTION: [PILOT_X=0(-500,500)]
-- DESCRIPTION: [PILOT_Y=30(-500,500)]
-- DESCRIPTION: [PILOT_Z=-15(-500,500)]
-- DESCRIPTION: [MAX_SPEED=30(1,500)]
-- DESCRIPTION: [VELOCITY=1(1,20)]
-- DESCRIPTION: [@FX_ANIMATION$=-1(0=AnimSetList)]
-- DESCRIPTION: [FX_MODULATION=60(1,100)]
-- DESCRIPTION: [SHOW_READOUTS!=1]
-- DESCRIPTION: <Sound0> for take-off sound
-- DESCRIPTION: <Sound1> loop for in-flight
-- DESCRIPTION: <Sound2> for landing sound
-- DESCRIPTION: <Sound3> for crash/explosion sound

local Q = require "scriptbank\\quatlib"
local U = require "scriptbank\\utillib"
local P = require "scriptbank\\physlib"
local rad  = math.rad
local deg  = math.deg
local atan = math.atan2
local modf = math.modf
local abs  = math.abs

g_LandingZone		= {}
local aircraft 		= {}
local vehicle 		= {}
local controlEnt    = {}
local timeLastFrame = {}
local timeDiff      = {}
local maxSpeed 		= {}
local playerPos 	= {}
local LandingMode	= {}
local XPressed		= {}
local timeNow		= {}
local omx			= {}
local omy			= {}
local use_range		= {}
local prompt_text	= {}
local use_text1		= {}
local fx_animation 	= {}
local pilot_x 		= {}
local pilot_y 		= {}
local pilot_z 		= {}
local max_speed		= {}
local vlift			= {}
local cmode			= {}
local vehicle_type 	= {}
local flying		= {}
local playonce		= {}
local doonce 		= {}
local tilt			= {}
local nheightangle  = {}
local status 		= {}
local animonce		= {}
local animspeed		= {}
local sndpause		= {}
local terrain 		= {}
local surface 		= {}
local heightcheck	= {}
local surfacecheck	= {}
local terraincheck	= {}
local deathfall     = {}
local craftheight   = {}
local craftlength   = {}
local last_gun      = ""
local colobj		= {}
local coltimer		= {}

function aircraft_properties(e, vehicle_type, use_range, prompt_text, use_text1, use_text2, lz_text, display_x, display_y, pilot_x, pilot_y, pilot_z, max_speed, velocity, fx_animation, fx_modulation, show_readouts)
	vehicle[e] = g_Entity[e]
	vehicle[e].vehicle_type = vehicle_type
	vehicle[e].use_range = use_range
	vehicle[e].prompt_text = prompt_text
	vehicle[e].use_text1 = use_text1
	vehicle[e].use_text2 = use_text2
	vehicle[e].lz_text = lz_text
	vehicle[e].display_x = display_x
	vehicle[e].display_y = display_y
	vehicle[e].pilot_x = pilot_x
	vehicle[e].pilot_y = pilot_y
	vehicle[e].pilot_z = pilot_z
	vehicle[e].max_speed = max_speed
	vehicle[e].velocity = velocity
	vehicle[e].fx_animation = "=" .. tostring(fx_animation)
	vehicle[e].fx_modulation = fx_modulation or 1
	vehicle[e].show_readouts = show_readouts
end

function aircraft_init(e)
	Include( "utillib.lua" )
	Include( "quatlib.lua" )
	Include( "physlib.lua" )

	vehicle[e] = g_Entity[e]
	vehicle[e].vehicle_type = 1
	vehicle[e].use_range = 100
	vehicle[e].prompt_text = "E to fly"
	vehicle[e].use_text1 = "WASD=Control, Mouse=Pitch,Roll, Q=Exit"
	vehicle[e].use_text2 = "Space=UP, Shift=DN"
	vehicle[e].lz_text = "Landing Zone detected - Press X to commence Auto-landing"
	vehicle[e].display_x = 50
	vehicle[e].display_y = 88
	vehicle[e].pilot_x = 0
	vehicle[e].pilot_y = 30
	vehicle[e].pilot_z = -15
	vehicle[e].max_speed = 30
	vehicle[e].velocity = 0
	vehicle[e].fx_animation = ""
	vehicle[e].fx_modulation = 60
	vehicle[e].show_readouts = 1

	controlEnt    	= nil
	timeLastFrame 	= nil
	timeDiff      	= 0
	g_LandingZone 	= 0
	status[e] 		= "init"
	vlift      		= 0
	tilt			= 0
	playonce[e]     = 0
	doonce[e]		= 0
	nheightangle[e]	= 0
	LandingMode[e] 	= 0
	animonce[e]		= 0
	animspeed[e]	= 0
	terrain[e]		= 0
	surface[e]		= 0
	heightcheck[e]  = 0
	terraincheck[e] = 0
	surfacecheck[e] = 0
	craftheight[e]  = 0
	craftlength[e]  = 0
	deathfall[e]  	= 0
	colobj[e]  		= 0
	coltimer[e]  	= math.huge
	sndpause[e]		= math.huge
	EPressed 		= false
end

local function AttachPlayer(h,xa,ya,za)
	local xo, yo, zo = U.Rotate3D( h.pp.xo, h.pp.yo, h.pp.zo, xa, ya, za )
	cx, cy, cz =  h.pos.x + xo, h.pos.y + yo, h.pos.z + zo
	SetCameraPosition ( 0, cx, cy, cz )
	if cmode == 1 then SetCameraAngle (0,deg(xa)+3,deg(ya),deg(za)) end
	if cmode == 2 then SetCameraAngle (0,g_PlayerAngX,g_PlayerAngY,g_PlayerAngZ) end
	SetFreezePosition( h.pos.x, h.pos.y + 80, h.pos.z - 80 )
	TransportToFreezePositionOnly()
end

local function posObject(h,vlift,tilt)
	local xA, yA, zA = Q.ToEuler( h.quat )
	local lift = h.vec.z / 4
	if vlift ~= 0 or vlift ~= nil then lift = vlift end
	local vx, vy, vz = U.Rotate3D( 0, h.vec.y + lift, h.vec.z, xA, yA, zA )
	h.pos.x, h.pos.y, h.pos.z = h.pos.x + vx, h.pos.y + vy, h.pos.z + vz
	CollisionOff( h.ent )
	PositionObject( h.obj, h.pos.x, h.pos.y, h.pos.z )
	RotateObject( h.obj, deg( xA )+tilt, deg( yA ), deg( zA ) )
	CollisionOn( h.ent )
	AttachPlayer( h, xA, yA, zA )
end

function aircraft_main(e)

	if status[e] == "init" then
		maxSpeed = vehicle[e].max_speed
		SetPlayerWeapons(1)
		ChangePlayerWeapon(last_gun)
		flying = 0
		coltimer[e] = g_Time + 200
		status[e] = "begin"
	end

	if status[e] == "begin" then
		playerPos = {xo = vehicle[e].pilot_x, yo = vehicle[e].pilot_y, zo = vehicle[e].pilot_z}
		local h = aircraft[e]
		if h == nil then			
			local Ent = g_Entity[e]
			local x, y, z, xa, ya, za = GetObjectPosAng( Ent.obj )
			aircraft[ e ] =
			{
				ent     = e,
				state   = 'init',
				timer   = math.huge,
				obj     = Ent.obj,
				pos     = { x = x, y = y, z = z },
				quat    = Q.FromEuler( rad( xa ), rad( ya ), rad( za ) ),
				vec     = { x = 0, y = -1, z = 0 },
				height  = 0,
				pp      = playerPos
			}
			local xmin, ymin, zmin, xmax, ymax, zmax = GetObjectColBox(g_Entity[e]['obj'])
			local sx, sy, sz = GetObjectScales(g_Entity[e]['obj'])
			local wx, hx, lx = (xmax - xmin) * sx, (ymax - ymin) * sy, (zmax - zmin) * sz
			craftheight[e] = hx	
			craftlength[e] = lx
			return
		end

		if animonce[e] == 0 then
			SetAnimationName(h.ent,vehicle[e].fx_animation)
			ModulateSpeed(h.ent,0)
			LoopAnimation(h.ent)
			animonce[e] = 1
		end

		timeNow = g_Time
		if h.state == 'init' then
			h.state = 'start'
			h.timer = timeNow + 1000
		end

		if controlEnt == nil then controlEnt = e end
		if controlEnt == e then
			if timeLastFrame == nil then
				timeLastFrame = timeNow
				timeDiff = 1
			else
				timeDiff = ( timeNow - timeLastFrame ) / 20
				timeLastFrame = timeNow
			end
		end
		if h.state == 'start' then
			if deathfall[e] == 1 and g_PlayerPosY <= terrain[e] + 80 then
				HurtPlayer(-1,5000)
				deathfall[e] = 0
				Hide(h.ent)
				Destroy(h.ent)
				status[e] = "init"
			end
			if h.inCab then
				if g_KeyPressQ == 1 and flying == 1 then
					animspeed[e] = 0
					if heightcheck[e] >= 30 and colobj[e] == 0 then deathfall[e] = 1 end
					if colobj[e] > 0 then deathfall[e] = 0 end
					terrain[e] = GetTerrainHeight( h.pos.x, h.pos.z )
					flying = 0
					SetGamePlayerStatePlrKeyForceKeystate(16)
				end
				if g_KeyPressQ == 1 and flying == 0 then
					if not EPressed then
						EPressed = true
						ModulateSpeed(h.ent,0)
						if colobj[e] >= 0 then
							StopSound(e,1)
							if playonce[e] == 1 then
								PlaySound(e,2)
								playonce[e] = 0									
							end
						end
						local ox,oy,oz = U.Rotate3D(0,0,18,math.rad(g_PlayerAngX),math.rad(g_PlayerAngY),math.rad(g_PlayerAngZ))
						local fposx, fposy, fposz = g_PlayerPosX + ox, g_PlayerPosY + oy, g_PlayerPosZ + oz
						fposx=fposx+200
						if deathfall[e] == 0 then fposy = fposy+25 end
						fposz=fposz+200
						SetFreezePosition(fposx,fposy,fposz)
						TransportToFreezePositionOnly(fposx,fposy,fposz)
						SetCameraOverride(0)
						DeactivateMouse()
						SetPlayerWeapons(1)
						ChangePlayerWeapon(last_gun)
						h.inCab = false
						h.height = 0
						h.yaw = 0
						playonce[e] = 0
						SetGamePlayerStatePlrKeyForceKeystate(0)
						g_LandingZone = 0						
						flying = 0
						doonce[e] = 0
						status[e] = "init"
						return
					end
				else
					EPressed = false
				end
				--(Plane)-------------------------------------------------------------------------------------------------
				if vehicle[e].vehicle_type == 1 then
					terrain[e] = GetTerrainHeight( h.pos.x, h.pos.z )
					heightcheck[e] = (h.pos.y - terrain[e])
					if h.inCab == true and doonce[e] == 0 then
						if animspeed[e] < (vehicle[e].fx_modulation/10) then
							ModulateSpeed(h.ent,animspeed[e])
							animspeed[e] = animspeed[e] + 0.2							
						end
						if animspeed[e] >= vehicle[e].fx_modulation/5 then doonce[e] = 1 end							
					end
					if g_KeyPressW == 1 then
						if playonce[e] == 0 then
							PlaySound(e,0)
							playonce[e] = 1
							sndpause[e] = g_Time + 2000
							ModulateSpeed(h.ent,vehicle[e].fx_modulation)
						end
						if g_Time > sndpause[e] then LoopSound(e,1) end
						if h.vec.z < maxSpeed then
							h.vec.z = h.vec.z + 0.09 * (timeDiff*vehicle[e].velocity)
						end
					elseif
					   g_KeyPressS == 1 then
						if h.vec.z > 0.09 then
							h.vec.z = h.vec.z - 0.09 * (timeDiff*vehicle[e].velocity)
						else
							h.vec.z = 0
						end
					end
					if h.pos.y <= terrain[e] + 40 and h.vec.z < 1 and g_KeyPressW == 0 then
						StopSound(e,1)
						if playonce[e] == 1 then
							PlaySound(e,2)
							playonce[e] = 0
							ModulateSpeed(h.ent,0)
							flying = 0
						end
					end
				end
				--(Helicopter)---------------------------------------------------------------------------------------------
				if vehicle[e].vehicle_type == 2 then
					terrain[e] = GetTerrainHeight( h.pos.x, h.pos.z )
					surface[e] = GetSurfaceHeight( h.pos.x, h.pos.y, h.pos.z )	
					terraincheck[e] = (h.pos.y - terrain[e])
					surfacecheck[e] = (h.pos.y - surface[e])					
					if surfacecheck[e] < terraincheck[e] then
						heightcheck[e] = surfacecheck[e]
					else
						heightcheck[e] = terraincheck[e]
					end	
					if h.inCab == true and doonce[e] == 0 then
						if playonce[e] == 0 then
							PlaySound(e,0)
							playonce[e] = 2
							sndpause[e] = g_Time + 2000
							if g_Time > sndpause[e] then LoopSound(e,1) end
						end
						if animspeed[e] < (vehicle[e].fx_modulation/10) then
							ModulateSpeed(h.ent,animspeed[e])
							animspeed[e] = animspeed[e] + 0.2
						end
						if animspeed[e] >= vehicle[e].fx_modulation/5 then
							doonce[e] = 1
							playonce[e] = 1
						end						
					end
					if g_KeyPressSPACE == 1 then
						LoopSound(e,1)
						if animspeed[e] < vehicle[e].fx_modulation then
							ModulateSpeed(h.ent,animspeed[e])
							animspeed[e] = animspeed[e] + 0.1
						end
						vlift = vlift + 0.01 * (timeDiff*vehicle[e].velocity/2)
					else
						vlift = 1
					end
					if g_KeyPressSHIFT == 1 and g_KeyPressSPACE == 0 and heightcheck[e] > 6 and colobj[e] == 0 then
						vlift = vlift - 1.5 * (timeDiff*vehicle[e].velocity/2)
						if h.vec.z > 0 then
							h.vec.z = h.vec.z - 0.09 * timeDiff
						end
						if h.vec.z <= 0 then h.vec.z = 0 end
					end
					if h.pos.y <= terrain[e] + 3 and g_KeyPressSPACE == 0 then
						StopSound(e,1)
						if playonce[e] == 1 then
							PlaySound(e,2)
							playonce[e] = 0
						end
					end
					if g_LandingZone == 1 then
						if g_InKey == "x" or g_InKey == "X" then LandingMode[e] = 1 end
						if LandingMode[e] == 1 then
							if abs( omx ) > 0.05 then omx = 0 end
							if abs( omy ) > 0.05 then omy = 0 end
							if heightcheck[e] > 5 then
								vlift = vlift - 1.2 * (timeDiff*vehicle[e].velocity/2)
							end
							if h.vec.z > 0 then
								h.vec.z = h.vec.z - 0.09 * timeDiff
							end
							if heightcheck[e] <= 6 then
								CollisionOff(h.ent)
								PositionObject(h.obj,h.pos.x,h.pos.x+1,h.pos.z)
								CollisionOn(h.ent)
								GravityOn(h.ent)
								StopSound(e,1)
								if playonce[e] == 1 then
									PlaySound(e,2)
									playonce[e] = 0									
								end
								if animspeed[e] > 0 then
									ModulateSpeed(h.ent,animspeed[e])
									animspeed[e] = animspeed[e] - 0.2
								end
								if vehicle[e].fx_animation == "=-1" then animspeed[e] = 0 end
								if animspeed[e] < 0 then
									ModulateSpeed(h.ent,0)
									flying = 0
									LandingMode[e] = 0
									SetGamePlayerStatePlrKeyForceKeystate(16)
								end
							end
						end
					end
					if h.pos.y <= terrain[e] + 25 or heightcheck[e] <= 6 then
						if animspeed[e] > 0 then
							ModulateSpeed(h.ent,animspeed[e])
							animspeed[e] = animspeed[e] - 0.1
						end
						if vehicle[e].fx_animation == "=-1" then animspeed[e] = 0 end
						if animspeed[e] <= 0 then							
							ModulateSpeed(h.ent,0)							
						end						
					end
					if h.pos.y > terrain[e] + 100 then
						flying = 1
						if g_KeyPressW == 1 then
							StopSound(e,0)
							LoopSound(e,1)							
							if h.vec.z < maxSpeed then
								h.vec.z = h.vec.z + 0.09 * (timeDiff*vehicle[e].velocity/3)
							end
							if tilt < 3 then
								tilt = tilt + 0.05 * timeDiff
							end
						elseif
						   g_KeyPressS == 1 then
							if h.vec.z > 0.09 then
								h.vec.z = h.vec.z - 0.09 * (timeDiff*vehicle[e].velocity/3)
							else
								h.vec.z = 0
							end
							if tilt > 0 then
								tilt = tilt - 0.05 * timeDiff
							end
						end
					end
				end
				--(VTOL Aircraft)-----------------------------------------------------------------------------------
				if vehicle[e].vehicle_type == 3 then
					terrain[e] = GetTerrainHeight( h.pos.x, h.pos.z )
					surface[e] = GetSurfaceHeight( h.pos.x, h.pos.y, h.pos.z )	
					terraincheck[e] = (h.pos.y - terrain[e])
					surfacecheck[e] = (h.pos.y - surface[e])					
					if surfacecheck[e] < terraincheck[e] then
						heightcheck[e] = surfacecheck[e]
					else
						heightcheck[e] = terraincheck[e]
					end	
					if h.inCab == true and doonce[e] == 0 then
						if playonce[e] == 0 then
							PlaySound(e,0)
							playonce[e] = 2
							sndpause[e] = g_Time + 2000
							if g_Time > sndpause[e] then LoopSound(e,1) end
						end
						if animspeed[e] < (vehicle[e].fx_modulation/10) then
							ModulateSpeed(h.ent,animspeed[e])
							animspeed[e] = animspeed[e] + 0.2
						end
						if animspeed[e] >= vehicle[e].fx_modulation/5 then
							doonce[e] = 1
							playonce[e] = 1
						end
					end
					if g_KeyPressSPACE == 1 then
						LoopSound(e,1)
						if g_Time > sndpause[e] then LoopSound(e,1) end
						vlift = vlift + 0.01 * (timeDiff*vehicle[e].velocity/2)
						if animspeed[e] < vehicle[e].fx_modulation then
							ModulateSpeed(h.ent,animspeed[e])
							animspeed[e] = animspeed[e] + 0.2
						end
					else
						vlift = 1
					end
					if g_KeyPressSHIFT == 1 and g_KeyPressSPACE == 0 and heightcheck[e] > 6 and colobj[e] == 0 then
						vlift = vlift - 1.2 * (timeDiff*vehicle[e].velocity/2)
						if h.vec.z > 0 then
							h.vec.z = h.vec.z - 0.09 * (timeDiff*vehicle[e].velocity/3)
						end
						if h.vec.z <= 0 then h.vec.z = 0 end
					end
					if h.pos.y <= terrain[e] + 3 and g_KeyPressSPACE == 0 then
						StopSound(e,1)
						if playonce[e] == 1 then
							PlaySound(e,2)
							playonce[e] = 0
						end
					end
					if g_LandingZone == 1 then
						if g_InKey == "x" or g_InKey == "X" then LandingMode[e] = 1 end
						if LandingMode[e] == 1 then
							if abs( omx ) > 0.05 then omx = 0 end
							if abs( omy ) > 0.05 then omy = 0 end
							if heightcheck[e] > 5 then
								vlift = vlift - 1.2 * (timeDiff*vehicle[e].velocity/2)
							end
							if h.vec.z > 0 then
								h.vec.z = h.vec.z - 0.09 * (timeDiff*vehicle[e].velocity/3)
							end
							if heightcheck[e] <= 6 then
								CollisionOff(h.ent)
								PositionObject(h.obj,h.pos.x,h.pos.x+1,h.pos.z )
								CollisionOn(h.ent)
								GravityOn(h.ent)
								StopSound(e,1)
								if playonce[e] == 1 then
									PlaySound(e,2)
									playonce[e] = 0									
								end
								if animspeed[e] > 0 then
									ModulateSpeed(h.ent,animspeed[e])
									animspeed[e] = animspeed[e] - 0.12
								end
								if vehicle[e].fx_animation == "=-1" then animspeed[e] = 0 end
								if animspeed[e] <= 0 then								
									ModulateSpeed(h.ent,0)
									flying = 0
									LandingMode[e] = 0
									SetGamePlayerStatePlrKeyForceKeystate(16)
								end
							end
						end
					end
					if h.pos.y <= terrain[e] + 25 or heightcheck[e] <= 6 then				
						if animspeed[e] > 0 then 
							ModulateSpeed(h.ent,animspeed[e])
							animspeed[e] = animspeed[e] - 0.12
						end						
						if vehicle[e].fx_animation == "=-1" then animspeed[e] = 0 end
						if animspeed[e] <= 0 then
							ModulateSpeed(h.ent,0)
						end						
					end		
					if h.pos.y > terrain[e] + 50 then
						flying = 1
						if g_KeyPressW == 1 then
							StopSound(e,0)
							LoopSound(e,1)
							if h.vec.z < maxSpeed then
								h.vec.z = h.vec.z + 0.09 * (timeDiff*vehicle[e].velocity/3)
							end
						elseif
						   g_KeyPressS == 1 then
							if h.vec.z > 0.09 then
								h.vec.z = h.vec.z - 0.09 * (timeDiff*vehicle[e].velocity/3)
							else
								h.vec.z = 0
							end
						end
					end
				end
				--Flight Collision Check ----------------------------------------------------------------
				if flying == 1 then 
					if g_Time > coltimer[e] then
						local ex,ey,ez,eax,eay,eaz = GetEntityPosAng(h.ent)
						local ox,oy,oz = U.Rotate3D(0,craftheight[e]/2,craftlength[e]/2, 0,math.rad(eay),0)
						colobj[e] = IntersectAll(ex,ey,ez,ex+ox,ey+oy,ez+oz,g_Entity[h.ent].obj)
						if colobj[e] > 0 then										
							if h.vec.z > 4 then
								StopSound(e,1)
								if playonce[e] == 1 or 0 then
									PlaySound(e,3)
									playonce[e] = 0
								end								
								SetCameraOverride(0)
								DeactivateMouse()
								SetGamePlayerStatePlrKeyForceKeystate(16)
								SetPlayerWeapons(1)
								ChangePlayerWeapon(last_gun)
								HurtPlayer(e,g_PlayerHealth)
								SetEntityHealth(h.ent,0)
								Destroy(h.ent)
							else
								SetFreezePosition(ex,ey,ez)								
								TransportToFreezePositionOnly()							
								h.vec.z = 0
								h.vec.y = 0
								h.vec.x = 0
							end
						end
						if colobj[e] > 0 then
							coltimer[e] = g_Time + 0
						else
							coltimer[e] = g_Time + 200
						end
					end
				end
				-----------------------------------------------------------------------------------------
				terrain[e] = GetTerrainHeight( h.pos.x, h.pos.z )
				omx, omy = (g_MouseX-50)/50, (g_MouseY-50)/50
				if abs( omx ) < 0.05 then omx = 0 end
				if abs( omy ) < 0.05 then omy = 0 end

				if vehicle[e].vehicle_type == 1 and h.vec.z < 3 and flying == 0 then
					if abs( omx ) > 0.05 then omx = 0 end
					if abs( omy ) > 0.05 then omy = 0 end
				end
				if vehicle[e].vehicle_type == 2	then
					if abs( omx ) > 0.05 then omx = 0 end
					if abs( omy ) > 0.05 then omy = 0 end
				end
				if vehicle[e].vehicle_type == 3 and h.pos.y < terrain[e] + 100 and g_KeyPressSPACE == 0 then
					if abs( omx ) > 0.05 then omx = 0 end
					if abs( omy ) > 0.05 then omy = 0 end
				end
				if vehicle[e].show_readouts == 0 then 
					if vehicle[e].vehicle_type == 1 then TextCenterOnX(vehicle[e].display_x,vehicle[e].display_y+2,2, vehicle[e].use_text1) end
					if vehicle[e].vehicle_type == 2 or vehicle[e].vehicle_type == 3 then TextCenterOnX(vehicle[e].display_x,vehicle[e].display_y+2,2, vehicle[e].use_text1.. ", " ..vehicle[e].use_text2) end
				end
				if vehicle[e].show_readouts == 1 then 
					if vehicle[e].vehicle_type == 1 then
						TextCenterOnX(vehicle[e].display_x,vehicle[e].display_y,3,"Speed: "  ..math.ceil(h.vec.z)..  "   Height: " ..math.ceil(h.pos.y).. "   Pitch: "  ..math.ceil(omy).. "   Roll: " ..math.ceil(omx))
						TextCenterOnX(vehicle[e].display_x,vehicle[e].display_y+2,2, vehicle[e].use_text1)
					end
					if vehicle[e].vehicle_type == 2 then
						TextCenterOnX(vehicle[e].display_x,vehicle[e].display_y,3,"Speed: "  ..math.ceil(h.vec.z)..  "   Height: " ..math.ceil(h.pos.y))
						TextCenterOnX(vehicle[e].display_x,vehicle[e].display_y+2,2, vehicle[e].use_text1.. ", " ..vehicle[e].use_text2)
						if g_LandingZone == 1 then Prompt(vehicle[e].lz_text) end
					end
					if vehicle[e].vehicle_type == 3 then
						TextCenterOnX(vehicle[e].display_x,vehicle[e].display_y,3,"Speed: "  ..math.ceil(h.vec.z)..  "   Height: " ..math.ceil(h.pos.y).. "   Pitch: "  ..math.ceil(omy).. "   Roll: " ..math.ceil(omx))
						TextCenterOnX(vehicle[e].display_x,vehicle[e].display_y+2,2, vehicle[e].use_text1.. ", " ..vehicle[e].use_text2)
						if g_LandingZone == 1 then Prompt(vehicle[e].lz_text) end
					end
				end				
				if g_LandingZone == 1 then Prompt(vehicle[e].lz_text) end
				
				if vehicle[e].vehicle_type == 2 and flying == 1 and h.pos.y > terrain[e] + 20 and h.vec.z <= 0 and g_KeyPressSPACE == 0 and g_KeyPressSHIFT == 0 then
					local nhoverheight = 0.5
					nheightangle[e] = nheightangle[e] + (GetAnimationSpeed(h.ent)/30.0)
					local fFinalY = h.pos.y - nhoverheight + (math.cos(nheightangle[e])*nhoverheight)
					CollisionOff(h.ent)
					GravityOff(h.ent)
					ResetPosition(h.ent,h.pos.x,fFinalY,h.pos.z)
					GravityOn(h.ent)
					CollisionOn(h.ent)
				end

				local pitchQ = Q.FromEuler( rad( omy ) * timeDiff, 0, 0 )
				local rollQ  = Q.FromEuler( 0, 0, -rad( omx ) * timeDiff )
				h.quat = Q.Mul( h.quat, pitchQ )
				h.quat = Q.Mul( h.quat, rollQ )

				if g_KeyPressA == 1 then
					if h.yaw < 1 then
						h.yaw = h.yaw + 0.01 * (timeDiff*vehicle[e].velocity/2)
					end
				elseif
				   g_KeyPressD == 1 then
					if h.yaw > -1 then
						h.yaw = h.yaw - 0.01 * (timeDiff*vehicle[e].velocity/2)
					end
				else
					if h.yaw > 0.01 then
						h.yaw = h.yaw - 0.01 * timeDiff
					elseif
					   h.yaw < -0.01 then
						h.yaw = h.yaw + 0.01 * timeDiff
					else
						h.yaw = 0
					end
				end

				if h.yaw ~= 0 then
					h.quat = Q.Mul( h.quat, Q.FromEuler( 0, -rad( h.yaw ) * timeDiff, 0 ) )
				end

				terrain[e] = GetTerrainHeight( h.pos.x, h.pos.z )
				if h.pos.y > terrain[e] and flying == 0 then
					if vehicle[e].vehicle_type == 1 then h.vec.y = 0 end
					if vehicle[e].vehicle_type == 2 then h.vec.y = -1 end
					if vehicle[e].vehicle_type == 3 then h.vec.y = -1 end
				end
				if h.pos.y > terrain[e]+150 then
					h.vec.y = -1
					flying = 1
				end
				if h.pos.y <= terrain[e]+5 and flying == 1 then
					if h.vec.z > 3 then
						StopSound(e,1)
						if playonce[e] == 1 or 0 then
							PlaySound(e,3)
							playonce[e] = 0
						end
						SetCameraOverride(0)
						DeactivateMouse()
						SetGamePlayerStatePlrKeyForceKeystate(16)
						SetPlayerWeapons(1)
						ChangePlayerWeapon(last_gun)
						HurtPlayer(e,g_PlayerHealth)
						SetEntityHealth(h.ent,0)
						Destroy(h.ent)						
					end
					h.vec.y = 0
					h.pos.y = terrain[e]+2
					h.yaw = 0
				end
				if vehicle[e].vehicle_type == 1 then posObject(h,0,0) end
				if vehicle[e].vehicle_type == 2 then posObject(h,vlift,tilt) end
				if vehicle[e].vehicle_type == 3 then posObject(h,vlift,0) end
			else
				if U.PlayerLookingNear(e,vehicle[e].use_range,180) then
					Prompt(vehicle[e].prompt_text)
					if g_KeyPressE == 1 then
						PositionMouse( GetDeviceWidth() / 2, GetDeviceHeight() / 2 )
						if vehicle[e].vehicle_type == 1 then
							SetCameraOverride(3)
							ActivateMouse()
							cmode = 1
						end
						if vehicle[e].vehicle_type == 2 then
							cmode = 2
						end
						if vehicle[e].vehicle_type == 3 then
							SetCameraOverride(3)
							ActivateMouse()
							cmode = 1
						end
						EPressed = true
						last_gun = g_PlayerGunName
						SetPlayerWeapons(0)
						h.inCab = true
						h.yaw = 0
						return
					else
						EPressed = false
					end
				end
			end
		end
	end
end