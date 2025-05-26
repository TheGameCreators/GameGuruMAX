-- Exploder v4 by Necrym59
-- DESCRIPTION: The attached object will destroy itself when player in range.
-- DESCRIPTION: Set Object to Explode.
-- DESCRIPTION: [DETECTION_RANGE=500(1,2000)]
-- DESCRIPTION: [DAMAGE=100(0,1000)]

local U = require "scriptbank\\utillib"
local lower = string.lower
local exploder 			= {}
local detection_range 	= {}
local damage 			= {}

local closestent		= {}
local doonce			= {}
local status			= {}

function exploder_properties(e, detection_range, damage)
	exploder[e].detection_range = detection_range
	exploder[e].damage = damage	
end

function exploder_init_name(e)
	exploder[e] = {}
	exploder[e].detection_range = 500
	exploder[e].damage = 100
	
	status[e] = "init"
	closestent[e] = 0
end

function exploder_main(e)

	if status[e] == "init" then
		status[e] = "endinit"
	end
	
	local PlayerDist = GetPlayerDistance(e)
	
	if PlayerDist < exploder[e].detection_range then
		SetEntityHealth(e,0)
		closestent[e] = U.ClosestEntToPlayer(90)	
		SetEntityHealth(closestent[e],g_Entity[closestent[e]]['health']-exploder[e].damage)
		HurtPlayer(-1,exploder[e].damage)
		Hide(e)
		CollisionOff(e)
		Destroy(e)		
	end
	
	if g_Entity[e]['activated'] == 1 then
		if PlayerDist < exploder[e].detection_range then
			SetEntityHealth(e,0)
			closestent[e] = U.ClosestEntToPlayer(90)	
			SetEntityHealth(closestent[e],g_Entity[closestent[e]]['health']-exploder[e].damage)
			HurtPlayer(-1,exploder[e].damage)
			Hide(e)
			CollisionOff(e)
			Destroy(e)
		end	
		if PlayerDist > exploder[e].detection_range then
			SetEntityHealth(e,0)
			Hide(e)
			CollisionOff(e)
			Destroy(e)	
		end
	end
end
