-- DESCRIPTION: Plays the audio <Sound0> at the volume based on the [RANGE=1000(100,3000)] distance the player is away from the object. The sound will fade in between the [MINVOLUME=0(0,99)] and the [MAXVOLUME=100(1,100)] values.

g_fadesound_instance = 0
g_fadesound_volume = 0
g_fadeinsound = {}

function fadeinsound_properties(e, range, minvolume, maxvolume)
 g_fadeinsound[e]['range'] = range 
 g_fadeinsound[e]['minvolume'] = minvolume
 g_fadeinsound[e]['maxvolume'] = maxvolume
end 

function fadeinsound_init(e)
 g_fadeinsound[e] = {}
 g_fadeinsound[e]['range'] = 1000
 g_fadeinsound[e]['minvolume'] = 0
 g_fadeinsound[e]['maxvolume'] = 100
end

function fadeinsound_main(e)
 local PlayerDist = GetPlayerDistance(e)
 local nrange = g_fadeinsound[e]['range'] 
 if PlayerDist < nrange then
  local nminvol = g_fadeinsound[e]['minvolume']
  local nmaxvol = g_fadeinsound[e]['maxvolume']
  local MaxDist = nrange
  local SndPerc = (MaxDist-PlayerDist)/MaxDist
  local SndVol = 0
  if SndPerc > 0 then
   SndVol = 50+(SndPerc*50)
  else
   SndVol = 0
  end
  if g_fadesound_instance == 0 then
   g_fadesound_instance = e
  end
  if g_fadesound_instance == e then
   LoopNon3DSound(e,0)
   if SndVol < nminvol then 
    SndVol = nminvol
   elseif SndVol > nmaxvol then 
    SndVol = nmaxvol
   end	
   g_fadesound_volume = SndVol
   SetSoundVolume(g_fadesound_volume)
  end
 end
end
