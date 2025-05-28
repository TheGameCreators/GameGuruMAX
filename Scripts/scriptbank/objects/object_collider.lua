-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Object Collider v4 
-- DESCRIPTION: The attached object will play sound upon collision with object or terrain.
-- DESCRIPTION: [TERRAIN_DISTANCE=10(0,100)]
-- DESCRIPTION: [#FORCE_DETECTION=0.05(0.00,0.1)]
-- DESCRIPTION: [COLLISION_TRIGGER!=0] to activate logic linked or IfUsed Entities
-- DESCRIPTION: [@TRIGGER_DETECTION=1(1=Terrain, 2=Object, 3=Terrain+Object)]
-- DESCRIPTION: [FORCE_BOUNCE!=0] to bounce object
-- DESCRIPTION: <Sound0> plays on collision with terrain
-- DESCRIPTION: <Sound1> plays collision with object

local U = require "scriptbank\\utillib"
local P = require "scriptbank\\physlib"

local collider 			= {}
local terrain_distance 	= {}
local force_detection 	= {}
local collision_trigger = {}
local trigger_detection = {}
local force_bounce		= {}

local terColPos		= {}
local checkInterval	= {}
local checkTimer	= {}
local initialise	= {}
local timeNow 		= {}
local playonce 		= {}
local playtime 		= {}
local doonce 		= {}
local collisionlist	= {}

function object_collider_properties(e,terrain_distance, force_detection, collision_trigger, trigger_detection, force_bounce)
	collider[e].terrain_distance = terrain_distance
	collider[e].force_detection = force_detection
	collider[e].collision_trigger = collision_trigger or 0
	collider[e].trigger_detection = trigger_detection or 3
	collider[e].force_bounce = force_bounce or 0
end

function object_collider_init(e)
	collider[e] = {}
	collider[e].terrain_distance = 10
	collider[e].force_detection = 0.05
	collider[e].collision_trigger =	0
	collider[e].trigger_detection = 3
	collider[e].force_bounce = 0
	
	checkInterval[e] = 100
	terColPos[e] = 0
	timeNow[e] = math.huge
	playonce[e] = 0
	doonce[e] = 0
	collisionlist[e] = {}
	SetEntityAlwaysActive(e,1)
	P.AddEntityCollisionCheck(e)
end

function object_collider_main(e)
	timeNow[e] = g_Time
	
	if checkTimer[e] == nil then 
		checkTimer[e] = timeNow[e] + 1000 
		initialise[e] = true
		terColPos[e] = { x = 0, y = 0, z = 0 }				
	elseif 
		timeNow[e] > checkTimer[e] then
			initialise[e] = false
			checkTimer[e] = timeNow[e] + 50
			CheckForCollision(e)			
	elseif
		initialise[e] then
			CheckForCollision(e,true)
	end	
end

function CheckForCollision(e,checkOnly)

	if 0 < P.GetEntityTerrainNumCollisions(e) then		
		collisionlist[e] = P.GetEntityTerrainCollisionDetails(e)
		tP = terColPos[e]
		for _, v in ipairs(collisionlist[e]) do
			if not U.CloserThan( tP.x, tP.y, tP.z, v.x, v.y, v.z, collider[e].terrain_distance ) then
				if not checkOnly then
					if playonce[e] == 0 then
						PlaySoundIfSilent(e,0)
						playonce[e] = 1
					end
					if collider[e].trigger_detection == 1 or collider[e].trigger_detection == 3 then
						if doonce[e] == 0 then						
							if collider[e].collision_trigger == 1 then
								PerformLogicConnections(e)
								ActivateIfUsed(e)
							end	
							if collider[e].force_bounce == 1 then
							end
							doonce[e] = 1
						end						
					end
				end
			end
			terColPos[e] = { x = v.x, y = v.y, z = v.z }
		end
	end
	
	if 0 < P.GetEntityObjectNumCollisions(e) then
		collisionlist[e] = P.GetEntityObjectCollisionDetails(e)
		for _, v in ipairs(collisionlist[e]) do
			if v.f > collider[e].force_detection/10 and not checkOnly then
				PlaySound(e,1)
				if collider[e].trigger_detection == 2 or collider[e].trigger_detection == 3 then
					if doonce[e] == 0 then
						if collider[e].collision_trigger == 1 then
							PerformLogicConnections(e)
							ActivateIfUsed(e)
						end
						doonce[e] = 1
					end
					if collider[e].force_bounce == 1 then
						PushObject(g_Entity[e]['obj'],math.random(0,0.01), math.random(0.1,1.9), math.random(0,0.01), math.random(0,1),  math.random(0,3),  math.random(0,1) )
					end
				end	
				break				
			end
		end		
	end		
end