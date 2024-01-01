-- DESCRIPTION: Makes the object loop around the nearest waypoint path and loops <Sound0>. The volume is based on the player’s maximum distance [RANGE=1000(100,3000)] to the object, between [MinVolume=0(0,99)] and [MaxVolume=100(1,100)].

g_follow_waypoint_mode = {}
g_follow_waypoint_pathindex = {}
g_follow_waypoint_pathpointindex = {}

g_loopwaypoint = {}

function loopwaypoint_properties(e, speech, range, minvolume, maxvolume)
	g_loopwaypoint[e]['speech'] = speech
	g_loopwaypoint[e]['range'] = range 
	g_loopwaypoint[e]['minvolume'] = minvolume
	g_loopwaypoint[e]['maxvolume'] = maxvolume
end 

function loopwaypoint_init(e)
	g_loopwaypoint[e]={}
	g_loopwaypoint[e]['speech'] = ""
	g_loopwaypoint[e]['range'] = 1000
	g_loopwaypoint[e]['minvolume'] = 50
	g_loopwaypoint[e]['maxvolume'] = 100
	CollisionOff(e)
	g_follow_waypoint_mode[e] = -1
	g_follow_waypoint_pathindex[e] = 0
	g_follow_waypoint_pathpointindex[e] = 0
end

function loopwaypoint_main(e)
 -- play and fade sound if present
 local PlayerDist = GetPlayerDistance(e)
 local nrange = g_loopwaypoint[e]['range']
 local nminvol = g_loopwaypoint[e]['minvolume']
 local nmaxvol = g_loopwaypoint[e]['maxvolume']
 local MaxDist = nrange
 local SndVol = 0
 local SndPerc = (MaxDist-PlayerDist)/MaxDist
 if SndPerc > 0 then
  SndVol = 50+(SndPerc*50)
 else
  SndVol = 0
 end
 LoopNon3DSound(e,0)
 if SndVol < nminvol then 
		SndVol = nminv
	elseif SndVol > nmaxvol then 
		SndVol = nmaxvol
	end		
 SetSoundVolume(SndVol) 
 -- handle waypoint movement
 if g_follow_waypoint_mode[e] == -1 then
  -- find closest waypoint
  PathIndex = -1
  PointIndex = 2
  pClosest = 99999
  for pa = 1, AIGetTotalPaths(), 1 do
   for po = 1, AIGetPathCountPoints(pa), 1 do
    pDX = g_Entity[e]['x'] - AIPathGetPointX(pa,po)
    pDY = g_Entity[e]['y'] - AIPathGetPointY(pa,po)
    pDZ = g_Entity[e]['z'] - AIPathGetPointZ(pa,po)
    pDist = math.sqrt(math.abs(pDX*pDX)+math.abs(pDY*pDY)+math.abs(pDZ*pDZ));
    if pDist < pClosest and pDist < 200 then
     pClosest = pDist
     PathIndex = pa
     PointIndex = po
    end
   end
  end
  if PathIndex > -1 then
   g_follow_waypoint_pathindex[e] = PathIndex
   g_follow_waypoint_pathpointindex[e] = PointIndex
   g_follow_waypoint_mode[e] = 1
  end
 end
 if g_follow_waypoint_mode[e] == 1 then
  pDX = g_Entity[e]['x'] - AIPathGetPointX(g_follow_waypoint_pathindex[e],g_follow_waypoint_pathpointindex[e])
  pDZ = g_Entity[e]['z'] - AIPathGetPointZ(g_follow_waypoint_pathindex[e],g_follow_waypoint_pathpointindex[e])
  pDD = math.sqrt(math.abs(pDX*pDX)+math.abs(pDZ*pDZ))
  pMinSpeed = GetMovementSpeed(e)
  if pMinSpeed < 1.0 then pMinSpeed = 1.0 end
  if pDD < pMinSpeed*15.0 then
   -- loop if at end of path
   if g_follow_waypoint_pathpointindex[e] >= AIGetPathCountPoints(g_follow_waypoint_pathindex[e]) then
    g_follow_waypoint_pathpointindex[e] = 1
   else
    g_follow_waypoint_pathpointindex[e] = g_follow_waypoint_pathpointindex[e] + 1
   end
  end
  -- move entity to path node
  patrolx = AIPathGetPointX(g_follow_waypoint_pathindex[e],g_follow_waypoint_pathpointindex[e])
  patroly = AIPathGetPointY(g_follow_waypoint_pathindex[e],g_follow_waypoint_pathpointindex[e])
  patrolz = AIPathGetPointZ(g_follow_waypoint_pathindex[e],g_follow_waypoint_pathpointindex[e])
  tDistX = g_Entity[e]['x'] - patrolx
  tDistZ = g_Entity[e]['z'] - patrolz
  tAngleY = math.atan2(tDistX,tDistZ)
  tAngleY = (tAngleY * 57.2957795)
  SetRotationYSlowly(e,tAngleY,GetMovementSpeed(e)*20)
  MoveForward(e,GetMovementSpeed(e)*-100.0)
 end
end

