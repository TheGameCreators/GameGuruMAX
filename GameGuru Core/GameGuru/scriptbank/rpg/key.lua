-- Key v2
-- DESCRIPTION: This object is treated as a key object for unlocking doors.
-- DESCRIPTION: [PICKUP_TEXT$="Collect Key"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Automatic, 2=Manual)]
-- DESCRIPTION: [COLLECTED_TEXT$="Key collected"]
-- DESCRIPTION: Play the audio <Sound0> when the object is picked up by the player.

local U = require "scriptbank\\utillib"
local key = {}

function key_properties(e,pickup_text, pickup_range, pickup_style, collected_text)
	key[e] = g_Entity[e]
	key[e].pickup_text = pickup_text
	key[e].pickup_range = pickup_range
	key[e].pickup_style = pickup_style
	key[e].collected_text = collected_text
end 

function key_init(e)
	key[e] = g_Entity[e]
	key[e].pickup_text = "Collect Key"
	key[e].pickup_range = 80
	key[e].pickup_style = 1
	key[e].collected_text = "Key collected"
end

function key_main(e)
	key[e] = g_Entity[e]
	local PlayerDist = GetPlayerDistance(e)
	
	if key[e].pickup_style == 1 then
		if PlayerDist < key[e].pickup_range then
			if GetEntityCollectable(e) == 1 then
				if GetEntityCollected(e) == 0 then
					PromptDuration(key[e].collected_text,1000)
					PlaySound(e,0)
					SetEntityCollected(e,1)									
					PerformLogicConnections(e)
				end
			end
			if GetEntityCollectable(e) == 0 then
				PromptDuration(key[e].collected_text,1000)
				PlaySound(e,0)
				Destroy(e)
				PerformLogicConnections(e)
			end
		end
	end
	
	if key[e].pickup_style == 2 then	
		local LookingAt = GetPlrLookingAtEx(e,1)
		if LookingAt == 1 and PlayerDist < key[e].pickup_range then		
			if GetEntityCollectable(e) == 1 then
				if GetEntityCollected(e) == 0 then
					if GetGamePlayerStateXBOX() == 1 then
						PromptLocalForVR(e,"Y button to " ..key[e].pickup_text,3)
					else
						if GetHeadTracker() == 1 then
							PromptLocalForVR(e,"Trigger to " ..key[e].pickup_text,3)
						else
							PromptLocalForVR(e,"E to " ..key[e].pickup_text,3)
						end
					end
					if g_KeyPressE == 1 then
						PromptLocalForVR(e,key[e].collected_text,3)				
						PlaySound(e,0)
						SetEntityCollected(e,1)
						PerformLogicConnections(e)
					end
				end
			end
			if GetEntityCollectable(e) == 0 then
				if GetEntityCollected(e) == 0 then
					if GetGamePlayerStateXBOX() == 1 then
						PromptLocalForVR(e,"Y button to " ..key[e].pickup_text,3)
					else
						if GetHeadTracker() == 1 then
							PromptLocalForVR(e,"Trigger to " ..key[e].pickup_text,3)
						else
							PromptLocalForVR(e,"E to " ..key[e].pickup_text,3)
						end
					end
					if g_KeyPressE == 1 then
						PromptLocalForVR(e,key[e].collected_text,3)				
						PlaySound(e,0)
						Destroy(e)						
						PerformLogicConnections(e)
					end
				end
			end
		end
	end
end
