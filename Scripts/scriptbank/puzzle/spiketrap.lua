-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Spiketrap v6 by Necrym59
-- DESCRIPTION: The attached object will act as a spike trap.
-- DESCRIPTION: When the player is within [RANGE=50(10,100)]
-- DESCRIPTION: the trap is triggered after [TRAP_DELAY=10(1,50)]
-- DESCRIPTION: causing [TRAP_DAMAGE=25(0,100)](0=Kill)
-- DESCRIPTION: Set the [@TRAP_MODE=1(1=Once only, 2=Multiple)]
-- DESCRIPTION: Time for [TRAP_RESET=50] and set the [TRAP_SPEED=100].
-- DESCRIPTION: Set the[@ACTIVE_ANIMATION=1(0=AnimSetList)]
-- DESCRIPTION: Set the[@INACTIVE_ANIMATION=2(0=AnimSetList)]
-- DESCRIPTION: Set the[@VISABILITY=1(1=Visible, 2=Invisible)]
-- DESCRIPTION: Is the [@TRAP_ACTIVE=1(1=Yes, 2=No)] if set to 'No' then activate via linked switch or zone.
-- DESCRIPTION: Trap will play <Sound0> on activation and <Sound1> on reset.

	local spiketrap = {}
	local range = {}
	local trap_delay = {}
	local trap_damage = {}
	local trap_mode = {}
	local trap_reset = {}
	local trap_speed = {}
	local active_animation = {}
	local inactive_animation = {}
	local visability = {}
	local trap_active = {}
	local doonce = {}
	local state = {}

function spiketrap_properties(e, range, trap_delay, trap_damage, trap_mode, trap_reset, trap_speed, active_animation, inactive_animation, visability, trap_active)
	spiketrap[e] = g_Entity[e]
	spiketrap[e].range = range					-- how close player can get before trap triggers
	spiketrap[e].trap_delay = trap_delay		-- how long after player enters trap range til trap really triggers (player reaction escape?)
	spiketrap[e].trap_damage = trap_damage		-- how much trap_damage the trap will apply to the player (set to 0 to automatically kill player)
	spiketrap[e].trap_mode = trap_mode			-- if trap can be triggered more than once (set to 1 for 1 time only trap)
	spiketrap[e].trap_reset = trap_reset		-- how long after trap has activated til it will reset (if trap_mode)
	spiketrap[e].trap_speed = trap_speed		-- adjusts how fast the spikes move
	spiketrap[e].active_animation = "=" .. tostring(active_animation)	-- selects the model animation for activation
	spiketrap[e].inactive_animation = "=" .. tostring(inactive_animation)	-- selects the model animation for inactivation
	spiketrap[e].visability = visability or 1		-- turns trap visible/invisible
	spiketrap[e].trap_active = trap_active or 1
end

function spiketrap_init(e)
	spiketrap[e] = g_Entity[e]
	spiketrap[e].range = 100
	spiketrap[e].trap_delay = 1
	spiketrap[e].trap_damage = 25
	spiketrap[e].trap_mode = 1
	spiketrap[e].trap_reset = 300
	spiketrap[e].trap_speed = 100
	spiketrap[e].active_animation = ""
	spiketrap[e].inactive_animation = ""
	spiketrap[e].visability = 1
	spiketrap[e].trap_active = 1
	state[e] = "init"
	doonce[e] = 0
end

function spiketrap_main(e)
	spiketrap[e] = g_Entity[e]

	if state[e] == "init" then
		if spiketrap[e].trap_active == 1 then SetActivated(e,1) end
		if spiketrap[e].trap_active == 2 then SetActivated(e,0) end
		if spiketrap[e].visability == 1 then Show(e) end
		if spiketrap[e].visability == 2 then Hide(e) end
		spiketrap[e].trap_reset = spiketrap[e].trap_reset*10
		ModulateSpeed(e,spiketrap[e].trap_speed)
		SetPlayerHealth(g_PlayerHealth)
		state[e] = "start_trap"
	end

	if g_Entity[e]['activated'] == 1 then
		doonce[e] = 0

		if state[e] == "start_trap" then
		
			if GetPlayerDistance(e) < spiketrap[e].range then				
				if GetTimer(e) > (spiketrap[e].trap_delay*10) then
					StartTimer(e)
					SetAnimationName(e,spiketrap[e].active_animation)
					PlayAnimation(e)
					PlaySound(e,0)						
					if spiketrap[e].trap_damage == 0 then
						if g_PlayerHealth > 0 then HurtPlayer(-1,5000) end
						SetPlayerHealth(0)
					end
					if spiketrap[e].trap_damage > 0 then
						if g_PlayerHealth > 0 then HurtPlayer(-1,spiketrap[e].trap_damage) end
						if g_PlayerHealth > 400 then 
							local tnewplayerhealth = g_PlayerHealth - spiketrap[e].trap_damage
							SetPlayerHealth(tnewplayerhealth)
						end
					end
					state[e] = "reset_trap"
				end
			else
				StartTimer(e)
			end
		end

		if state[e] == "reset_trap" then
			if spiketrap[e].trap_mode == 2 then
				if GetTimer(e) > spiketrap[e].trap_reset then
					StartTimer(e)
					SetAnimationName(e,spiketrap[e].inactive_animation)
					PlayAnimation(e)
					PlaySound(e,1)
					state[e] = "start_trap"
				end
			end
		end
	end

	if g_Entity[e]['activated'] == 0 then
		if doonce[e] == 0 then
			SetAnimationName(e,spiketrap[e].inactive_animation)
			PlayAnimation(e)
			PlaySound(e,1)
			doonce[e] = 1
		end
	end
end

function spiketrap_exit(e)
end