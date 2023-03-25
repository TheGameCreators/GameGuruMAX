-- Proximity Mine -- by Dave Hawkins --  v3 - modified by Necrym 59
-- DESCRIPTION:A landmine that will detonate when a player or NPC is in range. The mine can be disabled with the E key when close.
-- DESCRIPTION:Proximity [#RANGE=90(0,500)]
-- DESCRIPTION:Countdown [#DETONATION_TIME=2500(0,20000)]

proximine = {}
proximine_started = {}
proximine_active = {}

function proximity_mine_properties(e, range, detonation_time)
	proximine[e] = g_Entity[e]
	proximine[e].range = range
	proximine[e].detonation_time = detonation_time
end 

function proximity_mine_init(e,detonation_time)
	proximine[e] = g_Entity[e]
	proximine[e].range = 90
	proximine[e].detonation_time = 2500
	proximine_started[e] = 0
	proximine_active[e] = 1
end

function proximity_mine_main(e)
	PlayerDist = GetPlayerDistance(e)
	if g_Entity[e]['health'] > 0 then
		if proximine_started[e] == 0 then
			-- characters or player can trigger proximity mine
			for a = 1, 50000 do
				if ai_soldier_state[a] ~= nil then
					if g_Entity[a] ~= nil then
						if g_Entity[a]['health'] > 0 then
							MineDX = g_Entity[a]['x'] - g_Entity[e]['x'];
							MineDY = g_Entity[a]['y'] - g_Entity[e]['y'];
							MineDZ = g_Entity[a]['z'] - g_Entity[e]['z'];
							MineDist = math.sqrt(math.abs(MineDX*MineDX)+math.abs(MineDY*MineDY)+math.abs(MineDZ*MineDZ));
							if MineDist < proximine[e].range then
								proximine_started[e] = GetTimer(e)+proximine[e].detonation_time
								if proximine_active[e] == 1 then
									PlaySound(e,0)
								end
							end 
						end
					end
				end
			end
			if PlayerDist < proximine[e].range and proximine_active[e] == 1 then 
				proximine_started[e] = GetTimer(e)+proximine[e].detonation_time
				PlaySound(e,0)
			end
		end
	end
	if proximine_started[e]>0 then	
	local LookingAt = GetPlrLookingAtEx(e,1)
		if LookingAt == 1 and proximine_active[e] == 1  then
			Prompt("Press E to disarm")
			if g_KeyPressE == 1 then
				StopSound(e)
				proximine_started[e] = GetTimer(e)+proximine[e].detonation_time
				proximine_active[e] = 0
			end
		end
		if GetTimer(e) > proximine_started[e] and proximine_active[e] == 1 then
			StopSound(e) 
			proximine_active[e]=0
			proximine_started[e]=0
			SetEntityHealth(e,0)	
		end
	end
end
