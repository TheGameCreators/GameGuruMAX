-- Jumpscare v1 - thanks to Smallg
-- DESCRIPTION: Triggers a jump scare event for a trigger zone. 

local appearances = {}
local fade_delay = {}
local hidden = {}
local fade_delay_init = 0
 
--set these to 0 if you dont want any movement from original location
--how far away from starting location entity can move in x axis
local movex_random = 0

--how far away from starting location entity can move in z axis
local movez_random = 0
 
function jumpscare_init(e)
 --how many times entity will appear
 appearances[e] = math.random(1,1)
 --how long between appearances (and how long it will appear for)
 --is randomed so can be 25% shorter or longer (see near end of script)
 fade_delay[e] = 650  
 ---------------
 hidden[e] = 1
 CollisionOff(e)
 fade_delay_init = fade_delay[e]
 Hide(e)
end
 
function jumpscare_main(e)
 RotateToPlayer(e)
 CollisionOff(e)
 if g_Entity[e]['activated'] == 1 then
  if GetTimer(e) > fade_delay[e] then
   if appearances[e] > 0 then
    if hidden[e] == 1 then
     PlaySound(e,1)
     Show(e)
     RotateToPlayer(e)
     hidden[e] = 0
     appearances[e] = appearances[e] - 1
    else
     Hide(e)
     SetPosition(e,g_Entity[e]['x'] + math.random(-movex_random,movex_random),g_Entity[e]['y'],g_Entity[e]['z']+math.random(-movez_random,movez_random))
     hidden[e] = 1
    end
   else
    if hidden[e] == 0 then
     Hide(e)
    end
   end
   fade_delay[e] = GetTimer(e) + math.random(fade_delay_init * 0.75,fade_delay_init * 1.25)
  end
 end
end
 
function jumpscare_exit(e)
end
