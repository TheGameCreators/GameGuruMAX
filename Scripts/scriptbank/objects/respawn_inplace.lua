-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Repawn In-Place v6
-- DESCRIPTION: Creates a in-place respawn marker when a player death occurs
-- DESCRIPTION: [@@SPAWN_MARKER_USER_GLOBAL$=""(0=globallist)] user global required for using spawn markers (eg: MySpawnMarkers)
-- DESCRIPTION: [SPAWN_MARKER_NAME$=""] name of spawn marker to dynamically respawn to (eg: 'Player Respawn')

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
	respawn[e].spawn_marker_user_global = ""
	respawn[e].spawn_marker_name = ""
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
							GravityOff(ee)
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