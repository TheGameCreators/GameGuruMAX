-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Scenery v2 by Necrym59
-- DESCRIPTION: Allows an object to be used as scenery beyond the game area
-- DESCRIPTION: Attach this behavior to the scenery object.
-- DESCRIPTION: [SCENERY_SCALE=0]
-- DESCRIPTION: [#POSITION_AJUST_X=0.0(-20000.0,20000.0)]
-- DESCRIPTION: [#POSITION_AJUST_Y=0.0(-20000.0,20000.0)]
-- DESCRIPTION: [#POSITION_AJUST_Z=0.0(-20000.0,20000.0)]

local scenery 			= {}
local scenery_scale  	= {}
local position_adjust_x	= {}
local position_adjust_y	= {}
local position_adjust_z	= {}
local scenery_number  	= {}

local sceneryposx	= {}
local sceneryposy	= {}
local sceneryposz	= {}
local sceneryscalex	= {}
local sceneryscaley	= {}
local sceneryscalez	= {}
local status		= {}
local doonce		= {}

function scenery_properties(e, scenery_scale, position_adjust_x, position_adjust_y, position_adjust_z)
	scenery[e] = g_Entity[e]
	scenery[e].scenery_scale = scenery_scale	
	scenery[e].position_adjust_x = position_adjust_x
	scenery[e].position_adjust_y = position_adjust_y
	scenery[e].position_adjust_z = position_adjust_z
end

function scenery_init(e)
	scenery[e] = {}
	scenery[e].scenery_scale = 0
	scenery[e].position_adjust_x = 0
	scenery[e].position_adjust_y = 0
	scenery[e].position_adjust_z = 0
	
	sceneryposx[e] = 0
	sceneryposy[e] = 0
	sceneryposz[e] = 0
	sceneryscalex[e] = 0
	sceneryscaley[e] = 0
	sceneryscalez[e] = 0
	doonce[e] = 0
	status[e] = "scenery_build"
end

function scenery_main(e)
	
	if status[e] == "scenery_build" then
		GravityOff(e)
		CollisionOff(e)
		sceneryposx[e],sceneryposy[e],sceneryposz[e],_,_,_ = GetEntityPosAng(e)		
		sceneryposx[e] = sceneryposx[e] + scenery[e].position_adjust_x*10
		sceneryposy[e] = sceneryposy[e] + scenery[e].position_adjust_y*10
		sceneryposz[e] = sceneryposz[e] + scenery[e].position_adjust_z*10
		ResetPosition(e,sceneryposx[e],sceneryposy[e],sceneryposz[e])
		sceneryscalex[e],sceneryscaley[e],sceneryscalez[e] = GetEntityScales(e)
		ResetPosition(e,sceneryposx[e],sceneryposy[e],sceneryposz[e])
		ScaleObject(g_Entity[e]['obj'],sceneryscalex[e]*100+scenery[e].scenery_scale,sceneryscaley[e]*100+scenery[e].scenery_scale,sceneryscalez[e]*100+scenery[e].scenery_scale)
		status[e] = "end_scenery"
	end
end