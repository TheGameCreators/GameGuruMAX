-- Random Sound v1 by Necrym59
-- DESCRIPTION: Plays the audio from anyone of 4 random sound resources when the player enters this zone.
-- DESCRIPTION: [ZONEHEIGHT=100] controls how far above the zone the player can be before the zone is not triggered.
-- DESCRIPTION: <Sound0>
-- DESCRIPTION: <Sound1>
-- DESCRIPTION: <Sound2>
-- DESCRIPTION: <Sound3>

	local randomsound = {}
	local soundno	  = {}
	local played	  = {}
	local status	  = {}

function random_sound_properties(e, soundno, zoneheight)
	randomsound[e]['soundno'] = soundno
	randomsound[e]['zoneheight'] = zoneheight
end

function random_sound_init(e)
	randomsound[e] = {}
	randomsound[e]['soundno'] = 1
	randomsound[e]['zoneheight'] = 100
	played[e] = 0
	status[e] = 'init'
end

function random_sound_main(e)
	if status[e] == 'init' then
		randomsound[e]['soundno'] = math.random(0,3)
		played[e] = 0
		status[e] = 'done'
	end

	if randomsound[e]['zoneheight'] == nil then randomsound[e]['zoneheight'] = 100 end
	if randomsound[e]['zoneheight'] ~= nil then
		if g_Entity[e]['plrinzone']==1 and g_PlayerPosY+65 > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+randomsound[e]['zoneheight'] then			
			if played[e] == 0 then
				PlaySound(e,randomsound[e]['soundno'])
				played[e] = 1
			end	
		end		
		if g_Entity[e]['plrinzone']==0 and played[e]==1 then status[e] = 'init' end
		ActivateIfUsed(e)
		PerformLogicConnections(e)
	end
end
