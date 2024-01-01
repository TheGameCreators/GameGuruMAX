-- DESCRIPTION: Will animate and move this object as though a mammal.
local U = require "scriptbank\\utillib"

local modf  = math.modf
local min   = math.min
local max   = math.max
local rad   = math.rad
local abs   = math.abs
local find  = string.find
local sub   = string.sub
local lower = string.lower

local random = math.random

math.randomseed( os.time() )
random(); random(); random()

local mammals = {}

function mammal_generic_init_name( e, name )
	local nextPos = find( name, ' ' ) or #name + 1
	mammals[ e ] = { state = 'init', species = lower( sub( name, 1, nextPos - 1 ) ) }
end

local lastTime  = 0
local procTime  = 0
local tDiff     = 1
local plyrMoved = false
local waterHgt  = 0
local procEnt   = nil

local ppx, ppy, ppz = 0, 0, 0
local opx, opy, opz = 0, 0, 0

local function randVal( low, high )
	return low + random() * ( high - low ) 
end

local function chance( val )
	return random() < val
end

local function playRndSound( e, slotMin, slotMax )
	local slot = random( slotMin, slotMax )
	SetSound( e, slot )
	PlaySound( e, slot )
	SetSoundVolume( 90 )
end

local function changeStateLoop( e, critter, newState, animName, stopAnim )
	if animName ~= nil then
		if stopAnim then StopAnimation( e ) end
		SetAnimationName( e, animName )
		LoopAnimationFrom( e, random() * 100 )
	end
	if critter.state == 'flee' and chance( 0.4 ) then
		playRndSound( e, 0, 3 )
	end
	critter.state = newState
end

local function heardSound( critter, typ, dist )
	if typ == 'player' then
		local sound = GetNearestSoundDistance( critter.x, critter.y, critter.z, 1 )
		return sound > 0 and sound < dist
	else
		local sound = GetNearestSoundDistance( critter.x, critter.y, critter.z, 2 )
		return sound > 0 and sound < dist
	end
end

local function scare( e, critter, dist )
	if g_PlayerGunFired == 1 and
	   heardSound( critter, 'player', 1100 ) then
		return 'gunfire'
	
	elseif
	   U.PlayerCloserThan( e, dist / 3 ) then
		return 'player'
		
	elseif 
	   heardSound( critter, 'player', dist ) then
		return 'player'
	
	elseif 
	   heardSound( critter, 'other', dist ) then
		return 'npc'
	end
end

local function timedEvent( timeNow, critter, tmin, tmax )
	if critter.timer == nil then
		critter.timer = timeNow + randVal( tmin, tmax )
	elseif
	   timeNow > critter.timer then
		critter.timer = timeNow + randVal( tmin, tmax )
		return true
	end
end

local function getRndSpeed( e )
	local spd = GetEntityMoveSpeed( e ) / 100.0
	return randVal( spd * 0.85, spd * 1.15 )
end

-- function returns squared distance between two points
local function sqrd( x1, z1, x2, z2 )
	local dx, dz = x1 - x2, z1 - z2
	return dx*dx + dz*dz
end

local function aCritter( obj )
	for _, v in pairs( mammals ) do
		if v.obj == obj then return true end
	end
end

local function canSee( critter, dist, x, y, z )
	local cx, cy, cz, ax, ay, az = GetObjectPosAng( critter.obj )
	if x == nil then
		local xo, yo, zo = U.Rotate3D( 0, 0, dist, rad( ax ), rad( ay ), rad( az ) )
		x, y, z = cx + xo, cy + yo + 5, cz + zo
	end
	--local obj = IntersectAll( cx, cy + 5, cz, x, y, z, critter.obj )
	local obj = 0
	if critter.e ~= nil then
	 obj = IntersectStaticPerformant(cx, cy + 5, cz, x, y, z, critter.obj, critter.e, 500 ,1 , 1)
	end
	
	if obj == 0 or aCritter( obj ) then	
		return true
	end
end

local function pathFound( e, critter )
	local y = critter.y
	if critter.species == 'sewer' then 
		if chance( 0.2 ) then 
			if chance( 0.5 ) then
				y = y + 100
			else
				y = y - 100
			end
		end
	end
	y = RDGetYFromMeshPosition( critter.tx, y, critter.tz )
	
	RDFindPath( critter.x, critter.y, critter.z, critter.tx, y, critter.tz )
				
	local pc = RDGetPathPointCount()
	if pc > 0 then
		if canSee( critter, 0, RDGetPathPointX(1), RDGetPathPointY(1), RDGetPathPointZ(1) ) then
			StartMoveAndRotateToXYZ( e, 0, 1, 1 )
			return true
		end
	end
end

local function pickNewPos( e, critter, dist, typ, tx, tz )
	tx = tx or ppx
	tz = tz or ppz
	for i = 1, 5 do
		local x, z = U.RandomPos( dist, critter.x, critter.z )
		if x ~= nil then
		if ( typ == 'wander' or 
		     sqrd( x, z, tx, tz ) > sqrd( x, z, critter.x, critter.z ) ) and
		   waterHgt < GetTerrainHeight( x, z )then
		    critter.tx, critter.tz = x, z
			if pathFound( e, critter ) then return true end 
		end
		end
	end
end
				 
local function selectTargetPos( e, critter, typ, x, z )
	if typ == 'start' then
		critter.tx, critter.tz = U.RandomPos( 800, critter.startx, critter.startz )
		critter.tgtSpeed = getRndSpeed( e )
		return pathFound( e, critter )
	elseif
	   typ == 'wander' then
		critter.tgtSpeed = getRndSpeed( e )
		return pickNewPos( e, critter, randVal( 100, 300 ), typ )
	elseif
	   typ == 'player' or
	   typ == 'npc'    then
		if pickNewPos( e, critter, randVal( 300, 500 ), typ, x, z ) then
			critter.tgtSpeed = getRndSpeed( e ) * 2
			return true
		end
	elseif
	   typ == 'gunfire' then
		if pickNewPos( e, critter, randVal( 500, 800 ), typ ) then
			critter.tgtSpeed = getRndSpeed( e ) * 2
			return true
		end
	elseif 
	   typ == 'predator' then
		if pickNewPos( e, critter, randVal( 400, 600 ), typ, x, z ) then
			critter.tgtSpeed = getRndSpeed( e ) * 2
			return true
		end	
	end	
end

local function speciesSpecific( e, critter )
	if critter.procTimer == nil or
	   procTime > critter.procTimer then
		critter.procTimer = procTime + randVal( 100,300 )
		critter.critterList = U.ClosestEntities( 500, 10, critter.x, critter.z )
		
		if critter.species == 'rabbit' then
			for _, v in pairs( critter.critterList ) do
				if v ~= e then 
					local other = mammals[ v ]
					if other ~= nil and
					   other.species == 'fox' and
					   other.x ~= nil and
					   sqrd( other.x, other.z, critter.x, critter.z )  < 400 * 400 then
						if selectTargetPos( e, critter, 'predator', other.x, other.z ) then
							changeStateLoop( e, critter, 'flee', 'Move', true )	
							break
						end
					end
				end
			end
		
			-- tbd : add other species specific behaviour, maybe get rats to stop still if
			--       a fox is around or have rats hunt down carrion etc
		end
	end
end

local function warnOthers( e, critter )
	if critter.critterList == nil then return end											  
	for _, v in pairs( critter.critterList ) do
		if v ~= e then 
			local other = mammals[ v ]
			if other ~= nil and 
			   other.species == critter.species then
				other.warn = e
			end
		end
	end
end

local function processHunger( critter, val )
	if critter.canEat then
		if val > 0 then
			critter.hunger = min( 100, critter.hunger + val * tDiff )
		else
			critter.hunger = max(   0, critter.hunger + val * tDiff )
		end
	end
end
	
function mammal_generic_main( e )

	local critter = mammals[ e ]
	if critter == nil then return end

	local Ent = g_Entity[ e ]
	if Ent == nil then return end
	
	local timeNow = g_Time
	
	if timeNow > lastTime or procEnt == nil then
		tDiff = min( 5, ( timeNow - lastTime ) / 16.667 )
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
	
	critter.x, critter.y, critter.z = Ent.x, Ent.y, Ent.z

	if critter.state == 'init' then
		SetAnimationName( e, 'idle' )
		LoopAnimationFrom( e, random() * 100 )
		critter.e     = e
		critter.obj     = Ent.obj
		critter.state   = 'idle'
		critter.startx  = Ent.x
		critter.starty  = Ent.y
		critter.startz  = Ent.z
		critter.speed   = 0
		critter.tgtSpeed= getRndSpeed( e )
		critter.turnSpd = GetEntityTurnSpeed( e )
		SetEntityHealth( e, Ent.health + 10000 )
		critter.canEat  = GetEntityAnimationNameExist( e, 'eating' ) == 1
		if critter.canEat then 
			critter.hunger    = randVal( 0, 50 ) 
			critter.tgtHunger = randVal( 50, 100 )
		end
		return 
		
	elseif
	   critter.state == 'idle' then
		if not U.CloserThan( critter.x, 0, critter.z, 
		                     critter.startx, 0, critter.startz, 2500 ) then
			if selectTargetPos( e, critter, 'start' ) then			 
				changeStateLoop( e, critter, 'walk', 'Walk', true )
			end
			
		elseif 
		   timedEvent( procTime, critter, 2000, 6000 ) then
			if selectTargetPos( e, critter, 'wander' ) then
				changeStateLoop( e, critter, 'walk', 'Walk', true )
			end
		
		elseif critter.canEat then
			processHunger( critter, 0.01 )
			if critter.hunger > critter.tgtHunger then
				critter.tgtSpeed = getRndSpeed( e )
				changeStateLoop( e, critter, 'eat', 'Eating', true )
				critter.tgtHunger = randVal( 1, 30 )
			end
		end

	elseif
	   critter.state == 'eat' then 
	    processHunger( critter, -0.2 )
		if critter.hunger < critter.tgtHunger then
			critter.timer = nil
			changeStateLoop( e, critter, 'idle', 'Idle', true )
			critter.tgtHunger = randVal( 60, 100 )
		end
		
	elseif
	   critter.state == 'walk' then
		local pointindex = MoveAndRotateToXYZ( e, 0, critter.turnSpd, 1 )
		if pointindex == 0 or 
		   ( pointindex > 1 and not canSee( critter, 5 * critter.speed ) ) or
		   U.CloserThan( critter.x,  0, critter.z, 
		                 critter.tx, 0, critter.tz, 10 ) then
			MoveAndRotateToXYZ( e, 0, critter.turnSpd, 1 )
			critter.timer = nil
			critter.tgtSpeed = getRndSpeed( e )
			changeStateLoop( e, critter, 'idle', 'Idle', true )
		end
		processHunger( critter, 0.02 )
		
	end
	
	local spd = critter.tgtSpeed - critter.speed
	if abs( spd ) > 0.01 then
		critter.speed = critter.speed + spd / 77
	else
		critter.speed = critter.tgtSpeed
	end
	if critter.state == 'idle' or
	   critter.state == 'eat'  then
		SetAnimationSpeed( e, GetEntityMoveSpeed( e ) / 100.0 )
	elseif 
	   critter.species ~= 'fox'  or
       critter.state   ~= 'flee' then
		SetAnimationSpeed( e, critter.speed )
	else
		-- slow down the fox, if other large mammals are added
		-- they will probably need the same treatment
		critter.animspeed = critter.speed * 0.8
		SetAnimationSpeed( e, critter.animspeed )
	end
	
	if critter.state ~= 'dead' then
		if Ent.health <= 10000 then
			StopAnimation( e )
			SetAnimationName( e, 'death' )
			PlayAnimation( e )
			critter.state = 'dead'
			critter.timer = nil
			warnOthers( e, critter )
			playRndSound( e, 4, 4 )
			
		elseif 
		   critter.state == 'flee' then
			local pointindex = MoveAndRotateToXYZ( e, 0, critter.turnSpd, 1 )
			if pointindex == 0 or 
			   ( pointindex > 1 and not canSee( critter, 40 ) ) or
			   U.CloserThan( critter.x,  0, critter.z, 
		                     critter.tx, 0, critter.tz, 25 ) then
				MoveAndRotateToXYZ( e, 0, critter.turnSpd, 1 ) 
				critter.timer = nil
				if critter.fleeTimer then 
					if timeNow < critter.fleeTimer then
						if selectTargetPos( e, critter, critter.fleeTyp ) then
							changeStateLoop( e, critter, 'flee', 'Move', true )
						end
					else
						critter.fleeTimer = nil
					end
				else
					critter.tgtSpeed = getRndSpeed( e )
					changeStateLoop( e, critter, 'walk', 'Walk', true )
				end
			end
			processHunger( critter, 0.1 )

		else

			if critter.warn then
				local other = mammals[ critter.warn ]
				if other ~= nil and
				   selectTargetPos( e, critter, 'predator', other.x, other.z ) then
					changeStateLoop( e, critter, 'flee', 'Move', true )
					critter.warn = nil
				end
			else
				local fleeTyp = scare( e, critter, 400 )
				if fleeTyp ~= nil then
					if selectTargetPos( e, critter, fleeTyp ) then
						changeStateLoop( e, critter, 'flee', 'Move', true )
					end
					if fleeTyp == 'gunfire'then
						critter.fleeTimer = timeNow + randVal( 4000, 5000 )
					else
						critter.fleeTimer = timeNow + randVal( 2000, 3000 )
					end
					critter.fleeTyp = fleeTyp
				else
					speciesSpecific( e, critter )
				end
			end
		end
		
	elseif
	   timedEvent( procTime, critter, 5000, 5000 ) then
	   	SetEntityHealth( e, 0 )
		SwitchScript( e, 'no_behavior.lua' )
	end
	
	-- PromptLocal( e, critter.state .. ", " .. critter.speed )	
end
