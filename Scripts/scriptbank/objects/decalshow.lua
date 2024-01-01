-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- DecalShow v4
-- DESCRIPTION: Will show a hidden decal object activated from a trigger Zone
-- DESCRIPTION: Attach to the Decal object(s). Set Static Mode: Physics ON/OFF, Always active ON then link to a trigger Zone.
-- DESCRIPTION: Change the Decal Object view distance [POSITION_X=0(1,100)],[POSITION_Y=15(1,100)][POSITION_Z=80(1,300)] and set [#DISPLAY_TIME=1.5(0.1,100)].

local decalobject 	= {}
local position_x 	= {}
local position_y 	= {}
local position_z 	= {}
local display_time	= {}
local decal_time 	= {}
	
function decalshow_properties(e, position_x, position_y, position_z, display_time)
	decalobject[e] = g_Entity[e]
	decalobject[e].position_x = position_x
	decalobject[e].position_y = position_y
	decalobject[e].position_z = position_z
	decalobject[e].display_time = display_time
end
 
function decalshow_init(e)
	decalobject[e] = g_Entity[e]
	decalshow_properties(e, 50, 50, 50, 0)
	decal_time[e] = 0
	StartTimer(e)
end
 
function decalshow_main(e)	
	decalobject[e] = g_Entity[e]	
	Hide(e)	
	if g_Entity[e]['activated'] == 1 then
		GravityOff(e)	
		---Positioning----		
		local decalx = g_PlayerPosX
		local decaly = g_PlayerPosY
		local decalz = g_PlayerPosZ
		SetPosition(e,decalx,decaly+decalobject[e].position_y,decalz)		
		decalx = decalx + math.sin(math.rad(g_PlayerAngY))*decalobject[e].position_z
		decalz = decalz + math.cos(math.rad(g_PlayerAngY))*decalobject[e].position_z
		ResetPosition(e,decalx,decaly+decalobject[e].position_y,decalz)
		SetRotation(e,0,g_PlayerAngY,g_PlayerAngZ)
		Show(e)
		---Timing----
		if decal_time[e] == 0 then
			decal_time[e] = GetTimer(e) + (decalobject[e].display_time * 1000)
		end	
	end
	if GetTimer(e) >= decal_time[e] then
		Hide(e)
		g_Entity[e]['activated'] = 0		
	end	
end --main 
 
function decalshow_exit(e)
end


