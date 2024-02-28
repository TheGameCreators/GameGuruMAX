-- Spin v4
-- DESCRIPTION: Spins the object on its selected axis at the spin rate in clockwise or anticlockwise direction.  
-- DESCRIPTION: [SpinSpeed=10(1,500)]. 
-- DESCRIPTION: [AntiClockwise!=0].
-- DESCRIPTION: [x_axis!=0]
-- DESCRIPTION: [y_axis!=1]
-- DESCRIPTION: [z_axis!=0]
-- DESCRIPTION: [@USE_VARIABLE_SWITCH=2(1=Yes,2=No)]
-- DESCRIPTION: [VARIABLE_SWITCH_USER_GLOBAL$="Variable_Switch1"]

g_spin = {}
local use_variable_switch 			= {}
local variable_switch_user_global	= {}	
local currentvalue = {}

function spin_properties(e,spinspeed, anticlockwise, x_axis, y_axis, z_axis, use_variable_switch, variable_switch_user_global)
	g_spin[e]['spinspeed'] = spinspeed
	g_spin[e]['anticlockwise'] = anticlockwise
	g_spin[e]['x_axis'] = x_axis
	g_spin[e]['y_axis'] = y_axis
	g_spin[e]['z_axis'] = z_axis
	g_spin[e]['use_variable_switch'] = use_variable_switch	
	g_spin[e]['variable_switch_user_global'] = variable_switch_user_global
end

function spin_init_name(e,name)
	g_spin[e] = {}
	g_spin[e]['spinspeed'] = 1
	g_spin[e]['anticlockwise'] = 0
	g_spin[e]['axis'] = 2
	g_spin[e]['use_variable_switch'] = 2	
	g_spin[e]['variable_switch_user_global'] = ""

	currentvalue[e] = 0
	CollisionOff(e)
end
function spin_main(e)
	
	if g_spin[e]['use_variable_switch'] == 1 then
		if g_spin[e]['variable_switch_user_global'] ~= "" then
			if _G["g_UserGlobal['"..g_spin[e]['variable_switch_user_global'].."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..g_spin[e]['variable_switch_user_global'].."']"] end
			g_spin[e]['spinspeed'] = _G["g_UserGlobal['"..g_spin[e]['variable_switch_user_global'].."']"]
		end
	end	
	
	if g_spin[e]['anticlockwise'] == 0 then
		tspinspeed = g_spin[e]['spinspeed']
	else
		tspinspeed = g_spin[e]['spinspeed'] * -1 
	end	

	if g_spin[e]['x_axis'] == 1 then
		CollisionOff(e)
		RotateX(e,GetAnimationSpeed(e)*tspinspeed)
		CollisionOn(e)
	end	
	if g_spin[e]['y_axis'] == 1 then
		CollisionOff(e)
		RotateY(e,GetAnimationSpeed(e)*tspinspeed)
		CollisionOn(e)
	end
	if g_spin[e]['z_axis'] == 1 then
		CollisionOff(e)
		RotateZ(e,GetAnimationSpeed(e)*tspinspeed)
		CollisionOn(e)
	end
end

