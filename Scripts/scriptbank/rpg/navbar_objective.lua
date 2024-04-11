-- Navigation Bar Objective v8 by Smallg and Necrym59
-- DESCRIPTION: Adds the entity to the Navigation Bar.
-- DESCRIPTION: [IMAGEFILE$="imagebank\\navbar\\objective.png"] icon for the navigation bar.
-- DESCRIPTION: [FIXEDSIZE!=0] if 1 will scale icon with the distance
-- DESCRIPTION: [IGNORERANGE!=0] if 1 will show even out of range
-- DESCRIPTION: [PROXIMITY_RANGE=80(1,100)]
-- DESCRIPTION: [@WHEN_IN_PROXIMITY=1(1=Do Nothing, 2=Remove Marker, 3=Trigger + Remove Marker, 4=Destroy Entity)]
-- DESCRIPTION: [@ENTITY_TYPE=1(1=This Entity, 2=Named Entity)] 
-- DESCRIPTION: [ENTITY_NAME$=""]

local lower = string.lower

g_navigation_entity 	= {}
local isadded 			= {}
local imagename 		= {}
local fixedsize 		= {}
local ignorerange 		= {}
local tname 			= {}
local proximity_range 	= {}
local when_in_proximity = {}
local entity_type 		= {}
local entity_name 		= {}
local entity_no 		= {}

function navbar_objective_properties(e, imagename, fixedsize, ignorerange, proximity_range, when_in_proximity, entity_type, entity_name)
	g_navigation_entity[e].imagename = imagename or imagefile
	g_navigation_entity[e].fixedsize = fixedsize
	g_navigation_entity[e].ignorerange = ignorerange
	g_navigation_entity[e].proximity_range = proximity_range
	g_navigation_entity[e].when_in_proximity = when_in_proximity	
	g_navigation_entity[e].entity_type = entity_type
	g_navigation_entity[e].entity_name = lower(entity_name) or ""	
end 

function navbar_objective_init_name(e,name)
	g_navigation_entity[e] = {}
	g_navigation_entity[e].imagename = ""
	g_navigation_entity[e].fixedsize = 0
	g_navigation_entity[e].ignorerange = 0
	g_navigation_entity[e].proximity_range = 80
	g_navigation_entity[e].when_in_proximity = 1
	g_navigation_entity[e].entity_type = 1
	g_navigation_entity[e].entity_name = ""
	g_navigation_entity[e].tname = name
	entity_no[e] = 0
end 

function navbar_objective_main(e)

	local PlayerDist = GetPlayerDistance(e)

	if PlayerDist < g_navigation_entity[e].proximity_range then
		if g_navigation_entity[e].when_in_proximity == 1 then end
		if g_navigation_entity[e].when_in_proximity == 2 then RemoveFromNavbar(e) end
		if g_navigation_entity[e].when_in_proximity == 3 then
			PerformLogicConnections(e)
			ActivateIfUsed(e)
			RemoveFromNavbar(e)
		end
		if g_navigation_entity[e].when_in_proximity == 4 then
			RemoveFromNavbar(e)
			Destroy(e)
		end			
	end
	if g_Entity[entity_no[e]] ~= nil then 
		if isadded[e] == true and g_Entity[entity_no[e]]['health'] <= 0 then
			RemoveFromNavbar(entity_no[e])
			isadded[e] = -1
		end
	end
	if 	g_navigation_entity[e].entity_type == 1 and  GetEntityVisibility(e) ~= 0 then
		if not isadded[e] then 
			if nbar ~= 0 then 
				isadded[e] = AddToNavbar(e, g_navigation_entity[e].imagename,g_navigation_entity[e].fixedsize, g_navigation_entity[e].ignorerange, g_navigation_entity[e].tname)
			end
		end
	end	
	if 	g_navigation_entity[e].entity_type == 2 then
		if g_navigation_entity[e].entity_name ~= "" then
			if not isadded[e] then
				for n = 1, g_EntityElementMax do
					if n ~= nil and g_Entity[n] ~= nil then
						if lower(GetEntityName(n)) == g_navigation_entity[e].entity_name then 
							entity_no[e] = n
							break
						end
					end
				end			
				if nbar ~= 0 then 
					isadded[e] = AddToNavbar(entity_no[e], g_navigation_entity[e].imagename,g_navigation_entity[e].fixedsize, g_navigation_entity[e].ignorerange, g_navigation_entity[e].tname)
				end
			end
		end
	end		
end 