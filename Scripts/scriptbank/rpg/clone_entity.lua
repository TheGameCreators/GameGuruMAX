-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Clone_Entity v21 by Necrym59 and Lee
-- DESCRIPTION: Allows cloning of a Named Entity when triggered.
-- DESCRIPTION: Attach to an object then logic link to a switch or zone to activate, or directly assign to a zone.
-- DESCRIPTION: [CLONE_QUANTITY=1(1,100)]
-- DESCRIPTION: [CLONE_RANGE=100(25,1000)]
-- DESCRIPTION: [CLONE_LIFESPAN=3(1,1000)] Minutes (1000 = Eternal Lifespan)
-- DESCRIPTION: [@CLONE_HEALTH=1(1=Cloned, 2=Random)]
-- DESCRIPTION: [@RESPAWN_CYCLE=1(1=Off, 2=On)]
-- DESCRIPTION: [RESPAWN_EVENTS=1(1,10)]
-- DESCRIPTION: [RESPAWN_INTERVAL=10(5,100)] Seconds
-- DESCRIPTION: [ENTITY_NAME$="Rabbit"]
-- DESCRIPTION: [@FORMATION=1(1=Random, 2=Rows)]
-- DESCRIPTION: [NUMBER_PER_ROW=1(1,100)]

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
local formation			= {}
local number_per_row	= {}
local entity_no			= {}

local status			= {}
local clones			= {}
local lifetimer			= {}
local respawntimer		= {}
local respawnactive		= {}
local rowcount			= {}
local rowspace			= {}
local colcount			= {}
local colspace			= {}
local clhealth			= {}
local clonesdead 		= {}
local tableName 		= {}

function clone_entity_properties(e, clone_quantity, clone_range, clone_lifespan, clone_health, respawn_cycle, respawn_events, respawn_interval, entity_name, formation, number_per_row)
	clone_entity[e].clone_quantity = clone_quantity or 1
	clone_entity[e].clone_range = clone_range
	clone_entity[e].clone_lifespan = clone_lifespan
	clone_entity[e].clone_health = clone_health
	clone_entity[e].respawn_cycle = respawn_cycle
	clone_entity[e].respawn_events = respawn_events
	clone_entity[e].respawn_interval = respawn_interval
	clone_entity[e].entity_name = lower(entity_name)
	clone_entity[e].formation = formation
	clone_entity[e].number_per_row = number_per_row
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
	clone_entity[e].formation = 1
	clone_entity[e].number_per_row = 1
	clone_entity[e].entity_no = 0
	
	clone_entity[e].timetonextspawn = 0
	lifetimer[e] = math.huge
	respawntimer[e] = math.huge
	respawnactive[e] = 0
	status[e] = "init"
	clones[e] = 0
	rowcount[e] = 0
	rowspace[e] = 0
	colcount[e] = 0
	colspace[e] = 0
	tableName[e] = "clonelist" ..tostring(e)
	_G[tableName[e]] = {}
end

function clone_entity_main(e)

	if g_Entity[e]['plrinzone'] == 1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+100 then
		SetEntityActivated(e,1)
	end
	if status[e] == "init" then
		clones[e] = 0
		clone_entity[e].entity_no = 0
		clone_entity[e].timetonextspawn = g_Time + 200
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
				clone_entity[e].timetonextspawn = g_Time + 200
				local etoclone = clone_entity[e].entity_no
				if clone_entity[e].clone_health == 2 then SetEntityHealth(etoclone,math.random(clhealth[e]/2,clhealth[e]*2)) end
				local newEntn = SpawnNewEntity(etoclone)
				GravityOff(newEntn)
				if clone_entity[e].formation == 1 then
					local newposx = g_Entity[etoclone]['x'] + math.random(-clone_entity[e].clone_range,clone_entity[e].clone_range)
					local newposz = g_Entity[etoclone]['z'] + math.random(-clone_entity[e].clone_range,clone_entity[e].clone_range)
					local newposy = GetSurfaceHeight(g_Entity[etoclone]['x'],g_Entity[etoclone]['y'],g_Entity[etoclone]['z'])
					cx, cy, cz = GetObjectCentre(g_Entity[etoclone]['obj'])
					ResetPosition(newEntn,newposx,newposy+5+math.abs(cy),newposz)
					ResetRotation(newEntn,g_Entity[etoclone]['anglex'], g_Entity[etoclone]['angley']+math.random(0,359),g_Entity[etoclone]['anglez'])
				end
				if clone_entity[e].formation == 2 then
					local newposx = g_Entity[etoclone]['x'] + (clone_entity[e].clone_range + rowspace[e])
					local newposz = g_Entity[etoclone]['z'] + colspace[e]
					local newposy = GetSurfaceHeight(g_Entity[etoclone]['x'],g_Entity[etoclone]['y'],g_Entity[etoclone]['z'])
					cx, cy, cz = GetObjectCentre(g_Entity[etoclone]['obj'])
					ResetPosition(newEntn,newposx,newposy+5+math.abs(cy),newposz)
					ResetRotation(newEntn,g_Entity[etoclone]['anglex'], g_Entity[etoclone]['angley'],g_Entity[etoclone]['anglez'])
					if rowcount[e] > clone_entity[e].number_per_row then
					end	
					if rowcount[e] < clone_entity[e].number_per_row then
						rowcount[e] = rowcount[e] + 1
						rowspace[e] = rowspace[e] + clone_entity[e].clone_range
					end	
					if rowcount[e] == clone_entity[e].number_per_row then						
						colcount[e] = colcount[e] + 1
						colspace[e] = clone_entity[e].clone_range * colcount[e]
						rowcount[e] = 0
						rowspace[e] = 0
					end					
				end
				GravityOn(newEntn)
				CollisionOn(newEntn)
				Show(newEntn)
				clones[e] = clones[e] + 1
				table.insert(_G[tableName[e]],newEntn)
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
			for _,v in pairs (_G[tableName[e]]) do
				if g_Entity[v] ~= nil then
					if g_Entity[v]["health"] > 0 then
						if g_EntityExtra[v] == nil then g_EntityExtra[v] = {} end
						g_EntityExtra[v]['clonedsincelevelstart'] = clone_entity[e].entity_no
					end
				end
			end
			clonesdead[e] = true
			for _,v in pairs (_G[tableName[e]]) do
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
					for a,b in pairs (_G[tableName[e]]) do
						if g_Entity[b] ~= nil then
							DeleteNewEntity(b)
							_G[tableName[e]][a] = nil
						end
						clones[e] = 0
					end
				end
				if clones[e] == 0 and clone_entity[e].respawn_cycle == 1 then
					lifetimer[e] = math.huge
					status[e] = "init"
				end
				if clones[e] == 0 and clone_entity[e].respawn_cycle == 2 then
					respawntimer[e] = g_Time + (clone_entity[e].respawn_interval*1000)
					status[e] = "respawn"
				end
			end
		end
		if status[e] == "respawn" then
			if g_Time > respawntimer[e] and clone_entity[e].respawn_events > 0 then
				respawnactive[e] = 1
				clone_entity[e].respawn_events = clone_entity[e].respawn_events -1
				lifetimer[e] = math.huge
				respawntimer[e] = math.huge
				if GetPlayerDistance(e) > 5000 then
					respawnactive[e] = 0
					SetEntityActivated(e,0)
				end
				status[e] = "init"
			end
		end
	end
end