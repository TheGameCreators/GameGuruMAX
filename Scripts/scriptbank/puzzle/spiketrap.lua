-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Spiketrap v9 by Necrym59
-- DESCRIPTION: The attached object will act as a spike trap.
-- DESCRIPTION: When the player or an NPC is within [RANGE=80(10,200)]
-- DESCRIPTION: Trap is triggered after [TRAP_DELAY=10(1,50)]
-- DESCRIPTION: Causing [TRAP_DAMAGE=25(0,100)](0=Kill)
-- DESCRIPTION: Set the [@TRAP_MODE=1(1=Once only, 2=Multiple)]
-- DESCRIPTION: Time for [TRAP_RESET=50] and set the [TRAP_SPEED=100].
-- DESCRIPTION: Set the[@ACTIVE_ANIMATION=1(0=AnimSetList)]
-- DESCRIPTION: Set the[@INACTIVE_ANIMATION=2(0=AnimSetList)]
-- DESCRIPTION: Set the[@VISABILITY=1(1=Visible, 2=Invisible)]
-- DESCRIPTION: Is the [@TRAP_ACTIVE=1(1=Yes, 2=No)] if set to 'No' then activate via linked switch or zone.
-- DESCRIPTION: [HURT_NPC!=0]if set on then will damage npc's by damage amount.
-- DESCRIPTION: [HURT_PLAYER!=1]if set on then damage Player by damage amount.
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
local hurt_npc = {}
local tableName = {}
local trappedEnt = {}
local entrange = {}	
local doonce = {}
local status = {}

function spiketrap_properties(e, range, trap_delay, trap_damage, trap_mode, trap_reset, trap_speed, active_animation, inactive_animation, visability, trap_active, hurt_npc, hurt_player)
	spiketrap[e].range = range
	spiketrap[e].trap_delay = trap_delay
	spiketrap[e].trap_damage = trap_damage
	spiketrap[e].trap_mode = trap_mode
	spiketrap[e].trap_reset = trap_reset		
	spiketrap[e].trap_speed = trap_speed		
	spiketrap[e].active_animation = "=" .. tostring(active_animation)
	spiketrap[e].inactive_animation = "=" .. tostring(inactive_animation)
	spiketrap[e].visability = visability or 1
	spiketrap[e].trap_active = trap_active or 1
	spiketrap[e].hurt_npc = hurt_npc or 0
	spiketrap[e].hurt_player = hurt_player or 1
end

function spiketrap_init(e)
	spiketrap[e] = {}
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
	spiketrap[e].hurt_npc = 0
	spiketrap[e].hurt_player = 1
	
	status[e] = "init"
	trappedEnt[e] = 0
	entrange[e] = 0
	doonce[e] = 0
	tableName[e] = "npclist" ..tostring(e)
	_G[tableName[e]] = {}
end

function spiketrap_main(e)

	if status[e] == "init" then
		if spiketrap[e].trap_active == 1 then SetActivated(e,1) end
		if spiketrap[e].trap_active == 2 then SetActivated(e,0) end
		if spiketrap[e].visability == 1 then Show(e) end
		if spiketrap[e].visability == 2 then Hide(e) end
		spiketrap[e].trap_reset = spiketrap[e].trap_reset*10
		ModulateSpeed(e,spiketrap[e].trap_speed)
		SetPlayerHealth(g_PlayerHealth)
		for n = 1, g_EntityElementMax do
			if n ~= nil and g_Entity[n] ~= nil then
				if GetEntityAllegiance(n) > -1 then
					table.insert(_G[tableName[e]],n)
				end
			end
		end
		status[e] = "start_trap"
	end

	if g_Entity[e]['activated'] == 1 then
		doonce[e] = 0

		if status[e] == "start_trap" then
		
			for _,v in pairs (_G[tableName[e]]) do
				if g_Entity[v] ~= nil then
					entrange[e] = math.ceil(GetFlatDistance(e,v))
					if g_Entity[v]["health"] > 0 and entrange[e] < spiketrap[e].range then							
						trappedEnt[e] = v
						break
					else
						trappedEnt[e] = 0
					end
				end
			end
		
			if GetPlayerDistance(e) < spiketrap[e].range or entrange[e] < spiketrap[e].range and trappedEnt[e] > 0 then			
				if GetTimer(e) > (spiketrap[e].trap_delay*10) then
					StartTimer(e)
					if spiketrap[e].hurt_player == 1 or trappedEnt[e] > 0 then  
						SetAnimationName(e,spiketrap[e].active_animation)
						PlayAnimation(e)
						PlaySound(e,0)					
					end	
					if GetPlayerDistance(e) < spiketrap[e].range then
						if spiketrap[e].hurt_player == 1 then
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
						end
					end					
					if trappedEnt[e] ~= 0 then
						if entrange[e] < spiketrap[e].range and g_Entity[trappedEnt[e]]['health'] > 0 then
							if spiketrap[e].hurt_npc == 1 then
								if spiketrap[e].trap_damage == 0 then
									SetEntityHealth(trappedEnt[e],0)								
								end
								if spiketrap[e].trap_damage > 0 then
									SetEntityHealth(trappedEnt[e],g_Entity[trappedEnt[e]]['health']-spiketrap[e].trap_damage)
								end
							end
						end	
						if g_Entity[trappedEnt[e]]['health'] <= 0 then
							trappedEnt[e] = 0
							entrange[e] = 0
						end
					end	
					status[e] = "reset_trap"
				end
			else
				StartTimer(e)
			end
		end
		
		if status[e] == "reset_trap" then
			if spiketrap[e].trap_mode == 2 then
				if GetTimer(e) > spiketrap[e].trap_reset then
					StartTimer(e)
					SetAnimationName(e,spiketrap[e].inactive_animation)
					PlayAnimation(e)
					PlaySound(e,1)
					status[e] = "start_trap"
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

function GetFlatDistance(e,v)
	if g_Entity[e] ~= nil and g_Entity[v] ~= nil then
		local distDX = g_Entity[e]['x'] - g_Entity[v]['x']
		local distDZ = g_Entity[e]['z'] - g_Entity[v]['z']
		return math.sqrt((distDX*distDX)+(distDZ*distDZ));
	end
end