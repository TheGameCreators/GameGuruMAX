-- v4
-- DESCRIPTION: The object will give the player ammunition if collected.
-- DESCRIPTION: You can change the [PROMPTTEXT$="E to collect"]
-- DESCRIPTION: Set the Ammo [&QUANTITY=(1,100)]
-- DESCRIPTION: Set the [PICKUPRANGE=90(1,200)]
-- DESCRIPTION: and [@PICKUP_STYLE=1(1=Automatic, 2=Manual)]
-- DESCRIPTION: <Sound0> for collection

local g_ammo = {}

function ammo_properties(e, prompttext, quantity, pickuprange, pickup_style)
	g_ammo[e]['prompttext'] = prompttext
	g_ammo[e]['quantity'] = quantity
	g_ammo[e]['pickuprange'] = pickuprange
	g_ammo[e]['pickup_style'] = pickup_style	
end

function ammo_init(e)
	g_ammo[e] = {}
	g_ammo[e]['prompttext'] = "E to use"
	g_ammo[e]['quantity'] = 0
	g_ammo[e]['pickuprange'] = 90
	g_ammo[e]['pickup_style'] = 1
end

function ammo_main(e)	
	PlayerDist = GetPlayerDistance(e)
	if g_ammo[e]['pickup_style'] == 1 then
		if PlayerDist < g_ammo[e]['pickuprange'] and g_PlayerHealth > 0 then
			PlaySound(e,0)
			PerformLogicConnections(e)
			AddPlayerAmmo(e)
			PromptLocal("Ammunition collected",1000)
			ActivateIfUsed(e)			
			Destroy(e)			
		end
	end
	if g_ammo[e]['pickup_style'] == 2 then				
		if PlayerDist < g_ammo[e]['pickuprange'] and g_PlayerHealth > 0 then
			PromptLocalForVR(e,g_ammo[e]['prompttext'])
			if g_KeyPressE == 1 then
				PlaySound(e,0)
				PerformLogicConnections(e)
				AddPlayerAmmo(e)
				PromptDuration("Ammunition collected",1000)
				ActivateIfUsed(e)
				Destroy(e)
			end
		end
	end
end
