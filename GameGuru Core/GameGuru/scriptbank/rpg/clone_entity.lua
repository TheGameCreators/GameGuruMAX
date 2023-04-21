-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Clone_Entity v5 by Necrym59
-- DESCRIPTION: Allows cloning of a Named Entity when triggered.
-- DESCRIPTION: Attach to an object then logic link to a switch or zone. 
-- DESCRIPTION: [CLONE_QUANTITY=1(1,100)]
-- DESCRIPTION: [CLONE_RANGE=100(50,1000)]
-- DESCRIPTION: [ENTITY_NAME$="Rabbit"]

local lower = string.lower
	
local clone_entity 		= {}
local clone_quantity	= {}
local entity_name		= {}
local status			= {}
local clones			= {}

function clone_entity_properties(e, clone_quantity, clone_range, entity_name, entity_no)
	clone_entity[e].clone_quantity = clone_quantity or 1
	clone_entity[e].clone_range = clone_range
	clone_entity[e].entity_name = lower(entity_name)
	clone_entity[e].entity_no = 0
end

function clone_entity_init(e)
	clone_entity[e] = {}
	clone_entity[e].clone_quantity = 1
	clone_entity[e].clone_range = 100
	clone_entity[e].entity_name = "Rabbit"	
	clone_entity[e].entity_no = 0
	clone_entity[e].timetonextspawn = 0
	status[e] = "init"	
	clones[e] = 0
end

function clone_entity_main(e)
	if status[e] == "init" then
		clone_entity[e].entity_no = 0
		clone_entity[e].timetonextspawn = g_Time + 5
		status[e] = "find_entity"
	end
	if status[e] == "find_entity" then
		if clone_entity[e].entity_no == 0 or nil then
			for ee = 1, g_EntityElementMax do			
				if ee ~= nil and e~= ee and g_Entity[ee] ~= nil then										
					if lower(GetEntityName(ee)) == clone_entity[e].entity_name then
						clone_entity[e].entity_no = ee						
						status[e] = "clone_entity"
						break
					end			
				end				
			end			
		end
	end
	if g_Entity[e]['plrinzone'] == 1 and g_PlayerPosY > g_Entity[e]['y'] then
		ActivateIfUsed(e)
		PerformLogicConnections(e)
		SetEntityActivated(e,1)
	end	
	if g_Entity[e]['activated'] == 1 then
		if status[e] == "clone_entity" then		
			if g_Time > clone_entity[e].timetonextspawn then
				clone_entity[e].timetonextspawn = g_Time + 5
				local etoclone = clone_entity[e].entity_no
				local newEntn = SpawnNewEntity(etoclone)
				local newposx = g_Entity[etoclone]['x'] + math.random(-clone_entity[e].clone_range,clone_entity[e].clone_range)
				local newposz = g_Entity[etoclone]['z'] + math.random(-clone_entity[e].clone_range,clone_entity[e].clone_range)
				local newposy = GetTerrainHeight(newposx,newposz)
				cx, cy, cz = GetObjectCentre( g_Entity[e]['obj'] )
				ResetPosition(newEntn,newposx,newposy+1+math.abs(cy),newposz)
				ResetRotation(newEntn,g_Entity[etoclone]['anglex'], g_Entity[etoclone]['angley']+math.random(0,359),g_Entity[etoclone]['anglez'])
				clones[e] = clones[e] + 1
				if clones[e] >= clone_entity[e].clone_quantity then
					status[e] = "all_entities_cloned"					
				end
			end
		end
	end
end