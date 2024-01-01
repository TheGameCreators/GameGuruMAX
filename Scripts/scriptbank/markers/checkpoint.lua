-- DESCRIPTION: Plays <Sound0> when player enters this checkpoint zone. Removes the zone afterwards and restores the player to this location when the player dies. [ZONEHEIGHT=100] controls how far above the zone the player can be before the zone is not triggered.

g_checkpoint = {}

function checkpoint_init(e)
 g_checkpoint[e] = {}
 g_checkpoint[e]['zoneheight'] = 100
end

function checkpoint_properties(e, zoneheight)
 g_checkpoint[e]['zoneheight'] = zoneheight
end

function checkpoint_main(e)
 if g_checkpoint[e]['zoneheight'] == nil then g_checkpoint[e]['zoneheight'] = 100 end
 if g_checkpoint[e]['zoneheight'] ~= nil then
  if g_Entity[e]['plrinzone']==1 and g_PlayerPosY+65 > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+g_checkpoint[e]['zoneheight'] then
   Checkpoint(e)
   PlaySound(e,0)
   Destroy(e)
   PerformLogicConnections(e)
  end
 end
end
