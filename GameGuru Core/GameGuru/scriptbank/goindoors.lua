-- DESCRIPTION: Hides the terrain and water when player enters the zone, and activates the <If Used> field.

function goindoors_init(e)
end

function goindoors_main(e)
 if g_Entity[e]['plrinzone']==1 then
  HideTerrain()
  HideWater()
  ActivateIfUsed(e)
 end
end
