-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- DecalShow v7
-- DESCRIPTION: Attached decal object will be shown when activated from a linked trigger zone or switch
-- DESCRIPTION: Set Physics ON/OFF, Always active ON .
-- DESCRIPTION: [POSITION_X=50(1,100)]
-- DESCRIPTION: [POSITION_Y=50(1,100)]
-- DESCRIPTION: [POSITION_Z=100(1,500)]
-- DESCRIPTION: [#DISPLAY_TIME=1.5(0.1,100)]
-- DESCRIPTION: [#FADER_TIME=0.5(0.0,10.0)]
-- DESCRIPTION: [FIXED_POSITION!=1]

local decalobject 		= {}
local position_x 		= {}
local position_y 		= {}
local position_z 		= {}
local display_time		= {}
local display_fader 	= {}
local fixed_position	= {}

local decal_time 	= {}
local current_level	= {}
local fading		= {}
	
function decalshow_properties(e, position_x, position_y, position_z, display_time, fader_time, fixed_position)
	decalobject[e] = g_Entity[e]
	decalobject[e].position_x = position_x
	decalobject[e].position_y = position_y
	decalobject[e].position_z = position_z
	decalobject[e].display_time = display_time
	decalobject[e].fader_time = fader_time
	decalobject[e].fixed_position = fixed_position
end
 
function decalshow_init(e)
	decalobject[e] = {}
	decalobject[e].position_x = 50
	decalobject[e].position_y = 50
	decalobject[e].position_z = 100
	decalobject[e].display_time = 1.5
	decalobject[e].fader_time = 0.5
	decalobject[e].fixed_position = 1	
	
	decal_time[e] =  math.huge
	fading[e] = 0	
	current_level[e] = 0
	SetEntityTransparency(e,1)
	SetEntityBaseAlpha(e,current_level[e])
end
 
function decalshow_main(e)	
		
	if g_Entity[e]['activated'] == 1 then
		GravityOff(e)	
		if decalobject[e].fixed_position == 0 then 	
			local decalx = g_PlayerPosX
			local decaly = decalobject[e].position_y
			local decalz = g_PlayerPosZ
			decalx = decalx + math.sin(math.rad(g_PlayerAngY))*decalobject[e].position_z
			decalz = decalz + math.cos(math.rad(g_PlayerAngY))*decalobject[e].position_z
			ResetPosition(e,decalx,decaly,decalz)
			ResetRotation(e,0,g_PlayerAngY,g_PlayerAngZ)
		end	
		if decalobject[e].fader_time == 0 then
			SetEntityBaseAlpha(e,100)
			decal_time[e] = g_Time(e) + (decalobject[e].display_time * 1000)
		end	
		if decalobject[e].fader_time > 0 and fading[e] == 0 then
			if current_level[e] < 100 then				
				SetEntityBaseAlpha(e,current_level[e])
				current_level[e] = current_level[e] + 0.5
			end
			if current_level[e] >= 100 then
				current_level[e] = 100
				fading[e] = 1
				decal_time[e] = g_Time + (decalobject[e].display_time * 1000)
			end
		end
		if g_Time >= decal_time[e] then
			if decalobject[e].fader_time == 0 then SetEntityBaseAlpha(e,0) end
			if decalobject[e].fader_time > 0 and fading[e] == 1 then
				if current_level[e] > 0 then				
					SetEntityBaseAlpha(e,current_level[e])
					current_level[e] = current_level[e] - 0.5
				end
				if current_level[e] <= 0 then
					current_level[e] = 0
					fading[e] = 0
					SetActivated(e,0)
				end				
			end					
		end	
	end	
end
 
function decalshow_exit(e)
end


