-- MASTER INTERPRETER - V2 - Contributors; Necrym59
local master_interpreter_core = {}

-- Slows down logic for closer debugging
g_masterinterpreter_slowtimeinterval = 0 -- set in ms to slow down logic time to see debugged instruction block (introduces anim issue)
g_masterinterpreter_slowtime = 0
g_masterinterpreter_playerfrozenby = 0

-- Store player weapon id for freeze / unfreeze action
g_masterinterpreter_playerweaponid = 0

-- Other key globals
g_masterinterpreter_logicboostcount = 1

-- Conditions
g_masterinterpreter_cond_reserved0 = 0
g_masterinterpreter_cond_reserved1 = 1
g_masterinterpreter_cond_reserved2 = 2
g_masterinterpreter_cond_reserved3 = 3
g_masterinterpreter_cond_reserved4 = 4
g_masterinterpreter_cond_reserved5 = 5
g_masterinterpreter_cond_reserved6 = 6
g_masterinterpreter_cond_reserved7 = 7
g_masterinterpreter_cond_reserved8 = 8
g_masterinterpreter_cond_reserved9 = 9
g_masterinterpreter_cond_reserved9 = 10
g_masterinterpreter_cond_always = 11 -- Always (Is true all the time)
g_masterinterpreter_cond_targetwithin = 12 -- Target Within (Is True when the assigned target is within the specified distance)
g_masterinterpreter_cond_random = 13 -- Random (Is true when the random number generated from the specified range returns one)
g_masterinterpreter_cond_targetvisible = 14 -- Target Visible (Is true when the assigned target can be seen)
g_masterinterpreter_cond_endofanim = 15 -- End Of Animation (Is true when current playing animation ends)
g_masterinterpreter_cond_gothurt = 16 -- Taken Damage (Is true when damage has been taken)
g_masterinterpreter_cond_checklimbhit = 17 -- Check Limb Hit (Is true when the limb specified receives damage)
g_masterinterpreter_cond_checkdamage = 18 -- Check Damage (Is true when damage received is greater than specified threshold)
g_masterinterpreter_cond_checkhealth = 19 -- Health Within (Is true when health is lower than specified health value)
g_masterinterpreter_cond_flagvalid = 20 -- Flag Valid (Is true when there is a valid flag that can be used as a target)
g_masterinterpreter_cond_targetbeyond = 21 -- Target Beyond (Is True when the assigned target is beyond the specified distance)
g_masterinterpreter_cond_checktimer = 22 -- Check Timer (Is true when previously started timer is greater than specified milliseconds)
g_masterinterpreter_cond_canfireweapon = 23 -- Can Fire Weapon (Is true when an associated character weapon can be fired)
g_masterinterpreter_cond_targetangle = 24 -- Target Angle Within (Is true when target is within specified angle)
g_masterinterpreter_cond_targetangletoleft = 25 -- Target Angle To Left (Is true when target is beyond specified angle to the left)
g_masterinterpreter_cond_targetangletoright = 26 -- Target Angle To Right (Is true when target is beyond specified angle to the right)
g_masterinterpreter_cond_speechplaying = 27 -- Speech Playing (Is true when a previously played speech is still playing)
g_masterinterpreter_cond_targetpathvalid = 28 -- Target Path Valid (Is true when a previous turn to target finds a good path)
g_masterinterpreter_cond_targetpathcompleted = 29 -- Target Path Completed (Is true when reached end of path set with Goto commands)
g_masterinterpreter_cond_isvaluezero = 30 -- Is Value Zero (Is true when a behavior value indicated by '=yourvalue' equals zero)
g_masterinterpreter_cond_isactivated = 31 -- Is Activated (Is true when something activates this object)
g_masterinterpreter_cond_ifseeenemy = 32 -- If See Enemy (Is true when see the enemy based on the allegiance)
g_masterinterpreter_cond_ifseetarget = 33 -- If See Target (Is true when see the target position using line of sight)
g_masterinterpreter_cond_ifally = 34 -- If Ally (Is true if this is an ally character)
g_masterinterpreter_cond_isvalueone = 35 -- Is Value One (Is true when a behavior value indicated by '=yourvalue' equals 1)
g_masterinterpreter_cond_isvaluetwo = 36 -- Is Value Two (Is true when a behavior value indicated by '=yourvalue' equals 2)
g_masterinterpreter_cond_isvaluethree = 37 -- Is Value Three (Is true when a behavior value indicated by '=yourvalue' equals 3)
g_masterinterpreter_cond_isvaluefour = 38 -- Is Value Four (Is true when a behavior value indicated by '=yourvalue' equals 4)
g_masterinterpreter_cond_isvaluefive = 39 -- Is Value Five (Is true when a behavior value indicated by '=yourvalue' equals 5)
g_masterinterpreter_cond_istargetdead = 40 -- Is Target Dead (Is true when a target hostile has been killed)
g_masterinterpreter_cond_hasweapon = 41 -- Has Weapon (Is true if character has been assigned a weapon to use)
g_masterinterpreter_cond_isunderwater = 42 -- Is Underwater (Is true if object Y position is under the water line)
g_masterinterpreter_cond_animationexists = 43 -- Animation Exist (Is true if specified animation name exists for this object)
g_masterinterpreter_cond_ifpushedback = 44 -- If Pushed Back (Is true if object pushed back over specified milliseconds)
g_masterinterpreter_cond_checkgoaltimer = 45 -- Check Goal Timer (Is true if goal timer exceeds specified milliseconds)
g_masterinterpreter_cond_isplayerfrozen = 46 -- Is Player Frozen (Is true if the player is in a frozen state)
g_masterinterpreter_cond_heardanysound = 47 -- Heard Any Sound (Is true if heard any sound within optional range)
g_masterinterpreter_cond_heardenemysound = 48 -- Heard Enemy Sound (Is true if heard an enemy sound within optional range)
g_masterinterpreter_cond_ifneutral = 49 -- If Neutral (Is true if this is a neutral character)
g_masterinterpreter_cond_animationplaying = 50 -- Animation Playing (Is true if specified animation name is playing for this object)
g_masterinterpreter_cond_istargetalive = 51 -- Is Target Alive (Is true if the specified target is alive with health)
g_masterinterpreter_cond_reachtriggerone = 52 -- Reach Trigger A (Is true when animation frame passes built-in trigger A, i.e. moment of hit/footfall)
g_masterinterpreter_cond_reachtriggertwo = 53 -- Reach Trigger B (Is true when animation frame passes built-in trigger B, i.e. moment of hit/footfall)
g_masterinterpreter_cond_reachtriggerthree = 54 -- Reach Trigger C (Is true when animation frame passes built-in trigger C, i.e. moment of hit/footfall)
g_masterinterpreter_cond_pathdistancebeyond = 55 -- Path Distance Beyond (Is true if path destination distance is larger than specified distance)
g_masterinterpreter_cond_withinattackrange = 56 -- Within Attack Range (Is True when the assigned target is within the characters attack range)
g_masterinterpreter_cond_targetdestwithin = 57 -- Target Dest Within (Is True when the distance between target to nav mesh destination is within the specified distance)
g_masterinterpreter_cond_checktimerwithfirerate = 58 -- Check Timer With Firerate (Is true when previously started timer is greater than current weapons firerate delay)
g_masterinterpreter_cond_haveammo = 59 -- Check Have Ammo (Is true when this character has ammo for their weapon)
g_masterinterpreter_cond_havealerttarget = 60 -- Have Alert Target (Is true an alert target has been set and not yet reset)
g_masterinterpreter_cond_keypressed = 61 -- Key Pressed (Is true when the E key is pressed)
g_masterinterpreter_cond_usinghud = 62 -- Using HUD (Is true when using a HUD screen)
g_masterinterpreter_cond_targetreachable = 63 -- Target Reachable (Is true when a valid path can be made to the target)
g_masterinterpreter_cond_withinnavmesh = 64 -- Within Navmesh (Is true if the object is within a valid navmesh)
g_masterinterpreter_cond_canmeleetarget = 65 -- Can Melee Target (Is true if there is nothing between object and the target)
g_masterinterpreter_cond_istargetname = 66 -- Is Target Name (Is true if the current target name matches the specified string)

-- Actions
g_masterinterpreter_act_gotostate = 0 -- Go To State (Jumps immediately to the specified state if the state)
g_masterinterpreter_act_gotoinstruction = 1 -- Go To Instruction (Jumps immediately to the specified instruction)
g_masterinterpreter_act_reserved2 = 2
g_masterinterpreter_act_reserved3 = 3
g_masterinterpreter_act_reserved4 = 4
g_masterinterpreter_act_reserved5 = 5
g_masterinterpreter_act_reserved6 = 6
g_masterinterpreter_act_reserved7 = 7
g_masterinterpreter_act_reserved8 = 8
g_masterinterpreter_act_reserved9 = 9
g_masterinterpreter_act_reserved10 = 10
g_masterinterpreter_act_none = 11 -- Do Nothing (Perform no action)
g_masterinterpreter_act_play = 12 -- Play Animation (Play the specified animation)
g_masterinterpreter_act_loop = 13 -- Loop Animation (Loop the specified animation)
g_masterinterpreter_act_checktarget = 14 -- Check Target (Scans the level to determine if the assigned target is visible)
g_masterinterpreter_act_lookat = 15 -- Look At Target (If object has a head, will turn head to face assigned target)
g_masterinterpreter_act_facetarget = 16 -- Face Target (Turn to face the assigned target)
g_masterinterpreter_act_hurttarget = 17 -- Hurt Target (Apply specified damage to the assigned target)
g_masterinterpreter_act_starttimer = 18 -- Start Timer (Restart the stopwatch timer to zero milliseconds)
g_masterinterpreter_act_freezetarget = 19 -- Freeze Target (Freeze the assigned target in place so that it cannot move)
g_masterinterpreter_act_unfreezetarget = 20 -- Unfreeze Target (Unfreeze the assigned target so it can once again move)
g_masterinterpreter_act_grabtargetview = 21 -- Grab Target View (Rotate the assigned target to face the object)
g_masterinterpreter_act_stop = 22 -- Stop Animation (Stop any currently playing animation)
g_masterinterpreter_act_gototarget = 23 -- Go To Target (Plot a navigation path and turn to next point to the assigned target)
g_masterinterpreter_act_settargetenemy = 24 -- Target Enemy (Assign target as the determined enemy according to allegiance)
g_masterinterpreter_act_settargetflag = 25 -- Target Flag (Assign target as the nearest flag element within specified range)
g_masterinterpreter_act_turnright = 26 -- Turn Right (Turn right by the specified angle in degrees)
g_masterinterpreter_act_turnleft = 27 -- Turn Left (Turn left by the specified angle in degrees)
g_masterinterpreter_act_goawayfromtarget = 28 -- Go Away From Target (Go away from the assigned target)
g_masterinterpreter_act_restorehealth = 29 -- Restore Health (Set health back to the original strength value)
g_masterinterpreter_act_settargetstart = 30 -- Target Start Position (Assign target as the original start location)
g_masterinterpreter_act_moveforward = 31 -- Move Forward (Move forward by the specified speed value)
g_masterinterpreter_act_stopmoving = 32 -- Stop Moving (Stop moving if commanded with a previous move action)
g_masterinterpreter_act_hideweapon = 33 -- Hide Weapon (Hide any weapon or attachment associated with the character)
g_masterinterpreter_act_showweapon = 34 -- Show Weapon (Show any weapon or attachment associated with the character)
g_masterinterpreter_act_fireweapon = 35 -- Fire Weapon (Instantly fire any weapon associated with the character)
g_masterinterpreter_act_aimat = 36 -- Aim At Target (If object is a character, will aim to face assigned target)
g_masterinterpreter_act_stopaiming = 37 -- Stop Aiming (If object is a character, will stop aiming)
g_masterinterpreter_act_gototargetflank = 38 -- Go To Target Flank (Turn to a flank point towards the assigned target)
g_masterinterpreter_act_flankleft = 39 -- Flank Left (Always flank to the left when instructed)
g_masterinterpreter_act_flankright = 40 -- Flank Right (Always flank to the right when instructed)
g_masterinterpreter_act_flankrandomly = 41 -- Flank Randomly (Flank to the left or right randomly when instructed)
g_masterinterpreter_act_playspeech = 42 -- Play Speech (Play the speech specified once only when called)
g_masterinterpreter_act_stopspeech = 43 -- Stop Speech (Stop any previously played speech immediately)
g_masterinterpreter_act_playsound = 44 -- Play Sound (Play the sound in the specified slot number 0-3)
g_masterinterpreter_act_loopsound = 45 -- Loop Sound (Loop the sound in the specified slot number 0-3)
g_masterinterpreter_act_stopsound = 46 -- Stop Sound (Stop the sound in the specified slot number 0-3)
g_masterinterpreter_act_stopandturn = 47 -- Stop Animation And Turn (Stop any currently playing animation and instantly rotate by specified angle)
g_masterinterpreter_act_lookatplayer = 48 -- Look At Player (If object has a head, will turn head to face the player)
g_masterinterpreter_act_lookforward = 49 -- Look Forward (If object has a head, will turn to face forwards)
g_masterinterpreter_act_preventzerohealth = 50 -- Prevent Zero Health (Does not destroy the object when it reaches zero health)
g_masterinterpreter_act_destroy = 51 -- Destroy (Sets the objects health to zero, causing instant destruction)
g_masterinterpreter_act_multiplyanimspeed = 52 -- Multiply Animation Speed (Increases/decreases animation speed based on the specified multiplier)
g_masterinterpreter_act_sethealth = 53 -- Set Health (Sets the object health to the specified value)
g_masterinterpreter_act_adjustlookhlimit = 54 -- Adjust Look HLimit (Sets the maximum angle the head can turn left and right)
g_masterinterpreter_act_adjustlookhoffset = 55 -- Adjust Look HOffset (Sets the initial pose offset of the heads left/right angle)
g_masterinterpreter_act_adjustlookvlimit = 56 -- Adjust Look VLimit (Sets the maximum angle the head can angle up and down)
g_masterinterpreter_act_adjustlookvoffset = 57 -- Adjust Look VOffset (Sets the initial pose offset of the heads up/down angle)
g_masterinterpreter_act_adjustaimhlimit = 58 -- Adjust Aim HLimit (Sets the maximum angle the weapon can turn left and right)
g_masterinterpreter_act_adjustaimhoffset = 95 -- Adjust Aim HOffset (Sets the initial pose offset of the weapon left/right angle)
g_masterinterpreter_act_adjustaimvlimit = 60 -- Adjust Aim VLimit (Sets the maximum angle the weapon can angle up and down)
g_masterinterpreter_act_adjustaimvoffset = 61 -- Adjust Aim VOffset (Sets the initial pose offset of the weapon up/down angle)
g_masterinterpreter_act_gotostepleft = 62 -- Go To Step Left (Plot a navigation path to step left to ensure can navigate when stepping)
g_masterinterpreter_act_gotostepright = 63 -- Go To Step Right (Plot a navigation path to step right to ensure can navigate when stepping)
g_masterinterpreter_act_settargetactivator = 64 -- Target Alert Position (Assign target to the location the activation originated)
g_masterinterpreter_act_performlogicconnections = 65 -- Perform Logic Connections (Activates logic for all objects connected with this one)
g_masterinterpreter_act_playrandom = 66 -- Play Animation Random (Play the specified animation but with a random starting position)
g_masterinterpreter_act_looprandom = 67 -- Loop Animation Random (Loop the specified animation but with a random starting position)
g_masterinterpreter_act_settargetplayer = 68 -- Target Player (Assign target to the player)
g_masterinterpreter_act_flankforward = 69 -- Flank Forward (Do not flank, but instead go directly towards target)
g_masterinterpreter_act_setvaluetozero = 70 -- Set Value To Zero (Sets the value indicated by '=yourvalue' to 0)
g_masterinterpreter_act_setvaluetoone = 71 -- Set Value To One (Sets the value indicated by '=yourvalue' to 1)
g_masterinterpreter_act_lookrandomly = 72 -- Look Randomly (If object has a head, will turn to face a random direction)
g_masterinterpreter_act_runbehavior = 73 -- Run Behavior (Load and run the specified behavior and stop this one)
g_masterinterpreter_act_returnfrombehavior = 74 -- Return From Behavior (Returns to the master behavior for this object)
g_masterinterpreter_act_gotostepforward = 75 -- Go To Step Forward (Plot a navigation path to step toward target)
g_masterinterpreter_act_gotoneartarget = 76 -- Go To Near Target (Plot a navigation path to get near the assigned target)
g_masterinterpreter_act_becomeenemy = 77 -- Become Enemy (Change allegiance so the player becomes the enemy)
g_masterinterpreter_act_becomeally = 78 -- Become Ally (Change allegiance so the player becomes an ally)
g_masterinterpreter_act_becomeneutral = 79 -- Become Neutral (Change allegiance so the character becomes neutral)
g_masterinterpreter_act_resetgoaltimer = 80 -- Reset Goal Timer (Reset goal timer to zero to start a behavior event goal)
g_masterinterpreter_act_makeaisound = 81 -- Make AI Sound (Make an AI sound event that can be heard by others)
g_masterinterpreter_act_gotostepbackward = 82 -- Go To Step Backward (Plot a navigation path to step away from target)
g_masterinterpreter_act_setvaluetorandom = 83 -- Set Value To Random (Sets the value indicated by '=yourvalue' to a random value between 0 and 1)
g_masterinterpreter_act_setvaluetorandomtwo = 84 -- Set Value To Random 2 (Sets the value indicated by '=yourvalue' to a random value between 1 and 2)
g_masterinterpreter_act_setvaluetorandomthree = 85 -- Set Value To Random 3 (Sets the value indicated by '=yourvalue' to a random value between 1 and 3)
g_masterinterpreter_act_setvaluetorandomfour = 86 -- Set Value To Random 4 (Sets the value indicated by '=yourvalue' to a random value between 1 and 4)
g_masterinterpreter_act_setvaluetorandomfive = 87 -- Set Value To Random 5 (Sets the value indicated by '=yourvalue' to a random value between 1 and 5)
g_masterinterpreter_act_setimmunitytime = 88 -- Set Immunity Time (Sets the object to be immune from damage of any kind for specified time in milliseconds)
g_masterinterpreter_act_setalerttoenemy = 89 -- Set Alert To Enemy (Assign alert position to location of the enemy )
g_masterinterpreter_act_gotonearbyspot = 90 -- Go To Nearby Spot (Plot a navigation path to a spot within the immediate area)
g_masterinterpreter_act_instantragdoll = 91 -- Instant Ragdoll (Sets the objects health to zero, and trigger ragdoll if character)
g_masterinterpreter_act_setpathrotation = 92 -- Set Path Rotation (Set the object path rotating mode to 0 to disable rotation to face path direction)
g_masterinterpreter_act_gotoattackspot = 93 -- Go To Attack Spot (Plot a navigation path to take up unique position to melee attack)
g_masterinterpreter_act_facelasttarget = 94 -- Face Last Target (Turn to face the position of the last known target if ever had one)
g_masterinterpreter_act_triggercombatmusic = 95 -- Trigger Combat Music (Starts combat music if specified, and fades out if no longer called)
g_masterinterpreter_act_playbyname = 96 -- Play Animation By Name (Play the animation specified in the provided "=name")
g_masterinterpreter_act_loopbyname = 97 -- Loop Animation By Name (Loop the animation specified in the provided "=name")
g_masterinterpreter_act_hurttargetwithshake = 98 -- Hurt Target With Shake (Apply specified damage to the assigned target, causing more shake if player)
g_masterinterpreter_act_reloadammo = 99 -- Reload Ammo (Reload ammo of characters weapon with a full clip)
g_masterinterpreter_act_resetalerttarget = 100 -- Reset Alert Target (Reset the alert target to forget last known alert target)
g_masterinterpreter_act_showtext = 101 -- Show Text (Show a Text Prompt at the bottom of the screen)
g_masterinterpreter_act_showhud = 102 -- Show HUD (Show the specified HUD screen and remain there until player leaves)
g_masterinterpreter_act_changeglobal = 103 -- Change Global (Change the value of a global using LUA formatted string)
g_masterinterpreter_act_changecontainer = 104 -- Change Container (Change the non-player container for the next time a HUD screen is used)
g_masterinterpreter_act_logicboost = 105 -- Logic Boost (Instructs logic instructions to run in a batch of the specified count)
g_masterinterpreter_act_collisionoff = 106 -- Turn Collision Off (Switch off collision for this object)
g_masterinterpreter_act_collisionon = 107 -- Turn Collision On (Switch on collision for this object)


-- special callout manager to avoid insane chatter for characters
g_calloutmanager = {}
g_calloutmanagertime = {}

function masterinterpreter_scanforenemy ( e, output_e, anywilldo )
 local bestdistance = 99999
 local bestentityindex = -1
 for ee = 1, g_EntityElementMax, 1 do
  if e ~= ee then
   if g_Entity[ee] ~= nil then
    if g_Entity[ee]['active'] > 0 then
     if g_Entity[ee]['health'] > 0 then
	  local thisvisible = GetEntityVisibility(ee)
	  if thisvisible ~= 0 then
       local thisallegiance = GetEntityAllegiance(ee)
       local foundenemy = 0
       if thisallegiance == 0 and GetEntityAllegiance(e) == 1 then foundenemy = 1 end
       if thisallegiance == 1 and GetEntityAllegiance(e) == 0 then foundenemy = 1 end
	   if foundenemy == 1 then
	    local thowclosex = g_Entity[ ee ]['x'] - g_Entity[ e ]['x']
	    local thowclosez = g_Entity[ ee ]['z'] - g_Entity[ e ]['z']
	    local thowclosedd = math.sqrt(math.abs(thowclosex*thowclosex)+math.abs(thowclosez*thowclosez))
	    if thowclosedd < bestdistance then
         local fromx = g_Entity[ e ]['x']
         local fromy = g_Entity[ e ]['y']+60
         local fromz = g_Entity[ e ]['z']
		 --local ignoreplayer = 0
         local hit = masterinterpreter_rayscan(e, output_e,fromx,fromy,fromz,g_Entity[ ee ]['x'],g_Entity[ ee ]['y']+60,g_Entity[ ee ]['z'],g_Entity[e]['obj'], 1)
         if hit == 0 or hit == g_Entity[ee]['obj'] then
	      bestdistance = thowclosedd
	      bestentityindex = ee
		  if anywilldo == 1 then return bestentityindex end
		 else
		  --PromptLocal(e,"masterinterpreter_scanforenemy="..hit.." ("..g_Entity[e]['obj']..")")
	     end
		end
	   end
	  end
     end
    end
   end
  end
 end 
 return bestentityindex
end

function masterinterpreter_findnewenemytarget ( e, output_e )
 output_e['targete'] = masterinterpreter_scanforenemy (e, output_e, 0)
end

function masterinterpreter_gettargetxyz ( e, output_e )
 local TargetX = 0
 local TargetY = 0
 local TargetZ = 0
 if output_e['target'] == "player" then
  TargetX = g_PlayerPosX
  TargetY = g_PlayerPosY-35
  TargetZ = g_PlayerPosZ
 end
 if output_e['target'] == "flag" or output_e['target'] == "hostile" then
  if output_e['target'] == "hostile" then
   local ee = output_e['targete']
   if ee > 0 then
    if g_Entity[ee]['health'] <= 0 then
	 masterinterpreter_findnewenemytarget ( e, output_e )
	end
   end
  end
  local ee = output_e['targete']
  if ee > 0 then
   if g_Entity[ee] ~= nil then
    TargetX = g_Entity[ee]['x']
    TargetY = g_Entity[ee]['y']
    TargetZ = g_Entity[ee]['z']
   end
  end
 end
 if output_e['target'] == "start" then
  TargetX = output_e['startpositionx']
  TargetY = output_e['startpositiony']
  TargetZ = output_e['startpositionz']
 end 
 output_e['lasttarget'] = output_e['target']
 output_e['lasttargete'] = output_e['targete']
 return TargetX, TargetY, TargetZ
end

function masterinterpreter_gettargetslot ( e, output_e )
 local TargetSlot1 = 0
 local TargetSlot2 = 0
 local TargetSlot3 = 0
 local TargetSlot4 = 0
 if output_e['target'] == "player" then
  for slot = 0, 3, 1 do
   local testslote = 0
   if slot == 0 then testslote = g_PlayerTargetSlot1 end
   if slot == 1 then testslote = g_PlayerTargetSlot2 end
   if slot == 2 then testslote = g_PlayerTargetSlot3 end
   if slot == 3 then testslote = g_PlayerTargetSlot4 end
   if testslote > 0 then
    if g_Entity[testslote] ~= nil then
     local dx = g_Entity[ testslote ]['x'] - g_PlayerPosX
     local dz = g_Entity[ testslote ]['z'] - g_PlayerPosZ
	 local dd = math.sqrt(math.abs(dx*dx),math.abs(dz*dz))
	 if dd > 200 then 
      if slot == 0 then g_PlayerTargetSlot1 = 0 end
      if slot == 1 then g_PlayerTargetSlot2 = 0 end
      if slot == 2 then g_PlayerTargetSlot3 = 0 end
      if slot == 3 then g_PlayerTargetSlot4 = 0 end
	 end
    end
   end
  end
  TargetSlot1 = g_PlayerTargetSlot1
  TargetSlot2 = g_PlayerTargetSlot2
  TargetSlot3 = g_PlayerTargetSlot3
  TargetSlot4 = g_PlayerTargetSlot4
 end
 if output_e['target'] == "hostile" then
  local ee = output_e['targete']
  if ee > 0 then
   if g_Entity[ee] ~= nil then
    for slot = 0, 3, 1 do
     local testslote = 0
     if slot == 0 and g_Entity[ee]['targetslot1'] ~= nil then testslote = g_Entity[ee]['targetslot1'] end
     if slot == 1 and g_Entity[ee]['targetslot2'] ~= nil then testslote = g_Entity[ee]['targetslot2'] end
     if slot == 2 and g_Entity[ee]['targetslot3'] ~= nil then testslote = g_Entity[ee]['targetslot3'] end
     if slot == 3 and g_Entity[ee]['targetslot4'] ~= nil then testslote = g_Entity[ee]['targetslot4'] end
     if testslote > 0 then
      if g_Entity[testslote] ~= nil then
       local dx = g_Entity[ testslote ]['x'] - g_Entity[ e ]['x']
       local dz = g_Entity[ testslote ]['z'] - g_Entity[ e ]['z']
	   local dd = math.sqrt(math.abs(dx*dx),math.abs(dz*dz))
	   if dd > 200 then 
        if slot == 0 then g_Entity[ee]['targetslot1'] = 0 end
        if slot == 1 then g_Entity[ee]['targetslot2'] = 0 end
        if slot == 2 then g_Entity[ee]['targetslot3'] = 0 end
        if slot == 3 then g_Entity[ee]['targetslot4'] = 0 end
	   end
      end
     end
    end
    if g_Entity[ee]['targetslot1'] ~= nil then TargetSlot1 = g_Entity[ee]['targetslot1'] end
    if g_Entity[ee]['targetslot2'] ~= nil then TargetSlot2 = g_Entity[ee]['targetslot2'] end
    if g_Entity[ee]['targetslot3'] ~= nil then TargetSlot3 = g_Entity[ee]['targetslot3'] end
    if g_Entity[ee]['targetslot4'] ~= nil then TargetSlot4 = g_Entity[ee]['targetslot4'] end
   end
  end
 end
 return TargetSlot1, TargetSlot2, TargetSlot3, TargetSlot4
end

function masterinterpreter_settargetslot ( e, output_e, slot )
 if output_e['target'] == "player" then
  if slot == 0 then g_PlayerTargetSlot1 = e end
  if slot == 1 then g_PlayerTargetSlot2 = e end
  if slot == 2 then g_PlayerTargetSlot3 = e end
  if slot == 3 then g_PlayerTargetSlot4 = e end
 end
 if output_e['target'] == "hostile" then
  local ee = output_e['targete']
  if ee > 0 then
   if g_Entity[ee] ~= nil then
    if slot == 0 then g_Entity[ee]['targetslot1'] = e end
    if slot == 1 then g_Entity[ee]['targetslot2'] = e end
    if slot == 2 then g_Entity[ee]['targetslot3'] = e end
    if slot == 3 then g_Entity[ee]['targetslot4'] = e end
   end
  end
 end
end

function masterinterpreter_gettargetalive ( e, output_e )
 local TargetAlive = 0
 if output_e['target'] == "player" then
  if g_PlayerHealth > 0 then 
   TargetAlive = 1 
  end
 end
 if output_e['target'] == "flag" then
  TargetAlive = 0
 end
 if output_e['target'] == "start" then
  TargetAlive = 0
 end
 if output_e['target'] == "hostile" then
  local ee = output_e['targete']
  if ee > 0 then
   if g_Entity[ee]['health'] > 0 then
    TargetAlive = 1
   end
  end
 end
 return TargetAlive
end

function masterinterpreter_resetscan(e)
 -- resets database record when switch targets
 IntersectStaticPerformant(0,0,0,0,0,0,0,e,-1,0)
end

function masterinterpreter_rayscan(e, output_e, fromx,fromy,fromz,tox,toy,toz,ignoreobj,ignoreplayer)
 local hit = 0
 if RayTerrain(fromx,fromy,fromz,tox,toy,toz) ~= 0 then hit = -1 end
 if hit == 0 then
  hit = IntersectStaticPerformant(fromx,fromy,fromz,tox,toy,toz,ignoreobj,0,0,ignoreplayer) -- was e,500,ignoreplayer (messed with plrvisible!
  --hit = IntersectStaticPerformant(fromx,fromy,fromz,tox,toy,toz,ignoreobj,e,500,ignoreplayer) -- faster but not quite right
  local ee = output_e['targete']
  if ee > 0 then
   if hit == g_Entity[ ee ]['obj'] then hit = 0 end
  end
 end
 return hit
end

function masterinterpreter_getconditiontarget ( e, output_e, conditiontype )
 local TargetDistance = 999999
 local TargetVisible = 0
 local usestarget = 0
 if conditiontype == g_masterinterpreter_cond_targetwithin then usestarget = 1 end
 if conditiontype == g_masterinterpreter_cond_targetvisible then usestarget = 1 end
 if conditiontype == g_masterinterpreter_cond_targetbeyond then usestarget = 1 end
 if conditiontype == g_masterinterpreter_cond_targetangle then usestarget = 1 end
 if conditiontype == g_masterinterpreter_cond_targetangletoleft then usestarget = 1 end
 if conditiontype == g_masterinterpreter_cond_targetangletoright then usestarget = 1 end
 if conditiontype == g_masterinterpreter_cond_ifseetarget then usestarget = 1 end
 if conditiontype == g_masterinterpreter_cond_ifseeenemy then usestarget = 2 end
 if conditiontype == g_masterinterpreter_cond_withinattackrange then usestarget = 1 end
 if conditiontype == g_masterinterpreter_cond_targetdestwithin then usestarget = 1 end
 if conditiontype == g_masterinterpreter_cond_targetpathvalid then usestarget = 1 end
 if conditiontype == g_masterinterpreter_cond_targetreachable then usestarget = 1 end
 if conditiontype == g_masterinterpreter_cond_istargetname then usestarget = 1 end
 if usestarget ~= 0 then
  local usetargetXYZ = 0
  if usestarget == 1 then
   -- target specified by output_e['target']
   if output_e['target'] == "flag" or output_e['target'] == "start" then
    usetargetXYZ = 1
   else
    if output_e['target'] == "player" then
     TargetDistance = GetPlayerDistance(e)
     GetEntityPlayerVisibility(e)
     TargetVisible = g_Entity[e]['plrvisible']
    end
    if output_e['target'] == "hostile" then 
 	 usetargetXYZ = 1
    end
   end
  end
  if usestarget == 2 then 
   -- target specified by allegiance (so can have one target but still spot the enemy, see 'g_masterinterpreter_cond_ifseeenemy')
   if GetEntityAllegiance(e) ~= 2 then
    if GetEntityAllegiance(e) == 0 then
     TargetDistance = GetPlayerDistance(e)
     GetEntityPlayerVisibility(e)
	 if TargetDistance <= GetEntityViewRange(e) then
      TargetVisible = g_Entity[e]['plrvisible']
	 end
    end
    if GetEntityAllegiance(e) == 1 then
	 usetargetXYZ = 1
    end  
   end
  end
  if usetargetXYZ == 1 then
   if g_Entity [ e ]['avoid'] >= 1000 then
    -- special case to stop any target detection if fully interupted by path change
    TargetDistance = 0
    TargetVisible = 1
   else  
    local TargetX, TargetY, TargetZ = masterinterpreter_gettargetxyz ( e, output_e )
    if TargetX == 0 and TargetY == 0 and TargetZ == 0 then
     TargetDistance = 999999
     TargetVisible = 0
    else
     TargetDistance = GetDistanceTo(e,TargetX, TargetY, TargetZ)
     local fromx = g_Entity[ e ]['x']
     local fromy = g_Entity[ e ]['y']+60
     local fromz = g_Entity[ e ]['z']
	 local distancetoputplayerposbackwherecameraisY = TargetY + 35
 	 if masterinterpreter_rayscan(e, output_e, fromx, fromy, fromz, TargetX, distancetoputplayerposbackwherecameraisY, TargetZ, g_Entity[e]['obj'], 1 ) == 0 then
      TargetVisible = 1
	 end
    end
   end	
  end
 end
 return TargetDistance, TargetVisible
end

function masterinterpreter_getconditionresult ( e, output_e, conditiontype, conditionparam1 )

 -- Get values from string params
 local conditionparam1value = 0
 local isdlua = string.find(conditionparam1,"=") 
 if isdlua == 1 then
  local dluavarname = string.sub(conditionparam1,2)
  if output_e[dluavarname] ~= nil then
   conditionparam1value = output_e[dluavarname]
  else
   isdlua = 0
  end
 else
  isdlua = 0
 end
 if isdlua == 0 then
  conditionparam1value = tonumber(conditionparam1)
 end
 
 -- condition that needs target XYZ
 local TargetX = 0
 local TargetY = 0
 local TargetZ = 0
 local usestargetxyz = 0
 if conditiontype == g_masterinterpreter_cond_targetangle then usestargetxyz = 1 end
 if conditiontype == g_masterinterpreter_cond_targetangletoleft then usestargetxyz = 1 end
 if conditiontype == g_masterinterpreter_cond_targetangletoright then usestargetxyz = 1 end
 if conditiontype == g_masterinterpreter_cond_flagvalid then usestargetxyz = 1 end
 if usestargetxyz == 1 then
  TargetX, TargetY, TargetZ = masterinterpreter_gettargetxyz ( e, output_e )
 end

 -- Provide ancilliary data
 local TargetDistance, TargetVisible = masterinterpreter_getconditiontarget ( e, output_e, conditiontype )
 
 -- Check if condition is true
 if conditiontype == g_masterinterpreter_cond_always then return 1 end
 if conditiontype == g_masterinterpreter_cond_targetwithin then
  if conditionparam1value == nil then conditionparam1value = GetEntityViewRange(e) end
  if TargetDistance <= conditionparam1value then return 1 end
 end
 if conditiontype == g_masterinterpreter_cond_random and conditionparam1value ~= nil and math.random(0,conditionparam1value) == 0 then return 1 end
 if conditiontype == g_masterinterpreter_cond_targetvisible then
  if conditionparam1value == nil then conditionparam1value = 1 end
  if TargetVisible == conditionparam1value then return 1 end
 end
 if conditiontype == g_masterinterpreter_cond_endofanim then
  if conditionparam1value == nil then conditionparam1value = 0 end
  if GetObjectAnimationFinished(e,math.abs(conditionparam1value)) == 1 then 
   SetEntityPathRotationMode(e,1) -- end of animation always forces object to rotate to follow path again (used on a per play anim basis)
   return 1 
  end
 end
 if conditiontype == g_masterinterpreter_cond_gothurt and g_Entity[e]['health'] < output_e['oldhealth'] then
  output_e['oldhealth'] = g_Entity[e]['health']
  return 1 
 end
 if conditiontype == g_masterinterpreter_cond_checklimbhit then
  if string.match(string.lower(g_Entity[e]['limbhit']), string.lower(conditionparam1)) then
   return 1 
  end
 end
 if conditiontype == g_masterinterpreter_cond_checkdamage and conditionparam1value ~= nil and output_e['damagetaken'] > conditionparam1value then return 1 end
 if conditiontype == g_masterinterpreter_cond_checkhealth and conditionparam1value ~= nil and g_Entity[e]['health'] < conditionparam1value then return 1 end
 if conditiontype == g_masterinterpreter_cond_flagvalid and output_e['targete'] ~= -1 then 
  local tdx = g_Entity[ e ]['x'] - TargetX
  local tdy = g_Entity[ e ]['y'] - TargetY
  local tdz = g_Entity[ e ]['z'] - TargetZ
  local tdd = math.sqrt(math.abs(tdx*tdx)+math.abs(tdy*tdy)+math.abs(tdz*tdz))
  if tdd >= 10.0 then
   return 1
  end
 end
 if conditiontype == g_masterinterpreter_cond_targetbeyond then
  if conditionparam1value == nil then conditionparam1value = GetEntityViewRange(e) end
  if TargetDistance > conditionparam1value then return 1 end
 end
 if conditiontype == g_masterinterpreter_cond_checktimer and conditionparam1value ~= nil then
  if GetTimer(e) >= conditionparam1value then
   if output_e['lasttimerreached'] < conditionparam1value then
    output_e['lasttimerreached'] = conditionparam1value
    return 1
   end
  end
 end
 if conditiontype == g_masterinterpreter_cond_canfireweapon then 
  if g_Entity[ e ]['active'] == 0 then return 0 end
  local haveammotofire = 1
  if output_e['ammo'] == -1 then
   local weaponid = GetEntityHasWeapon(e)
   if weaponid > 0 then
    local reloadquantity = GetWeaponReloadQuantity(weaponid,0)
    output_e['ammo'] = math.random(0,reloadquantity)
   end   
  end
  if output_e['ammo'] == 0 then
   haveammotofire = 0
  end   
  if output_e['target'] == "player" then
   if GetEntityCanFire(e) == 1 then 
    return haveammotofire
   end
  end
  if output_e['target'] == "flag" then
   return haveammotofire
  end
  if output_e['target'] == "hostile" then
   return haveammotofire
  end
 end
 if conditiontype == g_masterinterpreter_cond_targetangle or conditiontype == g_masterinterpreter_cond_targetangletoleft or conditiontype == g_masterinterpreter_cond_targetangletoright then 
  local validtarget = 0
  if output_e['target'] == "player" or output_e['target'] == "flag" or output_e['target'] == "start" then validtarget = 1 end
  if output_e['target'] == "hostile" and output_e['targete'] > 0 then validtarget = 1 end
  if validtarget == 1 then
   if conditiontype == g_masterinterpreter_cond_targetangletoleft or conditiontype == g_masterinterpreter_cond_targetangletoright then
    if conditionparam1value == nil then conditionparam1value = 0 end
   else
    if conditionparam1value == nil then conditionparam1value = 45 end
   end
   if conditionparam1value ~= nil then 
    local diff = 0
    local tdx = TargetX - g_Entity[ e ]['x']
    local tdz = TargetZ - g_Entity[ e ]['z']
    local tda = (math.atan2(tdx,tdz)/6.28)*360.0
    if tda < -180 then tda=tda+360 end
    if tda >  180 then tda=tda-360 end
    local nowangle = g_Entity[e]['angley']
    if nowangle < -180 then nowangle=nowangle+360 end
    if nowangle >  180 then nowangle=nowangle-360 end
    diff = tda - nowangle
    if diff < -180 then diff=diff+360 end
    if diff >  180 then diff=diff-360 end
    if conditiontype == g_masterinterpreter_cond_targetangle then
     if math.abs(diff) <= conditionparam1value then
      return 1
     end
    end
    if conditiontype == g_masterinterpreter_cond_targetangletoleft then
     if diff < (conditionparam1value*-1) then
      return 1
     end
    end
    if conditiontype == g_masterinterpreter_cond_targetangletoright then
     if diff > conditionparam1value then
      return 1
     end
    end
   end
  end
 end
 if conditiontype == g_masterinterpreter_cond_speechplaying and GetSpeech(e) == 1 then return 1 end
 if conditiontype == g_masterinterpreter_cond_targetpathvalid then
  if output_e['goodtargetpointindex'] > 0 then 
   return 1
  end
 end
 if conditiontype == g_masterinterpreter_cond_targetpathcompleted then
  if conditionparam1value == nil then conditionparam1value = 10 end
  masterinterpreter_followtarget ( e, output_e, conditionparam1value )
  if output_e['goodtargetpointindex'] == 0 then 
   return 1 
  end
 end
 if conditiontype == g_masterinterpreter_cond_isvaluezero and conditionparam1value ~= nil then
  if conditionparam1value == 0 then return 1 end
 end
 if conditiontype == g_masterinterpreter_cond_isactivated and g_Entity[e]['activated'] ~= 0 then 
  if output_e['wholastactivated'] <= 0 then
   output_e['wholastactivated'] = GetEntityWhoActivated(e)
   return 1
  end
 end
 if conditiontype == g_masterinterpreter_cond_ifseeenemy then
  if g_Entity[ e ]['active'] == 0 then return 0 end
  if conditionparam1value == nil then conditionparam1value = GetEntityViewRange(e) end
  if TargetDistance <= conditionparam1value then
   local allegiance = GetEntityAllegiance(e)
   if allegiance == 0 then
    -- this is an enemy (hates the player)
    GetEntityPlayerVisibility(e)
    return g_Entity[e]['plrvisible']
   end  
   if allegiance == 1 then
    -- this is an ally (friends with player)
	local anywilldo = 1
	local resultofthis = masterinterpreter_scanforenemy (e, output_e, anywilldo)
	if resultofthis > 0 then return 1 end
   end
  end
 end
 if conditiontype == g_masterinterpreter_cond_ifseetarget then 
  if g_Entity[ e ]['active'] == 0 then return 0 end
  -- locally record result from XYZ to XYZ, and refer to that instead of expensive raycast (grid of say 10x10x10)
  local TargetX, TargetY, TargetZ = masterinterpreter_gettargetxyz ( e, output_e )
  local fromx = g_Entity[ e ]['x']
  local fromy = g_Entity[ e ]['y']+60
  local fromz = g_Entity[ e ]['z']
  local hit = masterinterpreter_rayscan(e, output_e, fromx,fromy,fromz,TargetX,TargetY+60,TargetZ,g_Entity[e]['obj'], 1)
  if hit == 0 then return 1 end
 end
 if conditiontype == g_masterinterpreter_cond_ifally then 
  local allegiance = GetEntityAllegiance(e)
  if allegiance == 1 then return 1 end
 end 
 if conditiontype == g_masterinterpreter_cond_isvalueone and conditionparam1value ~= nil then
  if conditionparam1value == 1 then 
   return 1 
  end
 end
 if conditiontype == g_masterinterpreter_cond_isvaluetwo and conditionparam1value ~= nil then
  if conditionparam1value == 2 then return 1 end
 end
 if conditiontype == g_masterinterpreter_cond_isvaluethree and conditionparam1value ~= nil then
  if conditionparam1value == 3 then return 1 end
 end
 if conditiontype == g_masterinterpreter_cond_isvaluefour and conditionparam1value ~= nil then
  if conditionparam1value == 4 then return 1 end
 end
 if conditiontype == g_masterinterpreter_cond_isvaluefive and conditionparam1value ~= nil then
  if conditionparam1value == 5 then return 1 end
 end
 if conditiontype == g_masterinterpreter_cond_istargetdead then
  if output_e['target'] == "player" then
  end
  if output_e['target'] == "flag" then
  end
  if output_e['target'] == "hostile" then
   if output_e['targete'] > 0 then
    ee = output_e['targete']
    if g_Entity[ee]['health'] <= 0 or g_Entity[ee]['active'] == 0 then
	 return 1
	end
   end
  end
 end 
 if conditiontype == g_masterinterpreter_cond_hasweapon and GetEntityHasWeapon(e) ~= 0 then return 1 end
 if conditiontype == g_masterinterpreter_cond_isunderwater then 
  if conditionparam1value == nil then conditionparam1value = 0 end
  if g_Entity[ e ]['y'] + conditionparam1value < GetGamePlayerStateWaterlineY() then return 1 end
 end
 if conditiontype == g_masterinterpreter_cond_animationexists then
  if GetEntityAnimationNameExist(e,conditionparam1) > 0 then return 1 end
 end
 if conditiontype == g_masterinterpreter_cond_ifpushedback then
  if conditionparam1value == nil then conditionparam1value = 0 end
  if g_Entity [ e ]['avoid'] >= conditionparam1value then return 1 end
 end
 if conditiontype == g_masterinterpreter_cond_checkgoaltimer then
  if conditionparam1value == nil then conditionparam1value = 3000 end
  if Timer() > output_e['goalstarttime'] + conditionparam1value then return 1 end
 end
 if conditiontype == g_masterinterpreter_cond_isplayerfrozen then
  if g_masterinterpreter_playerfrozenby > 0 then return 1 end
 end 
 if conditiontype == g_masterinterpreter_cond_heardanysound or conditiontype == g_masterinterpreter_cond_heardenemysound then
  if conditionparam1value == nil then conditionparam1value = 2500 end
  local tcategory = 0
  local allegiance = 0
  if conditiontype == g_masterinterpreter_cond_heardenemysound then
   allegiance = GetEntityAllegiance(e)
   if allegiance == 0 then
    -- this is an enemy, listen for player noises
    tcategory = 1
   end  
   if allegiance == 1 then
    -- this is an ally, listen for non-player noises
    tcategory = 2
   end  
  end
  tclosestsound, whoe = GetNearestSoundDistance(g_Entity[ e ]['x'],g_Entity[ e ]['y'],g_Entity[ e ]['z'],tcategory)
  if tclosestsound > 0 and tclosestsound < conditionparam1value then 
   if allegiance == 0 or allegiance == 1 then
    if whoe <= 0 then
	 if whoe == -1 then
      output_e['wholastactivated'] = 0 --player made this sound
	 else
      output_e['wholastactivated'] = -1
	 end
    else
     output_e['wholastactivated'] = whoe
    end
   end
   if allegiance == 2 then
    if whoe > 0 then
     output_e['wholastactivated'] = whoe
    end
   end
   return 1 
  end
 end 
 if conditiontype == g_masterinterpreter_cond_ifneutral then 
  local allegiance = GetEntityAllegiance(e)
  if allegiance == 2 then return 1 end
 end 
 if conditiontype == g_masterinterpreter_cond_animationplaying then
  if GetEntityAnimationNameExistAndPlaying(e,conditionparam1) == 1 then return 1 end
 end
 if conditiontype == g_masterinterpreter_cond_istargetalive then
  local talive = masterinterpreter_gettargetalive ( e, output_e )
  if talive == 1 then return 1 end
 end
 if conditiontype == g_masterinterpreter_cond_reachtriggerone or conditiontype == g_masterinterpreter_cond_reachtriggertwo or conditiontype == g_masterinterpreter_cond_reachtriggerthree then
  local ttriggerindex = 0
  if conditionparam1value == nil then conditionparam1value = 0 end
  if conditiontype == g_masterinterpreter_cond_reachtriggerone and output_e['lastanimtriggerindex'] ~= 1 then ttriggerindex = 1 end
  if conditiontype == g_masterinterpreter_cond_reachtriggertwo and output_e['lastanimtriggerindex'] ~= 2 then ttriggerindex = 2 end
  if conditiontype == g_masterinterpreter_cond_reachtriggerthree and output_e['lastanimtriggerindex'] ~= 3 then ttriggerindex = 3 end
  if ttriggerindex > 0 then
   if output_e['target'] == "player" then conditionparam1value = 0 end
   local ttriggerframe = GetEntityAnimationTriggerFrame(e,ttriggerindex) - conditionparam1value
   local tobj = g_Entity[ e ]['obj']
   local tthisframe = GetObjectFrame(tobj)
   if tthisframe >= ttriggerframe then
    output_e['lastanimtriggerindex'] = ttriggerindex
    return 1 
   end
  end
 end
 if conditiontype == g_masterinterpreter_cond_pathdistancebeyond then
  if conditionparam1value == nil then conditionparam1value = 0 end
  if output_e['lastgoodtargetdistance'] > conditionparam1value then return 1 end
 end
 if conditiontype == g_masterinterpreter_cond_withinattackrange then
  if conditionparam1value == nil then conditionparam1value = 0 end
  local weaponid = GetEntityHasWeapon(e)
  local attackrange = GetWeaponRange(weaponid,0)
  if attackrange < 50 then attackrange = 50 end
  if TargetDistance <= attackrange + 10 + conditionparam1value then
   return 1
  end
 end
 if conditiontype == g_masterinterpreter_cond_targetdestwithin then
  local tdist = 99999
  if output_e['goodtargetdestinationx'] ~= -1 then 
   TargetX, TargetY, TargetZ = masterinterpreter_gettargetxyz ( e, output_e )
   local dx = TargetX - output_e['goodtargetdestinationx']
   local dy = TargetY - output_e['goodtargetdestinationy']
   local dz = TargetZ - output_e['goodtargetdestinationz']
   tdist = math.sqrt(math.abs(dx*dx)+math.abs(dz*dz))
  end
  if conditionparam1value == nil then conditionparam1value = 50 end
  if tdist <= conditionparam1value then return 1 end
 end
 if conditiontype == g_masterinterpreter_cond_checktimerwithfirerate then
  -- typically firerate of 5 for automatic rifle
  if conditionparam1value == nil then 
   local weaponid = GetEntityHasWeapon(e)
   local firerate = GetWeaponFireRate(weaponid,0)
   -- adjust firerate to millisecond delay equiv to player
   if firerate >= 12 then
    -- 12 = favor slower standard non-auto weapons
    firerate = firerate * 10
   else
    -- 5 = favor faster standard automatic weapons
    firerate = firerate * 5
   end
   conditionparam1value = firerate 
  end
  if GetTimer(e) >= conditionparam1value then
   return 1
  end
 end
 if conditiontype == g_masterinterpreter_cond_haveammo then 
  local haveammotofire = 1
  if output_e['ammo'] == 0 then
   haveammotofire = 0
  end
  return haveammotofire
 end
 if conditiontype == g_masterinterpreter_cond_havealerttarget then 
  if output_e['wholastactivated'] >= 0 then
   return 1
  end
 end 
 if conditiontype == g_masterinterpreter_cond_keypressed then 
  if g_KeyPressE == 1 then
   return 1
  end
 end 
 if conditiontype == g_masterinterpreter_cond_usinghud then 
  if GetCurrentScreen() > -1 then
   return 1
  end
 end 
 if conditiontype == g_masterinterpreter_cond_targetreachable then 
  if output_e['goodtargetpointindex'] > 0 then 
   local tdist = 99999
   if output_e['goodtargetdestinationx'] ~= -1 then 
    local dx = output_e['goodtargetdestinationx'] - g_PlayerPosX
    local dz = output_e['goodtargetdestinationz'] - g_PlayerPosZ
    tdist = math.sqrt(math.abs(dx*dx)+math.abs(dz*dz))
   end
   if conditionparam1value == nil then conditionparam1value = 50 end
   if tdist <= conditionparam1value then return 1 end
  end
 end  
 if conditiontype == g_masterinterpreter_cond_withinnavmesh then 
  if RDIsWithinMesh(g_Entity[ e ]['x'],g_Entity[ e ]['y'],g_Entity[ e ]['z']) == 1 then return 1 end
 end
 if conditiontype == g_masterinterpreter_cond_canmeleetarget then 
  local TargetX, TargetY, TargetZ = masterinterpreter_gettargetxyz ( e, output_e )
  local waistheightforcharacter = 30
  local fromx = g_Entity[ e ]['x']
  local fromy = g_Entity[ e ]['y']+waistheightforcharacter
  local fromz = g_Entity[ e ]['z']
  local hit = masterinterpreter_rayscan(e, output_e, fromx,fromy,fromz,TargetX,TargetY+waistheightforcharacter,TargetZ,g_Entity[e]['obj'], 1)
  if hit == 0 then return 1 end
 end
 if conditiontype == g_masterinterpreter_cond_istargetname then 
  if conditionparam1 ~= nil then
   if conditionparam1 == GetEntityName(output_e['targete']) then return 1 end
  end
 end
 
 -- Condition is false
 return 0

end

function masterinterpreter_getactiontarget ( e, output_e, actiontype )
 local TargetX = 0
 local TargetY = 0
 local TargetZ = 0
 local usestarget = 0
 if actiontype == g_masterinterpreter_act_checktarget then usestarget = 1 end
 if actiontype == g_masterinterpreter_act_lookat then usestarget = 1 end
 if actiontype == g_masterinterpreter_act_facetarget then usestarget = 1 end
 if actiontype == g_masterinterpreter_act_hurttarget then usestarget = 1 end
 if actiontype == g_masterinterpreter_act_freezetarget then usestarget = 1 end
 if actiontype == g_masterinterpreter_act_unfreezetarget then usestarget = 1 end
 if actiontype == g_masterinterpreter_act_grabtargetview then usestarget = 1 end
 if actiontype == g_masterinterpreter_act_gototarget then usestarget = 1 end
 if actiontype == g_masterinterpreter_act_gototargetflank then usestarget = 1 end
 if actiontype == g_masterinterpreter_act_goawayfromtarget then usestarget = 1 end
 if actiontype == g_masterinterpreter_act_gotostepleft then usestarget = 1 end
 if actiontype == g_masterinterpreter_act_gotostepright then usestarget = 1 end 
 if actiontype == g_masterinterpreter_act_gotostepforward then usestarget = 1 end 
 if actiontype == g_masterinterpreter_act_gotoneartarget then usestarget = 1 end 
 if actiontype == g_masterinterpreter_act_gotostepbackward then usestarget = 1 end 
 if actiontype == g_masterinterpreter_act_gotonearbyspot then usestarget = 1 end 
 if actiontype == g_masterinterpreter_act_gotoattackspot then usestarget = 1 end 
 if usestarget == 1 then
  TargetX, TargetY, TargetZ = masterinterpreter_gettargetxyz ( e, output_e )
 end
 return TargetX, TargetY, TargetZ
end

function masterinterpreter_setnewtarget ( e, output_e, x, y, z, movingbackwards )

 -- no path initially
 output_e['goodtargetpointindex'] = 0
 -- first work out a path
 RDFindPath(g_Entity[ e ]['x'],g_Entity[ e ]['y'],g_Entity[ e ]['z'],x,y,z)
 local pointcount = RDGetPathPointCount()
 if pointcount > 0 then 
  -- first check the starting point matches entity position, RDFindPath can find 'nearest' start which is no good
  local firstpointx = RDGetPathPointX(0)
  local firstpointy = RDGetPathPointY(0)
  local firstpointz = RDGetPathPointZ(0)
  local tdx = firstpointx - g_Entity[ e ]['x']
  local tdz = firstpointz - g_Entity[ e ]['z']
  local tdd = math.sqrt(math.abs(tdx*tdx)+math.abs(tdz*tdz))
  if tdd > 20.0 then
   -- reject this path, we cannot get to the start of it!
   output_e['lastgoodtargetdistance'] = 0
  else
   -- last point is dest
   local lastpointx = RDGetPathPointX(pointcount-1)
   local lastpointy = RDGetPathPointY(pointcount-1)
   local lastpointz = RDGetPathPointZ(pointcount-1)
   -- no path if target very close to entity already
   local dx = lastpointx - g_Entity[ e ]['x']
   local dz = lastpointz - g_Entity[ e ]['z']
   local dd = math.sqrt(math.abs(dx*dx)+math.abs(dz*dz))
   -- initial distance to dest recorded (updated in masterinterpreter_followtarget)
   output_e['lastgoodtargetdistance'] = dd
   if dd > 10.0 then
    output_e['goodtargetdestinationx'] = lastpointx
    output_e['goodtargetdestinationy'] = lastpointy
    output_e['goodtargetdestinationz'] = lastpointz
    RDFindPath(g_Entity[ e ]['x'],g_Entity[ e ]['y'],g_Entity[ e ]['z'],x,y,z)
    output_e['goodtargetpointcount'] = pointcount
    if output_e['goodtargetpointcount'] == 0 then
     -- target high or low, try at the same Y as the character
     RDFindPath(g_Entity[ e ]['x'],g_Entity[ e ]['y'],g_Entity[ e ]['z'],x,g_Entity[ e ]['y'],z)
     output_e['goodtargetpointcount'] = pointcount
    end
    if output_e['goodtargetpointcount'] > 0 then
	 if movingbackwards == 1 then 
      StartMoveAndRotateToXYZ(e,(GetEntityMoveSpeed(e)/100.0)*-1,GetEntityTurnSpeed(e)/4.0) 
	 else
      StartMoveAndRotateToXYZ(e,GetEntityMoveSpeed(e)/100.0,GetEntityTurnSpeed(e)/4.0) 
	 end
     output_e['goodtargetpointindex'] = 1
    else
     output_e['goodtargetpointindex'] = 0
    end
   else
    output_e['goodtargetdestinationx'] = -1
    output_e['goodtargetdestinationy'] = -1
    output_e['goodtargetdestinationz'] = -1
   end
  end
 end
end

function masterinterpreter_followtarget ( e, output_e, stopfromend )
 if output_e['goodtargetpointindex'] > 0 then
  if output_e['movingmode'] ~= nil then 
   output_e['goodtargetpointindex'],output_e['lastgoodtargetdistance'] = MoveAndRotateToXYZ(e,GetEntityMoveSpeed(e)/100.0,GetEntityTurnSpeed(e)/4.0,stopfromend) 
   if output_e['lastgoodtargetdistance'] == 0 then
    local dx = output_e['goodtargetdestinationx'] - g_Entity[ e ]['x']
    local dz = output_e['goodtargetdestinationz'] - g_Entity[ e ]['z']
    local dd = math.sqrt(math.abs(dx*dx)+math.abs(dz*dz))
    output_e['lastgoodtargetdistance'] = dd
   end
   if output_e['goodtargetpointindex'] > 0 then
    if g_Entity [ e ]['avoid'] >= 500 then 
	 -- path is blocked, no progress being made here - make here the final destination
	 masterinterpreter_setnewtarget ( e, output_e, g_Entity[ e ]['x'],g_Entity[ e ]['y'],g_Entity[ e ]['z'], 0 )
	end
   end
  end
 end  
end

function masterinterpreter_doaction ( e, output_e, actiontype, actionparam1, actionparam2 )

 -- Get values from actionparam1 string param
 local actionparam1value = 0
 local isdlua = string.find(actionparam1,"=") 
 if isdlua == 1 then
  local dluavarname = string.sub(actionparam1,2)
  if output_e[dluavarname] ~= nil then
   actionparam1value = output_e[dluavarname]
  else
   isdlua = 0
  end
 else
  isdlua = 0
 end
 if isdlua == 0 then
  actionparam1value = tonumber(actionparam1)
 end
 local actionparam2value = tonumber(actionparam2)
 
 -- Provide ancilliary data
 local TargetX, TargetY, TargetZ = masterinterpreter_getactiontarget ( e, output_e, actiontype )

 -- Play Animation or Play Animation Random or Play By Name
 if actiontype == g_masterinterpreter_act_play or actiontype == g_masterinterpreter_act_playrandom or actiontype == g_masterinterpreter_act_playbyname then
  output_e['lastanimplayed'] = actionparam1
  output_e['lastanimtriggerindex'] = 0
  if actionparam2 ~= nil and string.len(actionparam2) > 0 then
   if math.random(0,1) == 0 then
    tusethisanim = actionparam1
   else
    tusethisanim = actionparam2
   end
  else
   if actiontype == g_masterinterpreter_act_playbyname then
    if isdlua == 1 then
	 tusethisanim = "=" .. tostring(actionparam1value)
	else
	 tusethisanim = actionparam1
	end
   else
    tusethisanim = actionparam1
   end
  end
  SetAnimationName(e,tusethisanim)
  local tsubtlevariationforrealism = math.random(100)/10.0
  SetAnimationSpeed(e,(GetEntityMoveSpeed(e)+tsubtlevariationforrealism)/100.0)
  if actiontype == g_masterinterpreter_act_playrandom then
   PlayAnimationFrom(e,math.random(5,50))
  else
   PlayAnimation(e)
  end
 end
 
 -- Loop Animation or Loop Animation Random or Loop By Name
 if actiontype == g_masterinterpreter_act_loop or actiontype == g_masterinterpreter_act_looprandom or actiontype == g_masterinterpreter_act_loopbyname then
  if output_e['lastanimplayed'] ~= actionparam1 then
   output_e['lastanimplayed'] = actionparam1
   output_e['lastanimtriggerindex'] = 0
   if actionparam2 ~= nil and string.len(actionparam2) > 0 then
    if math.random(0,1) == 0 then
     tusethisanim = actionparam1
    else
     tusethisanim = actionparam2
    end
   else
    if actiontype == g_masterinterpreter_act_loopbyname then
     if isdlua == 1 then
	  tusethisanim = "=" .. tostring(actionparam1value)
	 else
	  tusethisanim = actionparam1
	 end
	else
     tusethisanim = actionparam1
	end
   end
   SetAnimationName(e,tusethisanim)
   local tsubtlevariationforrealism = math.random(100)/10.0
   SetAnimationSpeed(e,(GetEntityMoveSpeed(e)+tsubtlevariationforrealism)/100.0)
   if actiontype == g_masterinterpreter_act_looprandom then
    LoopAnimationFrom(e,math.random(5,50))
   else
    LoopAnimation(e)
   end
   SetEntityPathRotationMode(e,90) -- restore object path direction rotation
  end
 end

 -- Trigger a check of the target visibility
 if actiontype == g_masterinterpreter_act_checktarget then
  if output_e['target'] == "player" then
   GetEntityPlayerVisibility(e)
  end
  if output_e['target'] == "flag" then
   -- hmm
  end
  if output_e['target'] == "hostile" then
   -- hmm
  end
 end
 
 -- Look At Target
 if actiontype == g_masterinterpreter_act_lookat then
  if actionparam1value == nil then actionparam1value = 10 end
  if actionparam1value == 0 then actionparam1value = 10 end
  if output_e['target'] == "player" then
   LookAtPlayer(e,actionparam1value)
  end
  if output_e['target'] == "flag" or output_e['target'] == "hostile" then
   LookAtTarget(e,actionparam1value,output_e['targete'])
  end
 end
 
 -- Face The Target
 if actiontype == g_masterinterpreter_act_facetarget then
  if actionparam1value == nil then actionparam1value = GetEntityTurnSpeed(e)/4.0 end
  local tdx = TargetX - g_Entity[ e ]['x']
  local tdz = TargetZ - g_Entity[ e ]['z']
  local tda = (math.atan2(tdx,tdz)/6.28)*360.0
  if tda < -180 then tda=tda+360 end
  if tda >  180 then tda=tda-360 end
  --SetEntityPathRotationMode(e,90) -- restore object path direction rotation
  SetRotationYSlowly(e,tda,actionparam1value)
 end
 
 -- Hurt Target
 if actiontype == g_masterinterpreter_act_hurttarget or actiontype == g_masterinterpreter_act_hurttargetwithshake then
  if actionparam1value == nil then actionparam1value = 1 actionparam1value = 10 end
  if actionparam2value == nil then actionparam2value = actionparam1value + 10 end
  local tdamagetodeal = math.random(actionparam1value,actionparam2value)
  if output_e['target'] == "player" then
   if actiontype == g_masterinterpreter_act_hurttargetwithshake then
    HurtPlayer(0,tdamagetodeal)
   else
    HurtPlayer(e,tdamagetodeal)
   end
  end
  if output_e['target'] == "flag" then
   -- cannot hurt flags
  end
  if output_e['target'] == "hostile" then
   -- hurt this entity
   local tothere = output_e['targete']
   if tothere > 0 then
    SetEntityHealthWithDamage(tothere,g_Entity[ tothere ]['health']-tdamagetodeal)
   end
  end
 end
 
 -- Start Timer
 if actiontype == g_masterinterpreter_act_starttimer then
  output_e['lasttimerreached'] = 0
  StartTimer(e)
 end
 
 -- Freeze and UnFreeze Target (and ensure Target looking at e doing the attacking)
 if actiontype == g_masterinterpreter_act_freezetarget then
  if output_e['target'] == "player" then
   if g_masterinterpreter_playerfrozenby == 0 then
    FreezePlayer()
    SetCameraOverride(2)
	g_masterinterpreter_playerweaponid = g_PlayerGunID
    ChangePlayerWeaponID(0)
    output_e['frozentheplayer'] = 1
	g_masterinterpreter_playerfrozenby = e
   end
  end
  if output_e['target'] == "flag" then
   -- cannot freeze flags
  end
  if output_e['target'] == "hostile" then
   -- can freeze flags
  end
 end
 if actiontype == g_masterinterpreter_act_unfreezetarget then
  if output_e['target'] == "player" then
   if g_masterinterpreter_playerfrozenby > 0 then
    SetCameraOverride(0)
    UnFreezePlayer()
	ChangePlayerWeaponID(g_masterinterpreter_playerweaponid)
    output_e['frozentheplayer'] = 0
	g_masterinterpreter_playerfrozenby = 0
   end
  end
  if output_e['target'] == "flag" then
   -- cannot freeze flags
  end
  if output_e['target'] == "hostile" then
   -- can freeze flags
  end
 end
 
 -- Grab Target View (would be good to find a way not to need frozentheplayer)
 if actiontype == g_masterinterpreter_act_grabtargetview then
  if actionparam1value == nil then actionparam1value = 25 end
  if output_e['frozentheplayer'] == 1 then
   if e == g_masterinterpreter_playerfrozenby then
    local tdx = g_Entity[ e ]['x'] - TargetX
    local tdz = g_Entity[ e ]['z'] - TargetZ
    local tda = (math.atan2(tdx,tdz)/6.28)*360.0
    if tda < -180 then tda=tda+360 end
    if tda >  180 then tda=tda-360 end
    local armsLengthX = g_Entity[ e ]['x'] - (math.sin(math.rad(tda))*actionparam1value)
    local armsLengthZ = g_Entity[ e ]['z'] - (math.cos(math.rad(tda))*actionparam1value) 
    SetCameraAngle(0,25.0,tda,0)
    SetCameraPosition(0, armsLengthX, g_Entity[ e ]['y']+60, armsLengthZ)
   else
    output_e['frozentheplayer'] = 0
   end
  end
 end
 
 -- Stop Animation
 if actiontype == g_masterinterpreter_act_stop then
  output_e['lastanimplayed'] = ""
  StopAnimation(e)
 end
 
 -- Go To Target
 if actiontype == g_masterinterpreter_act_gototarget then
  if g_Time > output_e['recalctimer'] + 500 then
   output_e['recalctimer'] = g_Time
   masterinterpreter_setnewtarget ( e, output_e, TargetX, TargetY, TargetZ, 0 )
  end   
  if output_e['goodtargetpointindex'] > 0 then
   masterinterpreter_followtarget ( e, output_e, -1 )
  end  
 end

 -- Set Target Enemy
 if actiontype == g_masterinterpreter_act_settargetenemy then
  -- reset any stored ray result for fresh target
  masterinterpreter_resetscan(e)
  -- determine target based on allegiance
  local allegiance = GetEntityAllegiance(e)
  if allegiance == 0 then
   -- enemy (hates the player) 
   output_e['targete'] = 0
   output_e['recalctimer'] = 0
   masterinterpreter_findnewenemytarget ( e, output_e )
   if output_e['targete'] > 0 and math.random(0,1) > 0 then
    -- enemies prefer allies in range half the time
    output_e['target'] = "hostile"
   else
    --for when hurt by sniping player, can still target the nasty ol' player
    --if g_Entity[e]['plrvisible'] == 1 then
     output_e['target'] = "player"
	--end
   end
  end  
  if allegiance == 1 then
   -- ally (friends with player)
   output_e['target'] = "hostile"
   masterinterpreter_findnewenemytarget ( e, output_e )
  end
  if allegiance == 2 then
   -- neutral (no target)
   output_e['target'] = ""
   output_e['targete'] = 0
  end
 end

 -- Set Target Flag
 if actiontype == g_masterinterpreter_act_settargetflag then
  -- reset any stored ray result for fresh target
  masterinterpreter_resetscan(e)
  -- target is flag
  output_e['target'] = "flag"
  output_e['recalctimer'] = 0
  if output_e['targete'] <= 0 then
   -- if no flag as yet, find a connected one or if none, nearest one
   if actionparam1value == nil then actionparam1value = 500 end
   local havepathe = 0
   for i = 0 , 9, 1 do
	local othere = GetEntityRelationshipID(e,i)
	if othere > 0 then
     if GetEntityMarkerMode(othere) == 11 then
	  havepathe = othere
	  break
	 end
	end
   end
   if havepathe > 0 then
	output_e['targete'] = havepathe
   else
    local closestdist = actionparam1value
    for ee = 1, g_EntityElementMax, 1 do
     if g_Entity[ee] ~= nil then
      if GetEntityMarkerMode(ee) == 11 then
	   local thisdistance = GetDistanceTo(e,g_Entity[ee]['x'],g_Entity[ee]['y'],g_Entity[ee]['z'])
	   if thisdistance < closestdist then 
	    closestdist = thisdistance 
		havepathe = ee
	   end
	  end
	 end
    end
    if havepathe > 0 then
	 output_e['targete'] = havepathe
	else
	 output_e['targete'] = -1 --no more flags!
    end
   end
   output_e['targetinitiale'] = output_e['targete']
   output_e['targetpreviouse'] = output_e['targete']
  else
   -- if have set a flag, we can proceed to navigate through path of flags based on patrol mode
   local pathpatrolmode = GetEntityPatrolMode(e)
   local currentflage = output_e['targete']
   local previousflage = output_e['targetpreviouse']
   local havepathe = 0
   local havenextbestpathe = 0
   if pathpatrolmode == 0 or pathpatrolmode == 1 or pathpatrolmode == 2 then 
    -- One Way or Looping or Bouncing
	local pointcount = 0
    for i = 0 , 9, 1 do
     local othere = GetEntityRelationshipID(currentflage,i)
     if othere > 0 then
      if GetEntityMarkerMode(othere) == 11 then
	   pointcount = pointcount + 1
	   if othere ~= currentflage then
 	    if othere ~= previousflage then 
	     -- if found a connected flag that is not the current or previous one, we will have it
         if havepathe == 0 then havepathe = othere end
		else
	     -- if found a connected flag that is not the current but is previous one, we will have it as a second choice if havepathe ends up zero
         if havenextbestpathe == 0 then havenextbestpathe = othere end
		end
       end	   	   
	  end
	 end
    end
	if pointcount == 2 then
 	 if havepathe == 0 and havenextbestpathe ~= 0 then havepathe = havenextbestpathe end
	end
   else
    -- Purely Random Choice
	local flagchoices = {}
	local flagchoicescount = 0	
    for i = 0 , 9, 1 do
     local othere = GetEntityRelationshipID(currentflage,i)
     if othere > 0 then
      if GetEntityMarkerMode(othere) == 11 then
	   flagchoices[flagchoicescount] = othere
	   flagchoicescount=flagchoicescount+1
	  end
	 end
    end
	for iter = 0, 20, 1 do
     for c = 0, flagchoicescount-1, 1 do
      local eachonee = flagchoices[c]
      for ii = 0 , 9, 1 do
       local otheree = GetEntityRelationshipID(eachonee,ii)
       if otheree > 0 then
        if GetEntityMarkerMode(otheree) == 11 then
		 local isunique = 1
         for testifunique = 0, flagchoicescount-1, 1 do
		  if flagchoices[testifunique] == otheree then isunique = 0 end
		 end
		 if isunique == 1 then
 	      flagchoices[flagchoicescount] = otheree
 	      flagchoicescount=flagchoicescount+1
		 end
	    end
 	   end
      end
     end
	end
	if flagchoicescount > 0 then
 	 havepathe = flagchoices[math.random(0,flagchoicescount-1)]
	end
   end 
   local usethispreviouse = output_e['targetpreviouse']
   output_e['targetpreviouse'] = output_e['targete']
   if havepathe > 0 then
    output_e['targete'] = havepathe
   else
    if pathpatrolmode == 0 then
	 -- go back the way you came, if more than one flag!
	 if output_e['targete'] ~= usethispreviouse then
      output_e['targete'] = usethispreviouse
	 else
	  output_e['targete'] = -1 
	 end
	end
    if pathpatrolmode == 1 then
	 -- go to initial flag specified, if other than current one
	 if output_e['targete'] ~= output_e['targetinitiale'] then
      output_e['targete'] = output_e['targetinitiale']
	 else
	  output_e['targete'] = -1 
	 end
	end
    if pathpatrolmode == 2 then
	 -- one way just stops
     output_e['targete'] = -1 
	end
    if pathpatrolmode == 3 then
	 -- no randoms found, unlikely, so just go to start
	 if output_e['targete'] ~= output_e['targetinitiale'] then
      output_e['targete'] = output_e['targetinitiale']
	 else
	  output_e['targete'] = -1 
	 end
	end
   end
  end 
 end
 
 -- Turn Right
 if actiontype == g_masterinterpreter_act_turnright and actionparam1value ~= nil then
  RotateY(e,actionparam1value)
 end
 
 -- Turn Left
 if actiontype == g_masterinterpreter_act_turnleft and actionparam1value ~= nil then
  RotateY(e,actionparam1value*-1)
 end
 
 -- Go Away From Target
 if actiontype == g_masterinterpreter_act_goawayfromtarget then
  if actionparam1value == nil then actionparam1value = 500 end
  if g_Time > output_e['recalctimer'] + 1000 or actionparam1value ~= nil then
   output_e['recalctimer'] = g_Time
   local tdx = g_Entity[ e ]['x'] - TargetX
   local tdz = g_Entity[ e ]['z'] - TargetZ
   local tda = math.deg(math.atan2(tdx,tdz))
   if tda < -180 then tda=tda+360 end
   if tda >  180 then tda=tda-360 end
   local bestx = 0
   local besty = 0
   local bestz = 0
   local bestdst = 0
   local toggle = 0
   for ang = 0, 180, 5 do
    local shiftang = ang / 2
	toggle = 1 - toggle
	if toggle ==0 then shiftang=shiftang*-1 end
    for dst = actionparam1value, 100, -50 do
     local awayTargetX = TargetX + (math.sin(math.rad(tda+shiftang))*dst)
     local awayTargetZ = TargetZ + (math.cos(math.rad(tda+shiftang))*dst) 
     if RDIsWithinMesh(awayTargetX,TargetY,awayTargetZ) == 1 then
	  if dst > bestdst then
	   bestdst = dst
	   bestx = awayTargetX
	   besty = TargetY
	   bestz = awayTargetZ
	  end
	 end
    end
   end
   if bestdst > 0 then
    masterinterpreter_setnewtarget ( e, output_e, bestx, besty, bestz, 0 )
   end
  end
  if output_e['goodtargetpointindex'] > 0 then
   masterinterpreter_followtarget ( e, output_e, -1 )
  end 
 end 

 -- Restore Health
 if actiontype == g_masterinterpreter_act_restorehealth then
  SetEntityHealth(e,g_Entity[ e ]['strength'])
  g_Entity[e]['health'] = g_Entity[ e ]['strength']
  output_e['oldhealth'] = g_Entity[e]['health']
 end
 
 -- Set Target To Start
 if actiontype == g_masterinterpreter_act_settargetstart then
  -- reset any stored ray result for fresh target
  masterinterpreter_resetscan(e)
  output_e['target'] = "start"
  output_e['recalctimer'] = 0
 end
 
 -- Move Forward
 if actiontype == g_masterinterpreter_act_moveforward and actionparam1value ~= nil then
  if output_e['movingmode'] == 0 then
   if actionparam1value > 0 then
    output_e['movingmode'] = actionparam1value
   end
  else
   if actionparam1value == 0 then
    output_e['movingmode'] = 0
    MoveAndRotateToXYZ(e,0,0) 
   end
  end
 end

 -- Stop Moving
 if actiontype == g_masterinterpreter_act_stopmoving then
  output_e['movingmode'] = 0
  MoveAndRotateToXYZ(e,0,0) 
 end

 -- Hide Weapon 
 if actiontype == g_masterinterpreter_act_hideweapon then
  HideEntityAttachment(e)
 end
 
 -- Show Weapon 
 if actiontype == g_masterinterpreter_act_showweapon then
  ShowEntityAttachment(e)
 end
 
 -- Fire Weapon 
 if actiontype == g_masterinterpreter_act_fireweapon then
  if output_e['ammo'] > 0 then
   output_e['ammo'] = output_e['ammo'] - 1
  end
  FireWeaponInstant(e)
 end
 
 -- Aim At Target
 if actiontype == g_masterinterpreter_act_aimat then
  if actionparam1value == nil then actionparam1value = 10 end
  if actionparam1value == 0 then actionparam1value = 10 end
  if output_e['target'] == "player" then
   AimAtPlayer(e,actionparam1value)
  end
  if output_e['target'] == "flag" then
  end
  if output_e['target'] == "hostile" then
   local ee = output_e['targete']
   if ee > 0 then
    if g_Entity[ee]['health'] > 0 then
     AimAtTarget(e,actionparam1value,ee)
	end
   end
  end
 end
 
 -- Stop Aiming
 if actiontype == g_masterinterpreter_act_stopaiming then
  AimAtPlayer(e,0)
 end
 
 -- Go To Target Flank
 if actiontype == g_masterinterpreter_act_gototargetflank then
  if g_Time > output_e['recalctimer'] + 3000 then
   output_e['recalctimer'] = g_Time
   local tflankdx = g_Entity[ e ]['x'] - TargetX
   local tflankdz = g_Entity[ e ]['z'] - TargetZ
   local tflankda = math.deg(math.atan2(tflankdx,tflankdz))
   local tflankdd = math.sqrt(math.abs(tflankdx*tflankdx)+math.abs(tflankdz*tflankdz))
   if tflankda < -180 then tflankda=tflankda+360 end
   if tflankda >  180 then tflankda=tflankda-360 end
   if tflankdd > 300 then tflankdd = tflankdd - 200 end
   if tflankdd < 200 then tflankdd = 200 end
   local adjustangle = 0
   if output_e['flankdirectionmode'] == 1 then adjustangle = output_e['flankangle'] end
   if output_e['flankdirectionmode'] == 2 then adjustangle = output_e['flankangle']*-1 end
   local flankTargetX = TargetX + (math.sin(math.rad(tflankda)+math.rad(adjustangle))*tflankdd)
   local flankTargetZ = TargetZ + (math.cos(math.rad(tflankda)+math.rad(adjustangle))*tflankdd) 
   local groundheight = TargetY
   local targetgroundheight = TargetY
   local radiusOfAdjustment = 100
   local flankSuccess = 0
   flankSuccess, flankTargetX, flankTargetZ = AdjustPositionToGetLineOfSight(g_Entity[ e ]['obj'], flankTargetX, groundheight, flankTargetZ, TargetX, targetgroundheight, TargetZ, radiusOfAdjustment )
   masterinterpreter_setnewtarget ( e, output_e, flankTargetX, TargetY, flankTargetZ, 0 )
  end   
  if output_e['goodtargetpointindex'] > 0 then
   masterinterpreter_followtarget ( e, output_e, -1 )
  end  
 end

 -- Flank Left
 if actiontype == g_masterinterpreter_act_flankleft then
  if actionparam1value == nil then actionparam1value = 45 end
  output_e['flankdirectionmode'] = 1
  output_e['flankangle'] = actionparam1value
 end

 -- Flank Right
 if actiontype == g_masterinterpreter_act_flankright then
  if actionparam1value == nil then actionparam1value = 45 end
  output_e['flankdirectionmode'] = 2
  output_e['flankangle'] = actionparam1value
 end
 
 -- Flank Randomly
 if actiontype == g_masterinterpreter_act_flankrandomly then
  if actionparam1value == nil then actionparam1value = 45 end
  output_e['flankdirectionmode'] = math.random(1,2)
  output_e['flankangle'] = 45
  -- if part of squad, intelegently choose flank instructions
  local squadcount = 0
  local squadrank = 0
  for i = 0 , 9, 1 do
   local othere = GetEntityRelationshipID(e,i)
   if othere > 0 then
    if g_Entity[ othere ]['health'] > 0 then
	 if e > othere then squadrank=squadrank+1 end
     squadcount=squadcount+1
    end   
   end
  end
  if squadcount > 0 then
   if squadrank == 0 then
    -- captain
    output_e['flankdirectionmode'] = math.random(1,2)
    output_e['flankangle'] = 25
   else
    -- troops
    local formationposition = 0
    for s = 0 , squadrank, 1 do
     formationposition=formationposition+1
     if formationposition > 1 then formationposition = 0 end
	end
    if formationposition == 0 then
     output_e['flankdirectionmode'] = 1
     output_e['flankangle'] = 80
	end
    if formationposition == 1 then
     output_e['flankdirectionmode'] = 2
     output_e['flankangle'] = 80
	end
   end
  end
 end
 
 -- Play Speech
 if actiontype == g_masterinterpreter_act_playspeech then
  PlaySound(e,1)
  PlaySpeech(e,1)
 end
 
 -- Stop Speech
 if actiontype == g_masterinterpreter_act_stopspeech then
  StopSound(e,1)
  StopSpeech(e)
 end
 
 -- Play Sound
 if actiontype == g_masterinterpreter_act_playsound and actionparam1value ~= nil then
  if actionparam1value < 0 then actionparam1value = 0 end
  if actionparam1value > 3 then actionparam1value = 3 end
  local allegianceindex = GetEntityAllegiance(e)
  --if allegianceindex >= 0 and allegianceindex <= 1 then
   -- special manager for enemy characters
   local calloutindex = (allegianceindex*10)+actionparam1value
   if g_calloutmanagertime[calloutindex] ~= nil then
    if g_Time > g_calloutmanagertime[calloutindex] then
	 g_calloutmanager[calloutindex] = 0
	end
   else
    g_calloutmanager[calloutindex] = 0
   end
   if g_calloutmanager[calloutindex] == 0 then
    g_calloutmanager[calloutindex] = e
    g_calloutmanagertime[calloutindex] = g_Time + 3000
    PlaySound(e,actionparam1value)
   end
  --else
  -- PlaySound(e,actionparam1value)
  --end
 end
 
 -- Loop Sound
 if actiontype == g_masterinterpreter_act_loopsound and actionparam1value ~= nil then
  if actionparam1value < 0 then actionparam1value = 0 end
  if actionparam1value > 3 then actionparam1value = 3 end
  LoopSound(e,actionparam1value)
 end
 
 -- Stop Sound
 if actiontype == g_masterinterpreter_act_stopsound and actionparam1value ~= nil then
  if actionparam1value < 0 then actionparam1value = 0 end
  if actionparam1value > 3 then actionparam1value = 3 end
  StopSound(e,actionparam1value)
 end
 
 -- Stop Animation and Turn
 if actiontype == g_masterinterpreter_act_stopandturn then
  if actionparam1value == nil then actionparam1value = 0 end
  output_e['lastanimplayed'] = ""
  output_e['lastanimtriggerindex'] = 0
  StopAnimation(e)
  PlayAnimation(e) -- this little trick restarts the animation at the first frame
  StopAnimation(e) -- so that the hard reset rotation on the Y matches up seamlessly
  ResetRotation(e,g_Entity[ e ]['anglex'],g_Entity[ e ]['angley']+actionparam1value,g_Entity[ e ]['anglez'])
 end
 
 -- Look At Player (even if has different target)
 if actiontype == g_masterinterpreter_act_lookatplayer then
  if actionparam1value == nil then actionparam1value = 10 end
  if actionparam1value == 0 then actionparam1value = 10 end
  LookAtPlayer(e,actionparam1value)
 end
 
 -- Look Forward
 if actiontype == g_masterinterpreter_act_lookforward then
  if actionparam1value == nil then actionparam1value = 10 end
  if actionparam1value == 0 then actionparam1value = 10 end
  LookForward(e,actionparam1value)
 end
 
 -- Prevent Zero Health
 if actiontype == g_masterinterpreter_act_preventzerohealth then
  SetEntityHealthSilent(e,-99999)
 end 
 
 -- Destroy
 if actiontype == g_masterinterpreter_act_destroy then
  SetEntityHealthSilent(e,0) -- switches off 'Prevent Zero', then uses regular health zero kill order
  SetEntityHealth(e,0)
  g_Entity[e]['health'] = 0
  output_e['oldhealth'] = 0
 end 
 
 -- Multiply Animation Speed
 if actiontype == g_masterinterpreter_act_multiplyanimspeed then
  if actionparam1value == nil then actionparam1value = 1.0 end
  if output_e['originalmovespeed'] == -1 then output_e['originalmovespeed'] = GetEntityMoveSpeed(e) end
  SetEntityMoveSpeed(e,output_e['originalmovespeed'] * actionparam1value)
 end 
 
 -- Set Health
 if actiontype == g_masterinterpreter_act_sethealth then
  if actionparam1value == nil then actionparam1value = g_Entity[ e ]['strength'] end
  SetEntityHealth(e,actionparam1value)
  g_Entity[e]['health'] = actionparam1value
  output_e['oldhealth'] = g_Entity[e]['health']
 end
 
 -- Adjust Look HLimit
 if actiontype == g_masterinterpreter_act_adjustlookhlimit then
  if actionparam1value == nil then actionparam1value = 45 end
  AdjustLookSettingHorizLimit(e,actionparam1value)
 end
 -- Adjust Look HOffset
 if actiontype == g_masterinterpreter_act_adjustlookhoffset then
  if actionparam1value == nil then actionparam1value = 0 end
  AdjustLookSettingHorizOffset(e,actionparam1value)
 end
 -- Adjust Look VLimit
 if actiontype == g_masterinterpreter_act_adjustlookvlimit then
  if actionparam1value == nil then actionparam1value = 45 end
  AdjustLookSettingVertLimit(e,actionparam1value)
 end
 -- Adjust Look VOffset
 if actiontype == g_masterinterpreter_act_adjustlookvoffset then
  if actionparam1value == nil then actionparam1value = 0 end
  AdjustLookSettingVertOffset(e,actionparam1value)
 end
 
 -- Adjust Aim HLimit
 if actiontype == g_masterinterpreter_act_adjustaimhlimit then
  if actionparam1value == nil then actionparam1value = 45 end
  AdjustAimSettingHorizLimit(e,actionparam1value)
 end
 -- Adjust Aim HOffset
 if actiontype == g_masterinterpreter_act_adjustaimhoffset then
  if actionparam1value == nil then actionparam1value = 0 end
  AdjustAimSettingHorizOffset(e,actionparam1value)
 end
 -- Adjust Aim VLimit
 if actiontype == g_masterinterpreter_act_adjustaimvlimit then
  if actionparam1value == nil then actionparam1value = 45 end
  AdjustAimSettingVertLimit(e,actionparam1value)
 end
 -- Adjust Aim VOffset
 if actiontype == g_masterinterpreter_act_adjustaimvoffset then
  if actionparam1value == nil then actionparam1value = 0 end
  AdjustAimSettingVertOffset(e,actionparam1value)
 end
  
 -- Go To Step Left or Right
 if actiontype == g_masterinterpreter_act_gotostepleft or actiontype == g_masterinterpreter_act_gotostepright or actiontype == g_masterinterpreter_act_gotostepforward or actiontype == g_masterinterpreter_act_gotostepbackward then
  if actionparam1value == nil then actionparam1value = 35 end
  local tflankdx = g_Entity[ e ]['x'] - TargetX
  local tflankdz = g_Entity[ e ]['z'] - TargetZ
  local tflankda = math.deg(math.atan2(tflankdx,tflankdz))
  if tflankda < -180 then tflankda=tflankda+360 end
  if tflankda >  180 then tflankda=tflankda-360 end
  local adjustangle
  if actiontype == g_masterinterpreter_act_gotostepleft then adjustangle = 90 end
  if actiontype == g_masterinterpreter_act_gotostepright then adjustangle = -90 end
  if actiontype == g_masterinterpreter_act_gotostepforward then adjustangle = 180 end
  if actiontype == g_masterinterpreter_act_gotostepbackward then adjustangle = 0 end  
  local stepTargetX = g_Entity[ e ]['x'] + (math.sin(math.rad(tflankda)+math.rad(adjustangle))*actionparam1value)
  local stepTargetZ = g_Entity[ e ]['z'] + (math.cos(math.rad(tflankda)+math.rad(adjustangle))*actionparam1value) 
  local movingbackwards = 0
  if actiontype == g_masterinterpreter_act_gotostepbackward then movingbackwards = 1 end  
  masterinterpreter_setnewtarget ( e, output_e, stepTargetX, g_Entity[ e ]['y'], stepTargetZ, movingbackwards )
  if output_e['goodtargetpointindex'] > 0 then
   -- cancel target location if not where wanted to step
   local pointcount = RDGetPathPointCount()
   if pointcount > 0 then 
    local lastpointx = RDGetPathPointX(pointcount-1)
    local lastpointz = RDGetPathPointZ(pointcount-1)
	if math.abs(stepTargetX-lastpointx) < 2 and math.abs(stepTargetZ-lastpointz) < 2 then
     masterinterpreter_followtarget ( e, output_e, -1 )
	else 
     output_e['goodtargetpointindex'] = 0
    end
   end
  end  
 end
 
 -- Target Alert Position
 if actiontype == g_masterinterpreter_act_settargetactivator then
  -- reset any stored ray result for fresh target
  masterinterpreter_resetscan(e)
  local sourceofactivatione = output_e['wholastactivated']
  if sourceofactivatione > 0 then
   output_e['target'] = "flag"
   output_e['targete'] = sourceofactivatione
   output_e['recalctimer'] = 0
  else
   if sourceofactivatione == 0 then
    output_e['target'] = "player"
    output_e['targete'] = 0
    output_e['recalctimer'] = 0
   else
    output_e['target'] = "start"
    output_e['targete'] = 0
    output_e['recalctimer'] = 0
   end
  end
 end
 
 -- Perform Logic Connections
 if actiontype == g_masterinterpreter_act_performlogicconnections then
  PerformLogicConnections(e)
 end
 
 -- Target Player
 if actiontype == g_masterinterpreter_act_settargetplayer then
  -- reset any stored ray result for fresh target
  masterinterpreter_resetscan(e)
  output_e['target'] = "player"
  output_e['targete'] = 0
  output_e['recalctimer'] = 0
 end
 
 -- Flank Forward
 if actiontype == g_masterinterpreter_act_flankforward then
  output_e['flankdirectionmode'] = 0
  output_e['flankangle'] = 0
 end 

 -- Set Value To Zero or One
 if (actiontype == g_masterinterpreter_act_setvaluetozero or actiontype == g_masterinterpreter_act_setvaluetoone) and actionparam1 ~= nil then
  if actiontype == g_masterinterpreter_act_setvaluetozero then
   output_e[actionparam1] = 0
  else
   output_e[actionparam1] = 1
  end
 end 
 
 -- Look Randomly
 if actiontype == g_masterinterpreter_act_lookrandomly then
  if actionparam1value == nil then actionparam1value = 45 end
  LookAtAngle(e,math.random(-actionparam1value,actionparam1value))
 end 
 
 -- Run Behavior
 if actiontype == g_masterinterpreter_act_runbehavior and actionparam1 ~= nil then
  SwitchScript(e,actionparam1)
 end
 
 -- Return From Behavior
 if actiontype == g_masterinterpreter_act_returnfrombehavior then
  SwitchScript(e,"")
 end

 -- Go To Near Target
 if actiontype == g_masterinterpreter_act_gotoneartarget then
  if g_Time > output_e['recalctimer'] + 2000 then
   output_e['recalctimer'] = g_Time
   local tneardx = g_Entity[ e ]['x'] - TargetX
   local tneardz = g_Entity[ e ]['z'] - TargetZ
   local tnearda = math.deg(math.atan2(tneardx,tneardz))
   local tneardd = math.sqrt(math.abs(tneardx*tneardx)+math.abs(tneardz*tneardz))
   if tnearda < -180 then tnearda=tnearda+360 end
   if tnearda >  180 then tnearda=tnearda-360 end
   tneardd = math.random(200,300)
   local nearTargetX = TargetX + (math.sin(math.rad(tnearda))*tneardd)
   local nearTargetY = TargetY
   local nearTargetZ = TargetZ + (math.cos(math.rad(tnearda))*tneardd) 
   local targetgroundheight = TargetY
   local radiusOfAdjustment = 100
   local flankSuccess = 0
   flankSuccess, nearTargetX, nearTargetZ = AdjustPositionToGetLineOfSight(g_Entity[ e ]['obj'], nearTargetX, nearTargetY, nearTargetZ, TargetX, targetgroundheight, TargetZ, radiusOfAdjustment )
   if flankSuccess == 0 then
    nearTargetX = TargetX + (math.sin(math.rad(tnearda))*75)
    nearTargetZ = TargetZ + (math.cos(math.rad(tnearda))*75) 
   end
   masterinterpreter_setnewtarget ( e, output_e, nearTargetX, nearTargetY, nearTargetZ, 0 )
  end   
  if output_e['goodtargetpointindex'] > 0 then
   masterinterpreter_followtarget ( e, output_e, -1 )
  end  
 end
 
 -- Become Enemy
 if actiontype == g_masterinterpreter_act_becomeenemy then
  SetEntityAllegiance(e,0)
 end

 -- Become Ally
 if actiontype == g_masterinterpreter_act_becomeally then
  SetEntityAllegiance(e,1)
 end

 -- Become Neutral
 if actiontype == g_masterinterpreter_act_becomeneutral then
  SetEntityAllegiance(e,2)
 end
 
 -- Reset Goal Timer
 if actiontype == g_masterinterpreter_act_resetgoaltimer then
  output_e['goalstarttime'] = Timer()
 end
 
 -- Make AI Sound
 if actiontype == g_masterinterpreter_act_makeaisound then
  if actionparam1value == nil then actionparam1value = 500 end
  local tcategory = 0
  local allegianceindex = GetEntityAllegiance(e)
  if allegianceindex == 0 then tcategory = 2 end
  MakeAISound(g_Entity[ e ]['x'],g_Entity[ e ]['y'],g_Entity[ e ]['z'],actionparam1value,tcategory,e)
 end
 
 -- Set Value To Random(s)
 if actiontype == g_masterinterpreter_act_setvaluetorandom and actionparam1 ~= nil then
  output_e[actionparam1] = math.random(0,1)
 end 
 if actiontype == g_masterinterpreter_act_setvaluetorandomtwo and actionparam1 ~= nil then
  output_e[actionparam1] = math.random(1,2)
 end 
 if actiontype == g_masterinterpreter_act_setvaluetorandomthree and actionparam1 ~= nil then
  output_e[actionparam1] = math.random(1,3)
 end 
 if actiontype == g_masterinterpreter_act_setvaluetorandomfour and actionparam1 ~= nil then
  output_e[actionparam1] = math.random(1,4)
 end 
 if actiontype == g_masterinterpreter_act_setvaluetorandomfive and actionparam1 ~= nil then
  output_e[actionparam1] = math.random(1,5)
 end 
 
 -- Set Immunity Time
 if actiontype == g_masterinterpreter_act_setimmunitytime and actionparam1 ~= nil then
  if output_e['immunitytimer'] == 0 then
   output_e['immunitylasthealth'] = g_Entity[e]['health']
  end
  output_e['immunitytimer'] = g_Time + actionparam1
  SetEntityHealth(e,99999)
  g_Entity[e]['health'] = 99999
  output_e['oldhealth'] = 99999
 end 
 
 -- Set Alert To Enemy
 if actiontype == g_masterinterpreter_act_setalerttoenemy then
  local allegianceindex = GetEntityAllegiance(e)
  if allegianceindex == 0 then
   output_e['wholastactivated'] = 0
  else
   output_e['wholastactivated'] = output_e['targete']
  end
 end
 
 -- Go To Nearby Spot
 if actiontype == g_masterinterpreter_act_gotonearbyspot then
  if actionparam1value == nil then actionparam1value = 200 end
  local tnewspotangle = math.random(0,360)
  local tnewspotx = output_e['startpositionx'] + (math.sin(math.rad(tnewspotangle))*actionparam1value)
  local tnewspotz = output_e['startpositionz'] + (math.cos(math.rad(tnewspotangle))*actionparam1value) 
  masterinterpreter_setnewtarget ( e, output_e, tnewspotx, g_Entity[ e ]['y'], tnewspotz, 0 )
  if output_e['goodtargetpointindex'] > 0 then
   local pointcount = RDGetPathPointCount()
   if pointcount > 0 then 
    local lastpointx = RDGetPathPointX(pointcount-1)
    local lastpointz = RDGetPathPointZ(pointcount-1)
	if math.abs(tnewspotx-lastpointx) < 2 and math.abs(tnewspotz-lastpointz) < 2 then
     masterinterpreter_followtarget ( e, output_e, -1 )
	else 
     output_e['goodtargetpointindex'] = 0
    end
   end
  end  
 end
 
 -- Instant Ragdoll
 if actiontype == g_masterinterpreter_act_instantragdoll then
  SetEntityHealthSilent(e,-12345)
  SetEntityHealth(e,-12345)
  g_Entity[e]['health'] = 0
  output_e['oldhealth'] = 0
 end 
 
 -- Set Path Rotation
 if actiontype == g_masterinterpreter_act_setpathrotation then
  if actionparam1value == nil then actionparam1value = 1 end
  SetEntityPathRotationMode(e,actionparam1value)
 end 
 
 -- Go To Attack Spot
 if actiontype == g_masterinterpreter_act_gotoattackspot then
  local weaponid = GetEntityHasWeapon(e)
  local attackrange = GetWeaponRange(weaponid,0)
  if attackrange < 50 then attackrange = 50 end
  if actionparam1value == nil then actionparam1value = attackrange end  
  local tneardx = g_Entity[ e ]['x'] - TargetX
  local tneardz = g_Entity[ e ]['z'] - TargetZ
  local tnearda = math.deg(math.atan2(tneardx,tneardz)) + 45
  if tnearda < -180 then tnearda=tnearda+360 end
  if tnearda >  180 then tnearda=tnearda-360 end
  if tnearda <    0 then tnearda=tnearda+360 end
  local attackslot = math.floor(tnearda / 90)
  if attackslot < 0 then attackslot = 0 end
  if attackslot > 3 then attackslot = 3 end
  local slot1
  local slot2
  local slot3
  local slot4
  slot1, slot2, slot3, slot4 = masterinterpreter_gettargetslot ( e, output_e )
  local useattackslot = -1
  if e == slot1 or e == slot2 or e == slot3 or e == slot4 then
   if e == slot1 then useattackslot = 0 end
   if e == slot2 then useattackslot = 1 end
   if e == slot3 then useattackslot = 2 end
   if e == slot4 then useattackslot = 3 end
  else
   for slotcycle = 0, 3, 1 do
    if attackslot == 0 then
     if slot1 == 0 then useattackslot = 0 else attackslot=1 end
    end
    if attackslot == 1 then
     if slot2 == 0 then useattackslot = 1 else attackslot=2 end
    end
    if attackslot == 2 then
     if slot3 == 0 then useattackslot = 2 else attackslot=3 end
    end
    if attackslot == 3 then
     if slot4 == 0 then useattackslot = 3 else attackslot=0 end
    end
   end
  end
  if useattackslot ~= -1 then
   masterinterpreter_settargetslot ( e, output_e, useattackslot )
   tnearda = useattackslot * 90
   local attackspotdistance = actionparam1value*0.8
   local nearTargetX = TargetX + (math.sin(math.rad(tnearda))*attackspotdistance)
   local nearTargetY = TargetY
   local nearTargetZ = TargetZ + (math.cos(math.rad(tnearda))*attackspotdistance)   
   masterinterpreter_setnewtarget ( e, output_e, nearTargetX, nearTargetY, nearTargetZ, 0 )
  end
 end 
 
 -- Face Last Target
 if actiontype == g_masterinterpreter_act_facelasttarget then
  if output_e['lasttarget'] ~= "" then
   if actionparam1value == nil then actionparam1value = GetEntityTurnSpeed(e)/4.0 end
   local storetarget = output_e['target']
   local storetargete = output_e['targete']
   output_e['target'] = output_e['lasttarget']
   output_e['targete'] = output_e['lasttargete']
   local TargetX, TargetY, TargetZ = masterinterpreter_gettargetxyz ( e, output_e )
   output_e['target'] = storetarget
   output_e['targete'] = storetargete 
   local tdx = TargetX - g_Entity[ e ]['x']
   local tdz = TargetZ - g_Entity[ e ]['z']
   local tda = (math.atan2(tdx,tdz)/6.28)*360.0
   if tda < -180 then tda=tda+360 end
   if tda >  180 then tda=tda-360 end
   SetRotationYSlowly(e,tda,actionparam1value)
  end
 end
 
 -- Trigger Combat Music
 if actiontype == g_masterinterpreter_act_triggercombatmusic then
  if GetCombatMusicTrackPlaying() == 0 then g_CombatMusicMode = 0 g_CombatMusicLatest = 0 end
  if g_CombatMusicLatest == 0 or g_CombatMusicLatest == nil then
   StartCombatMusicTrack()
   g_CombatMusicMode = 0
  end
  g_CombatMusicLatest = Timer()
 end
 
 -- Reload Ammo
 if actiontype == g_masterinterpreter_act_reloadammo then 
  local weaponid = GetEntityHasWeapon(e)
  if weaponid > 0 then
   local reloadquantity = GetWeaponReloadQuantity(weaponid,0)
   output_e['ammo'] = reloadquantity
  end   
 end
 
 -- Reset Alert Target
 if actiontype == g_masterinterpreter_act_resetalerttarget then
  output_e['wholastactivated'] = -1
 end 
 
 -- Show Text
 if actiontype == g_masterinterpreter_act_showtext then
  if actionparam1 ~= nil then
   Prompt(tostring(actionparam1value))
  end
 end 
 
 -- Show HUD
 if actiontype == g_masterinterpreter_act_showhud then
  if actionparam1value ~= nil then
   ScreenToggle(tostring(actionparam1value))
  end
 end 
   
 -- Change Global
 if actiontype == g_masterinterpreter_act_changeglobal then
  local i,j_ = string.find(actionparam1,"=")
  if i ~= nil then
   if i > 0 then
    local tuserglobal = string.sub(actionparam1,1,i-1)
    local tuservalue = string.sub(actionparam1,i+1,-1)
	if tuservalue ~= nil then
		if output_e[tostring(tuservalue)] ~= nil then
			tuservalue = output_e[tostring(tuservalue)]
		end
	end
    _G["g_UserGlobal['"..tuserglobal.."']"] = tuservalue
   end
  end
 end 
 
 -- Change Container
 if actiontype == g_masterinterpreter_act_changecontainer then
  g_UserGlobalContainer = actionparam1value
 end 
 
 -- Logic Boost
 if actiontype == g_masterinterpreter_act_logicboost then
  g_masterinterpreter_logicboostcount = actionparam1value
 end 
 
 -- Turn Collison Off
 if actiontype == g_masterinterpreter_act_collisionoff then
  CollisionOff(e)
 end 
 
 -- Turn Collison On
 if actiontype == g_masterinterpreter_act_collisionon then
  CollisionOn(e)
 end
 
end

function master_interpreter_core.masterinterpreter_restart( output_e, entity_e )

 -- for init and restarts
 output_e['currentbehaviorindex'] = 0
 output_e['lastanimplayed'] = ""
 output_e['lasttimerreached'] = 0
 output_e['frozentheplayer'] = 0
 output_e['oldhealth'] = entity_e['health']
 output_e['savedsuperstate'] = 0
 output_e['damagetaken'] = 0
 output_e['immunitytimer'] = 0
 output_e['immunitylasthealth'] = 0
 output_e['recalctimer'] = g_Time
 output_e['target'] = "player"
 output_e['targete'] = 0
 output_e['targetinitiale'] = 0
 output_e['targetpreviouse'] = 0
 output_e['lasttarget'] = ""
 output_e['lasttargete'] = 0
 output_e['startpositionx'] = entity_e['x']
 output_e['startpositiony'] = entity_e['y']
 output_e['startpositionz'] = entity_e['z']
 output_e['originalmovespeed'] = -1
 output_e['movingmode'] = 0
 output_e['flankdirectionmode'] = 1
 output_e['flankangle'] = 45
 output_e['goodtargetpointcount'] = 0
 output_e['goodtargetpointindex'] = 0
 output_e['goodtargetdestinationx'] = -1
 output_e['goodtargetdestinationy'] = -1
 output_e['goodtargetdestinationz'] = -1
 output_e['lastgoodtargetdistance'] = 0
 output_e['goalstarttime'] = Timer()
 output_e['lastanimtriggerindex'] = 0
 output_e['ammo'] = -1

 -- will persist script switches 
 if output_e['wholastactivated'] == nil then output_e['wholastactivated'] = -1 end

end

function master_interpreter_core.masterinterpreter_unfreeze( output_e, entity_e )
 -- for when object leaves logic range, and then re-enters
 -- must ensure logic is not stuck in a logic sequence broken by the freeze
 output_e['currentbehaviorindex'] = 0
 output_e['lastanimplayed'] = ""
 output_e['lasttimerreached'] = 0
 output_e['frozentheplayer'] = 0
 output_e['recalctimer'] = g_Time
 output_e['movingmode'] = 0
 output_e['goalstarttime'] = Timer()
end

function master_interpreter_core.masterinterpreter_stop( output_e, entity_e )
 output_e['currentbehaviorindex'] = -1
end

function master_interpreter_core.masterinterpreter ( passedin_behavior, listmax, e, output_e, entity_e )

 -- Slow down logic for debugging
 local tdoanotherlogiccycle = 1
 if g_masterinterpreter_slowtimeinterval > 0 then
  tdoanotherlogiccycle = 0
  if g_masterinterpreter_slowtime == 0 then g_masterinterpreter_slowtime = Timer() + g_masterinterpreter_slowtimeinterval end
  if Timer() > g_masterinterpreter_slowtime then
   g_masterinterpreter_slowtime = Timer() + g_masterinterpreter_slowtimeinterval
   tdoanotherlogiccycle = 1
  end
 end
 
 -- Go through all behavior entries (if active)
 local n = output_e["currentbehaviorindex"]
 if n == -1 then tdoanotherlogiccycle = 0 end

 -- Permit a logic cycle 
 if tdoanotherlogiccycle == 1 then

  -- Check this condition
  tconditionresult = masterinterpreter_getconditionresult(e, output_e, passedin_behavior[n].conditiontype, passedin_behavior[n].conditionparam1)
  if tconditionresult == 1 then

   -- If true, perform the action
   masterinterpreter_doaction(e, output_e, passedin_behavior[n].actiontype, passedin_behavior[n].actionparam1, passedin_behavior[n].actionparam2)
   if passedin_behavior[n].actionnewbehaviorindex == -1 then
    -- return from interupt
    for bi = 1, listmax, 1 do
	 if passedin_behavior[bi].superstate == output_e["savedsuperstate"] then
      output_e["currentbehaviorindex"] = bi
	  break
	 end
    end
	output_e["savedsuperstate"] = 0
   else
    output_e["currentbehaviorindex"] = passedin_behavior[n].actionnewbehaviorindex
   end
	
  else -- condition
	
   -- If condition is false, and new behavior set for this scenario, change behaviorindex
   if passedin_behavior[n].elsebehaviorindex > 0 then
    output_e["currentbehaviorindex"] = passedin_behavior[n].elsebehaviorindex
   end
	
  end -- condition
 
 end -- logic cycle

 -- suffering damage triggers interupt system 
 if n ~= -1 and g_Entity[e]['health'] < output_e['oldhealth'] then
  output_e['damagetaken'] = output_e['oldhealth'] - g_Entity[e]['health']
  output_e['oldhealth'] = g_Entity[e]['health']
  if output_e["currentbehaviorindex"] ~= output_e["damagebehaviorindex"] then
   if passedin_behavior[n].caninterupt == 1 then
    if output_e['frozentheplayer'] == 0 then
     if output_e["damagebehaviorindex"] ~= nil then
      if output_e["damagebehaviorindex"] > 0 then
	   if Timer() > output_e['damagebehaviortimer'] + 100 then
        output_e["savedsuperstate"] = passedin_behavior[n].superstate
        output_e["currentbehaviorindex"] = output_e["damagebehaviorindex"]
	    output_e['damagebehaviortimer'] = Timer()
	   end 
	  end
 	 end
    end
   end
  end
 end
 
 -- handle object immunity timer
 if output_e['immunitytimer'] > 0 then
  if g_Time > output_e['immunitytimer'] then
   g_Entity[e]['health'] = output_e['immunitylasthealth']
   SetEntityHealth(e,g_Entity[e]['health'])
   output_e['oldhealth'] = g_Entity[e]['health']
   output_e['immunitytimer'] = 0
   output_e['immunitylasthealth'] = 0
  end
 end
 
 -- handle player retstart events (to end any sequences in progress)
 if g_PlayerHealth <= 0 then
  if g_masterinterpreter_playerfrozenby > 0 then
   SetCameraOverride(0)
   UnFreezePlayer()
   g_masterinterpreter_playerfrozenby = 0
  end
 end 
 
 -- Debug View
 local tbehaviorindex = output_e["currentbehaviorindex"]
 if tbehaviorindex >= 0 then
  --PromptLocal(e,"Health = " .. g_Entity[e]['health'] )
 end
 
 -- Ensure proper foot planting (anim based movement for character) 
 -- move all this to the internal darkai_handlegotomove which can be done across multiple points in path and every frame,
 -- not every time this script is called!!
 --if n ~= -1 and output_e['movingmode'] == 0 then
 -- MoveWithAnimation(e,1)
 --end 
 
 -- Feed debug data to UI
 SetDebuggingData(e,output_e["currentbehaviorindex"])
 
 -- Some LUA Text for now (need reactions and sound to replace this!)
 --Prompt ( "PLAYER HEALTH : " .. g_PlayerHealth )
 
 -- and detect for debugger asking to reload BYC file
 if entity_e['debuggermode'] == 1 then
  local currentinstructionindex = output_e["currentbehaviorindex"]
  if currentinstructionindex < 0 then currentinstructionindex = 0 end
  local currentinstructionuniquecode = passedin_behavior[currentinstructionindex].uniquecode
  listmax = master_interpreter_core.masterinterpreter_load ( output_e, passedin_behavior )
  output_e["currentbehaviorindex"] = 0
  for index = 0 , listmax, 1 do
   if passedin_behavior[index].uniquecode == currentinstructionuniquecode then
    output_e["currentbehaviorindex"] = index
    break
   end
  end
  entity_e['debuggermode'] = 0
 end
 
 -- and detect for debugger asking to restart logic
 if entity_e['debuggermode'] == 2 then
  master_interpreter_core.masterinterpreter_restart(output_e,entity_e)
  entity_e['debuggermode'] = 0
 end
 
 -- and detect for debugger asking to unfreeze logic returning to active region
 if entity_e['debuggermode'] == 3 then
  master_interpreter_core.masterinterpreter_unfreeze(output_e,entity_e)
  entity_e['debuggermode'] = 0
 end
 
 -- and detect for debugger asking to stop logic
 if entity_e['debuggermode'] == 3 then
  output_e['lastanimplayed'] = ""
  StopAnimation(e)
  master_interpreter_core.masterinterpreter_stop(output_e,entity_e)
  entity_e['debuggermode'] = 0
 end
 
 -- this can be changed by the master interpreter when loading in new BYC
 if g_masterinterpreter_logicboostcount > 1 then
  local runlocallooptoprocessabatchofinstructions = g_masterinterpreter_logicboostcount
  g_masterinterpreter_logicboostcount = 1
  return runlocallooptoprocessabatchofinstructions * -1
 else
  return listmax
 end
  
end

function master_interpreter_core.masterinterpreter_load ( output_e, g_myscript_behavior )

 -- bytecodefile
 bytecodefile = output_e['bycfilename']
 
 -- clear values that may be set below
 output_e['damagebehaviorindex'] = -1
 output_e['damagebehaviortimer'] = Timer()
 
 -- inits
 g_myscript_behavior_count = -1
 g_myscript_behavior_damagestateindex = -1
	
 -- load in byte code for behavior	
 local file = io.open(bytecodefile, "r")
 if file ~= nil then
  -- magic number for byte code file
  local magicnumber = tonumber(file:read())
  if magicnumber == 42 then
   -- version number
   local versionnumber = tonumber(file:read())
   -- number of states
   g_myscript_behavior_state_count = tonumber(file:read())-1
   -- number of instructions
   g_myscript_behavior_count = tonumber(file:read()) - 1
   if versionnumber >= 101 then
    -- basic state data
	g_myscript_behavior_state_caninterupt = {}
    for stateindex = 0 , g_myscript_behavior_state_count, 1 do
	 local name = file:read()
	 local allowinterupt = tonumber(file:read())
	 g_myscript_behavior_state_caninterupt[stateindex] = allowinterupt
    end 
    local stateseparator = file:read()
    -- basic instruction data
    for index = 0 , g_myscript_behavior_count, 1 do
     g_myscript_behavior[index] = {}
     g_myscript_behavior[index].uniquecode = tonumber(file:read())
     g_myscript_behavior[index].superstate = tonumber(file:read())
     g_myscript_behavior[index].conditiontype = tonumber(file:read())
     g_myscript_behavior[index].conditionparam1 = file:read()
     g_myscript_behavior[index].conditionparam2 = file:read()
     g_myscript_behavior[index].actiontype = tonumber(file:read())
     g_myscript_behavior[index].actionparam1 = file:read()
     g_myscript_behavior[index].actionparam2 = file:read()
     g_myscript_behavior[index].actionnewbehaviorindex = tonumber(file:read())-1
     g_myscript_behavior[index].elsebehaviorindex = tonumber(file:read())-1
     local separator = file:read()
    end 
   end
   if versionnumber >= 102 then
    -- damage state included?
	output_e["damagebehaviorindex"] = tonumber(file:read())-1
   end
  end
  -- finish reading file
  file:close()
  -- populate allowinterupt flag
  for index = 0 , g_myscript_behavior_count, 1 do
   stateindex = g_myscript_behavior[index].superstate - 1
   g_myscript_behavior[index].caninterupt = g_myscript_behavior_state_caninterupt[stateindex]
  end  
 end
  
 -- returns the number of instructions
 return g_myscript_behavior_count
 
end

return master_interpreter_core
