-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Zone Switch v4 by Necrym59
-- DESCRIPTION: This object will be treated as a switch object for activating a zone. Set Always Active = On
-- DESCRIPTION: [PROMPT_TEXT$="E to use"], [USE_RANGE=80(1,100)], [ACTIVATED_TEXT$="Zone Activated"], [ZONE_NAME$="Switched Zone Name"]
-- DESCRIPTION: Play <Sound0> when used.
-- DESCRIPTION: Play <Sound0> when after activating.

	local U = require "scriptbank\\utillib"
	local lower = string.lower

	g_zoneswitch = {}
	local zoneswitch = {}
	local prompt_text = {}
	local use_range = {}
	local zone_name = {}
	local zone_number = {}	
	local status = {}
	local activated = {}
	local state = {}

function zoneswitch_properties(e, prompt_text, use_range, activated_text, zone_name, zone_number, status)
	zoneswitch[e] = g_Entity[e]
	zoneswitch[e].prompt_text = prompt_text
	zoneswitch[e].use_range = use_range
	zoneswitch[e].activated_text = activated_text
	zoneswitch[e].zone_name = lower(zone_name)
	zoneswitch[e].zone_number = 0
	zoneswitch[e].status = 1
end 

function zoneswitch_init(e)
	zoneswitch[e] = g_Entity[e]
	zoneswitch[e].prompt_text = ""
	zoneswitch[e].use_range = 80
	zoneswitch[e].activated_text = ""
	zoneswitch[e].zone_name = ""
	zoneswitch[e].zone_number = 0
	zoneswitch[e].status = 1
	state[e] = "init"
	activated[e] = 0
	g_zoneswitch = 0
end

function zoneswitch_main(e)
	zoneswitch[e] = g_Entity[e]	
	if state[e] == "init" then
		if zoneswitch[e].zone_number == 0 or nil then
			for a = 1, g_EntityElementMax do			
				if a ~= nil and g_Entity[a] ~= nil then										
					if lower(GetEntityName(a)) == zoneswitch[e].zone_name then
						zoneswitch[e].zone_number = a
						break
					end					
				end
			end
		end
		state[e] = "endinit"
	end
	
	local PlayerDist = GetPlayerDistance(e)		
	if PlayerDist < zoneswitch[e].use_range and g_PlayerHealth > 0 then
		local LookingAt = GetPlrLookingAtEx(e,1)
		
		if zoneswitch[e].status == 1 then  --Disabled		
			if LookingAt == 1 then
				if GetGamePlayerStateXBOX() == 1 then
					PromptLocalForVR(e,"Y Button " ..zoneswitch[e].prompt_text,3)
				else
					if GetHeadTracker() == 1 then
						PromptLocalForVR(e,"Trigger " ..zoneswitch[e].prompt_text,3)
					else
						PromptLocalForVR(e, zoneswitch[e].prompt_text,3)
					end					
				end					
				
				if g_KeyPressE == 1 then
					PlaySound(e,0)						
					zoneswitch[e].status = 2
				else						
					activated[e] = 0							
					zoneswitch[e].status = 1
				end			
			end			
		end
		
		if zoneswitch[e].status == 2 then --Enabled			
			if g_KeyPressE == 1 and activated[e] == 0 then
				PromptLocalForVR(e,zoneswitch[e].activated_text,3)				
				SetActivatedWithMP(e,201)
				activated[e] = 1
				g_zoneswitch = 1
				PlaySound(e,1)
				SetAnimationName(e,"on")
				PlayAnimation(e)				
				PerformLogicConnections(e)				
			end			
		end		
	end
end
