-- DESCRIPTION: Will set 'g_sendpulse_live=1' when object animates frame 51 or over.

g_sendpulse_live = -1

function sendpulse_init(e)
 g_sendpulse_live = -1
end

function sendpulse_main(e)
 if g_sendpulse_live == -1 then
  SetAnimationFrames(GetEntityAnimStart(GetEntityElementBankIndex(e),0),GetEntityAnimFinish(GetEntityElementBankIndex(e),0))
  LoopAnimation(e)
  g_sendpulse_live = 0
 else
  PulseObjNo = g_Entity[e]['obj']
  if PulseObjNo > 0 then
   if GetAnimationFrame(e) > 50 then
    g_sendpulse_live = 1
   else
    g_sendpulse_live = 0
   end
  end
 end
end
