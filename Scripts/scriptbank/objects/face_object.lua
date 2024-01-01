-- DESCRIPTION: Rotates the object to face nearest other object when within [RANGE=300(60,3000)].

g_face_object = {}

function face_object_properties(e,range,minimumrange)
 g_face_object[e]['range'] = range
end 

function face_object_init(e)
 g_face_object[e] = {}
 g_face_object[e]['range'] = 300
end
function face_object_main(e)
 local bestdistance = 99999
 local bestentityindex = -1
 for ee = 1, g_EntityElementMax, 1 do
  if e ~= ee then
   if g_Entity[ee] ~= nil then
    if g_Entity[ee]['active'] > 0 then
     if g_Entity[ee]['health'] > 0 then
	  local thowclosex = g_Entity[ ee ]['x'] - g_Entity[ e ]['x']
	  local thowclosez = g_Entity[ ee ]['z'] - g_Entity[ e ]['z']
	  local thowclosedd = math.sqrt(math.abs(thowclosex*thowclosex)+math.abs(thowclosez*thowclosez))
	  if thowclosedd < bestdistance then
       local fDetectionRange = g_face_object[e]['range']
       if thowclosedd < fDetectionRange then
	    bestdistance = thowclosedd
	    bestentityindex = ee
	   end
	  end
     end
    end
   end
  end
 end 
 if bestentityindex ~= -1 then
  local tDistX = g_Entity[bestentityindex]['x'] - g_Entity[e]['x']
  local tDistZ = g_Entity[bestentityindex]['z'] - g_Entity[e]['z']
  local tAngle = (math.atan2(tDistX,tDistZ)/6.28)*360.0
  SetRotation(e,0,tAngle,0)
 end
end
