-- DESCRIPTION: Pushes the object away from the player when the player moves within [RANGE=60(60,3000)] distance of the object.

g_move_away = {}

function move_away_properties(e,range)
	g_move_away[e]['range'] = range 
end 

function move_away_init(e)
	g_move_away[e] = {}
	g_move_away[e]['range'] = 300
end
function move_away_main(e)
 local PlayerDist = GetPlayerDistance(e)
 local nrange = g_move_away[e]['range']
 local fDetectionRange = nrange
 if PlayerDist < fDetectionRange then
  local tDistX = g_PlayerPosX - g_Entity[e]['x']
  local tDistZ = g_PlayerPosZ - g_Entity[e]['z']
  local tDistD = math.abs(tDistX) + math.abs(tDistZ)
  tDistX = (tDistX / tDistD) * 10.0
  tDistZ = (tDistZ / tDistD) * 10.0
  g_Entity[e]['x'] = g_Entity[e]['x'] - tDistX
  g_Entity[e]['z'] = g_Entity[e]['z'] - tDistZ
  local fFinalY = GetGroundHeight(g_Entity[e]['x'],g_Entity[e]['z'])
  SetPosition(e,g_Entity[e]['x'],fFinalY,g_Entity[e]['z'])
 end
end
