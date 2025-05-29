-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Spawn Locator v6 by Necrym59
-- DESCRIPTION: Will relocate the player to a designated spawn marker. Place under the Player Start Marker.
-- DESCRIPTION: [@@SPAWN_MARKER_USER_GLOBAL$=""(0=globallist)] user global required for using spawn markers (eg: MySpawnMarkers)

local lower = string.lower
local spawn_locator = {}
local spawn_marker_user_global = {}
local spawn_marker_name = {}
local spawn_marker_number = {}
local zoneheight = {}

local pos_x	= {}
local pos_y	= {}
local pos_z	= {}
local ang_y	= {}
local status = {}

function spawn_locator_properties(e, spawn_marker_user_global, spawn_marker_name)
	spawn_locator[e].spawn_marker_user_global = spawn_marker_user_global
	spawn_locator[e].spawn_marker_name = spawn_marker_name or ""
end 

function spawn_locator_init(e)
	spawn_locator[e] = {}
	spawn_locator[e].spawn_marker_user_global = "MySpawnMarkers"
	spawn_locator[e].spawn_marker_name = ""
	spawn_locator[e].spawn_marker_number = 0
	spawn_locator[e].zoneheight = 100
	SetActivated(e,1)
	status[e] = "init"
end

function spawn_locator_main(e)
	if status[e] == "init" then
		spawn_locator[e].spawn_marker_number = 0
		status[e] = "endinit"
	end
	
	if _G["g_UserGlobal['"..spawn_locator[e].spawn_marker_user_global.."']"] ~= nil then
		spawn_locator[e].spawn_marker_name = _G["g_UserGlobal['"..spawn_locator[e].spawn_marker_user_global.."']"]
		spawn_locator[e].spawn_marker_number = 0
	end	
	
	if spawn_locator[e].spawn_marker_number == 0 and spawn_locator[e].spawn_marker_name ~= "" then		
		for ee = 1, g_EntityElementMax do
			if ee ~= nil and g_Entity[ee] ~= nil then
				if lower(GetEntityName(ee)) == lower(spawn_locator[e].spawn_marker_name) then
					spawn_locator[e].spawn_marker_number = ee
					pos_x[e] = g_Entity[ee]['x']
					pos_y[e] = g_Entity[ee]['y']
					pos_z[e] = g_Entity[ee]['z']
					ang_y[e] = g_Entity[ee]['angley']
					GravityOff(ee)
					CollisionOff(ee)
					Hide(ee)
					status[e] = "spawnplayer"
					break
				end				
			end
		end
	end
	if status[e] == "spawnplayer" then
		if g_Entity[e]['plrinzone'] == 1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+spawn_locator[e].zoneheight and g_PlayerHealth >= g_gameloop_StartHealth then
			if spawn_locator[e].marker_number ~= 0 then
				SetFreezePosition(pos_x[e],pos_y[e]+35,pos_z[e])
				TransportToFreezePositionOnly()
				SetGamePlayerControlFinalCameraAngley(ang_y[e])
				status[e] = "init"
			end
		end
	end
end

