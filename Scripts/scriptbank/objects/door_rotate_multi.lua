-- Door Rotate Multi - v25 Necrym59 and AmenMoses and Lee
-- DESCRIPTION: Rotates a non-animating door by zone, switch or player.
-- DESCRIPTION: [@DOOR_STYLE=1(1=Manual,2=Switch/Zone, 3=Auto)]
-- DESCRIPTION: Change the [OPEN_PROMPT$="Press E to open door"]
-- DESCRIPTION: Change the [CLOSE_PROMPT$="Press E to close door"]
-- DESCRIPTION: [@ROTATION_STYLE=1(1=Right,2=Left)]
-- DESCRIPTION: [#ROTATION_SPEED=3.0(1.0,60.0)](seconds)
-- DESCRIPTION: [USE_RANGE=80(1,300)]
-- DESCRIPTION: [@PROMPT_DISPLAY=2(1=Local,2=Screen)]
-- DESCRIPTION: Play <Sound0> when opening
-- DESCRIPTION: Play <Sound1> when closing

local module_misclib = require "scriptbank\\module_misclib"
local Q = require "scriptbank\\quatlib"
local U = require "scriptbank\\utillib"
local NAVMESH = require "scriptbank\\navmeshlib"
g_tEnt = {}

local deg = math.deg
local rad = math.rad

local doors				= {}
local hingeTypes		= {'left','right'}
local keyPressed		= {}
local timeLastFrame		= {}
local timeThisFrame		= {}
local timeDiff 			= {}
local controlEnt		= {}
local tEnt 				= {}
local selectobj 		= {}

function door_rotate_multi_properties( e, door_style, open_prompt, close_prompt, rotation_style, rotation_speed, use_range, prompt_display)
	local door = doors[ e ]
	if door == nil then return end
	door.door_style = door_style
	door.open_prompt  = open_prompt
	door.close_prompt  = close_prompt
	door.hinge = hingeTypes[ rotation_style ]
	door.rotation_speed = rotation_speed * 1000 / 60
	door.use_range = use_range
	door.prompt_display = prompt_display
end

function door_rotate_multi_init( e )
	doors[ e ] ={ state = 'init' }
	keyPressed[e] = false
	timeLastFrame[e] = nil
	timeThisFrame[e] = nil
	timeDiff[e] = 1
	controlEnt[e] = nil
	tEnt[e] = 0
	selectobj[e] = 0
end

function door_rotate_multi_main( e )
	local PlayerDist = GetPlayerDistance(e)
	local door = doors[ e ]
	if door == nil then return end

	local Ent = g_Entity[ e ]
	if controlEnt[e] == nil then controlEnt[e] = e end

	timeThisFrame[e] = g_Time

	if controlEnt[e] == e then
		if timeLastFrame[e] == nil then
			timeLastFrame[e] = timeThisFrame[e]
			timeDiff[e] = 1
		else
			timeDiff[e] = ( timeThisFrame[e] - timeLastFrame[e] ) / 20
			timeLastFrame[e] = timeThisFrame[e]
		end
	end

	if door.state == 'init' then
		local x, y, z, ax, ay, az = GetObjectPosAng( Ent.obj )
		door.obj   = Ent.obj
		door.timer = math.huge
		door.quat  = Q.FromEuler( rad( ax ), rad( ay ), rad( az ) )
		door.blocking = 1
		door.origx = Ent.x
		door.origy = Ent.y
		door.origz = Ent.z
		door.actvd = Ent.activated == 1
		door.rInc  = 90 / door.rotation_speed
		if door.actvd then
			door.angle = 90
			door.state = 'open'
			RotateDoor( e, door )
		else
			door.angle = 0
			door.state = 'closed'
			door.blocking = 1
		end

	elseif
	   door.state == 'closed' then
		if Ent.activated == 1 then
			door.state = 'opening'
			PlaySound( e, 0 )
		end

		local LookingAt = GetPlrLookingAtEx(e,1)
		if PlayerDist < door.use_range and GetEntityVisibility(e) == 1 and LookingAt == 1 then
			--pinpoint select object--
			module_misclib.pinpoint(e,door.use_range,0)
			tEnt[e] = g_tEnt
			--end pinpoint select object--
		end

		if PlayerDist < door.use_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then
			if door.door_style == 1 then
				if door.prompt_display == 1 then TextCenterOnX(50,52,1,door.open_prompt) end
				if door.prompt_display == 2 then Prompt(door.open_prompt) end
				if g_KeyPressE == 1 then
					PlaySound( e, 0 )
					door.state = 'opening'
					keyPressed[e] = true
				end
			end
			if door.door_style == 3 then
				PlaySound( e, 0 )
				door.state = 'opening'
				keyPressed[e] = true
			end
		end

	elseif
	   door.state == 'open' then
		if Ent.activated == 0 then
			door.state = 'closing'
			PlaySound( e, 1 )
		end
		if PlayerDist < door.use_range then
			--pinpoint select object--
			module_misclib.pinpoint(e,door.use_range,0)
			tEnt[e] = g_tEnt
			--end pinpoint select object--
		end
		if PlayerDist < door.use_range and tEnt[e] ~= 0 then
			if door.door_style == 1 then
				if door.prompt_display == 1 then TextCenterOnX(50,52,1,door.close_prompt) end
				if door.prompt_display == 2 then Prompt(door.close_prompt) end
				if g_KeyPressE == 1 then
					door.state = 'closing'
					keyPressed[e] = true
					PlaySound( e, 1 )
				end
			end
			if door.door_style == 3 then
				PlaySound( e, 0 )
				door.state = 'closing'
				keyPressed[e] = true
			end
		end

	elseif
	   door.state == 'opening' then
		door.blocking = 2
		door.blocking = NAVMESH.HandleBlocker(e,door.blocking,door.origx,door.origy,door.origz)
		if door.angle < 90 then
			door.angle = door.angle + door.rInc * timeDiff[e]
			RotateDoor( e, door )
		else
			door.state = 'open'
			SetEntityActivated( e, 1 )
			door.blocking = 2
		end

	elseif
	   door.state == 'closing' then
		if door.angle > 0 then
			door.angle = door.angle - door.rInc * timeDiff[e]
			RotateDoor( e, door )
		else
			door.state = 'closed'
			SetEntityActivated( e, 0 )
			door.blocking = 1
		end
	end
	-- navmesh blocker system
	door.blocking = NAVMESH.HandleBlocker(e,door.blocking,door.origx,door.origy,door.origz)
	-- restore logic
	if g_EntityExtra[e]['restoremenow'] ~= nil then
     if g_EntityExtra[e]['restoremenow'] == 1 then
      g_EntityExtra[e]['restoremenow'] = 0
	  if door.state == 'closed' then
	   door.blocking = 1
	  end
     end
	end	
end

function RotateDoor( e, door )
	local rotAng = door.angle
	if door.hinge == 'right' then rotAng = -rotAng end
	local rotq = Q.FromEuler( 0, rad( rotAng ), 0 )
	local ax, ay, az = Q.ToEuler( Q.Mul( door.quat, rotq ) )
	CollisionOff( e )
	ResetRotation( e, deg( ax ), deg( ay ), deg( az ) )
	CollisionOn( e )
end