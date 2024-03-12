-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Resource Node v9 by Necrym59
-- DESCRIPTION: Allows to use this object as a resource node to give the player the selected resource item.
-- DESCRIPTION: [@NODE_TYPE=1(1=Growth, 2=Extraction)]
-- DESCRIPTION: [NODE_TOOL_NAME$="Any"] Weapon Name
-- DESCRIPTION: [NODE_MAX_QUANTITY=1(1,10)]
-- DESCRIPTION: [NODE_SPAWNTIME=0(0,100)] Minutes
-- DESCRIPTION: [NODE_ENTITY_SPAWNTIME=1(0,100)] Minutes
-- DESCRIPTION: [NODE_ENTITY_NAME$=""]

local lower = string.lower

g_ResnodeCollected 				= {}
local resnode					= {}
local node_type 				= {}
local node_tool_name			= {}
local node_max_quantity			= {}
local entity_spawntime			= {}
local node_entity_name			= {}
local node_entity_no			= {}

local origin_x		= {}
local origin_y		= {}
local origin_z		= {}
local newposx		= {}
local newposy		= {}
local newposz		= {}
local newEntn		= {}
local cntEntn		= {}
local status		= {}
local doonce		= {}
local playonce		= {}
local nodelist		= {}
local created		= {}
local spawntimer	= {}
local nodetimer		= {}
local spawning		= {}
local scaler		= {}
local setupnode		= {}
local starthealth	= {}
local healthcheck	= {}
local wait			= {}

function resource_node_properties(e, node_type, node_tool_name, node_max_quantity, node_spawntime, node_entity_spawntime, node_entity_name, node_entity_no)
	resnode[e].node_type = node_type
	resnode[e].node_tool_name = lower(node_tool_name)
	resnode[e].node_max_quantity = node_max_quantity
	resnode[e].node_spawntime = node_spawntime
	resnode[e].node_entity_spawntime = node_entity_spawntime
	resnode[e].node_entity_name = lower(node_entity_name) or ""
	resnode[e].node_entity_no = 0
end

function resource_node_init(e)
	resnode[e] = {}
	resnode[e].node_type = 1
	resnode[e].node_tool_name = "Any"
	resnode[e].node_max_quantity = 1
	resnode[e].node_spawntime = 0
	resnode[e].node_entity_spawntime = 1	
	resnode[e].node_entity_name = ""
	resnode[e].node_entity_no = 0

	status[e] = "init"
	newEntn[e] = 0
	cntEntn[e] = 0
	spawntimer[e] = 0
	nodetimer[e] = 0
	spawning[e] = 0
	doonce[e] = 0
	playonce[e] = 0
	scaler[e] = 100	
	setupnode[e] = 0
	created[e] = 0
	starthealth[e] = 500
	healthcheck[e] = starthealth[e]
	wait[e] = math.huge
end

function resource_node_main(e)

	if status[e] == "init" then
		if resnode[e].node_tool_name == "" then resnode[e].node_tool_name = "none" end
		if resnode[e].node_type == 1 then
			CollisionOff(e)
			Hide(e)
			status[e] = "create_resource"
		end
		if resnode[e].node_type == 2 then
			status[e] = "start"
		end
		if resnode[e].node_max_quantity > 10 then resnode[e].node_max_quantity = 10 end
		if resnode[e].node_entity_spawntime > 60 then resnode[e].node_entity_spawntime = 60 end
		if resnode[e].node_entity_no == 0 or resnode[e].node_entity_no == nil then
			for ee = 1, g_EntityElementMax do
				if ee ~= nil and e~= ee and g_Entity[ee] ~= nil then
					if lower(GetEntityName(ee)) == lower(resnode[e].node_entity_name) then
						resnode[e].node_entity_no = ee
						origin_x[e] = g_Entity[ee]['x']
						origin_y[e] = g_Entity[ee]['y']
						origin_z[e] = g_Entity[ee]['z']
						CollisionOff(ee)
						Hide(ee)						
						break
					end
				end
			end
		end
	end
	
	local PlayerDist = GetPlayerDistance(e)	
	
	if status[e] == "start" then
		if resnode[e].node_type == 1 then
			if g_ResnodeCollected ~= 0 and g_ResnodeCollected == newEntn[e] and spawning[e] == 0 then
				if resnode[e].node_entity_spawntime == 0 then
					spawntimer[e] = g_Time + (1 * 1000)*2
					spawning[e] = 1
				end			
				if resnode[e].node_entity_spawntime > 0 then
					spawntimer[e] = g_Time + (resnode[e].node_entity_spawntime * 1000)*60
					spawning[e] = 1
				end	
			end
			if created[e] == resnode[e].node_max_quantity then
				if resnode[e].node_spawntime == 0 then
					Destroy(e)
				end
				if resnode[e].node_spawntime > 0 and spawning[e] == 0 then
					nodetimer[e] = g_Time + (resnode[e].node_spawntime * 1000)*60
					spawning[e] = 2
				end
			end
		end
	
		if resnode[e].node_type == 2 then
			if setupnode[e] == 0 then
				SetEntityHealth(e,starthealth[e])
				scaler[e] = scaler[e] - (scaler[e]/resnode[e].node_max_quantity)
				Scale(e,scaler[e])
				setupnode[e] = 1
			end
			
			if PlayerDist < 200 then
				if resnode[e].node_tool_name ~= g_PlayerGunName and resnode[e].node_max_quantity > 0 then PromptLocal(e,"Tool required to extract Resources") end
				if resnode[e].node_tool_name == g_PlayerGunName or resnode[e].node_tool_name == "any" and created[e] < resnode[e].node_max_quantity then PromptLocal(e,"Extract Resources") end
			end	
			if resnode[e].node_tool_name == g_PlayerGunName or resnode[e].node_tool_name == "any" then
				if g_Entity[e]['health'] < healthcheck[e] then				
					local drop = math.random(1,2)
					if drop == 2 then
						scaler[e] = scaler[e] - (scaler[e]/resnode[e].node_max_quantity)
						Scale(e,scaler[e])
						status[e] = "create_resource"
						healthcheck[e] = g_Entity[e]['health']
						if resnode[e].node_tool_name == g_PlayerGunName or resnode[e].node_tool_name == "any" then
							SetEntityHealth(e,healthcheck[e])
						else
							SetEntityHealth(e,starthealth[e])
						end
					end				
				end
				if g_Entity[e]['health'] <= 0 then
					if created[e] < resnode[e].node_max_quantity then
						status[e] = "create_resource"
						healthcheck[e] = 1
						SetEntityHealth(e,healthcheck[e])
					end
				end
				if created[e] == resnode[e].node_max_quantity then
					healthcheck[e] = 0
					SetEntityHealth(e,healthcheck[e])
					if resnode[e].node_spawntime == 0 then
						PromptLocal(e,"")
						PerformLogicConnections(e)
						CollisionOff(e)
						Hide(e)						
						Destroy(e)
					end
					if resnode[e].node_spawntime > 0 and spawning[e] == 0 then
						nodetimer[e] = g_Time + (resnode[e].node_spawntime * 1000)*60
						CollisionOff(e)
						Hide(e)
						spawning[e] = 2
					end
				end	
			end			
		end
	end	

	if status[e] == "create_resource" then			
		if doonce[e] == 0 and created[e] < resnode[e].node_max_quantity then			
			local etoclone = resnode[e].node_entity_no
			newEntn[e] = SpawnNewEntity(etoclone)			
			Show(newEntn[e])
			GravityOff(newEntn[e])
			CollisionOff(newEntn[e])
			table.insert(nodelist,newEntn[e])
			if resnode[e].node_type == 1 then
				newposx[e] = origin_x[e]
				newposy[e] = origin_y[e]
				newposz[e] = origin_z[e]
			end	
			if resnode[e].node_type == 2 then
				local newya = math.random(0,360)
				newposx[e] = g_Entity[e]['x'] + math.cos(newya) * math.random(-30,30)
				newposz[e] = g_Entity[e]['z'] + math.sin(newya) * math.random(-30,30)
				newposy[e] = GetTerrainHeight(newposx[e],newposz[e])+ math.random(5,30)	
			end			
			ResetPosition(newEntn[e],newposx[e],newposy[e],newposz[e])
			CollisionOn(newEntn[e])
			GravityOn(newEntn[e])
			cntEntn[e] = cntEntn[e] + 1
			created[e] = created[e] +1	
			g_ResnodeCollected = 0
			wait[e] = g_Time + 600			
			doonce[e] = 1
		end
		if g_Time > wait[e] then status[e] = "resource_created" end
	end
	
	if status[e] == "resource_created" then
		doonce[e] = 0
		status[e] = "start"				
	end	
	
	if g_Time > spawntimer[e] and spawning[e] == 1 then
		status[e] = "create_resource"
		spawning[e] = 0
	end
	if g_Time > nodetimer[e] and spawning[e] == 2 then
		SetEntityHealth(e,starthealth[e])
		healthcheck[e] = starthealth[e]
		Scale(e,100)
		scaler[e] = 100	
		created[e] = 0		
		if resnode[e].node_type == 1 then
			CollisionOff(e)
			Hide(e)
			spawning[e] = 1
			status[e] = "create_resource"
		end		
		if resnode[e].node_type == 2 then
			CollisionOn(e)
			Show(e)
			spawning[e] = 0
			status[e] = "start"
		end
	end
end