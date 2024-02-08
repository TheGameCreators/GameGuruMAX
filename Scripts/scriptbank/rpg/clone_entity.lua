-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Clone_Entity v16 by Necrym59 and Lee
-- DESCRIPTION: Allows cloning of a Named Entity when triggered.
-- DESCRIPTION: Attach to an object then logic link to a switch or zone to activate.
-- DESCRIPTION: [CLONE_QUANTITY=1(1,100)]
-- DESCRIPTION: [CLONE_RANGE=100(50,1000)]
-- DESCRIPTION: [CLONE_LIFESPAN=3(1,1000)] Minutes (1000 = Eternal Lifespan)
-- DESCRIPTION: [@CLONE_HEALTH=1(1=Cloned, 2=Random)]
-- DESCRIPTION: [@RESPAWN_CYCLE=1(1=Off, 2=On)]
-- DESCRIPTION: [RESPAWN_EVENTS=1(1,10)]
-- DESCRIPTION: [RESPAWN_INTERVAL=10(1,100)] Seconds
-- DESCRIPTION: [ENTITY_NAME$="Rabbit"]

local lower = string.lower

local clone_entity 		= {}
local clone_quantity	= {}
local clone_range		= {}
local clone_lifespan	= {}
local clone_health		= {}
local respawn_cycle		= {}
local respawn_events	= {}
local respawn_interval	= {}
local entity_name		= {}
local entity_no			= {}

local status			= {}
local clones			= {}
local lifetimer			= {}
local respawntimer		= {}
local respawnactive		= {}
local clhealth			= {}
local clonelist 		= {}
local clonesdead 		= {}

function clone_entity_properties(e, clone_quantity, clone_range, clone_lifespan, clone_health, respawn_cycle, respawn_events, respawn_interval, entity_name)
	clone_entity[e].clone_quantity = clone_quantity or 1
	clone_entity[e].clone_range = clone_range
	clone_entity[e].clone_lifespan = clone_lifespan	
	clone_entity[e].clone_health = clone_health	
	clone_entity[e].respawn_cycle = respawn_cycle
	clone_entity[e].respawn_events = respawn_events
	clone_entity[e].respawn_interval = respawn_interval
	clone_entity[e].entity_name = lower(entity_name)
	clone_entity[e].entity_no = 0
end

function clone_entity_init(e)
	clone_entity[e] = {}
	clone_entity[e].clone_quantity = 1
	clone_entity[e].clone_range = 100
	clone_entity[e].clone_lifespan = 3
	clone_entity[e].clone_health = 1	
	clone_entity[e].respawn_cycle = 1
	clone_entity[e].respawn_events = 1
	clone_entity[e].respawn_interval = 10
	clone_entity[e].entity_name = "Rabbit"
	clone_entity[e].entity_no = 0
	clone_entity[e].timetonextspawn = 0
	lifetimer[e] = math.huge
	respawntimer[e] = math.huge	
	respawnactive[e] = 0	
	status[e] = "init"
	clones[e] = 0
end

function clone_entity_main(e)
	if status[e] == "init" then		
		clone_entity[e].entity_no = 0
		clone_entity[e].timetonextspawn = g_Time + 5
		if clone_entity[e].respawn_cycle == 1 then SetEntityActivated(e,0) end
		if clone_entity[e].respawn_cycle == 2 and respawnactive[e] == 1 then SetEntityActivated(e,1) end
		status[e] = "find_entity"
	end
	if status[e] == "find_entity" then
		if clone_entity[e].entity_no == 0 or clone_entity[e].entity_no == nil then
			for ee = 1, g_EntityElementMax do
				if ee ~= nil and e~= ee and g_Entity[ee] ~= nil then
					if lower(GetEntityName(ee)) == lower(clone_entity[e].entity_name) then
						clone_entity[e].entity_no = ee
						clhealth[e] = g_Entity[ee]['health']
						Hide(ee)
						ModulateSpeed(ee,0)	
						SetEntityActive(clone_entity[e].entity_no,0)											
						status[e] = "clone_entity"
						break
					end
				end
			end
		end
	end

	if g_Entity[e]['activated'] == 1 then
		if status[e] == "clone_entity" then
			if g_Time > clone_entity[e].timetonextspawn then
				clone_entity[e].timetonextspawn = g_Time + 5
				local etoclone = clone_entity[e].entity_no
				if clone_entity[e].clone_health == 2 then SetEntityHealth(etoclone,math.random(clhealth[e]/2,clhealth[e]*2)) end
				local newEntn = SpawnNewEntity(etoclone)
				GravityOff(newEntn)
				local newposx = g_Entity[etoclone]['x'] + math.random(-clone_entity[e].clone_range,clone_entity[e].clone_range)
				local newposz = g_Entity[etoclone]['z'] + math.random(-clone_entity[e].clone_range,clone_entity[e].clone_range)
				local newposy = GetSurfaceHeight(g_Entity[etoclone]['x'],g_Entity[etoclone]['y'],g_Entity[etoclone]['z'])
				cx, cy, cz = GetObjectCentre(g_Entity[etoclone]['obj'])
				ResetPosition(newEntn,newposx,newposy+5+math.abs(cy),newposz)
				ResetRotation(newEntn,g_Entity[etoclone]['anglex'], g_Entity[etoclone]['angley']+math.random(0,359),g_Entity[etoclone]['anglez'])
				GravityOn(newEntn)
				CollisionOn(newEntn)
				Show(newEntn)
				clones[e] = clones[e] + 1
				table.insert(clonelist,newEntn)
				if clones[e] >= clone_entity[e].clone_quantity then
					CollisionOff(clone_entity[e].entity_no)
					if clone_entity[e].clone_lifespan ~= 0 then
						lifetimer[e] = g_Time + (clone_entity[e].clone_lifespan * 1000)*60
						status[e] = "monitor_lifespan"
					end
					if clone_entity[e].clone_lifespan == 0 then
						status[e] = "eternal_lifespan"
					end
				end
			end
		end	
		if status[e] == "monitor_lifespan" then	
			clonesdead[e] = true
			for _,v in pairs (clonelist) do 
				if g_Entity[v] ~= nil then
					if g_Entity[v]["health"] > 0 then
						clonesdead[e] = false
						break
					end
				end
			end	
			if clone_entity[e].clone_lifespan < 999 then
				if g_Time > lifetimer[e] then clonesdead[e] = true end
			end	
			if clonesdead[e] == true then
				if clones[e] > 0 then
					for a,b in pairs (clonelist) do
						if g_Entity[b] ~= nil then							
							DeleteNewEntity(b)
							clonelist[a] = nil
						end
						clones[e] = 0
					end
				end
				if clones[e] == 0 and clone_entity[e].respawn_cycle == 1 then
					lifetimer[e] = math.huge					
					status[e] = "init"
				end
				if clones[e] == 0 and clone_entity[e].respawn_cycle == 2 then
					respawntimer[e] = g_Time + (clone_entity[e].respawn_interval* 1000)
					status[e] = "respawn"
				end
			end
		end
		if status[e] == "respawn" then 
			if g_Time > respawntimer[e] and clone_entity[e].respawn_events > 0 then
				lifetimer[e] = math.huge
				respawnactive[e] = 1
				clone_entity[e].respawn_events = clone_entity[e].respawn_events -1				
				status[e] = "init"
			end	
		end		
	end	
end