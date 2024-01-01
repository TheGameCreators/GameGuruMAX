-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Object Randomiser Behavior v5 by Necrym59
-- DESCRIPTION: Scales and Rotates attached object to random values for variety on each run. 
-- DESCRIPTION: [@ROTATE=1(1=Yes, 2=No)]
-- DESCRIPTION: [ROTATE_LOW_RANGE=10(10,350)]
-- DESCRIPTION: [ROTATE_HIGH_RANGE=350(10,350)]
-- DESCRIPTION: [@SCALE=1(1=Yes, 2=No)]
-- DESCRIPTION: [SCALE_LOW_RANGE=50(10,500)]
-- DESCRIPTION: [SCALE_HIGH_RANGE=180(10,500)]

local randomiser = {}
local rotate = {}
local rotate_low_range = {}
local rotate_high_range = {}
local scale = {}
local scale_low_range = {}
local scale_high_range = {}
local state = {}
local status = {}

function object_randomiser_properties(e, rotate, rotate_low_range, rotate_high_range, scale, scale_low_range, scale_high_range)
	randomiser[e] = {}
	randomiser[e]["rotate"] = rotate	
	randomiser[e]["rotate_low_range"] = rotate_low_range
	randomiser[e]["rotate_high_range"] = rotate_high_range
	randomiser[e]["scale"] = scale
	randomiser[e]["scale_low_range"] = scale_low_range
	randomiser[e]["scale_high_range"] = scale_high_range
	randomiser[e]["state"] = "waiting"
end 
 
function object_randomiser_init(e)
	randomiser[e] = {}	
	randomiser[e]["rotate"] = 1
	randomiser[e]["rotate_low_range"] = 50
	randomiser[e]["rotate_high_range"] = 180
	randomiser[e]["scale"] =	1
	randomiser[e]["scale_low_range"] = 10
	randomiser[e]["scale_high_range"] = 350
	randomiser[e]["state"] = "waiting"
	status[e] = "init"
end

function object_randomiser_main(e)
	if status[e] == "init" then
		if randomiser[e]["rotate_low_range"] == nil then randomiser[e]["rotate_low_range"] = rotate_low_range end
		if randomiser[e]["rotate_high_range"] == nil then randomiser[e]["rotate_high_range"] = rotate_high_range end
		if randomiser[e]["scale_low_range"] == nil then randomiser[e]["scale_low_range"] = scale_low_range end
		if randomiser[e]["scale_high_range"] == nil then randomiser[e]["scale_high_range"] = scale_high_range end
		status[e] = "endinit"
	end
	
	if randomiser[e]["state"] == "waiting" then
		CollisionOff(e)
		local nRotate = randomiser[e]['rotate']
		if randomiser[e]["scale"] == 1 then
			Scale(e,0)
			Scale(e,math.random(randomiser[e]["scale_low_range"],randomiser[e]["scale_high_range"]))
		end	
		if nRotate == 1 then			
			RotateObject(g_Entity[e]['obj'],0,math.random(randomiser[e]["rotate_low_range"],randomiser[e]["rotate_high_range"]),0)
		end
		local newy =GetGroundHeight(g_Entity[e]['x'],g_Entity[e]['z'])
		ResetPosition(g_Entity[e],g_Entity[e]['x'],newy,g_Entity[e]['z'])
		CollisionOn(e)
		randomiser[e]["state"] = "spawned"
	end
end