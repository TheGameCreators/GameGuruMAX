-- DESCRIPTION: When player enters zone, play <Sound0> and freeze the player, and stay frozen for [FREEZETIME=3000] milliseconds.

g_FreezePlayer = {}

function FreezePlayer_init(e)
 g_FreezePlayer[e] = {}
 FreezePlayer_properties(e,3000)
end

function FreezePlayer_properties(e, freezetime)
 g_FreezePlayer[e]['mode'] = 0
 g_FreezePlayer[e]['freezetime'] = freezetime
end

function FreezePlayer_main(e)
 if g_FreezePlayer[e]['freezetime'] ~= nil then
  if g_Entity[e]['plrinzone']==1 then
   if g_FreezePlayer[e]['mode'] == 0 then
    SetCameraOverride(3)
    g_FreezePlayer[e]['mode'] = Timer()
    PlaySound(e,0)
   end
  end
  if g_FreezePlayer[e]['mode'] > 0 then
   if Timer() > g_FreezePlayer[e]['mode'] + g_FreezePlayer[e]['freezetime'] then
    g_FreezePlayer[e]['mode'] = 0
    SetCameraOverride(0)
    Destroy(e)
   end
  end
 end
end
