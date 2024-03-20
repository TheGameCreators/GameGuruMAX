-- Heal v4
-- DESCRIPTION: Heals the player over time or once only when they enter the zone.
-- DESCRIPTION: Set the Hurt [AMOUNT=1(1-20)] and the [TIMING=1(1,10)] and the [ZONEHEIGHT=100(1,500)]
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: [HealOnceOnly!=0] to heal once only then zone is destroyed.
-- DESCRIPTION: <Sound0> for when entering the zone

module_activationcontrol = require "scriptbank\\module_activationcontrol"

local heal 			= {}
local amount 		= {}
local timing 		= {}
local zoneheight	= {}
local SpawnAtStart	= {}
local HealOnceOnly	= {}

local wait			= {}
local heal_now		= {}
local doonce		= {}
local status		= {}

function heal_properties(e, amount, timing, zoneheight, SpawnAtStart, HealOnceOnly)
	heal[e] = g_Entity[e]
	heal[e]['amount'] = amount
	heal[e]['timing'] = timing
	heal[e]['zoneheight'] = zoneheight or 100
	heal[e]['SpawnAtStart'] = SpawnAtStart
	heal[e]['HealOnceOnly'] = HealOnceOnly	
end

function heal_init(e)
	heal[e] = {}
	module_activationcontrol.init(e,1)
	heal[e]['amount'] = 1
	heal[e]['timing'] = 100
	heal[e]['zoneheight'] = 100
	heal[e]['SpawnAtStart'] = 1
	heal[e]['HealOnceOnly'] = 0
	
	StartTimer(e)
	wait[e] = math.huge
	heal_now[e] = 0
	doonce[e] = 0
	status[e] = "init"
end

function heal_main(e)

	if status[e] == "init" then
		if heal[e]['SpawnAtStart'] == 1 then SetActivated(e,1) end
		if heal[e]['SpawnAtStart'] == 0 then SetActivated(e,0) end
		heal[e]['timing'] = heal[e]['timing']*1000
		status[e] = "endinit"
	end
	if g_Entity[e]['activated'] == 1 then
		if g_Entity[e]['plrinzone']==1 and g_PlayerHealth > 0 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y'] + heal[e]['zoneheight'] then				
			if heal[e]['HealOnceOnly'] == 0 then
				if GetTimer(e) > heal[e]['timing'] then
					SetPlayerHealth(g_PlayerHealth+heal[e]['amount'])
					if g_PlayerHealth > g_PlayerStartStrength then g_PlayerHealth = g_PlayerStartStrength end
					SetPlayerHealthCore(g_PlayerHealth)					
					StartTimer(e)
					if heal_now[e] == 0 then
						PlaySoundIfSilent(e,0) 
						heal_now[e] = Timer()
					end
				end
			end	
			if heal[e]['HealOnceOnly'] == 1 then
				if GetTimer(e) > heal[e]['timing'] then
					SetPlayerHealth(g_PlayerHealth+heal[e]['amount'])
					if g_PlayerHealth > g_PlayerStartStrength then g_PlayerHealth = g_PlayerStartStrength end
					SetPlayerHealthCore(g_PlayerHealth)
					StartTimer(e)
					if heal_now[e] == 0 then
						PlaySoundIfSilent(e,0) 
						heal_now[e] = Timer()
						wait[e] = heal_now[e] + heal[e]['timing']/3
					end
				end
				if g_Time > wait[e] then						
					Destroy(e)
				end	
			end
		else
			if Timer() > heal_now[e] + heal[e]['timing'] then
				heal_now[e] = 0
			end
		end
		
		if g_Entity[e]['plrinzone'] == 0 and g_Time > wait[e] then
			Destroy(e)
		end
	end
end