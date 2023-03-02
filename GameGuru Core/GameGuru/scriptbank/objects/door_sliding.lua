-- Door Sliding v6 - thanks to Necrym59 and AmenMoses
-- DESCRIPTION: Open and close a sliding door. 
-- DESCRIPTION: [ANGLE=0(0,360)] [DISTANCE=90] 
-- DESCRIPTION: [DOOR_DELAY=90] milliseconds [PERIOD=1500] milliseconds.
-- DESCRIPTION: When door opening, play <Sound0>. 
-- DESCRIPTION: When the door is closing, play <Sound1>. 
-- DESCRIPTION: Customize the [LockedText$="Door is locked. Find a way to open it"] 
-- DESCRIPTION: and optionally [!OpenWithEKey=1]
-- DESCRIPTION: [UnLockedText$="E to open door"] 
-- DESCRIPTION: [@DOOR_TYPE=2(1=Auto, 2=Manual, 3=DelayedClose)]
-- DESCRIPTION: [DOOR_RANGE=100(0,500)]
-- DESCRIPTION: [CLOSE_DELAY#=5.0] seconds

local Q = require "scriptbank\\quatlib"
local U = require "scriptbank\\utillib"
local V = require "scriptbank\\vectlib"

local rad = math.rad
local deg = math.deg

local modf = math.modf

local defaultDoorType     = 'Manual'
local defaultDoorRange    = 100
local defaultAngle        = 0
local defaultDistance     = 90
local defaultDoorDelay	  = 200
local defaultPeriod       = 1500
local defaultCloseDelay   = 5000
local defaultLockedText   = "Door is locked. Find a way to open it"
local defaultOpenWithEKey = false
local defaultUnLockedText = "E to open door"

g_door_sliding = {}

local doorTypes = { 'Auto', 'Manual', 'DelayedClose' }

function door_sliding_properties( e, angle, distance, door_delay, period, lockedtext, 
                                  openwithekey, unlockedtext, door_type, door_range,
								  closedelay )
	local door = g_door_sliding[ e ]
	if door == nil then return end
	if door_type ~= nil then
		door.door_type = doorTypes[ door_type ]    
	end
	door.door_range = door_range or defaultDoorRange
	door.angle      = angle      or defaultAngle
	door.distance   = distance   or defaultDistance
	door.door_delay = door_delay or defaultDoorDelay
	door.period     = period     or defaultPeriod
	if closedelay ~= nil then
		door.closedelay = closedelay * 1000
	end
	if openwithekey ~= nil then
		door.openwithEkey = openwithekey == 1
	end
	if lockedtext ~= nil then
		door.lockedtext = lockedtext
	end
	if unlockedtext ~= nil then
		door.unlockedtext = unlockedtext
	end
end 

function door_sliding_init( e )
	g_door_sliding[ e ] = { mode         = 'init',
	                        raiseoffset  = 0,
							door_type    = defaultDoorType,
							door_range   = defaultDoorRange,
							door_delay   = defaultDoorDelay,
							angle        = defaultAngle,
							distance     = defaultDistance,
							period       = defaultPeriod,
							lockedtext   = defaultLockedText,
							openwithEkey = defaultOpenWithEKey
					      }	
end
	 
local function LookingAt( e )
	return ( g_PlayerHealth > 0 and
			 GetPlrLookingAt( e ) == 1 )
end

local function PositionDoor( e, door )
	local v = V.Add( door.pos, V.Mul( door.vec, door.raiseoffset ) )
	CollisionOff( e )
	PositionObject( door.obj, v.x, v.y, v.z )
	CollisionOn( e)
end

local lastTime  = 0
local timeDiff  = 1
local timeSlice = 1000 / 60

function door_sliding_main(e)	
	local door = g_door_sliding[ e ]
	if door == nil then return end
	
	local timeNow = g_Time
	if timeNow > lastTime then
		timeDiff = ( timeNow - lastTime ) / timeSlice
		lastTime = timeNow
	end	

	local Ent = g_Entity[ e ]

	if door.mode == 'init' then
		door.obj = g_Entity[ e ].obj
		
		local x, y, z, xa, ya, za = GetObjectPosAng( door.obj )
		
		door.quat  = Q.FromEuler( rad( xa ), rad( ya ), rad( za ) )
		local quat = Q.Mul( door.quat, Q.FromEuler( 0, 0, rad( door.angle ) ) )
		xa, ya, za = Q.ToEuler( quat )
		door.pos   = V.Create( x, y, z )
		door.perf  = door.distance / door.period
		door.vec   = V.FromEuler( xa, ya, za, true )
		colx, coly, colz = GetObjectColCentre( door.obj )
		if Ent.activated == 0 then
			door.mode = 'closed'
			RDBlockNavMesh( door.pos.x+colx, door.pos.y, door.pos.z+colz, 30, 1 )
		else
			door.mode = 'open'
			RDBlockNavMesh( door.pos.x+colx, door.pos.y, door.pos.z+colz, 30, 0 )
			door.closeTime = 0
		end
		
	elseif
	   door.mode == 'closed' then
		if Ent.activated == 0 then
			if U.PlayerCloserThanPos( door.pos.x, door.pos.y, door.pos.z, 
			                          door.door_range ) then
				if door.door_type == 'Auto' then
					SetEntityActivated( e, 1 )
					PerformLogicConnections(e)
				elseif
				   door.door_type == 'Manual' and
				   LookingAt( e ) then
					
					if door.openwithEkey then
						Prompt( door.unlockedtext )					
						if g_KeyPressE == 1 then
							SetEntityActivated( e, 1 )
							PerformLogicConnections(e)
						end
					else
						Prompt( door.lockedtext )	
					end
				end
			end
		else
			door.mode     = 'delay'
			door.nextMode = 'opening'
			door.time     =  g_Time + door.door_delay
			PlaySound( e, 0 )
		end
		
	elseif
	   door.mode == 'delay' then
		if g_Time >= door.time then		
			door.mode = door.nextMode
		end
		PositionDoor( e, door )
		
	elseif
	   door.mode == 'opening' then
	   	door.raiseoffset = door.raiseoffset + door.perf * timeSlice * timeDiff
		if door.raiseoffset >= door.distance then
			door.raiseoffset = door.distance
			door.mode = 'open'
			colx, coly, colz = GetObjectColCentre( door.obj )
			RDBlockNavMesh( door.pos.x+colx, door.pos.y, door.pos.z+colz, 30, 0 )
			if door.door_type == 'DelayedClose' then
				door.closeTime = g_Time + door.closedelay
			end
		end
		PositionDoor( e, door )
		
	elseif
	   door.mode == 'open' then
		if ( door.door_type == 'Auto' and 
		     not U.PlayerCloserThanPos( door.pos.x, door.pos.y, door.pos.z, 
			                            door.door_range ) ) or
		   ( door.door_type == 'Manual' and
		     ( Ent.activated == 0 or 
			   ( door.openwithEkey and
			     g_KeyPressE == 1  and 
			     LookingAt( e ) and U.PlayerCloserThanPos( door.pos.x, door.pos.y, door.pos.z, 
			                          door.door_range ) ) ) ) or
		   ( door.door_type == 'DelayedClose' and 
		     g_Time >= door.closeTime ) then
			door.mode     = 'delay'			
			door.nextMode = 'closing'
			door.time     =  g_Time + door.door_delay
			PlaySound( e, 1 )
		end
		door.raiseoffset = door.distance 
		PositionDoor( e, door )		
		
	elseif
	   door.mode == 'closing' then
		door.raiseoffset = door.raiseoffset - door.perf * timeSlice * timeDiff
		if door.raiseoffset <= 0 then
			door.raiseoffset = 0
			door.mode = 'closed'
			colx, coly, colz = GetObjectColCentre( door.obj )
			RDBlockNavMesh( door.pos.x+colx, door.pos.y, door.pos.z+colz, 30, 1 )
		end
		PositionDoor( e, door )
		SetEntityActivated( e, 0 )		
	end
end

  
