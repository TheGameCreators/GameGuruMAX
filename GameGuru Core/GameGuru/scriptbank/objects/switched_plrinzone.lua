-- DESCRIPTION: Plays <Sound0> when player enters the zone. Removes the zone afterwards so should be used to trigger an event or play a one-time-only sound. [ZONEHEIGHT=100] controls how far above the zone the player can be before the zone is not triggered.

g_plrinzone = {}
g_zoneswitch = {}

function plrinzone_init(e)
g_plrinzone[e] = {}
g_plrinzone[e]['zoneheight'] = 100
g_plrinzone[e]['active'] = 1
end

function plrinzone_properties(e, zoneheight)
g_plrinzone[e]['zoneheight'] = zoneheight
end

function plrinzone_main(e)
 if g_Entity[e]['plrinzone']==1 and g_plrinzone[e]['active'] ~= nil and g_plrinzone[e]['active'] == 1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+g_plrinzone[e]['zoneheight'] and g_zoneswitch == 1 then
  PlaySound(e,0)
  Destroy(e)
  ActivateIfUsed(e)
  PerformLogicConnections(e)
  g_plrinzone[e]['active'] = 0
  g_zoneswitch = 0
 end
end
