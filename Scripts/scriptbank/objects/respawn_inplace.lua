-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Repawn In-Place v3
-- DESCRIPTION: Creates a in-place respawn point when a player death occurs. Attach to an object set Always On. 
-- DESCRIPTION: Place one object as the Spawn Marker on the map and give it a unique name. Place that name in here.
-- DESCRIPTION: [SPAWN_MARKER_USER_GLOBAL$="MySpawnMarkers"] user global required for using spawn markers
-- DESCRIPTION: [SPAWN_MARKER_NAME$="Player Respawn"] name of spawn marker to dynamically respawn to

local lower = string.lower
local respawn					= {}
local spawn_marker_user_global	= {}
local spawn_marker_name			= {}
local spawn_marker_number		= {}
local status					= {}

function respawn_inplace_properties(e, spawn_marker_user_global, spawn_marker_name)
	respawn[e].spawn_marker_user_global = spawn_marker_user_global
	respawn[e].spawn_marker_name = spawn_marker_name	
end 

function respawn_inplace_init(e)
	respawn[e] = {}
	respawn[e].spawn_marker_user_global = "MySpawnMarkers"
	respawn[e].spawn_marker_name = "Player Respawn"
	respawn[e].spawn_marker_number = 0
	status[e] = "init"
end

function respawn_inplace_main(e)

	if status[e] == "init" then
		status[e] = "check"
	end

	if status[e] == "check" then		
		if g_PlayerHealth <= 1 then
			if respawn[e].spawn_marker_number == 0 and respawn[e].spawn_marker_name ~= "" then
				for ee = 1, g_EntityElementMax do
					if ee ~= nil and g_Entity[ee] ~= nil then
						if lower(GetEntityName(ee)) == lower(respawn[e].spawn_marker_name) then
							respawn[e].spawn_marker_number = ee
							CollisionOff(ee)
							Hide(ee)
							_G["g_UserGlobal['"..respawn[e].spawn_marker_user_global.."']"] = respawn[e].spawn_marker_name
							break
						end
					end
				end
			end
			SetPosition(respawn[e].spawn_marker_number,g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
			ResetPosition(respawn[e].spawn_marker_number,g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
			SetRotation(respawn[e].spawn_marker_number,g_Entity[respawn[e].spawn_marker_number]['anglex'],g_PlayerAngY,g_Entity[respawn[e].spawn_marker_number]['anglez'])		
		end
	end	
end