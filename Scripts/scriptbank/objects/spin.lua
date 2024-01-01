-- Spin v3
-- DESCRIPTION: Spins the object on its selected axis at the spin rate in clockwise or anticlockwise direction.  
-- DESCRIPTION: [SpinSpeed=10(1,500)]. 
-- DESCRIPTION: [AntiClockwise!=0].
-- DESCRIPTION: [x_axis!=0]
-- DESCRIPTION: [y_axis!=1]
-- DESCRIPTION: [z_axis!=0]

g_spin = {}

function spin_properties(e,spinspeed, anticlockwise, x_axis, y_axis, z_axis)
	g_spin[e]['spinspeed'] = spinspeed
	g_spin[e]['anticlockwise'] = anticlockwise
	g_spin[e]['x_axis'] = x_axis
	g_spin[e]['y_axis'] = y_axis
	g_spin[e]['z_axis'] = z_axis
end

function spin_init_name(e,name)
	g_spin[e] = {}
	g_spin[e]['spinspeed'] = 1
	g_spin[e]['anticlockwise'] = 0
	g_spin[e]['axis'] = 2
	CollisionOff(e)
end
function spin_main(e)
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

