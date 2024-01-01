-- DESCRIPTION: Will animate and move this object as though a bird.
local Q = require "scriptbank\\quatlib"
local U = require "scriptbank\\utillib"
local V = require "scriptbank\\vectlib"

local random = math.random

math.randomseed( os.time() )
random(); random(); random()

local modf = math.modf
local rad  = math.rad
local deg  = math.deg
local abs  = math.abs
local atan = math.atan2
local min  = math.min
local max  = math.max
local pi   = math.pi
local find  = string.find
local sub   = string.sub
local lower = string.lower

local birds   = {}
local perches = {}

local maxHeight    =   500
local maxPauseTime =  8000
local maxSpeed     =     2
local spawnTimeMin = 20000
local spawnTimeMax = 40000

local ppx, ppy, ppz = 0, 0, 0

local function switchToCharacter( e, bird )
	SetCharacterMode( e, 1 )
	CollisionOn( e )
end

local function switchToNonCharacter( e, bird )
	SetCharacterMode( e, 0 )
	CollisionOff( e )
end

function bird_init_name( e, name )
	local Ent = g_Entity[ e ]
	local nextPos = find( name, ' ' ) or #name + 1
	birds[ e ] = { state = 'init', e = e, obj = Ent.obj, species = lower( sub( name, 1, nextPos - 1 ) ) }
	SetAnimationFrame( e, 1 )
	SetEntityMoveSpeed( e, 0 ) -- will set move speed to zero while decide where to move this 'non-spine tracked' character
end

local function leavePerch( e )
	for _, v in pairs( perches ) do
		if v.taken == e then
			v.taken = nil
			break
		end
	end
end

local function findPerch( e )
	for k, v in pairs( perches ) do
		if not v.taken then
			return k
		end
	end
end

local function heardSound( e, bird, typ, dist )
	if typ == 'player' then
		local sound = GetNearestSoundDistance( bird.pos.x, bird.pos.y, bird.pos.z, 1 )
		return sound > 0 and sound < dist
	else
		local sound = GetNearestSoundDistance( bird.pos.x, bird.pos.y, bird.pos.z, 2 )
		return sound > 0 and sound < dist
	end
end

local function scare( e, bird, dist )
	if g_PlayerGunFired == 1 and
	   heardSound( e, bird, 'player', 1500 ) then
		return 'gunfire'
	
	elseif
	   U.PlayerCloserThan( e, dist ) then
		return 'player'
		
	elseif 
	   heardSound( e, bird, 'player', dist * 2 ) then
		return 'player'
	
	elseif 
	   heardSound( e, bird, 'other', dist * 2 ) then
		return 'npc'
	end
end

local function randVal( low, high )
	return low + random() * ( high - low ) 
end

local function changeStateLoop( e, bird, newState, animName, rand, stopAnim )
	if animName ~= nil then
		if stopAnim then StopAnimation( e ) end
		SetAnimationName( e, animName )
		if rand then
			LoopAnimationFrom( e, random() * 100 )
		else
			LoopAnimationFrom( e )
		end
	end
	bird.timer = nil
	bird.state = newState
end

local function changeStatePlay( e, bird, newState, animName, nextAnim )
	StopAnimation( e )
	SetAnimationName( e, animName )
	PlayAnimation( e )
	bird.state     = 'transition'
	bird.nextState = newState
	bird.nextAnim  = nextAnim
	bird.timer = nil
end

local function getTerOrWaterHeight( pos )
	return max( GetTerrainHeight( pos.x, pos.z ), GetWaterHeight() )
end

local function timedEvent( timeNow, bird, tmin, tmax, recur )
	if bird.timer == nil then
		bird.timer = timeNow + randVal( tmin, tmax )

	elseif
	   timeNow > bird.timer then
		if recur then
			bird.timer = timeNow + randVal( tmin, tmax )
		end
		return true
	end
end

local function chance( val )
	return random() < val
end

local function canSee( bird, dist, x, y, z )
	local cx, cy, cz, ax, ay, az = GetObjectPosAng( bird.obj )
	if x == nil then
		local xo, yo, zo = U.Rotate3D( 0, 0, dist, rad( ax ), rad( ay ), rad( az ) )
		x, y, z = cx + xo, cy + yo + 5, cz + zo
	end
	
	--local obj = IntersectAll( cx, cy + 5, cz, x, y, z, bird.obj )
	local obj = 0
	if bird.e ~= nil then
	 obj = IntersectStaticPerformant(cx, cy + 5, cz, x, y, z, bird.obj, bird.e, 500 )
	end
	
	if obj == 0 then	
		return true
	end
end

local function pathFound( e, bird )
	local tx, tz = bird.tgt.x, bird.tgt.z
	local ty = bird.pos.y
	if chance( 0.2 ) then 
		if chance( 0.5 ) then
			ty = ty + 100
		else
			ty = ty - 100
		end
	end
	
	ty = RDGetYFromMeshPosition( tx, ty, tz )
	
	RDFindPath( bird.pos.x, bird.pos.y, bird.pos.z, tx, ty, tz )			
	local pc = RDGetPathPointCount()
	if pc > 0 then
		if canSee( bird, 0, RDGetPathPointX(1), RDGetPathPointY(1), RDGetPathPointZ(1) ) then
			StartMoveAndRotateToXYZ( e, bird.speed, bird.turnSpd, 1 )
			return true
		end
	end
end

-- function returns squared distance between two points
local function sqrd( x1, z1, x2, z2 )
	local dx, dz = x1 - x2, z1 - z2
	return dx*dx + dz*dz
end

local function pickNewPos( e, bird, dist )
	for i = 1, 5 do
		local x, z = U.RandomPos( dist, bird.pos.x, bird.pos.z )
		if GetWaterHeight() < GetTerrainHeight( x, z ) then
		    bird.tgt = V.Create( x, 0, z )
			if pathFound( e, bird ) then return true end 
		end
	end
end

local function getRndSpeed( e )
	local spd = GetEntityMoveSpeed( e ) / 100.0
	if spd == 0 then spd = 1 end
	return randVal( spd * 0.85, spd * 1.15 )
end				 

local function selectTargetPos( e, bird )
	return pickNewPos( e, bird, randVal( 100, 300 ) )
end

local function stopNavigating( e, bird )
	MoveAndRotateToXYZ( e, 0, 0 )
	bird.timer  = nil
	bird.tgtSpd = 0
	bird.speed  = 0
	bird.lpos   = V.Create( 0, 0, 0 )
end

local function moveBird( e, bird )
	
	local ny = getTerOrWaterHeight( bird.pos ) + 0.1
	ny = ny + bird.height
	
	local _, _, _, xa, ya, za = GetObjectPosAng( bird.obj )

	local vect = V.FromEuler( rad( xa ), rad( ya ), rad( za ) )

	bird.pos = V.Add( bird.pos, V.Mul( vect, bird.speed * 3 ) ) 
	
	PositionObject( bird.obj, bird.pos.x, ny, bird.pos.z )

	if bird.height < maxHeight then
		bird.height = bird.height + bird.speed * 2
	end
end

local function limitAngle ( Angle )
	while Angle < 0 do Angle = 360 + Angle end
	while Angle > 360 do Angle = Angle - 360 end
	return Angle
end

function playerLookingToward( x, z, dist, fov )
	local pxp, pzp = g_PlayerPosX, g_PlayerPosZ	
	if not U.CloserThan( pxp, 0, pzp, x, 0, z, dist ) then return false end
	
	local angle = limitAngle( atan( x - pxp, z - pzp ) * ( 180.0 / pi ) )
	local pAng  = limitAngle( g_PlayerAngY )

	local L = limitAngle( angle - fov / 2 )
	local R = limitAngle( angle + fov / 2 )
	
	return ( L < R and ( pAng > L and pAng < R ) ) or
	       ( L > R and ( pAng > L or  pAng < R ) ) 
end

local function checkPerch( e, bird, timeNow, dist )
	if bird.perch then
		local pp = bird.perch.pos
		if not U.CloserThan( bird.pos.x, 0, bird.pos.z, pp.x, 0, pp.z, dist ) and
		   not playerLookingToward( bird.pos.x, bird.pos.z, 4000, 140 )       then
			bird.state = 'respawn'
			Hide( e )
			CollisionOff( e )
			bird.timer = nil
		end
	end
end

local function respawn( e, bird, timeNow )
	if timedEvent( timeNow, bird, spawnTimeMin, spawnTimeMax ) then
		local np = perches[ findPerch( e ) ]
		if np == nil then return end
		if not U.PlayerCloserThanPos( np.pos.x, np.pos.y, np.pos.z, 1500 ) and
		   not playerLookingToward( np.pos.x, np.pos.z, 3000, 140 ) then
			np.taken   = e
			bird.perch = np
			pp = bird.perch.pos
			PositionObject( bird.obj, pp.x, pp.y, pp.z )
			RotateObject( bird.obj, 0, bird.perch.ang, 0 )
			switchToCharacter( e, bird )
			Show( e )
			stopNavigating( e, bird )
			bird.pos  = V.Create( pp.x, pp.y, pp.z )
			bird.height = bird.perch.height
			bird.timer = timeNow + random() * maxPauseTime
			changeStateLoop( e, bird, 'perched', 'Idle', true )
			bird.state = 'perched'
		end
	end
end

function bird_main( e )

	local bird = birds[ e ]
	if bird == nil then return end

	local timeNow = g_Time
	
	ppx, ppy, ppz = g_PlayerPosX, g_PlayerPosY, g_PlayerPosZ

	local Ent = g_Entity[ e ]
		
	if bird.state == 'init' then
		
		bird.pos  = V.Create( Ent.x, Ent.y, Ent.z )
		bird.lpos = V.Create( 0, 0, 0 )
		bird.tgt  = V.Create( Ent.x, 0, Ent.z )
		
		bird.ang     = Ent.angley
		bird.hunger  = randVal( 0, 100 )
		bird.timer   = timeNow + random() * maxPauseTime
		bird.canEat  = GetEntityAnimationNameExist( e, 'eating' ) == 1
		bird.canJmp  = GetEntityAnimationNameExist( e, 'jump' )   == 1
		bird.canRun  = GetEntityAnimationNameExist( e, 'run' )    == 1
		bird.canWalk = GetEntityAnimationNameExist( e, 'move' )   == 1
		bird.height  = max( 0, Ent.y - getTerOrWaterHeight( bird.pos ) )
		bird.onGrnd  = bird.height < 2
		bird.speed   = 0
		bird.tgtSpd  = 0
		bird.turnSpd = GetEntityTurnSpeed( e )
		if bird.turnSpd == 0 then bird.turnSpd = 10 end
		changeStateLoop( e, bird, 'perched', 'Idle', true )
		perches[ #perches + 1 ] = { taken  = e, 
		                            pos    = V.Create( Ent.x, Ent.y, Ent.z ),
									ang    = Ent.angley, 
									height = bird.height
								  }
		bird.perch   = perches[ #perches ]

	elseif 
	   bird.state == 'perched' then
		local scareTyp = scare( e, bird, 400 )

		if scareTyp ~= nil or
		   timedEvent( timeNow, bird, 0, maxPauseTime, true ) then
			
			if bird.onGrnd and scareTyp == nil and bird.canJmp and chance( 0.5 ) then
				leavePerch( e )
				changeStateLoop( e, bird, 'hopping', 'jump', true )
				selectTargetPos( e, bird )
				bird.tgtSpd = getRndSpeed( e ) * 1.2
			elseif
			   bird.onGrnd and scareTyp == nil and bird.canWalk then
				leavePerch( e )
				changeStateLoop( e, bird, 'walking', 'move', true )
				selectTargetPos( e, bird )
				bird.tgtSpd = getRndSpeed( e )
			elseif 
			   scareTyp ~= nil then
				leavePerch( e )
				switchToNonCharacter( e, bird )
				changeStatePlay( e, bird, 'flying', 'flying start', 'flying loop' )
			end
		end
		
	elseif 
	   bird.state == 'transition' then
		moveBird( e, bird )

		if GetObjectAnimationFinished( e ) == 1 then
			changeStateLoop( e, bird, bird.nextState, bird.nextAnim )
		else
			bird.speed = max( 0.5, bird.speed + 0.02 )
		end
		
	elseif
	   bird.state == 'flying' then

		moveBird( e, bird )
		if bird.speed < maxSpeed then 
			bird.speed = bird.speed + 0.02
		end
		checkPerch( e, bird, timeNow, 3000 )
	
	elseif
	   bird.state == 'respawn'	then
		respawn( e, bird, timeNow )
		
	elseif
	   bird.state == 'hopping' or
	   bird.state == 'walking' or
	   bird.state == 'running' then

		bird.pos = V.Create( Ent.x, Ent.y, Ent.z )
		
		local scareTyp = scare( e, bird, 400 )
		
		if scareTyp ~= nil then
			switchToNonCharacter( e, bird )
			changeStatePlay( e, bird, 'flying', 'flying start', 'flying loop' )
			return
		end
		
		local moving = true
		if V.Closer( bird.pos, bird.lpos, 0.1 ) then moving = false end
		if moving then
			bird.lpos = V.Create( Ent.x, Ent.y, Ent.z )
			local pointindex = MoveAndRotateToXYZ( e, bird.speed, bird.turnSpd, 1 )
			if pointindex == 0 or 
			   ( pointindex > 1 and not canSee( bird, 5 * bird.speed ) ) or
				V.Closer( bird.pos, bird.tgt, 10 ) then
				moving = false
			end
		end
		if not moving then
			stopNavigating( e, bird )
			if bird.canEat and 
			   bird.hunger < 5 then
				changeStateLoop( e, bird, 'eating', 'eating', true, true ) 
			else
				changeStateLoop( e, bird, 'perched', 'Idle', true, true )
			end
		else
			if bird.state == 'hopping' then
				bird.hunger = max( 0, bird.hunger - bird.speed / 2 )
			else
				bird.hunger = max( 0, bird.hunger - bird.speed / 3 )
			end
			checkPerch( e, bird, timeNow, 500 )
		end
		
	elseif
	   bird.state == 'eating' then
		
		local scareTyp = scare( e, bird, 300 )
		
		if scareTyp ~= nil then
			switchToNonCharacter( e, bird )
			changeStatePlay( e, bird, 'flying', 'flying start', 'flying loop' )
			return
		end

		bird.hunger = min( 100, bird.hunger + randVal( 0.1, 0.4 ) )

		if bird.hunger > 99 then
			if bird.canJmp and chance( 0.5 ) then
				changeStateLoop( e, bird, 'hopping', 'jump', true )
			elseif
			   bird.canWlk then
				changeStateLoop( e, bird, 'walking', 'move', true )
			else
				changeStateLoop( e, bird, 'perched', 'Idle', true )
			end
		end
	end
	
	if bird.state ~= 'flying' and
	   bird.state ~= 'transition' then
		local sd = bird.tgtSpd - bird.speed
		if abs( sd ) > 0.01 then
			bird.speed = bird.speed + sd / 7
		else
			bird.speed = bird.tgtSpd
		end
	end
	
	if bird.speed > 0 then 
		SetAnimationSpeed( e, min( 1.7, bird.speed ) )
	else
		SetAnimationSpeed( e, 1 )
	end
end
