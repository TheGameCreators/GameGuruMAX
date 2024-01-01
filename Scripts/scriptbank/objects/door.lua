-- Door v24 by Lee and Necrym59
-- DESCRIPTION: Open and closes an 'animating' door when the player is within [Range=70(50,500)],
-- DESCRIPTION: and when triggered will open the door, play <Sound0> and turn collision off after a delay of [DELAY=1000].
-- DESCRIPTION: When the door is closed, play <Sound1> is played. You can elect to keep the door [Unlocked!=1], and customize the [LockedText$="Door locked. Find key"].
-- DESCRIPTION: Select if the door also [CannotClose!=0], and customize the [ToOpenText$="to open door"]
-- DESCRIPTION: Select if the door can [AutoClose!=0] after a [AutoCloseDelay=5000]

local U = require "scriptbank\\utillib"
local NAVMESH = require "scriptbank\\navmeshlib"

local door 			= {}
local door_pressed 	= {}
local tEnt 			= {}
local autodelay		= {}
local selectobj 	= {}

function door_properties(e, range, delay, unlocked, lockedtext, cannotclose, toopentext, autoclose, autoclosedelay)
	door[e]['range'] = range
	door[e]['delay'] = delay
	door[e]['unlocked'] = unlocked
	door[e]['lockedtext'] = lockedtext
	door[e]['cannotclose'] = cannotclose
	door[e]['toopentext'] = toopentext
	door[e]['autoclose'] = autoclose
	door[e]['autoclosedelay'] = autoclosedelay or 0
end

function door_init(e)
	door[e] = {}
	door[e]['mode'] = 0
	door[e]['blocking'] = 1
	door[e]['originalx'] = -1
	door[e]['originaly'] = -1
	door[e]['originalz'] = -1
	door_pressed[e] = 0
	tEnt[e] = 0
	autodelay[e] = math.huge
	selectobj[e] = 0
end

function door_main(e)

	if door[e]['range'] == nil then door[e]['range'] = 70 end
	if door[e]['delay'] == nil then door[e]['delay'] = 1000 end
	if door[e]['unlocked'] == nil then door[e]['unlocked'] = 1 end
	if door[e]['lockedtext'] == nil then door[e]['lockedtext'] = "Door locked. Find key" end
	if door[e]['toopentext'] == nil then door[e]['toopentext'] = "to open door" end
	
	if door[e]['originalx'] == -1 then
		door[e]['originalx'] = g_Entity[e]['x']
		door[e]['originaly'] = g_Entity[e]['y']
		door[e]['originalz'] = g_Entity[e]['z']
		return
	end
	
	local PlayerDist = GetPlayerDistance(e)
	local nRange = door[e]['range']
	if nRange == nil then nRange = 100 end
	
	if door[e]['unlocked'] == 1 then
		door[e]['unlocked'] = 0
		SetEntityHasKey(e,1)
		g_Entity[e]['haskey'] = 1
	else
		-- if was spawned at start, and it was locked, and then activated, this means we want to unlock the door
		if GetEntitySpawnAtStart(e) == 1 and g_Entity[e]['activated'] == 1 then
			SetEntityHasKey(e,1)
			g_Entity[e]['haskey'] = 1
		end
	end
	
	local LookingAt = GetPlrLookingAtEx(e,1)
	if PlayerDist < nRange and GetEntityVisibility(e) == 1 and LookingAt == 1 then
		-- pinpoint select object--
		local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
		local rayX, rayY, rayZ = 0,0,nRange
		local paX, paY, paZ = math.rad(GetCameraAngleX(0)), math.rad(GetCameraAngleY(0)), math.rad(GetCameraAngleZ(0))
		rayX, rayY, rayZ = U.Rotate3D(rayX, rayY, rayZ, paX, paY, paZ)
		selectobj[e]=IntersectAll(px,py,pz, px+rayX, py+rayY, pz+rayZ,e)
		if selectobj[e] ~= 0 or selectobj[e] ~= nil then
			if g_Entity[e]['obj'] == selectobj[e] then
				TextCenterOnXColor(50-0.01,50,3,"+",255,255,255) 
				tEnt[e] = e
			else
				tEnt[e] = 0
			end
			if g_Entity[e]['obj'] ~= selectobj[e] then selectobj[e] = 0 end
		end
		if selectobj[e] == 0 or selectobj[e] == nil then tEnt[e] = 0 end
		--end pinpoint select object--

		if PlayerDist < nRange and GetEntityVisibility(e) == 1 and tEnt[e] ~= 0 then
			if g_Entity[e]['haskey'] ~= 1 then
				Prompt(door[e]['lockedtext'])
			else
				if door[e]['mode'] == 0 then
					if 1 then
						if GetGamePlayerStateXBOX() == 1 then
							Prompt("Press Y button " .. door[e]['toopentext'])
						else
							if GetHeadTracker() == 1 then
								Prompt("Trigger to " .. door[e]['toopentext'])
							else
								Prompt("Press E to " .. door[e]['toopentext'])
							end
						end
						if g_KeyPressE == 1 and g_Entity[e]['animating'] == 0 and door_pressed[e] == 0 then
							door[e]['mode'] = 201
							door_pressed[e] = 1
							autodelay[e] = g_Time + door[e]['autoclosedelay']
						end
					end
				else
					if door[e]['mode'] == 1 then
						if door[e]['cannotclose'] == 0 then
							if g_KeyPressE == 1 and g_Entity[e]['animating'] == 0 and door_pressed[e] == 0 then
								door[e]['mode'] = 202
								door_pressed[e] = 1
							end
						end
					end						
				end				
			end
		end
	end
	if door[e]['autoclose'] == 1 and door[e]['cannotclose'] == 0 and g_Entity[e]['animating'] == 0 then
		if g_Time > autodelay[e] then
			door[e]['mode'] = 202
			door_pressed[e] = 1
			autodelay[e] = math.huge
		end
	end	
	if door[e]['mode'] == 201 then
		-- open door trigger
		SetAnimationName(e,"open")
		PlayAnimation(e)
		door[e]['mode'] = 2
		PlaySound(e,0)
		PerformLogicConnections(e)
		StartTimer(e)
	end
	if door[e]['mode'] == 2 then
		-- door collision after X second
		if GetTimer(e)>door[e]['delay'] then
			CollisionOff(e)
			door[e]['mode'] = 1
			door[e]['blocking'] = 2
		end
	end
	if door[e]['mode'] == 202 then
		-- close door trigger
		SetAnimationName(e,"close")
		PlayAnimation(e)
		door[e]['mode'] = 3
		StartTimer(e)
	end
	if door[e]['mode'] == 3 then
		-- door collision after X second
		if GetTimer(e)>door[e]['delay'] then
			CollisionOn(e)
			PlaySound(e,1)
			PerformLogicConnections(e)
			door[e]['mode'] = 0
			door[e]['blocking'] = 1			
		end
	end
	if g_KeyPressE == 0 then
		door_pressed[e] = 0
	end
	
	-- navmesh blocker system
	door[e]['blocking'] = NAVMESH.HandleBlocker(e,door[e]['blocking'],door[e]['originalx'],door[e]['originaly'],door[e]['originalz'])

	
end