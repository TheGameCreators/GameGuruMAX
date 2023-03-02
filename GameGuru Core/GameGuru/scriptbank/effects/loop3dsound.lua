-- DESCRIPTION: Loop the audio <Sound0> at the volume based on the [RANGE=1000(100,3000)] distance the player is away from the object, between [MinVolume=0(0,99)] and [MaxVolume=100(1,100)].

module_activationcontrol = require "scriptbank\\module_activationcontrol"

g_loop3dsound = {}
g_loop3dsound = {}

function loop3dsound_properties(e, range, minvolume, maxvolume)
 module_activationcontrol.init(e,1)
 g_loop3dsound[e]['range'] = range 
 g_loop3dsound[e]['minvolume'] = minvolume
 g_loop3dsound[e]['maxvolume'] = maxvolume
end 
function loop3dsound_init(e)
 g_loop3dsound[e] = {}
 g_loop3dsound[e]['range'] = 1000
 g_loop3dsound[e]['minvolume'] = 0
 g_loop3dsound[e]['maxvolume'] = 100
end
function loop3dsound_main(e)
 if module_activationcontrol.control(e) == 1 and g_Entity[e]['active'] == 1 then
  local PlayerDist = GetPlayerDistance(e)
  local nrange = g_loop3dsound[e]['range']
  if PlayerDist < nrange then	
   local nminvol = g_loop3dsound[e]['minvolume']
   local nmaxvol = g_loop3dsound[e]['maxvolume']
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
  else
   StopSound(e,0)
  end
 else
  StopSound(e,0)
 end
end
