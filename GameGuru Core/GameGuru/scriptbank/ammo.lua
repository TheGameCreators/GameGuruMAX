-- DESCRIPTION: The object will give the player ammunition if collected.
-- v3
-- DESCRIPTION: You can change [PromptText$="Ammunition Collected"]
-- DESCRIPTION: [&Quantity=(1,80)]
-- DESCRIPTION: [pickuprange=80(1,100)]
-- DESCRIPTION: [@pickup_style=1(1=Automatic, 2=Manual)]
-- DESCRIPTION: <Sound0> for collection

g_ammo = {}

function ammo_properties(e, prompttext, quantity, pickuprange, pickup_style)
	g_ammo[e]['prompttext'] = prompttext	
	g_ammo[e]['pickuprange'] = pickuprange
	g_ammo[e]['pickup_style'] = pickup_style	
end

function ammo_init(e)
	g_ammo[e] = {}
	g_ammo[e]['prompttext'] = ""
	g_ammo[e]['quantity'] = 0
	g_ammo[e]['pickuprange'] = 80
	g_ammo[e]['pickup_style'] = 1
end

function ammo_main(e)	
	PlayerDist = GetPlayerDistance(e)
	if g_ammo[e]['pickup_style'] == 1 then
		if PlayerDist < g_ammo[e]['pickuprange'] and g_PlayerHealth > 0 and g_PlayerThirdPerson==0 then
			PlaySound(e,0)
			PerformLogicConnections(e)
			AddPlayerAmmo(e)
			Prompt(g_ammo[e]['prompttext'])
			ActivateIfUsed(e)
			Destroy(e)			
		end
	end
	if g_ammo[e]['pickup_style'] == 2 then		
		local LookingAt = GetPlrLookingAtEx(e,1)
		if LookingAt == 1 and PlayerDist < g_ammo[e]['pickuprange'] then
			Prompt(g_ammo[e]['prompttext'])
			if g_KeyPressE == 1 then
				PlaySound(e,0)
				PerformLogicConnections(e)
				AddPlayerAmmo(e)
				ActivateIfUsed(e)
				Destroy(e)
			end
		end
	end
end
