-- DESCRIPTION: Will animate and move this object as though a fish.
-- Fish V3
-- DESCRIPTION: [RandomScale!=1]
local U = require "scriptbank\\utillib"
local Q = require "scriptbank\\quatlib"
local V = require "scriptbank\\vectlib"
local P = require "scriptbank\\physlib"

local random = math.random

math.randomseed( os.time() )
random(); random(); random()

local modf = math.modf
local rad  = math.rad
local deg  = math.deg
local atan = math.atan2
local min  = math.min
local max  = math.max
local abs  = math.abs
local pi   = math.pi

local fishes   = {}

local maxSpeed  = 800
local scareDist = 150
local lastTime  = 0
local procTime  = 0
local tDiff     = 1
local waterHgt  = 0
local plyrMoved = false

local ppx, ppy, ppz = 0, 0, 0
local opx, opy, opz = 0, 0, 0

local function randVal( low, high )
	return low + random() * ( high - low ) 
end

function fish_properties( e, RandomScale )
	fishes[ e ].RandomScale = RandomScale
end

function fish_init_name( e, name )
	local Ent = g_Entity[ e ]
	fishes[ e ] = { state = 'init', e = e, obj = Ent.obj, 
					species = name,
					scale = randVal( 40, 112 ) / 100,															
					RandomScale = 1 }
	CollisionOff( e )
	MoveWithAnimation( e, 0 )
	--	Scale( e, fishes[ e ].scale * 100 ) **moved to state init in main 
end

local function chance( val )
	return random() < val
end

local function timedEvent( timeNow, fish, tmin, tmax )
	if fish.timer == nil then
		fish.timer = timeNow + randVal( tmin, tmax )
	elseif
	   timeNow > fish.timer then
		fish.timer = timeNow + randVal( tmin, tmax )
		return true
	end
end

local function scarefish( fish, dist )
	if plyrMoved and U.CloserThan( fish.x, fish.y, fish.z, ppx, ppy, ppz, dist ) then
		return 1
	elseif 
	   g_PlayerGunFired == 1 and
	   U.CloserThan( fish.x, fish.y, fish.z, ppx, ppy, ppz, 700 ) then
		return 2
	end
end

local function wrapAng( ang )
	if ang <   0 then return ang + 360 end
	if ang > 360 then return ang - 360 end
	return ang
end

local function angDiff( ang1, ang2 )
	if ang1 < ang2 then 
		return abs( wrapAng( ang1 - ang2 ) )
	else
		return abs( wrapAng( ang2 - ang1 ) )
	end
end

local function addAng( ang1, ang2 )
	return wrapAng( ang1 + ang2 )
end

local function setAvoid( fish, rot )
	if not fish.avoiding then
		fish.avoiding = true
		fish.rot = addAng( fish.rot, rot )
	else
		fish.tgtSpd = fish.tgtSpd * 0.99
	end
end

local function normAngle( ang )
	if ang < 0 then return 360 + ang end
	return ang
end

local function angleToPlayer( fish )
	return normAngle( deg( atan( fish.x - ppx, fish.z - ppz ) ) )
end

local function angleToFish( f1, f2 )
	return normAngle( deg( atan( f1.x - f2.x, f1.z - f2.z ) ) )
end

-- fish face -Z direction
local flv = V.Create(  10, 0, -40 )
local fcv = V.Create(   0, 0, -40 )
local frv = V.Create( -10, 0, -40 )

local function avoidObjects( e, fish, timeNow )
	
	local ax, ay, az = rad( fish.ax ), rad( fish.ay ), rad( fish.az )
	
	local posV = V.Create( fish.x, fish.y, fish.z )
	
	-- calculate 3 points ahead of fish
	local plv = V.Rot( flv, ax, ay, az )
	local pcv = V.Rot( fcv, ax, ay, az )
	local prv = V.Rot( frv, ax, ay, az )
	
	local plp = V.Add( posV, plv )
	local pcp = V.Add( posV, pcv )
	local prp = V.Add( posV, prv )
	
	-- check for terrain at locations
	local plh = GetTerrainHeight( plp.x, plp.z )
	local pch = GetTerrainHeight( pcp.x, pcp.z )
	local prh = GetTerrainHeight( prp.x, prp.z )
	
	local Y = fish.y - fish.yoff
	local terL = plh > Y
	local terC = pch > Y
	local terR = prh > Y
		
	if terL and terR then 
		if chance( 0.5 ) then
			setAvoid( fish, -150 )
		else
			setAvoid( fish, 150 )
		end	
		fish.tgtHgt = min( waterHgt - fish.yoff, fish.tgtHgt + 5 )
		
	elseif
	   terL then
		if terC then setAvoid( fish, 90 ) else setAvoid( fish, 45 ) end
		fish.tgtHgt = min( waterHgt - fish.yoff, fish.tgtHgt + 5 )
		
	elseif
	   terR then
		if terC then setAvoid( fish, -90 ) else setAvoid( fish, -45 ) end
		fish.tgtHgt = min( waterHgt - fish.yoff, fish.tgtHgt + 5 )
		
	elseif
	   terC then 
		if fish.y < waterHgt - fish.yoff then 
			fish.tgtSpd = fish.tgtSpd * 0.99
			fish.tgtHgt = min( waterHgt - fish.yoff, fish.tgtHgt + 5 )
		else
			setAvoid( fish, -180 )
		end
		
	elseif 
	   fish.state == 'flee' and
	   not fish.avoiding    and
	   U.CloserThan( pcp.x, pcp.y, pcp.z, ppx, ppy, ppz, 50 ) then 
		if chance( 0.5 ) then
			setAvoid( fish, -150 )
		else
			setAvoid( fish, 150 )
		end
	else
		fish.avoiding = false
	end
	
	-- TBD, avoid objects, i.e. raycast in front of fish and if any hits take avoiding action
	if fish.procTimer3 == nil or 
	   procTime > fish.procTimer3 then
		fish.procTimer3 = procTime + randVal( 100, 200 )
		if not fish.avoiding then	
		
			local obj = 0
			if fish.e ~= nil then
			 --obj = IntersectAll( fish.x, fish.y, fish.z,  pcp.x, fish.y, pcp.z, fish.obj )	
			 obj = IntersectStaticPerformant(fish.x, fish.y, fish.z,  pcp.x, fish.y, pcp.z, fish.obj, fish.e, 500 )
			end
									   
			if obj ~= nil and obj > 0 then 
				fish.procTimer3 = nil
				if fish.speed > 0 then fish.speed = fish.speed - 1 end
				if fish.tgtSpd > 0 then fish.tgtSpd = fish.tgtSpd - 1 end
				setAvoid( fish, -15 ) -- predictable evasion direction
			end
		end
	end

	-- TBD, avoid other fish, i.e. for each entry in fish.fishList check:
	--		a) if fish is in front of us and same species match speed and rotation with it
	--		b) if fish is different species turn away from it
	--      c) other behaviours?
	if fish.procTimer2 == nil or 
	   procTime > fish.procTimer2 then
		fish.procTimer2 = procTime + randVal( 100, 200 )
		if not fish.avoiding and 
		   fish.state ~= 'flee' then
			for _, v in pairs( fish.fishList ) do
				local fish2 = fishes[ v.e ]
				if fish.species == fish2.species then
					if fish.willSchool and
				       v.dist < 50 * 50 then
						fish.rot = addAng( fish2.rot, randVal( -1, 1 ) ) 
						fish.tgtSpd = fish2.tgtSpd
						break
					end
				elseif 
				   fish.len < fish2.len and
				   v.dist < 50 * 50 then
					fish.rot = addAng( angleToFish( fish, fish2 ), - 180 + randVal( -90, 90  ) )
					fish.tgtSpd = fish.tgtSpd + 1
					fish.procTimer = nil
					break
				end
			end
		end
	end
	
end

-- function returns squared distance between two points
local function sqrd( x1, y1, z1, x2, y2, z2 )
	local dx, dy, dz = x1 - x2, y1 - y2, z1 - z2
	return dx*dx + dy*dy + dz*dz
end

local function closestFish( fish, dist, num )	
	local entityList = {}
	for k, v in pairs( fishes ) do
		if fish ~= v and
		   U.CloserThan( fish.x, fish.y, fish.z, v.x, v.y, v.z, dist ) then
			entityList[ k ] = sqrd( fish.x, fish.y, fish.z, v.x, v.y, v.z )
		end
	end
	
	local sortedList = {}
	
	-- next sort and place in new list until 'num' entities found
	for k, _ in U.SortPairs( entityList, function( list, a, b ) return list[ a ] < list[ b ] end ) do
		sortedList[ #sortedList + 1 ] = { e = k, dist = entityList[ k ] }
		if #sortedList == num then break end
	end
	
	return sortedList	
end

local procEnt = nil

function fish_main( e )

	local fish = fishes[ e ]
	if fish == 'nil' then return end
	
	local timeNow = g_Time
	
	if timeNow > lastTime or procEnt == nil then
		tDiff = min( 5, ( timeNow - lastTime ) / 60 )
		lastTime = timeNow
		procEnt = e 
	end	
	
	if procEnt == e then
		procTime = timeNow
		waterHgt = GetWaterHeight()
        ppx, ppy, ppz = g_PlayerPosX, g_PlayerPosY, g_PlayerPosZ
		if ppx ~= opx or ppy ~= opy or opz ~= ppz then
			plyrMoved = true
			opx, opy, opz = ppx, ppy, ppz
		else
			plyrMoved = false
		end
	end
	
	fish.x, fish.y, fish.z, fish.ax, fish.ay, fish.az = GetObjectPosAng( fish.obj )

	if fish.state == 'init' then
		if fishes[ e ].RandomScale == 1 then
			Scale( e, fishes[ e ].scale * 100 )
		end
	
		fish.tgtHgt = fish.y
		fish.rot    = fish.ay			   
		fish.speed  = 1	
		local Dims = P.GetObjectDimensions( fish.obj )
		fish.yoff   = Dims.h * fish.scale
		if Dims.w > Dims.l then
			fish.len = Dims.w * fish.scale
		else
			fish.len = Dims.l * fish.scale
		end		


		if fish.y > waterHgt then 
			SetAnimationName( e, "death" )
			MoveUp( e, Dims.w / 2 )
			fish.state = 'dying'
			fish.tgtSpd = 1
		else	
			fish.tgtSpd = randVal(   5, 100 )
			SetAnimationName( e, "idle" )
			fish.timer = 0
			fish.state = 'swim'
		end
		LoopAnimationFrom( e, random() * 100 )
		return
		
	elseif 
	   fish.state == 'dying' then
		-- do nothing for now, maybe in future run a timer
		-- and if we haven't found water by the time it pops
		-- go to a dead state
		return
		
	elseif 
	   fish.state == 'swim' then
		local scare = scarefish( fish, scareDist )
		
		if scare then
			if scare == 1 then
				local ang = angleToPlayer( fish )
				fish.rot = addAng( ang, - 180 + randVal( -45, 45  ) )
			elseif 
			   not fish.avoiding then 
				fish.rot = addAng( fish.rot, randVal( -90, 90 ) ) 
			end
			fish.tgtSpd = randVal( maxSpeed / 2, maxSpeed )
			SetAnimationName( e, "move" )
			LoopAnimationFrom( e, random() * 100 )
			fish.timer = nil
			fish.state = 'flee'
			
		else
			if timedEvent( timeNow, fish, 2000, 5000 ) then
				if not fish.avoiding then 
					fish.rot = addAng( fish.rot, randVal( -40, 40 ) )
					fish.tgtSpd = randVal( 5, 100 )
					fish.grnd = GetTerrainHeight( fish.x, fish.z )
					if fish.grnd + fish.yoff < waterHgt - fish.yoff then
						fish.tgtHgt = randVal( fish.grnd + fish.yoff, waterHgt - fish.yoff )
					end
					fish.willSchool = chance( 0.90 )
				end
			end
		end

	elseif
	   fish.state == 'flee' then
		if timedEvent( timeNow, fish, 2000, 5000 ) then
			SetAnimationName( e, "idle" )
			LoopAnimationFrom( e, random() * 100 )
			fish.tgtSpd = randVal( 5, 100 )
			fish.state = 'swim'
		elseif 
		   not fish.avoiding then 
			fish.rot = addAng( fish.rot, randVal( -1, 1 ) )
			fish.tgtSpd = randVal( maxSpeed / 2, maxSpeed )
		end
	end
	
	if fish.procTimer == nil or
	   procTime > fish.procTimer then
		fish.procTimer = procTime + randVal( 100, 200 )
		fish.fishList = closestFish( fish, fish.len * 5, 5 )
	end
	
	local spdDiff = fish.tgtSpd - fish.speed
	if abs( spdDiff ) > 0.1 then
		fish.speed = fish.speed + spdDiff / 20 * tDiff
		if fish.flee then
			SetAnimationSpeed( e, max( 1, fish.speed / 100 ) )
		else
			SetAnimationSpeed( e, max( 1, fish.speed / 40 ) )
		end
	else
		fish.speed = fish.tgtSpd
	end
	
	avoidObjects( e, fish )
	
	if not fish.avoiding then MoveForward( e, -fish.speed * tDiff ) end
	
	local hgtDiff = fish.tgtHgt - fish.y
	
	
	if abs( hgtDiff ) > 0.1 then
		MoveUp( e, hgtDiff / 19 )
	end

	--PromptLocal( e, fish.state .. ", " .. fish.len )
	if fish.rot and fish.rot ~= 0 then 
		SetRotationYSlowly( e, fish.rot, 7 ) 
	end
end
