-- DESCRIPTION: This object is treated as a key object for unlocking doors. Play the audio <Sound0> when the object is picked up by the player.

g_key = {}

function key_properties(e, speech)
 g_key[e]['speech'] = speech
end 

function key_init(e)
 g_key[e] = {}
 g_key[e]['speech'] = ""
end

function key_main(e)
 local PlayerDist = GetPlayerDistance(e)
 if PlayerDist < 100 and g_PlayerHealth > 0 then
  local LookingAt = GetPlrLookingAtEx(e,1)
  if LookingAt == 1 then
   if GetGamePlayerStateXBOX() == 1 then
    PromptLocalForVR(e,"Y button to take key",3)
   else
    if GetHeadTracker() == 1 then
     PromptLocalForVR(e,"Trigger to take key",3)
    else
     PromptLocalForVR(e,"E to take key",3)
    end
   end
   if g_KeyPressE == 1 then
    PromptLocalForVR(e,"Collected key",3)
    PlaySound(e,0)
    --Collected(e)
    Destroy(e)
    PerformLogicConnections(e)
   end
  end
 end
end
