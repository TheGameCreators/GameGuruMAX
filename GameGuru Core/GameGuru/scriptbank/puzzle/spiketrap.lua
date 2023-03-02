-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Spiketrap v4
-- DESCRIPTION: Walk on spike trap.
-- DESCRIPTION: When the player is within [RANGE=50(10,100)] the spiketrap is triggered after [TRAP_DELAY=2(1,10)] causing [TRAP_DAMAGE=25(0,100)](0=Kill), can be triggered [@TRAP_REPEAT=1(1=Once only, 2=Multiple)]. Time for [TRAP_RESET=300] and set the [TRAP_SPEED=100]. Change the trap [@VISABILITY=1(1=Visible, 2=Invisible)] Trap will play <Sound0> on activation and <Sound1> on reset. 


	g_spiketrap = {}
	local range = {}
	local trap_delay = {}
	local trap_damage = {}
	local trap_repeat = {}
	local trap_reset = {}
	local trap_speed = {}
	local state = {}
	local hit = {}
	

function spiketrap_properties(e, range, trap_delay, trap_damage, trap_repeat, trap_reset, trap_speed, visability)
	g_spiketrap[e] = g_Entity[e]	
	g_spiketrap[e]['range'] = range					-- how close player can get before trap triggers
	g_spiketrap[e]['trap_delay'] = trap_delay		-- how long after player enters trap range til trap really triggers (player reaction escape?)
	g_spiketrap[e]['trap_damage'] = trap_damage		-- how much trap_damage the trap will apply to the player (set to 0 to automatically kill player)
	g_spiketrap[e]['trap_repeat'] = trap_repeat		-- if trap can be triggered more than once (set to 1 for 1 time only trap)
	g_spiketrap[e]['trap_reset'] = trap_reset		-- how long after trap has activated til it will reset (if trap_repeat)
	g_spiketrap[e]['trap_speed'] = trap_speed		-- adjusts how fast the spikes move
	g_spiketrap[e]['visability'] = visability		-- turns trap visible/invisible
end 

function spiketrap_init(e)
	g_spiketrap[e] = g_Entity[e]	
	g_spiketrap[e]['range'] = 100
	g_spiketrap[e]['trap_delay'] = 1
	g_spiketrap[e]['trap_damage'] = 25
	g_spiketrap[e]['trap_repeat'] = 1
	g_spiketrap[e]['trap_reset'] = 300
	g_spiketrap[e]['trap_speed'] = 100
	g_spiketrap[e]['visability'] = 1	
	state[e] = "init"
	hit[e] = 0
end

function spiketrap_main(e)
	if state[e] == "init" then
		if g_spiketrap[e]['trap_repeat'] == nil then g_spiketrap[e]['trap_repeat'] = 1 end
		if g_spiketrap[e]['visability'] == nil then g_spiketrap[e]['visability'] = 1 end
		if g_spiketrap[e]['visability'] == 1 then Show(e) end
		if g_spiketrap[e]['visability'] == 2 then Hide(e) end
		g_spiketrap[e]['trap_reset'] = g_spiketrap[e]['trap_reset']*10
		state[e] = "wait"
	end
	
	g_spiketrap[e] = g_Entity[e]
	ModulateSpeed(e,g_spiketrap[e]['trap_speed'])
	if state[e] == "wait" then
		hit[e] = 0
		if GetPlayerDistance(e) < g_spiketrap[e]['range'] then
			if g_Entity[e]['animating'] == 0 then
				if GetTimer(e) > (g_spiketrap[e]['trap_delay']*10) then
					StartTimer(e)
					state[e] = "activate trap"
					SetAnimationName(e,"Up")
					PlayAnimation(e)
					PlaySound(e,0)
					g_Entity[e]['animating'] = 1 
					if g_spiketrap[e]['trap_damage'] == 0 then
						HurtPlayer(-1,1000)						
					else
						HurtPlayer(-1,g_spiketrap[e]['trap_damage'])
					end
				end
			end
		else
			StartTimer(e)
		end

	elseif state[e] == "activate trap" then
		if g_spiketrap[e]['trap_repeat'] == 2 then			
			if GetTimer(e) > g_spiketrap[e]['trap_reset'] then
				StartTimer(e)
				SetAnimationName(e,"Down")
				PlayAnimation(e)
				PlaySound(e,1)
				state[e] = "wait"
			end
		else
			--Destroy(e)
		end	
	end --state
end --main

function spiketrap_exit(e)
end