-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Border Maps v3 by Necrym59
-- DESCRIPTION: A global behavior to check when player is near the map edge
-- DESCRIPTION: and will prompt to load the named next map.
-- DESCRIPTION: Attach to an object and set to AlwaysActive.
-- DESCRIPTION: Use the behavior diagram for map# borders
-- DESCRIPTION: [NOTIFICATION_TEXT$="Press E if you wish to leave the area"]
-- DESCRIPTION: [MAP1$=""]
-- DESCRIPTION: [MAP2$=""]
-- DESCRIPTION: [MAP3$=""]
-- DESCRIPTION: [MAP4$=""]

local bordermaps 		= {}
local notification_text = {}
local map1 				= {}
local map2 				= {}
local map3 				= {}
local map4 				= {}
	
function border_maps_properties(e, notification_text, map1, map2, map3, map4)
	bordermaps[e] = g_Entity[e]
	bordermaps[e].notification_text = notification_text
	bordermaps[e].map1 = map1
	bordermaps[e].map2 = map2
	bordermaps[e].map3 = map3
	bordermaps[e].map4 = map4	
end
 
function border_maps_init(e)
	bordermaps[e] = {}
	bordermaps[e].notification_text = "Press E if you wish to leave the area"
	bordermaps[e].map1 = "nextmap.fpm"
	bordermaps[e].map2 = ""
	bordermaps[e].map3 = ""
	bordermaps[e].map4 = ""
end
 
function border_maps_main(e)
	local mapsizeminx = GetTerrainEditableArea(0) + 200
    local mapsizeminz = GetTerrainEditableArea(1) + 200
    local mapsizemaxx = GetTerrainEditableArea(2) - 200
    local mapsizemaxz = GetTerrainEditableArea(3) - 200
	local leavingmap = 0
    if ( GetPlrObjectPositionX() < mapsizeminx ) then leavingmap=3 end
    if ( GetPlrObjectPositionX() > mapsizemaxx ) then leavingmap=4 end
    if ( GetPlrObjectPositionZ() < mapsizeminz ) then leavingmap=1 end
    if ( GetPlrObjectPositionZ() > mapsizemaxz ) then leavingmap=2 end
    if leavingmap == 1 then		
		Prompt(bordermaps[e].notification_text)
		if g_KeyPressE == 1 then JumpToLevel("mapbank\\" ..bordermaps[e].map1)end
    end
	if leavingmap == 2 then		
		Prompt(bordermaps[e].notification_text)
		if g_KeyPressE == 1 then JumpToLevel("mapbank\\" ..bordermaps[e].map2)end
    end
	if leavingmap == 3 then		
		Prompt(bordermaps[e].notification_text)
		if g_KeyPressE == 1 then JumpToLevel("mapbank\\" ..bordermaps[e].map3)end
    end
	if leavingmap == 4 then		
		Prompt(bordermaps[e].notification_text)
		if g_KeyPressE == 1 then JumpToLevel("mapbank\\" ..bordermaps[e].map4)end
    end	
end
 
function border_maps_exit(e)
end