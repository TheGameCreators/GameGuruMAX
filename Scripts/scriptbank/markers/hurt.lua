-- Hurt v4
-- DESCRIPTION: Hurts the player over time or once only when they enter the zone.
-- DESCRIPTION: Set the Hurt [AMOUNT=1(1-20)] and the [TIMING=1(1,10)] and the [ZONEHEIGHT=100(1,500)]
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: [HurtOnceOnly!=0] to hurt once only then zone is destroyed.
-- DESCRIPTION: <Sound0> for when entering the zone

local hurt 			= {}
local amount 		= {}
local timing 		= {}
local zoneheight	= {}
local SpawnAtStart	= {}
local HurtOnceOnly	= {}
local doonce 		= {}
local triggered		= {}
local wait			= {}
local hurt_now 		= {}
local status		= {}

function hurt_properties(e, amount, timing, zoneheight, SpawnAtStart, HurtOnceOnly)
	hurt[e] = g_Entity[e]
	hurt[e]['amount'] = amount
	hurt[e]['timing'] = timing
	hurt[e]['zoneheight'] = zoneheight or 100
	hurt[e]['SpawnAtStart'] = SpawnAtStart
	hurt[e]['HurtOnceOnly'] = HurtOnceOnly
end

function hurt_init(e)
	hurt[e] = g_Entity[e]
	hurt[e]['amount'] = 1
	hurt[e]['timing'] = 1
	hurt[e]['zoneheight'] = 100
	hurt[e]['SpawnAtStart'] = 1
	hurt[e]['HurtOnceOnly'] = 0
	StartTimer(e)
	wait[e] = math.huge
	hurt_now[e] = 0
	doonce[e] = 0
	status[e] = "init"
end

function hurt_main(e)
	hurt[e] = g_Entity[e]
	if status[e] == "init" then
		if hurt[e]['SpawnAtStart'] == 1 then SetActivated(e,1) end
		if hurt[e]['SpawnAtStart'] == 0 then SetActivated(e,0) end
		hurt[e]['timing'] = hurt[e]['timing']*1000
		status[e] = "endinit"
	end
	if g_Entity[e]['activated'] == 1 then
		if g_Entity[e]['plrinzone']==1 and g_PlayerHealth > 0 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y'] + hurt[e]['zoneheight'] then				
			if hurt[e]['HurtOnceOnly'] == 0 then
				if GetTimer(e) > hurt[e]['timing'] then
					HurtPlayer(-1,hurt[e]['amount'])
					StartTimer(e)
					if hurt_now[e] == 0 then
						PlaySoundIfSilent(e,0) 
						hurt_now[e] = Timer()
					end
				end
			end	
			if hurt[e]['HurtOnceOnly'] == 1 then
				if GetTimer(e) > hurt[e]['timing'] then
					HurtPlayer(-1,hurt[e]['amount'])
					StartTimer(e)
					if hurt_now[e] == 0 then
						PlaySoundIfSilent(e,0) 
						hurt_now[e] = Timer()
						wait[e] = hurt_now[e] + hurt[e]['timing']/3
					end
				end
				if g_Time > wait[e] then						
					Destroy(e)
				end	
			end
		else
			if Timer() > hurt_now[e] + hurt[e]['timing'] then
				hurt_now[e] = 0
			end
		end
		
		if g_Entity[e]['plrinzone']==0 and g_Time > wait[e] then
			Destroy(e)
		end
	end
end
