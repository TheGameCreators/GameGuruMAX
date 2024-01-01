-- Spinner v4
-- DESCRIPTION: When activated by a switch or zone will spins an object on its selected axis at the spin rate in clockwise or anticlockwise direction.
-- DESCRIPTION: Set Physics=ON, Gravity=ON. IsImobile=YES
-- DESCRIPTION: [#Acceleration=0.01(0.01,5.00)].
-- DESCRIPTION: [MaxSpinSpeed=10(1,1000)].
-- DESCRIPTION: [AntiClockwise!=0].
-- DESCRIPTION: [x_axis!=0]
-- DESCRIPTION: [y_axis!=1]
-- DESCRIPTION: [z_axis!=0]
-- DESCRIPTION: [DamageRange=1(1,1000)]

local spinner 			= {}
local status 			= {}
local current_level 	= {}
local objectheight 		= {}
local objectwidth 		= {}	
local objectlength 		= {}

function spinner_properties(e, acceleration, maxspinspeed, anticlockwise, x_axis, y_axis, z_axis, damagerange)
	spinner[e].acceleration = acceleration
	spinner[e].maxspinspeed = maxspinspeed
	spinner[e].anticlockwise = anticlockwise
	spinner[e].x_axis = x_axis
	spinner[e].y_axis = y_axis
	spinner[e].z_axis = z_axis
	spinner[e].damagerange = damagerange
end

function spinner_init(e)
	spinner[e] = {}
	spinner[e].acceleration = 1
	spinner[e].maxspinspeed = 1
	spinner[e].anticlockwise = 0
	spinner[e].x_axis = 0
	spinner[e].y_axis = 1
	spinner[e].z_axis = 0
	spinner[e].damagerange = 1
	
	status[e] = "init"
	current_level[e] = 0
	objectheight[e] = 0
	objectwidth[e] = 0		
	objectlength[e] = 0
	CollisionOff(e)
end

function spinner_main(e)

	if status[e] == "init" then
		if spinner[e].anticlockwise == 0 then current_level[e] = 0 end
		if spinner[e].anticlockwise == 1 then spinner[e].maxspinspeed = (spinner[e].maxspinspeed - spinner[e].maxspinspeed*2) end
		status[e] = "endinit"
	end

	if g_Entity[e].activated == 1 then
		if spinner[e].anticlockwise == 0 then
			if current_level[e] < spinner[e].maxspinspeed then current_level[e] = current_level[e] + spinner[e].acceleration end	
		end
		if spinner[e].anticlockwise == 1 then
			if current_level[e] >= spinner[e].maxspinspeed then	current_level[e] = current_level[e] - spinner[e].acceleration end	
		end		
		if spinner[e].x_axis == 1 then 
			CollisionOff(e)
			RotateX(e,GetAnimationSpeed(e)*current_level[e])
			CollisionOn(e)
		end
		if spinner[e].y_axis == 1 then
			CollisionOff(e)
			RotateY(e,GetAnimationSpeed(e)*current_level[e])		
			CollisionOn(e)		
		end
		if spinner[e].z_axis == 1 then
			CollisionOff(e)
			RotateZ(e,GetAnimationSpeed(e)*current_level[e])
			CollisionOn(e)		
		end		
		if GetPlayerDistance(e) <= spinner[e].damagerange and current_level[e] >= spinner[e].maxspinspeed/2 then HurtPlayer(-1,1) end
	end
	if g_Entity[e].activated == 0 then
		if current_level[e] > 0 then current_level[e] = current_level[e] - spinner[e].acceleration end		
		if spinner[e].x_axis == 1 then 
			CollisionOff(e)
			RotateX(e,GetAnimationSpeed(e)*current_level[e])
			CollisionOn(e)
		end
		if spinner[e].y_axis == 1 then
			CollisionOff(e)
			RotateY(e,GetAnimationSpeed(e)*current_level[e])		
			CollisionOn(e)		
		end
		if spinner[e].z_axis == 1 then
			CollisionOff(e)
			RotateZ(e,GetAnimationSpeed(e)*current_level[e])
			CollisionOn(e)		
		end	
		if current_level[e] <= 0 then g_Entity[e].activated = 0 end
		if GetPlayerDistance(e) <= spinner[e].damagerange and current_level[e] >= spinner[e].maxspinspeed/2 then HurtPlayer(-1,1) end
	end
end