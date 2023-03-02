-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Battery v3: by Necrym59
-- DESCRIPTION: The attached object will give the player a energy resource if collected.
-- DESCRIPTION: [PROMPT_TEXT$="E to collect battery"]
-- DESCRIPTION: [ENERGY_LEVEL=10(1,30)]
-- DESCRIPTION: [PICKUP_RANGE=50(1,100)]
-- DESCRIPTION: Play the audio <Sound0> when picked up.

g_energy = {}
local battery = {}
local prompt_text = {}
local energy_level = {}
local pickup_range = {}	
local pressed  = {}		

function battery_properties(e, prompt_text, energy_level, pickup_range)
	battery[e] = g_Entity[e]
	battery[e].prompt_text = prompt_text
	battery[e].energy_level = energy_level
	battery[e].pickup_range = pickup_range
end

function battery_init_name(e)	
	battery[e] = g_Entity[e]	
	battery[e].prompt_text = "Collected battery"
	battery[e].energy_level = 0
	battery[e].pickup_range = 50
	g_energy = 0
	pressed[e] = 0
end

function battery_main(e)
	battery[e] = g_Entity[e]
	PlayerDist = GetPlayerDistance(e)
	
	if PlayerDist < battery[e].pickup_range and g_PlayerHealth > 0 then
		local LookingAt = GetPlrLookingAtEx(e,1)	
		if LookingAt == 1 then		
			Prompt(battery[e].prompt_text)			
			if g_KeyPressE == 1 and pressed[e] == 0 then
				PlaySound(e,0)
				PerformLogicConnections(e)		
				g_energy = g_energy + battery[e].energy_level
				Hide(e)
				CollisionOff(e)
				Destroy(e)
				pressed[e] = 1
			end
		end
	end	
end

