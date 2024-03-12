-- Emission Control v3

-- DESCRIPTION: Can change the emissive level and/or color of an object.
-- DESCRIPTION: Can also trigger other logic linked or IfUsed entities when activated.
-- DESCRIPTION: Attach to an object and activate from a switch or zone.
-- DESCRIPTION: Set Physics=ON or OFF and IsImmobile=Yes
-- DESCRIPTION: [#OFF_STRENGTH=0.0] Off strength.
-- DESCRIPTION: [#ON_STRENGTH=1000.0] On strength.
-- DESCRIPTION: [FADE_SPEED=50(1,100)]
-- DESCRIPTION: [R_VALUE=0(0,255)]
-- DESCRIPTION: [G_VALUE=0(0,255)]
-- DESCRIPTION: [B_VALUE=0(0,255)]
-- DESCRIPTION: [@ACTIVATE_LOGIC=1(1=Off, 2=On Activation, 3=On Deactivation, 4=On Activation+Deactivation)]

local emc = {}
local off_strength 	= {}
local on_strength 	= {}
local fade_speed	= {}
local r_value 		= {}
local g_value 		= {}
local b_value 		= {}

local status		= {}
local doonce		= {}
local current_state	= {}
local current_level = {}

function emission_control_properties(e, off_strength, on_strength, fade_speed, r_value, g_value, b_value, activate_logic)
	emc[e].off_strength = off_strength
	emc[e].on_strength = on_strength
	emc[e].fade_speed = fade_speed
	emc[e].r_value = r_value
	emc[e].g_value = g_value
	emc[e].b_value = b_value
	emc[e].activate_logic = activate_logic
end

function emission_control_init(e)
	emc[e] = {}
	emc[e].off_strength = 0
	emc[e].on_strength = 0
	emc[e].fade_speed = 1	
	emc[e].r_value = 0
	emc[e].g_value = 0
	emc[e].b_value = 0
	emc[e].activate_logic = 0

	status[e] = "init"
	doonce[e] = 0
	current_state[e] = ""
	current_level[e] = 0
	
	SetEntityEmissiveStrength(e,emc[e].off_strength)
	SetEntityEmissiveColor(e,emc[e].r_value,emc[e].g_value,emc[e].b_value)
	SetActivated(e,0)
end

function emission_control_main(e)

	if status[e] == "init" then		
		current_state[e] = "is-off"		
		SetEntityEmissiveColor(e,emc[e].r_value,emc[e].g_value,emc[e].b_value)
		SetEntityEmissiveStrength(e,emc[e].off_strength)		
		status[e] = "endinit"
	end	
	
	if g_Entity[e]['activated'] == 1 and current_state[e] == "is-off" then
		SetEntityEmissiveColor(e,emc[e].r_value,emc[e].g_value,emc[e].b_value)
		SetEntityEmissiveStrength(e,emc[e].on_strength)
		current_level[e] = emc[e].on_strength
		if emc[e].activate_logic == 2 or emc[e].activate_logic == 4 then
			PerformLogicConnections(e)
			ActivateIfUsed(e)
		end	
		current_state[e] = "is-on"
	end	

	if g_Entity[e]['activated'] == 0 and current_state[e] == "is-on" then
		if current_level[e] > emc[e].off_strength then		
			SetEntityEmissiveColor(e,emc[e].r_value,emc[e].g_value,emc[e].b_value)
			SetEntityEmissiveStrength(e,current_level[e])
			current_level[e] = current_level[e] - emc[e].fade_speed
		end
		if current_level[e] <= emc[e].off_strength then
			SetEntityEmissiveStrength(e,emc[e].off_strength)
			if emc[e].activate_logic == 3 or emc[e].activate_logic == 4 then
				PerformLogicConnections(e)
				ActivateIfUsed(e)
			end	
			current_state[e] = "is-off"			
		end
	end	
end
