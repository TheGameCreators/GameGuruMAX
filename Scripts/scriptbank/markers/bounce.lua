-- Bounce v4 - thanks to Necrym59
-- DESCRIPTION: Will launch the player upwards at a specfic [HEIGHT=300]
-- DESCRIPTION: and forward at [#VELOCITY=3.0(0.1,10.0)]
-- DESCRIPTION: and play audio specified in <Sound0>.

local bounce = {}
local status	= {}
local wait		= {}

function bounce_properties(e, height, velocity)
	bounce[e] = g_Entity[e]
	bounce[e].height = height
	bounce[e].velocity = velocity
end

function bounce_init(e)
	bounce[e] = {}
	bounce[e].height = 200
	bounce[e].velocity = 1
	status[e] = "start"
	wait[e] = math.huge	
end

function bounce_main(e)
	if status[e] == "start" then
		if g_Entity[e].plrinzone == 1 then
			ttdeductspeed=(GetGamePlayerControlMaxspeed()*GetGamePlayerControlSpeedRatio())*GetTimeElapsed()*0.01
			SetGamePlayerControlSpeed(GetGamePlayerControlSpeed()-ttdeductspeed)		
			if GetGamePlayerControlJumpMode() == 0 and g_gameplayercontrol_powerjump == 0 then
				g_gameplayercontrol_powerjump = bounce[e].height
				PlaySound(e,0)
			end
			status[e] = "bounce"
			wait[e] = g_Time + 1000
		end
	end
	if status[e] == "bounce" then
		ForcePlayer(g_PlayerAngY,bounce[e].velocity/10)
		if g_Time > wait[e] then status[e] = "start" end
	end	
end