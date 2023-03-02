-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Electrocute v7
-- DESCRIPTION: Will elecrocute player when activated from a trigger Zone and plays <Sound0>
-- DESCRIPTION: [PROMPT_TEXT$=""]
-- DESCRIPTION: [DAMAGE=10(1,500)]

	local electrocute 	= {}
	local prompt_text 	= {}
	local damage 		= {}
	local shock_time	= {}
	local zoneheight	= {}

function electrocute_properties(e, prompt_text, damage, zoneheight)
	electrocute[e] = g_Entity[e]
	electrocute[e].prompt_text = prompt_text
	electrocute[e].damage = damage
	electrocute[e].zoneheight = zoneheight or 100
end

function electrocute_init(e)
	electrocute[e] = g_Entity[e]
	electrocute[e].prompt_text = ""
	electrocute[e].damage = 10
	electrocute[e].zoneheight = 100
	shock_time[e] = 0
	StartTimer(e)
end

function electrocute_main(e)
	electrocute[e] = g_Entity[e]

	if g_Entity[e]['plrinzone'] == 1 and g_PlayerHealth > 0 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+electrocute[e].zoneheight then
		Prompt(electrocute[e].prompt_text)
		if GetTimer(e) > 100 then
			HurtPlayer(e,electrocute[e].damage)
			StartTimer(e)
			GamePlayerControlAddShakeTrauma(7.5)
			GamePlayerControlAddShakePeriod(0.8)
			GamePlayerControlAddShakeFade  (1.60)
			GamePlayerControlSetShakeTrauma(18.9)
			GamePlayerControlSetShakePeriod(1.0)
			if shock_time[e] == 0 then
				PlaySound(e,0)
				shock_time[e] = Timer()
			end
		else
			GamePlayerControlAddShakeTrauma(0.0)
			GamePlayerControlAddShakePeriod(0.0)
			GamePlayerControlAddShakeFade  (1.0)
			GamePlayerControlSetShakeTrauma(0.0)
			GamePlayerControlSetShakePeriod(0.0)
		end
	else
		if Timer() > shock_time[e] then
			shock_time[e] = 0
		end
		if g_PlayerHealth >= 1 then
			StopSound(e,0)
		end
		if g_PlayerHealth <= 1 then
			StopSound(e,0)
			GamePlayerControlAddShakeTrauma(0.0)
			GamePlayerControlAddShakePeriod(0.0)
			GamePlayerControlAddShakeFade  (1.0)
			GamePlayerControlSetShakeTrauma(0.0)
			GamePlayerControlSetShakePeriod(0.0)
			SetGamePlayerControlCameraShake( 0 )
			StartTimer(e)
		end
	end
end --main

function electrocute_exit(e)
end


