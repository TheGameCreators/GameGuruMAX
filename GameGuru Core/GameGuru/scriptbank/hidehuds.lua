-- DESCRIPTION: Use this script as a zone behavior if you want the in-game experience to hide all the HUDs on display.

function hidehuds_init(e)
end

function hidehuds_main(e)
 if g_Entity[e]['plrinzone']==1 then
  HideHuds()
  Destroy(e)
 end
end
