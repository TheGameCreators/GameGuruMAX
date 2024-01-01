-- Destroy Object v5 by Necrym59
-- DESCRIPTION: Attached object can monitor a Named Objects health or destroy instantly when triggered by a linked switch or zone.
-- DESCRIPTION: [MESSAGE$="Destroyed"]
-- DESCRIPTION: [OBJECT_NAME$=""]
-- DESCRIPTION: [PARTICLE_NAME$=""]
-- DESCRIPTION: [!DESTROY_PARTICLE=0]
-- DESCRIPTION: [!HEALTH_MONITOR=0]


local lower = string.lower
local desobject				= {}
local message				= {}
local object_name 			= {}
local particle_name 		= {}
local destroy_particle		= {}
local health_monitor		= {}
local object_no				= {}
local particle_no			= {}

local status				= {}
local destroy_delay			= {}
local doonce				= {}

function destroy_object_properties(e, message, object_name, particle_name, destroy_particle, health_monitor)
	desobject[e] = g_Entity[e]
	desobject[e].message = message
	desobject[e].object_name = lower(object_name)
	desobject[e].particle_name = lower(particle_name)	
	desobject[e].destroy_particle = destroy_particle
	desobject[e].health_monitor	= health_monitor
	desobject[e].object_no = 0
	desobject[e].particle_no = 0
end

function destroy_object_init(e)
	desobject[e] = {}
	desobject[e].message = message
	desobject[e].object_name = ""		
	desobject[e].particle_name = ""
	desobject[e].destroy_particle = 0
	desobject[e].health_monitor	= 0
	desobject[e].object_no = 0
	desobject[e].particle_no = 0

	destroy_delay[e] = 0
	doonce[e] = 0
	status[e] = "init"
end
 
function destroy_object_main(e)

	if status[e] == "init" then

		if desobject[e].object_no == 0 or desobject[e].object_no == nil then
			for m = 1, g_EntityElementMax do
				if m ~= nil and g_Entity[m] ~= nil then
					if lower(GetEntityName(m)) == lower(desobject[e].object_name) then
						desobject[e].object_no = m
						break
					end
				end			
			end
		end	
	
		if desobject[e].particle_no == 0 or desobject[e].particle_no == nil then
			for n = 1, g_EntityElementMax do
				if n ~= nil and g_Entity[n] ~= nil then
					if lower(GetEntityName(n)) == lower(desobject[e].particle_name) then
						desobject[e].particle_no = n
						Hide(n)
						break
					end
				end			
			end
		end

		if desobject[e].health_monitor == 1 then SetEntityActivated(e,1) end
		if desobject[e].health_monitor == 0 then SetEntityActivated(e,0) end
		StartTimer(e)
		status[e] = "run"
	end
	
	if status[e] == "run" then

		if desobject[e].health_monitor == 0 then
			if g_Entity[e]['activated'] == 1 then
				SetEntityHealth(desobject[e].object_no,1)
				desobject[e].health_monitor = 1
			end	
		end
		
		if desobject[e].health_monitor == 1 then
			if g_Entity[desobject[e].object_no]['health'] < 2 and doonce[e] == 0 then
				ResetPosition(desobject[e].particle_no,g_Entity[desobject[e].object_no]['x'],g_Entity[desobject[e].object_no]['y'],g_Entity[desobject[e].object_no]['z'])
				if desobject[e].particle_name > "" then Show(desobject[e].particle_no) end
				PerformLogicConnections(e)
				destroy_delay[e] = GetTimer(e) + 500
				doonce[e] = 1
			end	

			if destroy_delay[e] > 0 then 
				if GetTimer(e) > destroy_delay[e] then
					if desobject[e].object_no > 0 then
						PromptDuration(desobject[e].message,1500)		
						SetEntityHealth(desobject[e].object_no,0)
						Hide(desobject[e].object_no)
						CollisionOff(desobject[e].object_no)
						Destroy(desobject[e].object_no)
					end	
					if desobject[e].destroy_particle == 1 then
						if desobject[e].particle_name > "" then
							Hide(desobject[e].particle_no)
							Destroy(desobject[e].particle_no)
						end	
					end
					SwitchScript(e,"no_behavior_selected.lua")
				end
			end
		end
	end	
end

function destroy_object_exit(e)
end