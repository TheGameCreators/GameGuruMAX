-- DESCRIPTION: Loop all audio specified in <Sound0>, <Sound1>, <Sound2>, <Sound3>, <Sound4> and <Sound5> at the volume based on the [RANGE=1000(100,3000)] distance the player is away from the object, between [MinVolume=0(0,99)] and [MaxVolume=100(1,100)].

module_activationcontrol = require "scriptbank\\module_activationcontrol"

loop3dsound_multiple = {}

function loop3dsound_multiple_properties(e, range, minvolume, maxvolume)
 module_activationcontrol.init(e,1)
 loop3dsound_multiple[e]['range'] = range 
 loop3dsound_multiple[e]['minvolume'] = minvolume
 loop3dsound_multiple[e]['maxvolume'] = maxvolume
end 

function loop3dsound_multiple_init(e)
 loop3dsound_multiple[e] = {}
 loop3dsound_multiple[e]['range'] = 1000
 loop3dsound_multiple[e]['minvolume'] = 0
 loop3dsound_multiple[e]['maxvolume'] = 100
end

function loop3dsound_multiple_main(e)
 if module_activationcontrol.control(e) == 1 and g_Entity[e]['active'] == 1 then
  local PlayerDist = GetPlayerDistance(e)
  local nrange = loop3dsound_multiple[e]['range']
  if PlayerDist < nrange then	
   local nminvol = loop3dsound_multiple[e]['minvolume']
   local nmaxvol = loop3dsound_multiple[e]['maxvolume']
   local MaxDist = nrange
   local SndVol = 0
   local SndPerc = (MaxDist-PlayerDist)/MaxDist
   if SndPerc > 0 then
    SndVol = 50+(SndPerc*50)
   else
    SndVol = 0
   end	
   LoopNon3DSound(e,0)
   LoopNon3DSound(e,1)
   LoopNon3DSound(e,2)
   LoopNon3DSound(e,3)
   LoopNon3DSound(e,4)
   LoopNon3DSound(e,5)
   if SndVol < nminvol then 
    SndVol = nminv
   elseif SndVol > nmaxvol then 
    SndVol = nmaxvol
   end		
   SetSoundVolume(SndVol)
  else
   StopSound(e,0)
   StopSound(e,1)
   StopSound(e,2)
   StopSound(e,3)
   StopSound(e,4)
   StopSound(e,5)
  end
 else
  StopSound(e,0)
  StopSound(e,1)
  StopSound(e,2)
  StopSound(e,3)
  StopSound(e,4)
  StopSound(e,5)
 end
end
