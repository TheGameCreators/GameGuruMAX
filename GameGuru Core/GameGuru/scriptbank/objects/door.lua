-- DESCRIPTION: Open and closes an 'animating' door when the player is within [Range=70(50,500)], and when triggered will open the door, play <Sound0> and turn collision off after a delay of [DELAY=1000]. When the door is closed, play <Sound1> is played. You can opt to keep the door [Unlocked!=1], and customize the [LockedText$="Door locked. Find key"]. The door also [CannotClose!=0]. [ToOpenText$="to open door"]

door_pressed = 0

g_door = {}

function door_properties(e, range, delay, unlocked, lockedtext, cannotclose, toopentext)
 g_door[e]['range'] = range
 g_door[e]['delay'] = delay
 g_door[e]['unlocked'] = unlocked
 g_door[e]['lockedtext'] = lockedtext
 g_door[e]['cannotclose'] = cannotclose
 g_door[e]['toopentext'] = toopentext
end 

function door_init(e)
 g_door[e] = {}
 g_door[e]['mode'] = 0
 g_door[e]['blocking'] = 1
 g_door[e]['originalx'] = -1
 g_door[e]['originaly'] = -1
 g_door[e]['originalz'] = -1
end
	 
function door_main(e)
 if g_door[e]['lockedtext'] == nil then g_door[e]['lockedtext'] = "Door locked. Find key" end
 if g_door[e]['toopentext'] == nil then g_door[e]['toopentext'] = "to open door" end
 if g_door[e]['originalx'] == -1 then
  g_door[e]['originalx'] = g_Entity[e]['x']
  g_door[e]['originaly'] = g_Entity[e]['y']
  g_door[e]['originalz'] = g_Entity[e]['z']
  colx, coly, colz = GetObjectColCentre( g_Entity[e]['obj'] )
  g_door[ e ].originalx = g_door[ e ].originalx + colx
  g_door[ e ].originaly = g_door[ e ].originaly
  g_door[ e ].originalz = g_door[ e ].originalz + colz
 end
 local PlayerDist = GetPlayerDistance(e)
 local nRange = g_door[e]['range']
 if nRange == nil then nRange = 100 end
 if g_door[e]['unlocked'] == 1 then
  g_door[e]['unlocked'] = 0
  SetActivated(e,1)
 end
 if (PlayerDist < nRange ) and g_PlayerHealth > 0 then
  local LookingAt = GetPlrLookingAt(e)	
  if LookingAt == 1 then
   if g_Entity[e]['activated'] == 0 then
    Prompt(g_door[e]['lockedtext'])
   else 
    if g_door[e]['mode'] == 0 then
     if 1 then
	  if GetGamePlayerStateXBOX() == 1 then
       Prompt("Press Y button " .. g_door[e]['toopentext'])
	  else
	   if GetHeadTracker() == 1 then
        Prompt("Trigger to " .. g_door[e]['toopentext'])
	   else
        Prompt("Press E to " .. g_door[e]['toopentext'])
	   end
	  end
      if g_KeyPressE == 1 and g_Entity[e]['animating'] == 0 and door_pressed == 0 then  
       g_door[e]['mode'] = 201
	   door_pressed = 1
	  end
	 end
    else
     if g_door[e]['mode'] == 1 then
	  if g_door[e]['cannotclose'] == 0 then
 	   if g_KeyPressE == 1 and g_Entity[e]['animating'] == 0 and door_pressed == 0 then
        g_door[e]['mode'] = 202
	    door_pressed = 1
	   end
	  end
	 end
    end	
   end
  end
 end
 if g_door[e]['mode'] == 201 then
  -- open door trigger
  SetAnimationName(e,"open")
  PlayAnimation(e)  
  g_door[e]['mode'] = 2
  PlaySound(e,0)
  PerformLogicConnections(e)
  StartTimer(e)
 end
 if g_door[e]['mode'] == 2 then
  -- door collision after X second
  if GetTimer(e)>g_door[e]['delay'] then
   CollisionOff(e)
   g_door[e]['mode'] = 1
   g_door[e]['blocking'] = 2
  end
 end
 if g_door[e]['mode'] == 202 then
  -- close door trigger
  SetAnimationName(e,"close")
  PlayAnimation(e)  
  g_door[e]['mode'] = 3
  StartTimer(e)
 end	
 if g_door[e]['mode'] == 3 then
  -- door collision after X second
  if GetTimer(e)>g_door[e]['delay'] then
   CollisionOn(e)
   PlaySound(e,1)
   PerformLogicConnections(e)
   g_door[e]['mode'] = 0
   g_door[e]['blocking'] = 1
  end
 end
 if g_KeyPressE == 0 then 
  door_pressed = 0
 end
 -- navmesh blocker system
 if g_door[e]['blocking'] ~= 0 then
  local blockmode = 0
  if g_door[e]['blocking'] == 1 then blockmode = 1 end
  if g_door[e]['blocking'] == 2 then blockmode = 0 end
  RDBlockNavMesh(g_door[e]['originalx'],g_door[e]['originaly'],g_door[e]['originalz'],30,blockmode)
  g_door[e]['blocking'] = 0
 end
end
