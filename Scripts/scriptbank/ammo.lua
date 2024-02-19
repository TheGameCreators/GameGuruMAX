-- v11 by Necrym and Lee
-- DESCRIPTION: The object will give the player ammunition if collected, and optionally play <Sound0> for collection.
-- DESCRIPTION: You can change the [PROMPTTEXT$="E to collect"]
-- DESCRIPTION: Set the Ammo [&QUANTITY=(1,100)]
-- DESCRIPTION: Set the [PICKUPRANGE=90(1,200)]
-- DESCRIPTION: and [@PICKUP_STYLE=1(1=Automatic, 2=Manual)]
-- DESCRIPTION: You can change the [COLLECTION_TEXT$="Ammunition collected"]
-- DESCRIPTION: [!PLAY_PICKUP=1]
-- DESCRIPTION: [!ACTIVATE_LOGIC=1]

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}
local g_ammo = {}

function ammo_properties(e, prompttext, quantity, pickuprange, pickup_style, collection_text,play_pickup,activate_logic)
	g_ammo[e]['prompttext'] = prompttext
	g_ammo[e]['quantity'] = quantity
	g_ammo[e]['pickuprange'] = pickuprange
	g_ammo[e]['pickup_style'] = pickup_style
	g_ammo[e]['collection_text'] = collection_text
	g_ammo[e]['play_pickup'] = play_pickup or 1
	g_ammo[e]['activate_logic'] = activate_logic or 1
end

function ammo_init(e)
	g_ammo[e] = {}
	g_ammo[e]['prompttext'] = "E to use"
	g_ammo[e]['quantity'] = 0
	g_ammo[e]['pickuprange'] = 90
	g_ammo[e]['pickup_style'] = 1
	g_ammo[e]['collection_text'] = "Ammunition collected"
	g_ammo[e]['play_pickup'] = 1
	g_ammo[e]['activate_logic'] = 1
end

function ammo_main(e)	
	PlayerDist = GetPlayerDistance(e)
	local triggerthepickup = 0
	if g_ammo[e]['pickup_style'] == 1 then
		if PlayerDist < g_ammo[e]['pickuprange'] and g_PlayerHealth > 0 and GetEntityVisibility(e) == 1 then
			triggerthepickup = 1
		end
	end
	if g_ammo[e]['pickup_style'] == 2 then
		--pinpoint select object--
		module_misclib.pinpoint(e,g_ammo[e]['pickuprange'],200)
		--end pinpoint select object--
		if PlayerDist < g_ammo[e]['pickuprange'] and g_tEnt ~= 0 then
			PromptLocalForVR(e,g_ammo[e]['prompttext'])
			if g_KeyPressE == 1 then
				triggerthepickup = 1
			end
		end
	end
	if triggerthepickup == 1 then
		if g_ammo[e]['play_pickup'] == 1 then
			PlayNon3DSound(e,0)
		end
		if g_ammo[e]['activate_logic'] == 1 then
			PerformLogicConnections(e)
			ActivateIfUsed(e)
		end
		AddPlayerAmmo(e)
		PromptDuration(g_ammo[e]['collection_text'],1000)
		Destroy(e)
	end
end
