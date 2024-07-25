-- DESCRIPTION: When the player enters this zone any zones with the same script which are currently playing ambience/background music/sounds stops and audio from this <Sound0> is looped. [!OnlyPlayInZone=0] will cause the sound to stop playing when the player leaves the zone. [ZONEHEIGHT=100] controls how far above the zone the player can be before the zone is not triggered.

g_ambienceinzone_soundlooping = 0
g_ambienceinzone = {}

function ambienceinzone_init(e)
 g_ambienceinzone[e] = {}
 g_ambienceinzone[e]['zoneheight'] = 100
end

function ambienceinzone_properties(e, onlyplayinzone, zoneheight)
 g_ambienceinzone[e]['onlyplayinzone'] = onlyplayinzone
 g_ambienceinzone[e]['zoneheight'] = zoneheight
end 

function ambienceinzone_main(e)
 if g_PlayerHealth <= 0 then
  g_ambienceinzone_soundlooping = 0
 end
 if g_ambienceinzone[e]['zoneheight'] == nil then g_ambienceinzone[e]['zoneheight'] = 100 end
 if g_ambienceinzone[e]['zoneheight'] ~= nil then
  if g_Entity[e]['plrinzone']==1 and g_PlayerPosY+65 > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+g_ambienceinzone[e]['zoneheight'] then
   if g_ambienceinzone_soundlooping ~= e then
    SetSoundMusicMode(GetEntityRawSound(g_ambienceinzone_soundlooping,0),0)
    StopSound(g_ambienceinzone_soundlooping,0)
    LoopNon3DSound(e,0)
    SetSoundMusicMode(GetEntityRawSound(e,0),1)
    PerformLogicConnections(e)
    g_ambienceinzone_soundlooping = e
   end
  elseif g_Entity[e]['plrinzone']==0 then
   if g_ambienceinzone[e]['onlyplayinzone'] == 1 then
    if g_ambienceinzone_soundlooping == e then
     SetSoundMusicMode(GetEntityRawSound(g_ambienceinzone_soundlooping,0),0)
     StopSound(g_ambienceinzone_soundlooping,0)
     g_ambienceinzone_soundlooping = 0
    end
   end
  end
 end
end
