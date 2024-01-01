-- One way Door v3
-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- DESCRIPTION: Rotates a non-animating door when player interacts with it. When door is opened, will close and lock permanently after a few seconds delay or when closed.
-- DESCRIPTION: <Sound0> When door is opened.
-- DESCRIPTION: <Sound1> When the door is closing.
-- DESCRIPTION: [CLOSE_DELAY=2(0,30)]

local Q = require "scriptbank\\quatlib"
local U = require "scriptbank\\utillib"
local NAVMESH = require "scriptbank\\navmeshlib"

local deg = math.deg
local rad = math.rad

local doors = {}
local names = {}
local keyPressed = false
local oneway_door = {}
local close_delay = {}

function oneway_door_properties( e, close_delay)
	oneway_door[e] = g_Entity[e]
	oneway_door[e].close_delay = close_delay
end

function oneway_door_init_name( e, name )
	Include( "quatlib.lua" )
	Include( "utillib.lua" )
	names[ e ] = name
	oneway_door[e] = g_Entity[e]
	oneway_door[e].close_delay = 3
end

local timeLastFrame = nil
local timeDiff      = 1
local controlEnt    = nil

function oneway_door_main(e)
	oneway_door[e] = g_Entity[e]
	local door = doors[ e ]
	
	if door == nil then
		-- initialise
		local Ent = g_Entity[ e ]
		local _, _, _, ax, ay, az = GetObjectPosAng( Ent.obj )
		doors[ e ] = { obj = Ent.obj, timer = math.huge, state = 'Closed', angle = 0, quat = Q.FromEuler( rad( ax ), rad( ay ), rad( az ) ), blocking = 1 }
		doors[ e ].originalx   = -1
		doors[ e ].originaly   = -1
		doors[ e ].originalz   = -1
		return
	end
	
	if doors[ e ].originalx == -1 then
	 doors[ e ].originalx = g_Entity[e]['x']
	 doors[ e ].originaly = g_Entity[e]['y']
	 doors[ e ].originalz = g_Entity[e]['z']
	 return
	end

	if controlEnt == nil then controlEnt = e end
	
	local timeThisFrame = g_Time
	
	if controlEnt == e then

		if timeLastFrame == nil then 
			timeLastFrame = timeThisFrame
			timeDiff = 1
		else
			timeDiff = ( timeThisFrame - timeLastFrame ) / 20
			timeLastFrame = timeThisFrame
		end
	end
	
	if U.PlayerLookingNear( e, 100, 140 ) then
		if door.state == 'Locked' then
			Prompt( "Door is Locked" )
		end
		if door.state == 'Closed' then
			Prompt( "Press E to open door" )
			if g_KeyPressE == 1 then
				if not keyPressed then
					SetAnimationFrames( 1, 30)
					PlayAnimation( e )
					PlaySound( e, 0 )
					PerformLogicConnections(e)
					door.state = 'Knob'
					door.timer = timeThisFrame + 500
					keyPressed = true
				end
			else
				keyPressed = false
			end
		elseif
			door.state == 'Open' then						
			Prompt( "Press E to close door" )			
			if g_KeyPressE == 1 then
				if not keyPressed then
					door.state = 'Closing' 
					keyPressed = true										
				end
			else				
				keyPressed = false
			end
		end
	end
	
	if oneway_door[e].close_delay > 0 then
		if door.state == 'Open' and GetTimer(e) > oneway_door[e].close_delay*1000 then	
			g_KeyPressE = 1
			door.state = 'Closing' 
			keyPressed = true			
		end
	end
	if door.state == 'Knob' and timeThisFrame > door.timer then
		door.state = 'Opening'
		door.timer = math.huge
	end
	
	if door.state == 'Opening' then		
		if door.angle < 90 then
			door.angle = door.angle + timeDiff
			local rotAng = door.angle
			if names[ e ] == 'Right' then rotAng = -rotAng end
			local rotq = Q.FromEuler( 0, rad( rotAng ), 0 )
			local ax, ay, az = Q.ToEuler( Q.Mul( door.quat, rotq ) )
			CollisionOff( e )
			ResetRotation( e, deg( ax ), deg( ay ), deg( az ) )
			CollisionOn( e )
			StartTimer(e)
		else
			door.state = 'Open' 
			door.blocking = 2			
		end		
	elseif
		door.state == 'Closing' then
		if door.angle > 0 then
			door.angle = door.angle - timeDiff
			local rotAng = door.angle
			if names[ e ] == 'Right' then rotAng = -rotAng end
			local rotq = Q.FromEuler( 0, rad( rotAng ), 0 )
			local ax, ay, az = Q.ToEuler( Q.Mul( door.quat, rotq ) )
			CollisionOff( e )
			ResetRotation( e, deg( ax ), deg( ay ), deg( az ) )
			CollisionOn( e )
		else
			PlaySound( e, 1 )
			door.state = 'Closed' 
			door.state = 'Locked'
			door.blocking = 1
		end
	end	

	-- navmesh blocker system
	door.blocking = NAVMESH.HandleBlocker(e,door.blocking,door.originalx,door.originaly,door.originalz)
end
