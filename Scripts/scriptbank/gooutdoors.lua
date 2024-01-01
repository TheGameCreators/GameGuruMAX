-- DESCRIPTION: Shows the terrain and water if they were previously hidden (by goindoors.lua). Also activates the <If Used> property.

function gooutdoors_init(e)
end

function gooutdoors_main(e)
 if g_Entity[e]['plrinzone']==1 then
  ShowTerrain()
  ShowWater()
  ActivateIfUsed(e)
 end
end
