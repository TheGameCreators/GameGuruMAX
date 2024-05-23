-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Border Maps v5 by Necrym59
-- DESCRIPTION: A global behavior to check when player is near the map edge and will prompt to load the named next map.
-- DESCRIPTION: Attach to an object and set to AlwaysActive. Use the behavior diagram for map# borders. 
-- DESCRIPTION: [NOTIFICATION_TEXT$="Press E if you wish to leave the area"]
-- DESCRIPTION: [MAP1$=""]
-- DESCRIPTION: [MAP2$=""]
-- DESCRIPTION: [MAP3$=""]
-- DESCRIPTION: [MAP4$=""]
-- DESCRIPTION: [BORDER_DISTANCE=200(100,1000)]
-- DESCRIPTION: [@LOAD_TYPE=1(1=Ask, 2=Auto)]
-- DESCRIPTION: [SPAWN_MARKER_USER_GLOBAL$="MySpawnMarkers"] user global for using spawn markers
-- DESCRIPTION: [SPAWN_MARKER_NAME1$=""] for spawn marker on map 1
-- DESCRIPTION: [SPAWN_MARKER_NAME2$=""] for spawn marker on map 2
-- DESCRIPTION: [SPAWN_MARKER_NAME3$=""] for spawn marker on map 3
-- DESCRIPTION: [SPAWN_MARKER_NAME4$=""] for spawn marker on map 4


local bordermaps 		= {}
local notification_text = {}
local map1 				= {}
local map2 				= {}
local map3 				= {}
local map4 				= {}
local border_distance	= {}
local load_type			= {}
local spawn_marker_user_global	= {}
local spawn_marker_name1		= {}
local spawn_marker_name2		= {}
local spawn_marker_name3		= {}
local spawn_marker_name4		= {}
	
function border_maps_properties(e, notification_text, map1, map2, map3, map4, border_distance, load_type, spawn_marker_user_global, spawn_marker_name1, spawn_marker_name2, spawn_marker_name3, spawn_marker_name4)
	bordermaps[e] = g_Entity[e]
	bordermaps[e].notification_text = notification_text
	bordermaps[e].map1 = map1
	bordermaps[e].map2 = map2
	bordermaps[e].map3 = map3
	bordermaps[e].map4 = map4
	bordermaps[e].border_distance = border_distance
	bordermaps[e].load_type = load_type
	bordermaps[e].spawn_marker_user_global = spawn_marker_user_global
	bordermaps[e].spawn_marker_name1 = spawn_marker_name1
	bordermaps[e].spawn_marker_name2 = spawn_marker_name2
	bordermaps[e].spawn_marker_name3 = spawn_marker_name3
	bordermaps[e].spawn_marker_name4 = spawn_marker_name4	
end
 
function border_maps_init(e)
	bordermaps[e] = {}
	bordermaps[e].notification_text = "Press E if you wish to leave the area"
	bordermaps[e].map1 = "nextmap.fpm"
	bordermaps[e].map2 = ""
	bordermaps[e].map3 = ""
	bordermaps[e].map4 = ""
	bordermaps[e].border_distance = 200
	bordermaps[e].load_type = 1
	bordermaps[e].spawn_marker_user_global = "MySpawnMarkers"
	bordermaps[e].spawn_marker_name1 =  ""
	bordermaps[e].spawn_marker_name2 =  ""
	bordermaps[e].spawn_marker_name3 =  ""
	bordermaps[e].spawn_marker_name4 =  ""
end
 
function border_maps_main(e)
	local mapsizeminx = GetTerrainEditableArea(0) + bordermaps[e].border_distance
    local mapsizeminz = GetTerrainEditableArea(1) + bordermaps[e].border_distance
    local mapsizemaxx = GetTerrainEditableArea(2) - bordermaps[e].border_distance
    local mapsizemaxz = GetTerrainEditableArea(3) - bordermaps[e].border_distance
	local leavingmap = 0
    if ( GetPlrObjectPositionX() < mapsizeminx ) then leavingmap=3 end
    if ( GetPlrObjectPositionX() > mapsizemaxx ) then leavingmap=4 end
    if ( GetPlrObjectPositionZ() < mapsizeminz ) then leavingmap=1 end
    if ( GetPlrObjectPositionZ() > mapsizemaxz ) then leavingmap=2 end
    if leavingmap == 1 then
		if _G["g_UserGlobal['"..bordermaps[e].spawn_marker_user_global.."']"] ~= nil then _G["g_UserGlobal['"..bordermaps[e].spawn_marker_user_global.."']"] = bordermaps[e].spawn_marker_name1 end
		if bordermaps[e].load_type == 1 then
			Prompt(bordermaps[e].notification_text)
			if g_KeyPressE == 1 then
				JumpToLevel("mapbank\\" ..bordermaps[e].map1)
			end	
		end
		if bordermaps[e].load_type == 2 then JumpToLevel("mapbank\\" ..bordermaps[e].map1) end
    end
	if leavingmap == 2 then	
		if _G["g_UserGlobal['"..bordermaps[e].spawn_marker_user_global.."']"] ~= nil then _G["g_UserGlobal['"..bordermaps[e].spawn_marker_user_global.."']"] = bordermaps[e].spawn_marker_name2 end
		if bordermaps[e].load_type == 1 then
			Prompt(bordermaps[e].notification_text)
			if g_KeyPressE == 1 then
				JumpToLevel("mapbank\\" ..bordermaps[e].map2)
			end	
		end
		if bordermaps[e].load_type == 2 then JumpToLevel("mapbank\\" ..bordermaps[e].map2) end
    end
	if leavingmap == 3 then
		if _G["g_UserGlobal['"..bordermaps[e].spawn_marker_user_global.."']"] ~= nil then _G["g_UserGlobal['"..bordermaps[e].spawn_marker_user_global.."']"] = bordermaps[e].spawn_marker_name3 end
		if bordermaps[e].load_type == 1 then
			Prompt(bordermaps[e].notification_text)
			if g_KeyPressE == 1 then
				JumpToLevel("mapbank\\" ..bordermaps[e].map3)
			end	
		end
		if bordermaps[e].load_type == 2 then JumpToLevel("mapbank\\" ..bordermaps[e].map3) end
    end
	if leavingmap == 4 then
		if _G["g_UserGlobal['"..bordermaps[e].spawn_marker_user_global.."']"] ~= nil then _G["g_UserGlobal['"..bordermaps[e].spawn_marker_user_global.."']"] = bordermaps[e].spawn_marker_name4 end
		if bordermaps[e].load_type == 1 then
			Prompt(bordermaps[e].notification_text)
			if g_KeyPressE == 1 then
				JumpToLevel("mapbank\\" ..bordermaps[e].map4)
			end	
		end
		if bordermaps[e].load_type == 2 then JumpToLevel("mapbank\\" ..bordermaps[e].map4) end
    end	
end
 
function border_maps_exit(e)
end