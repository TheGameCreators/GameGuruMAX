-- Valve v2
-- DESCRIPTION: The attached object will give the player a valve wheel resource if collected.
-- DESCRIPTION: [PROMPT_TEXT$="to collect"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: Play the audio <Sound0> when picked up.

g_valve = {}
local valve = {}
local prompt_text = {}
local pickup_range = {}	
local amount = {}	

function valve_properties(e, prompt_text, pickup_range)
	valve[e] = g_Entity[e]
	valve[e].prompt_text = prompt_text
	valve[e].pickup_range = pickup_range
end

function valve_init_name(e)	
	valve[e] = g_Entity[e]	
	valve[e].prompt_text = "to collect"	
	valve[e].pickup_range = 80
	amount[e] = 1
	g_valve = 0
end

function valve_main(e)
	valve[e] = g_Entity[e]
	PlayerDist = GetPlayerDistance(e)	
	if PlayerDist <= valve[e].pickup_range and g_PlayerHealth > 0 then	
		local LookingAt = GetPlrLookingAtEx(e,1)			
		if LookingAt == 1 then
			if GetGamePlayerStateXBOX() == 1 then
				PromptLocal(e,"Y Button " ..valve[e].prompt_text,3)
			else
				if GetHeadTracker() == 1 then
					PromptLocal(e,"Trigger " ..valve[e].prompt_text,3)
				else
					PromptLocal(e,"E " ..valve[e].prompt_text,3)
				end					
			end			
			if g_KeyPressE == 1 then
				PlaySound(e,0)
				Prompt(e,"Collected")
				PerformLogicConnections(e)
				g_valve = 1
				Destroy(e)				
			end
		end
	end		
end