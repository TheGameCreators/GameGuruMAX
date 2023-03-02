-- DESCRIPTION: Hurts the player when they enter the zone and initially plays <Sound0> when entering the zone.
-- DESCRIPTION: Set the Hurt [AMOUNT=1(1-20)] and the [TIMING=100(1,1000)] and the [ZONEHEIGHT=100(1,500)]
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
module_activationcontrol = require "scriptbank\\module_activationcontrol"

g_hurt_once = {}

local hurt 			= {}
local amount 		= {}
local timing 		= {}
local zoneheight	= {}
local SpawnAtStart	= {}
local status		= {}

function hurt_properties(e, amount, timing, zoneheight, SpawnAtStart)
	hurt[e] = g_Entity[e]
	hurt[e]['amount'] = amount
	hurt[e]['timing'] = timing
	hurt[e]['zoneheight'] = zoneheight or 100
	hurt[e]['SpawnAtStart'] = SpawnAtStart
end

function hurt_init(e)
	hurt[e] = g_Entity[e]
	module_activationcontrol.init(e,1)
	g_hurt_once[e] = 0
	hurt[e]['amount'] = 1
	hurt[e]['timing'] = 100
	hurt[e]['zoneheight'] = 100
	hurt[e]['SpawnAtStart'] = 1
	StartTimer(e)
	status[e] = "init"
end

function hurt_main(e)
	hurt[e] = g_Entity[e]
	if status[e] == "init" then
		if hurt[e]['SpawnAtStart'] == 1 then SetActivated(e,1) end
		if hurt[e]['SpawnAtStart'] == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end
	if g_Entity[e]['activated'] == 1 then
		if module_activationcontrol.control(e) == 1 then
			if g_Entity[e]['plrinzone']==1 and g_PlayerHealth > 0 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y'] + hurt[e]['zoneheight'] then
				if GetTimer(e) > hurt[e]['timing'] then
					HurtPlayer(-1,hurt[e]['amount'])
					StartTimer(e)
					if g_hurt_once[e] == 0 then
						PlaySoundIfSilent(e,0) 
						g_hurt_once[e] = Timer()
					end
				end
			else
				if Timer() > g_hurt_once[e] + 3000 then
					g_hurt_once[e] = 0
				end
			end
		else
			g_hurt_once[e] = 0
		end
	end
end
