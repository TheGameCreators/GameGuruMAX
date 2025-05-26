-- Skysphere v7 by Necrym59
-- DESCRIPTION: Allows the use of a skysphere and can spin on a selected axis at the spin rate in designated direction  
-- DESCRIPTION: [SPINSPEED#=1.0(0.1,500.0)] 
-- DESCRIPTION: [ANTICLOCKWISE!=0]
-- DESCRIPTION: [X_AXIS!=0]
-- DESCRIPTION: [Y_AXIS!=1]
-- DESCRIPTION: [Z_AXIS!=0]
-- DESCRIPTION: [SCALE_MULTIPLIER=1(1,1000)]
-- DESCRIPTION: [!DAYNIGHT_CYCLE=0] Use with the the day night behavior
-- DESCRIPTION: [FADE_SPEED#=2.50(0.00,50.00)] fade in/out rate of the skysphere at day/night

g_sunrollposition = {}

local skysphere = {}
local anticlockwise = {}
local x_axis = {}
local y_axis = {}
local z_axis = {}
local scale_multiplier = {}
local daynight_cycle
local fade_speed

local tspinspeed = {}
local fade_level = {}
local status = {}

function skysphere_properties(e,spinspeed, anticlockwise, x_axis, y_axis, z_axis, scale_multiplier, daynight_cycle, fade_speed)
	skysphere[e].spinspeed = spinspeed
	skysphere[e].anticlockwise = anticlockwise
	skysphere[e].x_axis = x_axis or 0
	skysphere[e].y_axis = y_axis or 0
	skysphere[e].z_axis = z_axis or 0
	skysphere[e].scale_multiplier = scale_multiplier
	skysphere[e].daynight_cycle = daynight_cycle or 0
	skysphere[e].fade_speed = fade_speed	
end

function skysphere_init_name(e,name)
	skysphere[e] = {}
	skysphere[e].spinspeed = 1
	skysphere[e].anticlockwise = 0
	skysphere[e].x_axis = 0
	skysphere[e].y_axis = 0
	skysphere[e].z_axis = 0	
	skysphere[e].scale_multiplier = scale_multiplier	
	skysphere[e].daynight_cycle = 0
	skysphere[e].fade_speed = 2.50
	
	g_sunrollposition = 0
	tspinspeed[e] = 0
	if skysphere[e].daynight_cycle == 1 then
		fade_level[e] = GetEntityBaseAlpha(e)
		SetEntityTransparency(e,1)
	end	
	status[e] = "init"
end

function skysphere_main(e)
	if status[e] == "init" then
		CollisionOff(e)
		local xs, ys, zs = GetObjectScales(g_Entity[e]['obj'])
		ScaleObject(g_Entity[e]['obj'],(xs*100)*skysphere[e].scale_multiplier,(ys*100)*skysphere[e].scale_multiplier,(zs*100)*skysphere[e].scale_multiplier)
		SetPosition(e,0,0,0)		
		if skysphere[e].daynight_cycle == 1 then
			if g_sunrollposition > -90 and g_sunrollposition < 85 then fade_level[e] = 0 end
			if g_sunrollposition > 85 then fade_level[e] = 100 end
		else
			fade_level[e] = 0
		end	
		status[e] = "skysphere"
	end
	
	if status[e] == "skysphere" then
		ResetPosition(e,0,0,0)
		if skysphere[e].daynight_cycle == 0	then
			if g_sunrollposition > -90 and g_sunrollposition < 85 then Hide(e) end --Day
			if g_sunrollposition > 85 then Show(e) end --Night
		end
		if skysphere[e].daynight_cycle == 1 then
			if g_sunrollposition > -90 and g_sunrollposition < 85 then  --Day
				if fade_level[e] > 0 then
					SetEntityBaseAlpha(e,fade_level[e])
					fade_level[e] = fade_level[e]-(skysphere[e].fade_speed/100)
				end							
				if fade_level[e] <= 0 then
					fade_level[e] = 0
					Hide(e)
				end
			end	
			if g_sunrollposition > 85 then  --Night
				Show(e)
				if fade_level[e] < 100 then
					SetEntityBaseAlpha(e,fade_level[e])
					fade_level[e] = fade_level[e]+(skysphere[e].fade_speed/100)
				end							
				if fade_level[e] >= 100 then
					fade_level[e] = 100					
				end			
			end			
		end	

		if skysphere[e].anticlockwise == 0 then
			tspinspeed[e] = skysphere[e].spinspeed
		else
			tspinspeed[e] = skysphere[e].spinspeed * -1 
		end	
		if skysphere[e].x_axis == 1 then RotateX(e,GetAnimationSpeed(e)*tspinspeed[e]) end	
		if skysphere[e].y_axis == 1 then RotateY(e,GetAnimationSpeed(e)*tspinspeed[e]) end
		if skysphere[e].z_axis == 1 then RotateZ(e,GetAnimationSpeed(e)*tspinspeed[e]) end
	end	
end

