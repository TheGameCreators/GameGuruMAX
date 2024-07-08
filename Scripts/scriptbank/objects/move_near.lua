-- DESCRIPTION: Pulls the object towards the player when the player moves within [RANGE=300(60,3000)] distance of the object.

g_move_near = {}

function move_near_properties(e,range,minimumrange)
	g_move_near[e]['range'] = range
end 

function move_near_init(e)
	g_move_near[e] = {}
	g_move_near[e]['range'] = 300
end
function move_near_main(e)
 local PlayerDist = GetPlayerDistance(e)
 local fDetectionRange = g_move_near[e]['range']
 if PlayerDist > 100 and PlayerDist < fDetectionRange then
  local tDistX = g_PlayerPosX - g_Entity[e]['x']
  local tDistZ = g_PlayerPosZ - g_Entity[e]['z']
  local tDistD = math.abs(tDistX) + math.abs(tDistZ)
  tDistX = (tDistX / tDistD) * 10.0
  tDistZ = (tDistZ / tDistD) * 10.0
  g_Entity[e]['x'] = g_Entity[e]['x'] + tDistX
  g_Entity[e]['z'] = g_Entity[e]['z'] + tDistZ
  local fFinalY = GetGroundHeight(g_Entity[e]['x'],g_Entity[e]['z'])
  SetPosition(e,g_Entity[e]['x'],fFinalY,g_Entity[e]['z'])
 end
end
