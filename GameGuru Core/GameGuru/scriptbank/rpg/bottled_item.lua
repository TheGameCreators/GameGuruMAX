-- DESCRIPTION: The object will give the player a health boost or loss if consumed.
-- Bottled_Item v4  by Necrym59
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [PICKUP_TEXT$="E to pickup and consume"]
-- DESCRIPTION: [@BOTTLED_TYPE=1(1=Healthy, 2=Poisoned)]
-- DESCRIPTION: [BOTTLED_VALUE=10(1,50)]
-- DESCRIPTION: [CONSUMED_TEXT$="Tasted Fine"]
-- DESCRIPTION: [@POISON_EFFECT=1(1=On, 2=Off)]
-- DESCRIPTION: <Sound0> to play if healthy
-- DESCRIPTION: <Sound1> to play if poisoned 

	local bottled 			= {}
	local pickup_range 		= {}
	local pickup_text 		= {}
	local bottled_type 		= {}
	local bottled_value 	= {}
	local consumed_text 	= {}
	local poison_effect 	= {}
	
	local poisoned	= {}	
	local status ={}

function bottled_item_properties(e, pickup_range, pickup_text, bottled_type, bottled_value, consumed_text, poison_effect)
	bottled[e] = g_Entity[e]
	bottled[e].pickup_range = 80
	bottled[e].pickup_text = pickup_text
	bottled[e].bottled_type = bottled_type
	bottled[e].bottled_value = bottled_value
	bottled[e].consumed_text = consumed_text
	bottled[e].poison_effect = poison_effect	
end

function bottled_item_init_name(e)
	bottled[e] = g_Entity[e]
	bottled[e].pickup_range = 80
	bottled[e].pickup_text = "E to pickup and consume"
	bottled[e].bottled_type = 1
	bottled[e].bottled_value = 10
	bottled[e].consumed_text = "Tasted Fine"
	bottled[e].poison_effect = 1
	poisoned[e] = 0
	fov = g_PlayerFOV
	status[e] = "init"
end

function bottled_item_main(e)
	bottled[e] = g_Entity[e]
	if status[e] == "init" then 
		poisoned[e] = bottled[e].bottled_value * 50
		status[e] = "consume"
	end	
	
	if status[e] == "consume" then
		local PlayerDist = GetPlayerDistance(e)
		if PlayerDist < bottled[e].pickup_range and g_PlayerHealth > 0 then
			local LookingAt = GetPlrLookingAtEx(e,1)	
			if LookingAt == 1 then	
				if bottled[e].bottled_type == 1 then
					Prompt(bottled[e].pickup_text)
					if g_KeyPressE == 1 then
						PlaySound(e,0)
						PromptDuration(bottled[e].consumed_text,3000)
						PerformLogicConnections(e)
						SetPlayerHealth(g_PlayerHealth + bottled[e].health)	
						Hide(e)
						CollisionOff(e)
						Destroy(e)
					end
				end
				if bottled[e].bottled_type == 2 then
					Prompt(bottled[e].pickup_text)
					if g_KeyPressE == 1 then
						PlaySound(e,1)
						PromptDuration(bottled[e].consumed_text,3000)
						PerformLogicConnections(e)						
						SetPlayerHealth(g_PlayerHealth - bottled[e].health)	
						Hide(e)
						CollisionOff(e)						
						status[e] = "poisoned"
					end
				end
			end
		end
	end
	if status[e] == "poisoned" then
		if bottled[e].poison_effect == 1 then			
			if poisoned[e] > 0 then				
				GamePlayerControlSetShakeTrauma(565.0)
				GamePlayerControlSetShakePeriod(960.00)		
				poisoned[e] = poisoned[e]-1			
			end
			if poisoned[e] == 0 then
				GamePlayerControlSetShakeTrauma(0.0)
				GamePlayerControlSetShakePeriod(0.00)
				Destroy(e)
			end
		end
	end
end

