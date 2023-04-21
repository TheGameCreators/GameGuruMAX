-- DESCRIPTION: When switched ON and the player enters this zone, <Sound0> will play and the level is complete. [ZONEHEIGHT=100] controls how far above the zone the player can be before the zone is not triggered. [@GoToLevelMode=1(1=Use Storyboard Logic,2=Go to Specific Level)] controls whether the next level in the Storyboard, or another level is loaded after entry to the zone.

g_winzones = {}
g_zoneswitch = {}

function switched_winzone_init(e)
g_winzones[e] = {}
g_winzones[e]['zoneheight'] = 100
end

function switched_winzone_properties(e, zoneheight)
g_winzones[e]['zoneheight'] = zoneheight
end

function switched_winzone_main(e)
	if g_winzones[e]['zoneheight'] == nil then g_winzones[e]['zoneheight'] = 100 end
	if g_winzones[e]['zoneheight'] ~= nil then
		if g_winzones[e]['zoneheight'] == 1 then g_winzones[e]['zoneheight'] = 100 end	
		if g_Entity[e]['plrinzone']==1 and g_PlayerPosY+65 > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+g_winzones[e]['zoneheight'] and g_zoneswitch == 1 then
			PlaySound(e,0)
			Destroy(e)
			PerformLogicConnections(e)
			JumpToLevelIfUsed(e)
			g_zoneswitch = 0
		end
	end
end

