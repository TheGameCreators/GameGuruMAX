-- videoinzone v5 by Lee and Necrym59
-- DESCRIPTION: When the player enters this zone the video from <Video Slot>.
-- DESCRIPTION: [ZONEHEIGHT=100] controls how far above the zone the player can be before the zone is not triggered.
-- DESCRIPTION: [@ALLOW_SKIP=1(1=Yes, 2=No)]
-- DESCRIPTION: [@PAUSE_AMBIENT_TRACK=1(1=Yes, 2=No)]

g_videoinzone_mode = {}
g_videoinzone = {}

function videoinzone_init(e)
	g_videoinzone_mode[e] = 0
	g_videoinzone[e] = {}
	g_videoinzone[e]['zoneheight'] = 100
	g_videoinzone[e]['allow_skip'] = 1
	g_videoinzone[e]['pause_ambient_track'] = 1	
end

function videoinzone_properties(e, zoneheight, allow_skip, pause_ambient_track)
	g_videoinzone[e]['zoneheight'] = zoneheight
	if allow_skip ~=1 and allow_skip ~=2 then allow_skip = 1 end
	g_videoinzone[e]['allow_skip'] = allow_skip
	g_videoinzone[e]['pause_ambient_track'] = pause_ambient_track		
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
					if g_videoinzone[e]['pause_ambient_track'] == 1 then StopAmbientMusicTrack() end
					if g_videoinzone[e]['allow_skip'] == 1 then PromptVideo(e,1) end
					if g_videoinzone[e]['allow_skip'] == 2 then PromptVideoNoSkip(e,1) end
					g_videoinzone_mode[e] = 2
				else 
					if g_videoinzone_mode[e] == 2 then
						if radar_showallsprites ~= nil then radar_showallsprites() end
						ShowHuds()
						PerformLogicConnections(e)
						if g_videoinzone[e]['pause_ambient_track'] == 1 then StartAmbientMusicTrack() end
						g_videoinzone_mode[e] = 3 
						Destroy(e)
					end
				end
			end
		end
	end
end
