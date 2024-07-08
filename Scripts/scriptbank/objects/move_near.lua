-- Move Near v3
-- DESCRIPTION: Pulls the object towards the player when when the player moves within
-- DESCRIPTION: [RANGE=100(0,3000)] distance of the object.

local movenear	= {}
local range 	= {}

local tDistX	= {}
local tDistZ	= {}
local tDistD	= {}
local fFinalY	= {}

function move_near_properties(e,range)
	movenear[e].range = range
end 

function move_near_init(e)
	movenear[e] = {}
	movenear[e].range = 100

	tDistX[e] = 0
	tDistZ[e] = 0
	tDistD[e] = 0
	fFinalY[e] = 0	
end

function move_near_main(e)
	
	local PlayerDist = GetPlayerDistance(e)
	if PlayerDist < movenear[e].range then
		tDistX[e] = g_PlayerPosX - g_Entity[e]['x']
		tDistZ[e] = g_PlayerPosZ - g_Entity[e]['z']
		tDistD[e] = math.abs(tDistX[e]) + math.abs(tDistZ[e])
		tDistX[e] = (tDistX[e] / tDistD[e]) * 10.0
		tDistZ[e] = (tDistZ[e] / tDistD[e]) * 10.0
		g_Entity[e]['x'] = g_Entity[e]['x'] + tDistX[e]
		g_Entity[e]['z'] = g_Entity[e]['z'] + tDistZ[e]
		fFinalY[e] = GetSurfaceHeight(g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'])
		SetPosition(e,g_Entity[e]['x'],fFinalY[e],g_Entity[e]['z'])
	end
end
