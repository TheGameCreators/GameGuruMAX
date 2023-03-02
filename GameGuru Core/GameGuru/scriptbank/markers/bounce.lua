-- Bounce v2 - thanks to Necrym59
-- DESCRIPTION: Will launch the player upwards at a specfic [HEIGHT=500] and forward [VELOCITY=1(1,20)] and play audio specified in <Sound0>.

g_bounce = {}
g_gameplayercontrol_powerjump = 0

function bounce_init(e)
	g_bounce[e] = {}
	bounce_properties(e,500,1)
end

function bounce_properties(e, height, velocity)
	g_bounce[e]['height'] = height
	g_bounce[e]['velocity'] = velocity
end

function bounce_main(e)
	if g_Entity[e]['plrinzone']==1 then
		if GetGamePlayerControlJumpMode() == 0 and g_gameplayercontrol_powerjump == 0 then
			g_gameplayercontrol_powerjump = g_bounce[e]['height']
			ttdeductspeed=(GetGamePlayerControlMaxspeed()*GetGamePlayerControlSpeedRatio())*GetTimeElapsed()*0.01
			SetGamePlayerControlSpeed(GetGamePlayerControlSpeed()-ttdeductspeed)
			ForcePlayer(g_PlayerAngY,g_bounce[e]['velocity'])
			PlaySound(e,0)
		end  
	end
end