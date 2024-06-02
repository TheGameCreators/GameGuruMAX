-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Electrocute v11
-- DESCRIPTION: Will elecrocute player when entering the zone
-- DESCRIPTION: [PROMPT_TEXT$=""]
-- DESCRIPTION: [DAMAGE=10(1,500)]
-- DESCRIPTION: [ZONEHEIGHT=100(0,1000)]
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: <Sound0> for sound effect

local electrocute 	= {}
local prompt_text 	= {}
local damage 		= {}
local zoneheight	= {}
local SpawnAtStart	= {}

local shock_time	= {}
local status		= {}

function electrocute_properties(e, prompt_text, damage, zoneheight, SpawnAtStart)
	electrocute[e].prompt_text = prompt_text
	electrocute[e].damage = damage
	electrocute[e].zoneheight = zoneheight or 100
	electrocute[e].SpawnAtStart = SpawnAtStart
end

function electrocute_init(e)
	electrocute[e] = {}
	electrocute[e].prompt_text = ""
	electrocute[e].damage = 10
	electrocute[e].zoneheight = 100
	electrocute[e].SpawnAtStart = 1
	shock_time[e] = 0
	status[e] = "init"
	StartTimer(e)
end

function electrocute_main(e)
	
	if status[e] == "init" then
		if electrocute[e].SpawnAtStart == 1 then SetActivated(e,1) end
		if electrocute[e].SpawnAtStart == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end

	if g_Entity[e]['activated'] == 1 then
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
	end
end

function electrocute_exit(e)
end


