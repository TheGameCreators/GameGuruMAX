-- DESCRIPTION: Fade out the audio <Sound0> based on the [RANGE=1000(100,3000)] distance the player is from the object. The sound will fade out between the [MINVOLUME=0(0,99)] and the [MAXVOLUME=100(1,100)] values.

g_fadesound_instance = 0
g_fadesound_volume = 0
g_fadeoutsound = {}

function fadeoutsound_properties(e, range, minvolume, maxvolume)
 g_fadeoutsound[e]['range'] = range 
 g_fadeoutsound[e]['minvolume'] = minvolume
 g_fadeoutsound[e]['maxvolume'] = maxvolume
end 

function fadeoutsound_init(e)
 g_fadeoutsound[e] = {}
 g_fadeoutsound[e]['range'] = 1000
 g_fadeoutsound[e]['minvolume'] = 0
 g_fadeoutsound[e]['maxvolume'] = 100
end
 
function fadeoutsound_main(e)
 local PlayerDist = GetPlayerDistance(e)
 local nrange = g_fadeoutsound[e]['range'] 
 if PlayerDist < nrange then
  local nminvol = g_fadeoutsound[e]['minvolume']
  local nmaxvol = g_fadeoutsound[e]['maxvolume']
  local MaxDist = nrange
  local SndPerc = (MaxDist-PlayerDist)/MaxDist
  local SndVol = 0
  if SndPerc > 0 then
   SndVol = 100-(SndPerc*50)
  else
   SndVol = 100
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
