-- DESCRIPTION: When the player enters this zone any zones with the same script which is currently playing ambience/background music/sounds stops and audio from this <Sound0> is played. [ZONEHEIGHT=100] controls how far above the zone the player can be before the zone is not triggered.

g_ambienceinzone_soundlooping = 0
g_ambienceonceinzone = {}

function ambienceonceinzone_init(e)
 g_ambienceonceinzone[e] = {}
 g_ambienceonceinzone[e]['zoneheight'] = 100
end

function ambienceonceinzone_properties(e, zoneheight)
 g_ambienceonceinzone[e]['zoneheight'] = zoneheight
end

function ambienceonceinzone_main(e)
 if g_ambienceonceinzone[e]['zoneheight'] == nil then g_ambienceonceinzone[e]['zoneheight'] = 100 end
 if g_ambienceonceinzone[e]['zoneheight'] ~= nil then
  if g_Entity[e]['plrinzone']==1 and g_PlayerPosY+65 > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+g_ambienceonceinzone[e]['zoneheight'] then
   if g_ambienceinzone_soundlooping ~= e then
    SetSoundMusicMode(GetEntityRawSound(g_ambienceinzone_soundlooping,0),0)
    StopSound(g_ambienceinzone_soundlooping,0)
    PlayNon3DSound(e,0)
    SetSoundMusicMode(GetEntityRawSound(e,0),1)
    PerformLogicConnections(e)
    g_ambienceinzone_soundlooping = e
   end
  end
 end
end
