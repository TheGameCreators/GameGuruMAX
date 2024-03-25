-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Crawling Insect V3
-- All crawling insect animations are named idle, walk, run, death, eat
-- DESCRIPTION: Will animate and move this object as though a crawling insect.

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

local insects = {}

function crawling_insect_init_name( e, name )
	local nextPos = find( name, ' ' ) or #name + 1
	insects[ e ] = { state = 'init', species = lower( sub( name, 1, nextPos - 1 ) ) }
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

local function changeStateLoop( e, crawler, newState, animName, stopAnim )
	if animName ~= nil then
		if stopAnim then StopAnimation( e ) end
		SetAnimationName( e, animName )
		LoopAnimationFrom( e, random() * 100 )
	end
	if crawler.state == 'flee' and chance( 0.4 ) then
		playRndSound( e, 0, 3 )
	end
	crawler.state = newState
end

local function heardSound( crawler, typ, dist )
	if typ == 'player' then
		local sound = GetNearestSoundDistance( crawler.x, crawler.y, crawler.z, 1 )
		return sound > 0 and sound < dist
	else
		local sound = GetNearestSoundDistance( crawler.x, crawler.y, crawler.z, 2 )
		return sound > 0 and sound < dist
	end
end

local function scare( e, crawler, dist )
	if g_PlayerGunFired == 1 and
	   heardSound( crawler, 'player', 50 ) then
		return 'gunfire'
	
	elseif
	   U.PlayerCloserThan( e, dist / 3 ) then
		return 'player'
		
	elseif 
	   heardSound( crawler, 'player', dist ) then
		return 'player'
	
	elseif 
	   heardSound( crawler, 'other', dist ) then
		return 'npc'
	end
end

local function timedEvent( timeNow, crawler, tmin, tmax )
	if crawler.timer == nil then
		crawler.timer = timeNow + randVal( tmin, tmax )
	elseif
	   timeNow > crawler.timer then
		crawler.timer = timeNow + randVal( tmin, tmax )
		return true
	end
end

local function getRndSpeed( e )
	local spd = GetEntityMoveSpeed( e ) / 100.0
	return randVal( spd * 0.85, spd * 1.15 )
end

local function sqrd( x1, z1, x2, z2 )
	local dx, dz = x1 - x2, z1 - z2
	return dx*dx + dz*dz
end

local function acrawler( obj )
	for _, v in pairs( insects ) do
		if v.obj == obj then return true end
	end
end

local function canSee( crawler, dist, x, y, z )
	local cx, cy, cz, ax, ay, az = GetObjectPosAng( crawler.obj )
	if x == nil then
		local xo, yo, zo = U.Rotate3D( 0, 0, dist, rad( ax ), rad( ay ), rad( az ) )
		x, y, z = cx + xo, cy + yo + 5, cz + zo
	end

	local obj = 0
	if crawler.e ~= nil then
	 obj = IntersectStaticPerformant(cx, cy + 5, cz, x, y, z, crawler.obj, crawler.e, 500 )
	end
	
	if obj == 0 or acrawler( obj ) then	
		return true
	end
end

local function pathFound( e, crawler )
	local y = crawler.y
	if crawler.species == 'sewer' then 
		if chance( 0.2 ) then 
			if chance( 0.5 ) then
				y = y + 100
			else
				y = y - 100
			end
		end
	end
	y = RDGetYFromMeshPosition( crawler.tx, y, crawler.tz )
	
	RDFindPath( crawler.x, crawler.y, crawler.z, crawler.tx, y, crawler.tz )
				
	local pc = RDGetPathPointCount()
	if pc > 0 then
		if canSee( crawler, 0, RDGetPathPointX(1), RDGetPathPointY(1), RDGetPathPointZ(1) ) then
			StartMoveAndRotateToXYZ( e, crawler.speed, 1, 1 )
			return true
		end
	end
end

local function pickNewPos( e, crawler, dist, typ, tx, tz )
	tx = tx or ppx
	tz = tz or ppz
	for i = 1, 5 do
		local x, z = U.RandomPos( dist, crawler.x, crawler.z )
		if ( typ == 'wander' or 
		     sqrd( x, z, tx, tz ) > sqrd( x, z, crawler.x, crawler.z ) ) and
		   waterHgt < GetTerrainHeight( x, z )then
		    crawler.tx, crawler.tz = x, z
			if pathFound( e, crawler ) then return true end 
		end
	end
end
				 
local function selectTargetPos( e, crawler, typ, x, z )
	if typ == 'start' then
		crawler.tx, crawler.tz = U.RandomPos( 800, crawler.startx, crawler.startz )
		crawler.tgtSpeed = getRndSpeed( e )
		return pathFound( e, crawler )
	elseif
	   typ == 'wander' then
		crawler.tgtSpeed = getRndSpeed( e )
		return pickNewPos( e, crawler, randVal( 100, 300 ), typ )
	elseif
	   typ == 'player' or
	   typ == 'npc'    then
		if pickNewPos( e, crawler, randVal( 300, 500 ), typ, x, z ) then
			crawler.tgtSpeed = getRndSpeed( e ) * 2
			return true
		end
	elseif
	   typ == 'gunfire' then
		if pickNewPos( e, crawler, randVal( 200, 300 ), typ ) then
			crawler.tgtSpeed = getRndSpeed( e ) * 2
			return true
		end
	elseif 
	   typ == 'predator' then
		if pickNewPos( e, crawler, randVal( 400, 600 ), typ, x, z ) then
			crawler.tgtSpeed = getRndSpeed( e ) * 2
			return true
		end	
	end	
end

local function speciesSpecific( e, crawler )
	if crawler.procTimer == nil or
	   procTime > crawler.procTimer then
		crawler.procTimer = procTime + randVal( 100,300 )
		crawler.crawlerList = U.ClosestEntities( 500, 10, crawler.x, crawler.z )
	end
end

local function warnOthers( e, crawler )
	if crawler.crawlerList == nil then return end											  
	for _, v in pairs( crawler.crawlerList ) do
		if v ~= e then 
			local other = insects[ v ]
			if other ~= nil and 
			   other.species == crawler.species then
				other.warn = e
			end
		end
	end
end

local function processHunger( crawler, val )
	if crawler.canEat then
		if val > 0 then
			crawler.hunger = min( 100, crawler.hunger + val * tDiff )
		else
			crawler.hunger = max(   0, crawler.hunger + val * tDiff )
		end
	end
end
	
function crawling_insect_main( e )
	CollisionOff(e)
	local crawler = insects[ e ]
	if crawler == nil then return end

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
	
	crawler.x, crawler.y, crawler.z = Ent.x, Ent.y, Ent.z
	
	if crawler.state == 'init' then
		SetAnimationName( e, 'idle' )
		LoopAnimationFrom( e, random() * 100 )
		crawler.e     = e
		crawler.obj     = Ent.obj
		crawler.state   = 'idle'
		crawler.startx  = Ent.x
		crawler.starty  = Ent.y
		crawler.startz  = Ent.z
		crawler.speed   = 0
		crawler.tgtSpeed= getRndSpeed( e )
		crawler.turnSpd = GetEntityTurnSpeed( e )
		SetEntityHealth( e, Ent.health + 10000 )
		crawler.canEat  = GetEntityAnimationNameExist( e, 'eat' ) == 1
		if crawler.canEat then 
			crawler.hunger    = randVal( 0, 50 ) 
			crawler.tgtHunger = randVal( 50, 100 )
		end
		return 
		
	elseif
	   crawler.state == 'idle' then
		if not U.CloserThan( crawler.x, 0, crawler.z, 
		                     crawler.startx, 0, crawler.startz, 2500 ) then
			if selectTargetPos( e, crawler, 'start' ) then			 
				changeStateLoop( e, crawler, 'walk', 'walk', true )
			end
			
		elseif 
		   timedEvent( procTime, crawler, 2000, 6000 ) then
			if selectTargetPos( e, crawler, 'wander' ) then
				changeStateLoop( e, crawler, 'walk', 'walk', true )
			end
		
		elseif crawler.canEat then
			processHunger( crawler, 0.01 )
			if crawler.hunger > crawler.tgtHunger then
				crawler.tgtSpeed = getRndSpeed( e )
				changeStateLoop( e, crawler, 'eat', 'eat', true )
				crawler.tgtHunger = randVal( 1, 30 )
			end
		end

	elseif
	   crawler.state == 'eat' then 
	    processHunger( crawler, -0.2 )
		if crawler.hunger < crawler.tgtHunger then
			crawler.timer = nil
			changeStateLoop( e, crawler, 'idle', 'idle', true )
			crawler.tgtHunger = randVal( 60, 100 )
		end
		
	elseif
	   crawler.state == 'walk' then
		local pointindex = MoveAndRotateToXYZ( e, crawler.speed, crawler.turnSpd, 1 )
		if pointindex == 0 or 
		   ( pointindex > 1 and not canSee( crawler, 5 * crawler.speed ) ) or
		   U.CloserThan( crawler.x,  0, crawler.z, 
		                 crawler.tx, 0, crawler.tz, 10 ) then
			MoveAndRotateToXYZ( e, crawler.speed, crawler.turnSpd, 1 )
			crawler.timer = nil
			crawler.tgtSpeed = getRndSpeed( e )
			changeStateLoop( e, crawler, 'idle', 'idle', true )
		end
		processHunger( crawler, 0.02 )
		
	end
	
	local spd = crawler.tgtSpeed - crawler.speed
	if abs( spd ) > 0.01 then
		crawler.speed = crawler.speed + spd / 77
	else
		crawler.speed = crawler.tgtSpeed
	end
	if crawler.state == 'idle' or
	   crawler.state == 'eat'  then
		SetAnimationSpeed( e, GetEntityMoveSpeed( e ) / 100.0 )
	elseif 
       crawler.state   ~= 'flee' then
		SetAnimationSpeed( e, crawler.speed * 0.9 )
	else
		crawler.animspeed = crawler.speed * 0.8
		SetAnimationSpeed( e, crawler.animspeed )
	end
	
	if crawler.state   == 'flee' then
		SetAnimationSpeed( e, crawler.speed * 1.2 )
	end	
	
	if crawler.state ~= 'dead' then
		if Ent.health <= 10000 then
			StopAnimation( e )
			SetAnimationName( e, 'death' )
			PlayAnimation( e )
			crawler.state = 'dead'
			crawler.timer = nil
			warnOthers( e, crawler )
			playRndSound( e, 4, 4 )
			
		elseif 
		   crawler.state == 'flee' then
			local pointindex = MoveAndRotateToXYZ( e, crawler.speed, crawler.turnSpd, 1 )
			if pointindex == 0 or 
			   ( pointindex > 1 and not canSee( crawler, 40 ) ) or
			   U.CloserThan( crawler.x,  0, crawler.z, 
		                     crawler.tx, 0, crawler.tz, 25 ) then
				MoveAndRotateToXYZ( e, crawler.speed, crawler.turnSpd, 1 ) 
				crawler.timer = nil
				if crawler.fleeTimer then 
					if timeNow < crawler.fleeTimer then
						if selectTargetPos( e, crawler, crawler.fleeTyp ) then
							changeStateLoop( e, crawler, 'flee', 'run', true )
						end
					else
						crawler.fleeTimer = nil
					end
				else
					crawler.tgtSpeed = getRndSpeed( e )
					changeStateLoop( e, crawler, 'walk', 'walk', true )
				end
			end
			processHunger( crawler, 0.1 )

		else

			if crawler.warn then
				local other = insects[ crawler.warn ]
				if other ~= nil and
				   selectTargetPos( e, crawler, 'predator', other.x, other.z ) then
					changeStateLoop( e, crawler, 'flee', 'run', true )
					crawler.warn = nil
				end
			else
				local fleeTyp = scare( e, crawler, 400 )
				if fleeTyp ~= nil then
					if selectTargetPos( e, crawler, fleeTyp ) then
						changeStateLoop( e, crawler, 'flee', 'run', true )
					end
					if fleeTyp == 'gunfire'then
						crawler.fleeTimer = timeNow + randVal( 500, 1000 )
					else
						crawler.fleeTimer = timeNow + randVal( 500, 1000 )
					end
					crawler.fleeTyp = fleeTyp
				else
					speciesSpecific( e, crawler )
				end
			end
		end
		
	elseif
	   timedEvent( procTime, crawler, 5000, 5000 ) then
	   	SetEntityHealth( e, 0 )
		SwitchScript( e, 'no_behavior.lua' )
	end
	
	--PromptLocal( e, crawler.state .. ", " .. crawler.speed )	
end
