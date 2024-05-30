-- Teleport v14 - by Necrym59
-- DESCRIPTION: Allows for a teleport to a local connected point or to another level.
-- DESCRIPTION: [TELEPORT_ZONEHEIGHT=100]
-- DESCRIPTION: [@TELEPORT_TYPE=1(1=Instant, 2=Delayed, 3=Delayed + Countdown)]
-- DESCRIPTION: [@TELEPORT_MODE=1(1=Single-use, 2=Re-useable)]
-- DESCRIPTION: [TELEPORT_DELAY=5(1,5))]
-- DESCRIPTION: [@TELEPORT_EFFECT=1(1=None, 2=Warp, 3=Particle)]
-- DESCRIPTION: [TELEPORT_EXIT_ANGLE=1(1,360))] Player exit angle upon teleport
-- DESCRIPTION: [PARTICLE_NAME$="TeleportParticle"]
-- DESCRIPTION: [PLAYER_LEVEL=0(0,100))] player level to be able use this teleport
-- DESCRIPTION: [@DESTINATION=1(1=Local, 2=Level)]
-- DESCRIPTION: [SPAWN_MARKER_USER_GLOBAL$=""] user global required for using inter-level spawn markers (eg: MySpawnMarkers)
-- DESCRIPTION: [SPAWN_MARKER_NAME$=""] for optional spawning using spawn markers
-- DESCRIPTION: [@GoToLevelMode=1(1=Use Storyboard Logic,2=Go to Specific Level)] controls whether the next level in the Storyboard, or another level is loaded after entry to the zone.
-- DESCRIPTION: [ResetStates!=0] when entering the next level
-- DESCRIPTION: Will play <Sound0> for local teleports to the location of the object you connected with the zone.
-- DESCRIPTION: Will play <Sound1> for level teleport. It is better to use a small object or a flat object to avoid getting stuck when you reappear in the level.
 
g_teleport = {}	
local lower = string.lower

local teleport_target = {}
local teleport_zoneheight = {}
local teleport_type = {}
local teleport_mode = {}
local teleport_delay = {}	
local teleport_timer = {}
local teleport_effect = {}
local teleport_exit_angle = {}	
local player_level = {}
local particle_name = {}
local destination = {}
local spawn_marker_user_global	= {}
local spawn_marker_name	= {}
local resetstates = {}

local particle_no = {}
local teleport_timer = {}
local tlevelrequired = {}
local tplayerlevel = {}
local dest_angle = {}
local effect = {}
local played = {}
local doonce = {}
local status = {}
	
function teleport_properties(e, teleport_zoneheight, teleport_type, teleport_mode, teleport_delay, teleport_effect, teleport_exit_angle, particle_name, player_level, destination, spawn_marker_user_global, spawn_marker_name, resetstates)
	g_teleport[e].teleport_target = GetEntityString(e,0)
	g_teleport[e].teleport_zoneheight = teleport_zoneheight
	g_teleport[e].teleport_type = teleport_type
	g_teleport[e].teleport_mode = teleport_mode
	g_teleport[e].teleport_delay = teleport_delay
	g_teleport[e].teleport_effect = teleport_effect
	g_teleport[e].teleport_exit_angle = teleport_exit_angle
	g_teleport[e].particle_name	= lower(particle_name)
	g_teleport[e].teleport_level = player_level or 0	
	g_teleport[e].destination = destination
	g_teleport[e].spawn_marker_user_global = spawn_marker_user_global
	g_teleport[e].spawn_marker_name = spawn_marker_name
	g_teleport[e].resetstates = resetstates
end

function teleport_init(e)
	g_teleport[e] = {}
	g_teleport[e].teleport_target = GetEntityString(e,0)	
	g_teleport[e].teleport_zoneheight = 100
	g_teleport[e].teleport_type = 1
	g_teleport[e].teleport_delay = 0
	g_teleport[e].teleport_mode = 2
	g_teleport[e].teleport_effect = 0
	g_teleport[e].teleport_exit_angle = 0	
	g_teleport[e].particle_name = ""
	g_teleport[e].teleport_level = 0
	g_teleport[e].destination = 1
	g_teleport[e].spawn_marker_user_global = ""
	g_teleport[e].spawn_marker_name = ""
	g_teleport[e].resetstates = resetstates	

	g_teleport[e].particle_no = 0
	g_teleport[e].teleport_timer = 0	
	fov = GetGamePlayerStateCameraFov()
	tplayerlevel[e] = 0
	dest_angle[e] = 0
	tlevelrequired[e] = 0
	effect[e] = 0
	played[e] = 0
	doonce[e] = 0
	status[e] = "init"
end

function teleport_main(e)

	if status[e] == "init" then
		tlevelrequired[e] = g_teleport[e].teleport_level
		fov = GetGamePlayerStateCameraFov()
		dest_angle[e] = g_teleport[e].teleport_exit_angle

		if g_teleport[e].teleport_effect == 3 then
			if g_teleport[e].particle_no == 0 or g_teleport[e].particle_no == nil then
				for n = 1, g_EntityElementMax do
					if n ~= nil and g_Entity[n] ~= nil then
						if lower(GetEntityName(n)) == g_teleport[e].particle_name then
							g_teleport[e].particle_no = n
							SetPosition(n,g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'])
							Hide(n)
							break
						end
					end
				end
			end
		end	
		status[e] = "endinit"
	end

	if g_teleport[e].teleport_type == 1 then
		if g_teleport[e].destination == 1 then
			if g_Entity[e]['plrinzone']==1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+g_teleport[e].teleport_zoneheight then				
				if _G["g_UserGlobal['".."MyPlayerLevel".."']"] ~= nil then tplayerlevel[e] = _G["g_UserGlobal['".."MyPlayerLevel".."']"] end
				if tplayerlevel[e] < tlevelrequired[e] then Prompt("You need to be level "..tlevelrequired[e].." to use this teleport") end
				if tplayerlevel[e] >= tlevelrequired[e] then					
					if g_teleport[e].teleport_target ~= nil then
						if g_teleport[e].teleport_effect == 1 then
							PlaySound(e,0)
							TransportToIfUsed(e)
							SetGamePlayerControlFinalCameraAngley(dest_angle[e])
							PerformLogicConnections(e)
							SetPlayerFOV(fov)
							effect[e] = 0
							if g_teleport[e].teleport_mode == 1 then Destroy(e) end	
							g_teleport[e].teleport_timer = 0
						end
						if g_teleport[e].teleport_effect == 2 then
							local entID = GetEntityRelationshipID(e,0)
							PointCamera(0, GetEntityPositionX(entID), GetEntityPositionY(entID), GetEntityPositionZ(entID))
							if effect[e] > -50 then
								SetPlayerFOV(fov+effect[e])
								effect[e] = effect[e] - 5						
							end
							if effect[e] <= -50 then
								PlaySound(e,0)
								TransportToIfUsed(e)
								SetGamePlayerControlFinalCameraAngley(dest_angle[e])
								SetPlayerFOV(fov)
								effect[e] = 0
								if g_teleport[e].teleport_mode == 1 then Destroy(e) end
								g_teleport[e].teleport_timer = 0
							end
						end	
						if g_teleport[e].teleport_effect == 3 then							
							if doonce[e] == 0 then
								ResetPosition(g_teleport[e].particle_no,g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
								Show(g_teleport[e].particle_no)
								SetCameraOverride(3)
								doonce[e] = 1
							end	
							if effect[e] < 100 then	effect[e] = effect[e] + 0.5 end
							if effect[e] >= 100 then
								PlaySound(e,0)
								TransportToIfUsed(e)
								SetGamePlayerControlFinalCameraAngley(dest_angle[e])								
								PerformLogicConnections(e)
								effect[e] = 0
								SetCameraOverride(0)
								Hide(g_teleport[e].particle_no)
								if g_teleport[e].teleport_mode == 1 then Destroy(e) end
								doonce[e] = 0
								g_teleport[e].teleport_timer = 0
							end
						end	
					end
				end
			end
		end
		if g_teleport[e].destination == 2 then
			if g_Entity[e]['plrinzone']==1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+g_teleport[e].teleport_zoneheight then
				if _G["g_UserGlobal['".."MyPlayerLevel".."']"] ~= nil then tplayerlevel[e] = _G["g_UserGlobal['".."MyPlayerLevel".."']"] end
				if _G["g_UserGlobal['"..g_teleport[e].spawn_marker_user_global.."']"] ~= nil then _G["g_UserGlobal['"..g_teleport[e].spawn_marker_user_global.."']"] = g_teleport[e].spawn_marker_name end
				if tplayerlevel[e] < tlevelrequired[e] then Prompt("You need to be level "..tlevelrequired[e].." to use this teleport") end
				if tplayerlevel[e] >= tlevelrequired[e] then
					if played[e] == 0 then					
						PlaySound(e,1)
						played[e] = 1
					end				
					PerformLogicConnections(e)
					JumpToLevelIfUsedEx(e,g_teleport[e].resetstates)					
				end
			end
		end
	end
	
	if g_teleport[e].teleport_type == 2 or 3 then
		if g_teleport[e].destination == 1 then						
			if g_Entity[e]['plrinzone']==1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+g_teleport[e].teleport_zoneheight then
				if _G["g_UserGlobal['".."MyPlayerLevel".."']"] ~= nil then tplayerlevel[e] = _G["g_UserGlobal['".."MyPlayerLevel".."']"] end
				if tplayerlevel[e] < tlevelrequired[e] then Prompt("You need to be level "..tlevelrequired[e].." to use this teleport") end
				if tplayerlevel[e] >= tlevelrequired[e] then
					if g_teleport[e].teleport_target ~= nil then
						g_teleport[e].teleport_timer = g_teleport[e].teleport_timer + GetElapsedTime()
						if g_teleport[e].teleport_type == 3 then Prompt("Teleport activating in "..math.floor(g_teleport[e].teleport_delay - g_teleport[e].teleport_timer).." seconds")	end
						if g_teleport[e].teleport_timer >= g_teleport[e].teleport_delay and g_teleport[e].teleport_effect == 1 or g_teleport[e].teleport_effect == 2 then							
							if g_teleport[e].teleport_effect == 1 then
								Prompt("")
								PlaySound(e,0)
								TransportToIfUsed(e)
								SetGamePlayerControlFinalCameraAngley(dest_angle[e])
								PerformLogicConnections(e)
								SetPlayerFOV(fov)
								effect[e] = 0
								if g_teleport[e].teleport_mode == 1 then Destroy(e) end
								g_teleport[e].teleport_timer = 0								
							end
							if g_teleport[e].teleport_effect == 2 and g_teleport[e].teleport_timer >= g_teleport[e].teleport_delay then
								local entID = GetEntityRelationshipID(e,0)
								PointCamera(0, GetEntityPositionX(entID), GetEntityPositionY(entID), GetEntityPositionZ(entID))
								if effect[e] > -50 then
									SetPlayerFOV(fov+effect[e])	
									effect[e] = effect[e] - 5							
								end
								if effect[e] <= -50 then
									Prompt("")
									PlaySound(e,0)
									TransportToIfUsed(e)
									SetGamePlayerControlFinalCameraAngley(dest_angle[e])									
									PerformLogicConnections(e)
									effect[e] = 0
									SetPlayerFOV(fov)																		
									if g_teleport[e].teleport_mode == 1 then Destroy(e) end
									g_teleport[e].teleport_timer = 0
								end								
							end
						end
						if g_teleport[e].teleport_effect == 3 and g_teleport[e].particle_no > 0 then
							if doonce[e] == 0 then
								ResetPosition(g_teleport[e].particle_no,g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
								Show(g_teleport[e].particle_no)
								SetCameraOverride(3)
								doonce[e] = 1
							end	
							if effect[e] < 100 then	effect[e] = effect[e] + 1 end
							if effect[e] >= 100 and g_teleport[e].teleport_timer >= g_teleport[e].teleport_delay then
								Prompt("")
								PlaySound(e,0)
								TransportToIfUsed(e)
								SetGamePlayerControlFinalCameraAngley(dest_angle[e])
								PerformLogicConnections(e)
								SetCameraOverride(0)
								effect[e] = 0
								Hide(g_teleport[e].particle_no)
								if g_teleport[e].teleport_mode == 1 then Destroy(e) end
								doonce[e] = 0
								g_teleport[e].teleport_timer = 0
							end	
						end
					end
				end
			end
		end

		if g_teleport[e].destination == 2 then
			if g_Entity[e]['plrinzone']==1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+g_teleport[e].teleport_zoneheight then
				if _G["g_UserGlobal['".."MyPlayerLevel".."']"] ~= nil then tplayerlevel[e] = _G["g_UserGlobal['".."MyPlayerLevel".."']"] end
				if _G["g_UserGlobal['"..g_teleport[e].spawn_marker_user_global.."']"] ~= nil then _G["g_UserGlobal['"..g_teleport[e].spawn_marker_user_global.."']"] = g_teleport[e].spawn_marker_name end
				if tplayerlevel[e] < tlevelrequired[e] then Prompt("You need to be level "..tlevelrequired[e].." to use this teleport") end
				if tplayerlevel[e] >= tlevelrequired[e] then
					g_teleport[e].teleport_timer = g_teleport[e].teleport_timer + GetElapsedTime()
					if g_teleport[e].teleport_type == 3 then
						Prompt("Level Teleport in "..math.floor(g_teleport[e].teleport_delay - g_teleport[e].teleport_timer).." seconds")
					end
					if g_teleport[e].teleport_timer >= g_teleport[e].teleport_delay then
						if played[e] == 0 then					
							PlaySound(e,1)
							played[e] = 1						
						end	
						PerformLogicConnections(e)
						JumpToLevelIfUsedEx(e,g_teleport[e].resetstates)
						SetPlayerFOV(fov)
					end
				end
			end
		end		
	end	
end
	