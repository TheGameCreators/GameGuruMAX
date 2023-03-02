-- DESCRIPTION: This object will be treated as a switch object for activating other objects or game elements. Play the audio <Sound0> when the object is switched ON by the player, and <Sound1> when the object is switched OFF. Use the [SwitchedOn!=1] state to decide if the switch is initially off or on, and customize the [OnText$="To Turn Switch ON"] and [OffText$="To Turn Switch OFF"].

g_switch = {}

function switch_properties(e, switchedon, ontext, offtext)
 g_switch[e]['initialstate'] = switchedon
 g_switch[e]['ontext'] = ontext
 g_switch[e]['offtext'] = offtext
end 

function switch_init(e)
 g_switch[e] = {}
end

function switch_main(e)
 if g_switch[e]['ontext'] == nil then g_switch[e]['ontext'] = "To Turn Switch ON" end
 if g_switch[e]['offtext'] == nil then g_switch[e]['offtext'] = "To Turn Switch OFF" end
 if g_switch[e]['initialstate'] ~= nil then
  if g_switch[e]['initialstate'] >=0 then
   if g_switch[e]['initialstate'] == 0 then SetActivatedWithMP(e,101) end
   if g_switch[e]['initialstate'] == 1 then SetActivatedWithMP(e,201) end
  end
 end
 local PlayerDist = GetPlayerDistance(e)
 if PlayerDist < 90 and g_PlayerHealth > 0 then
  local LookingAt = GetPlrLookingAtEx(e,1)
  if LookingAt == 1 then
   if g_Entity[e]['activated'] == 0 then
    if g_switch[e]['ontext'] ~= "" then
     if GetGamePlayerStateXBOX() == 1 then
      PromptLocalForVR(e,"Y Button " .. g_switch[e]['ontext'],3)
     else
      if GetHeadTracker() == 1 then
       PromptLocalForVR(e,"Trigger " .. g_switch[e]['ontext'],3)
      else
       PromptLocalForVR(e,"E " .. g_switch[e]['ontext'],3)
      end
     end
    end
    if g_KeyPressE == 1 then
     SetActivatedWithMP(e,201)
    end
   else
    if g_Entity[e]['activated'] == 1 then
     if g_switch[e]['offtext'] ~= "" then
      if GetGamePlayerStateXBOX() == 1 then
       PromptLocalForVR(e,"Y Button " .. g_switch[e]['offtext'],3)
      else
       if GetHeadTracker() == 1 then
        PromptLocalForVR(e,"Trigger " .. g_switch[e]['offtext'],3)
       else
        PromptLocalForVR(e,"E " .. g_switch[e]['offtext'],3)
       end
      end
     end
     if g_KeyPressE == 1 then
      SetActivatedWithMP(e,101)
     end
    end
   end
  end
 end
 -- proximity independence
 if g_Entity[e]['activated'] == 100 then
  SetActivated(e,0)
 end
 if g_Entity[e]['activated'] == 101 then
  SetAnimationName(e,"off")
  PlayAnimation(e)
  if g_switch[e]['initialstate'] == -1 then 
   PerformLogicConnections(e)
   PlaySound(e,1) 
  end
  g_switch[e]['initialstate'] = -1
  SetActivated(e,102)
 end
 if g_Entity[e]['activated'] == 102 then
  if g_KeyPressE == 0 then SetActivated(e,0) end
 end
 if g_Entity[e]['activated'] == 201 then
  SetAnimationName(e,"on")
  PlayAnimation(e)
  if g_switch[e]['initialstate'] == -1 then 
   PerformLogicConnections(e)
   PlaySound(e,0) 
  end
  g_switch[e]['initialstate'] = -1
  SetActivated(e,202)
 end
 if g_Entity[e]['activated'] == 202 then
  if g_KeyPressE == 0 then SetActivated(e,1) end
 end  
end
