-- Trap Door Rotate v5
-- DESCRIPTION: For use with non-animating door as a trapdoor when a player stands on it,
-- DESCRIPTION: and will auto close after a [#CLOSE_DELAY=2.0(1.0,10.0)].
-- DESCRIPTION: Apply floor texture to disguise door if required.
-- DESCRIPTION: Set if [Visible!=1)]
-- DESCRIPTION: Plays <Sound0> when opening. 
-- DESCRIPTION: Plays <Sound1> when closing.

local Q = require "scriptbank\\quatlib"
local U = require "scriptbank\\utillib"

local deg = math.deg
local rad = math.rad

g_trapdoor = {}
local names = {}
local keyPressed = false

local timeLastFrame = nil
local timeDiff      = 1
local controlEnt    = nil
local ontrapdoor    = {}
local status 		= {}
local close_delay	= {}
local delaytimer 	= {}
local door_vis 		= {}

local defaultDoorType     = 'Auto'

function trapdoor_rotate_properties( e, close_delay, visible, door_type )
	local door = g_trapdoor[e]
	if door == nil then return end
	door.door_delay = close_delay
	door.door_vis = visible
	door.door_type = 'Auto'	
end 

function trapdoor_rotate_init_name( e, name )
	Include( "quatlib.lua" )
	Include( "utillib.lua" )
	names[ e ] = name
	g_trapdoor[e] = 	{ obj = nil,
							timer = math.huge,
							state = 'Closed',
							angle = 0,
							quat = Q.FromEuler( 0, 0, 0 ),
							blocking = 1,
							originalx = -1,
							originaly = -1,
							originalz = -1,
							door_delay = 2.0,
							door_vis = 1,
							door_type = defaultDoorType,
					    }
	ontrapdoor[e] = 0
	delaytimer[e] = math.huge
	status[e] = 'init'
end

function trapdoor_rotate_main(e)
	
	if status[e] == 'init' then
		if g_trapdoor[e].door_vis == 0 then Hide(e) end
		if g_trapdoor[e].door_vis == 1 then Show(e) end
		status[e] = 'endinit'
	end			
	
	local door = g_trapdoor[e]
	if door == nil then return end
	if door.obj == nil then
		-- initialise
		local Ent = g_Entity[ e ]
		local _, _, _, ax, ay, az = GetObjectPosAng( Ent.obj )
		g_trapdoor[e].obj = Ent.obj
		g_trapdoor[e].timer = math.huge
		g_trapdoor[e].state = 'Closed'
		g_trapdoor[e].angle = 0
		g_trapdoor[e].quat = Q.FromEuler( rad( ax ), rad( ay ), rad( az ) )
		g_trapdoor[e].blocking = 1
		g_trapdoor[e].originalx = -1
		g_trapdoor[e].originaly = -1
		g_trapdoor[e].originalz = -1
		return
	end
	
	if g_trapdoor[e].originalx == -1 then
		g_trapdoor[e].originalx = g_Entity[e]['x']
		g_trapdoor[e].originaly = g_Entity[e]['y']
		g_trapdoor[e].originalz = g_Entity[e]['z']
		colx, coly, colz = GetObjectColCentre( g_Entity[e]['obj'] )
		g_trapdoor[e].originalx = g_trapdoor[e].originalx + colx
		g_trapdoor[e].originaly = g_trapdoor[e].originaly
		g_trapdoor[e].originalz = g_trapdoor[e].originalz + colz
		return
	end
	
	if controlEnt == nil then controlEnt = e end	
	local timeThisFrame = g_Time
	local timeSlice = 500 / 60
	if controlEnt == e then
		if timeLastFrame == nil then 
			timeLastFrame = timeThisFrame
			timeDiff = 1
		else
			timeDiff = (timeThisFrame - timeLastFrame)/timeSlice
			timeLastFrame = timeThisFrame
		end
	end

	-- determine if local to door
	ontrapdoor[e]=IntersectAll(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,g_Entity[e]['x'],-1,g_Entity[e]['z'],0)
	if ontrapdoor[e] ~= g_Entity[e]['obj'] then
		ontrapdoor[e] = 0
	else
		ontrapdoor[e] = 1
	end	

	if ontrapdoor[e] == 1 then	
		-- handle door when closed
		if door.state == 'Closed' then
			local tcanopennow = 0
			if door.door_type == 'Auto' then
				tdotheopennow = 1
				if tdotheopennow == 1 then
					SetAnimationFrames( 1, 30)
					PlayAnimation( e )
					PlaySound( e, 0 )
					PerformLogicConnections(e)
					door.state = 'Trap'
					door.timer = timeThisFrame + 500					
				end
			end
		end		
	end
	
	if door.state == 'Open' then
		if door.door_type == 'Auto' then
			if g_Time > delaytimer[e] then
				door.state = 'Closing' 
				PlaySound( e, 1 )
			end
		end
	end
	
	if door.state == 'Trap' and timeThisFrame > door.timer then
		door.state = 'Opening'
		door.timer = math.huge
	end
	
	if door.state == 'Opening' then
		if door.angle < 90 then
			door.angle = door.angle + timeDiff
			local rotAng = door.angle
			local rotAng = 90
			if names[ e ] == 'Right' then rotAng = -rotAng end
			local rotq = Q.FromEuler( 0, rad( rotAng ), 0 )
			local ax, ay, az = Q.ToEuler( Q.Mul( door.quat, rotq ) )
			CollisionOff( e )
			ResetRotation( e, deg( ax ), deg( ay ), deg( az ) )
			CollisionOn( e )
			delaytimer[e] = g_Time + (door.door_delay*1000)
		else
			door.state = 'Open' 
			door.blocking = 2
		end

	elseif door.state == 'Closing' then
		if door.angle > 0 then
			door.angle = door.angle - timeDiff			
			local rotAng = door.angle			
			if names[ e ] == 'Right' then rotAng = -rotAng end
			local rotq = Q.FromEuler( 0, rad( rotAng), 0 )
			local ax, ay, az = Q.ToEuler( Q.Mul( door.quat, rotq ) )
			CollisionOff( e )
			ResetRotation( e, deg( ax ), deg( ay ), deg( az ) )
			CollisionOn( e )
		else
			door.state = 'Closed' 
			door.blocking = 1
		end
	end
	
	-- navmesh blocker system
	if door.blocking ~= 0 then
		local blockmode = 0
		if door.blocking == 1 then blockmode = 1 end
		if door.blocking == 2 then blockmode = 0 end
		RDBlockNavMesh(door.originalx,door.originaly,door.originalz,30,blockmode)
		door.blocking = 0
	end
	
end
