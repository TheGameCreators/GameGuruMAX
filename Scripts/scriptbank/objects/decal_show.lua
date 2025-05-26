-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Decal Show v10
-- DESCRIPTION: Attached object will be shown when activated from a linked trigger zone or switch
-- DESCRIPTION: Set Physics ON/OFF, Always active ON.
-- DESCRIPTION: [@STYLE=1(1=Static XYZ, 2=Static Facing Camera, 3=Face/Move with Player, 4=Drift Y Fixed, 5=Drift Y Random)]
-- DESCRIPTION: [X_ADJUSTMENT=0(0,100)]
-- DESCRIPTION: [Y_ADJUSTMENT=0(0,100)]
-- DESCRIPTION: [Z_ADJUSTMENT=0(0,500)]
-- DESCRIPTION: [#DISPLAY_TIME=1.5(0.1,100)]
-- DESCRIPTION: [#FADE_IN_SPEED=50.0(0.0,100.0)]
-- DESCRIPTION: [#FADE_OUT_SPEED=50.0(0.0,100.0)]
-- DESCRIPTION: [DRIFT_AMOUNT=10(0,1000)]

local decalobject 		= {}
local style 			= {}
local x_adjustment 		= {}
local y_adjustment 		= {}
local z_adjustment 		= {}
local display_time		= {}
local fade_in_speed 	= {}
local fade_out_speed 	= {}
local drift_amount 		= {}

local decal_time 	= {}
local decalx		= {}
local decaly		= {}
local decalz		= {}
local startx		= {}
local starty		= {}
local startz		= {}
local current_level	= {}
local fading		= {}
local randomdrift	= {}
local randomtimer	= {}
local current_x		= {}
local status		= {}
	
function decal_show_properties(e, style, x_adjustment, y_adjustment, z_adjustment, display_time, fade_in_speed, fade_out_speed, drift_amount)
	decalobject[e].style = style
	decalobject[e].x_adjustment = x_adjustment
	decalobject[e].y_adjustment = y_adjustment
	decalobject[e].z_adjustment = z_adjustment
	decalobject[e].display_time = display_time
	decalobject[e].fade_in_speed = fade_in_speed
	decalobject[e].fade_out_speed = fade_out_speed
	decalobject[e].drift_amount = drift_amount	
end
 
function decal_show_init(e)
	decalobject[e] = {}
	decalobject[e].style = 1	
	decalobject[e].x_adjustment = 0
	decalobject[e].y_adjustment = 0
	decalobject[e].z_adjustment = 0
	decalobject[e].display_time = 1.5
	decalobject[e].fade_in_speed = 0.5
	decalobject[e].fade_out_speed = 0.5	
	decalobject[e].drift_amount = 20	
	
	decal_time[e] =  math.huge
	fading[e] = 0
	randomdrift[e] = 0
	randomtimer[e] = 0	
	current_level[e] = 0
	current_x[e] = 0
	SetEntityTransparency(e,1)	
	SetEntityBaseAlpha(e,GetEntityBaseAlpha(e))
	status[e] = "init"
end
 
function decal_show_main(e)
	if status[e] == "init" then		
		startx[e] = g_Entity[e]['x'] + decalobject[e].x_adjustment
		starty[e] = g_Entity[e]['y'] + decalobject[e].y_adjustment
		startz[e] = g_Entity[e]['z'] + decalobject[e].z_adjustment
		status[e] = "endinit"
	end
	
	if g_Entity[e]['activated'] == 1 then
		GravityOff(e)
		CollisionOff(e)		
	
		if decalobject[e].style == 1 then
		end
		if decalobject[e].style == 2 then
			RotateToCamera(e)
		end		
		if decalobject[e].style == 3 then	
			decalx[e] = g_PlayerPosX
			decaly[e] = starty[e]
			decalz[e] = g_PlayerPosZ
			decalx[e] = decalx[e] + math.sin(math.rad(g_PlayerAngY))*decalobject[e].z_adjustment
			decalz[e] = decalz[e] + math.cos(math.rad(g_PlayerAngY))*decalobject[e].z_adjustment
			ResetPosition(e,decalx[e],decaly[e],decalz[e])
			ResetRotation(e,0,g_PlayerAngY,g_PlayerAngZ)
		end
		if decalobject[e].style == 4 then
			RotateToCamera(e)
			MoveUp(e,decalobject[e].drift_amount)
		end
		if decalobject[e].style == 5 then
			RotateToCamera(e)
			MoveUp(e,randomdrift[e])
		end		
		if g_Time >= randomtimer[e] and decalobject[e].style == 5 then
			randomdrift[e] = math.random(-decalobject[e].drift_amount, decalobject[e].drift_amount)
			randomtimer[e] = g_Time + 1000
		end	
		
		if decalobject[e].fade_in_speed > 0 and fading[e] == 0 then
			if current_level[e] < 100 then				
				SetEntityBaseAlpha(e,current_level[e])				
				current_level[e] = current_level[e] + decalobject[e].fade_in_speed/100
			end
			if current_level[e] >= 100 then
				current_level[e] = 100
				fading[e] = 1
				decal_time[e] = g_Time + (decalobject[e].display_time * 1000)
			end
		end
		
		if g_Time >= decal_time[e] then
			if decalobject[e].fade_out_speed == 0 then SetEntityBaseAlpha(e,0) end
			if decalobject[e].fade_out_speed > 0 and fading[e] == 1 then
				if current_level[e] > 0 then				
					SetEntityBaseAlpha(e,current_level[e])
					current_level[e] = current_level[e] - decalobject[e].fade_out_speed/100
				end
				if current_level[e] <= 0 then
					current_level[e] = 0
					fading[e] = 0
					ResetPosition(e,startx[e],starty[e],startz[e])
					SetActivated(e,0)
				end				
			end					
		end	
	end	
end
 
function decal_show_exit(e)
end