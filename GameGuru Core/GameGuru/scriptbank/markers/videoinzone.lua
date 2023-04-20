-- videoinzone v2
-- DESCRIPTION: When the player enters this zone the video from <Video Slot>.
-- DESCRIPTION: [ZONEHEIGHT=100] controls how far above the zone the player can be before the zone is not triggered.
-- DESCRIPTION: [@NO_SKIP=2(1=Yes, 2=No)]

g_videoinzone_mode = {}
g_videoinzone = {}

function videoinzone_init(e)
 g_videoinzone_mode[e] = 0
 g_videoinzone[e] = {}
 g_videoinzone[e]['zoneheight'] = 100
 g_videoinzone[e]['no_skip'] = 2
end

function videoinzone_properties(e, zoneheight, no_skip)
 g_videoinzone[e]['zoneheight'] = zoneheight
 g_videoinzone[e]['no_skip'] = no_skip
end

function videoinzone_main(e)
 if g_Entity[e]['plrinzone']==1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+g_videoinzone[e]['zoneheight'] then
  if g_videoinzone_mode[e] ~= nil then
   if g_videoinzone_mode[e] == 0 then
    g_videoinzone_mode[e] = 1
    HideHuds()
	if radar_hideallsprites ~= nil then radar_hideallsprites() end
   else
    if g_videoinzone_mode[e] == 1 then
     --PlaySound(e,0)
	 if g_videoinzone[e]['no_skip'] == 1 then PromptVideoNoSkip(e,1) end
	 if g_videoinzone[e]['no_skip'] == 2 then PromptVideo(e,1) end
  	 g_videoinzone_mode[e] = 2
    else 
     if g_videoinzone_mode[e] == 2 then
  	  if radar_showallsprites ~= nil then radar_showallsprites() end
      ShowHuds()
      Destroy(e)
      PerformLogicConnections(e)
      --StopSound(e,0) 
 	  g_videoinzone_mode[e] = 3 
 	 end
    end
   end
  end
 end
end
