-- DESCRIPTION: heals the player when they enter the zone and initially plays <Sound0> when entering the zone.
-- DESCRIPTION: Set the heal [AMOUNT=1(1-20)] and the [TIMING=100(1,1000)] and the [ZONEHEIGHT=100(1,500)]
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
module_activationcontrol = require "scriptbank\\module_activationcontrol"

g_heal_once = {}

local heal 			= {}
local amount 		= {}
local timing 		= {}
local zoneheight	= {}
local SpawnAtStart	= {}
local status		= {}

function heal_properties(e, amount, timing, zoneheight, SpawnAtStart)
	heal[e] = g_Entity[e]
	heal[e]['amount'] = amount
	heal[e]['timing'] = timing
	heal[e]['zoneheight'] = zoneheight or 100
	heal[e]['SpawnAtStart'] = SpawnAtStart
end

function heal_init(e)
	heal[e] = g_Entity[e]
	module_activationcontrol.init(e,1)
	g_heal_once[e] = 0
	heal[e]['amount'] = 1
	heal[e]['timing'] = 100
	heal[e]['zoneheight'] = 100
	heal[e]['SpawnAtStart'] = 1
	StartTimer(e)
	status[e] = "init"
end

function heal_main(e)
	heal[e] = g_Entity[e]
	if status[e] == "init" then
		if heal[e]['SpawnAtStart'] == 1 then SetActivated(e,1) end
		if heal[e]['SpawnAtStart'] == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end
	if g_Entity[e]['activated'] == 1 then
		if module_activationcontrol.control(e) == 1 then
			if g_Entity[e]['plrinzone']==1 and g_PlayerHealth > 0 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y'] + heal[e]['zoneheight'] then
				if g_PlayerHealth < g_gameloop_StartHealth then
					if GetTimer(e) > heal[e]['timing'] then
						SetPlayerHealth(g_PlayerHealth+heal[e]['amount'])
						StartTimer(e)
						if g_heal_once[e] == 0 then
							PlaySoundIfSilent(e,0) 
							g_heal_once[e] = Timer()
						end
					end
				end
			else
				if Timer() > g_heal_once[e] + 3000 then
					g_heal_once[e] = 0
				end
			end
		else
			g_heal_once[e] = 0
		end
	end
end
