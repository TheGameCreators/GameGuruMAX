-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- NPC Control v72 by Necrym 59
-- DESCRIPTION: The attached npc entity will be controlled by this behavior.
-- DESCRIPTION: [#SENSE_TEXT$="Who's that ..an intruder??"]
-- DESCRIPTION: [#SENSE_RANGE=500(0,2000)]
-- DESCRIPTION: [@NPC_WILL_FLEE=2(1=Yes,2=No)]
-- DESCRIPTION: [#IDLE_TIME=3000(0,20000)]
-- DESCRIPTION: [#ATTACK_RANGE=100(0,1000)]
-- DESCRIPTION: [#ATTACK_INTERVAL=1000(0,10000)]
-- DESCRIPTION: [#ATTACK_DAMAGE=10(0,100)]
-- DESCRIPTION: [@RANDOM_DAMAGE=2(1=Yes,2=No)]
-- DESCRIPTION: [@NPC_CAN_ROAM=1(1=Yes,2=No)]
-- DESCRIPTION: [#ROAM_RANGE=500(0,3000)]
-- DESCRIPTION: [#NPC_ANIM_SPEED=0.8(0.1,5.0)]
-- DESCRIPTION: [#NPC_MOVE_SPEED=100(0,1000)]
-- DESCRIPTION: [#NPC_RUN_SPEED=110(0,1000)]
-- DESCRIPTION: [#NPC_TURN_SPEED=100(0,1000)]
-- DESCRIPTION: [@NPC_CAN_SHOOT=2(1=Yes,2=No)]
-- DESCRIPTION: [@IDLE1_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@IDLE2_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@IDLE3_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@IDLE4_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@WALK_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@RUN_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@THREAT_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@ATTACK1_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@ATTACK2_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@ATTACK3_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@SHOOT_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@HURT_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@DEATH1_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@DEATH2_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@LASTFLAG_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [#LASTFLAG_TIME=3000(0,20000)]
-- DESCRIPTION: [@LASTFLAG_LOOP=2(1=On,2=Off)]
-- DESCRIPTION: [@FORCE_MOVE=2(1=On,2=Off)]
-- DESCRIPTION: [@NPC_TILTING=1(1=On,2=Off)]
-- DESCRIPTION: [DIAGNOSTICS!=0]
-- DESCRIPTION: <Sound0> for movement sound
-- DESCRIPTION: <Sound1> for attacking sound effect
-- DESCRIPTION: <Sound2> for threat/hurt sound effect
-- DESCRIPTION: <Sound3> for death sound effect

local U = require "scriptbank\\utillib"

local lower = string.lower
local npc_control = {}
local sense_text = {}
local sense_range = {}
local idle_time = {}
local attack_range = {}
local attack_interval = {}
local attack_damage = {}
local random_damage = {}
local npc_can_roam = {}
local roam_range = {}
local npc_anim_speed = {}
local npc_move_speed = {}
local npc_run_speed = {}
local npc_turn_speed = {}
local npc_can_shoot = {}
local idle1_animation = {}
local idle2_animation = {}
local idle3_animation = {}
local idle4_animation = {}
local walk_animation = {}
local run_animation = {}
local threat_animation = {}
local attack1_animation = {}
local attack2_animation = {}
local attack3_animation = {}
local shoot_animation = {}
local hurt_animation = {}
local death1_animation = {}
local death2_animation = {}
local lastflag_animation = {}
local lastflag_time = {}
local lastflag_loop = {}
local force_move = {}
local npc_tilting = {}
local diagnostics = {}

local name1 = {}
local tdamage = {}
local status = {}
local state = {}
local state_choice = {}
local idle_delay = {}
local attack_delay = {}
local action_delay = {}
local lastflag_delay = {}
local start_health = {}
local anim_var = {}
local senseonce = {}
local issensed = {}
local animonce = {}
local idlemonce = {}
local attkonce = {}
local wandonce = {}
local doonce = {}
local patrolonce = {}
local shootonce = {}
local sayonce = {}
local scareonce = {}
local shooting = {}
local plrseen = {}
local heardistance = {}
local lostsight = {}
local currpause = {}
local plrlost = {}
local hurtonce = {}
local actiononce = {}
local aggro = {}
local scare = {}
local wait = {}
local startx = {}
local starty = {}
local startz = {}
local destx = {}
local desty = {}
local destz = {}
local pathdelay = {}
local pointcount = {}
local pointcountp = {}
local callhelponce = {}
local start_sense_range = {}
local regen = {}
local patrolmode = {}
local closestflag = {}
local previousflag = {}
local startfr = {}
local finishfr = {}
local colobj = {}
local playgsound = {}
local plrwithinmesh = {}
local allegiance = {}
local avoidance = {}
local svolume = {}
local root_time	= {}
local resetstate = {}

function npc_control_properties(e, sense_text, sense_range, npc_will_flee, idle_time, attack_range, attack_interval, attack_damage, random_damage, npc_can_roam, roam_range, npc_anim_speed, npc_move_speed, npc_run_speed, npc_turn_speed, npc_can_shoot, idle1_animation,  idle2_animation, idle3_animation, idle4_animation, walk_animation, run_animation, threat_animation, attack1_animation, attack2_animation, attack3_animation, shoot_animation, hurt_animation, death1_animation, death2_animation, lastflag_animation, lastflag_time, lastflag_loop, force_move, npc_tilting, diagnostics)
	npc_control[e].sense_text = sense_text
	npc_control[e].sense_range = sense_range
	npc_control[e].npc_will_flee = npc_will_flee
	npc_control[e].idle_time = idle_time
	npc_control[e].attack_range = attack_range
	npc_control[e].attack_interval = attack_interval
	npc_control[e].attack_damage = attack_damage
	npc_control[e].damage_delay = damage_delay
	npc_control[e].random_damage = random_damage
	npc_control[e].npc_can_roam = npc_can_roam or 1
	npc_control[e].roam_range = roam_range
	npc_control[e].npc_anim_speed = npc_anim_speed
	npc_control[e].npc_move_speed = npc_move_speed
	npc_control[e].npc_run_speed = npc_run_speed
	npc_control[e].npc_turn_speed = npc_turn_speed
	npc_control[e].npc_can_shoot = npc_can_shoot or 2
	npc_control[e].idle1_animation = "=" .. tostring(idle1_animation)
	npc_control[e].idle2_animation = "=" .. tostring(idle2_animation)
	npc_control[e].idle3_animation = "=" .. tostring(idle3_animation)
	npc_control[e].idle4_animation = "=" .. tostring(idle4_animation)
	npc_control[e].walk_animation = "=" .. tostring(walk_animation)
	npc_control[e].run_animation = "=" .. tostring(run_animation)
	npc_control[e].threat_animation = "=" .. tostring(threat_animation)
	npc_control[e].attack1_animation = "=" .. tostring(attack1_animation)
	npc_control[e].attack2_animation = "=" .. tostring(attack2_animation)
	npc_control[e].attack3_animation = "=" .. tostring(attack3_animation)
	npc_control[e].shoot_animation = "=" .. tostring(shoot_animation)
	npc_control[e].hurt_animation = "=" .. tostring(hurt_animation)
	npc_control[e].death1_animation = "=" .. tostring(death1_animation)
	npc_control[e].death2_animation = "=" .. tostring(death2_animation)
	npc_control[e].lastflag_animation = "=" .. tostring(lastflag_animation)
	npc_control[e].lastflag_time = lastflag_time
	npc_control[e].lastflag_loop = lastflag_loop
	npc_control[e].force_move = force_move or 2
	npc_control[e].npc_tilting = npc_tilting or 1
	npc_control[e].diagnostics = diagnostics or 0
end

function npc_control_init_name(e,name)
	npc_control[e] = {}
	npc_control[e].sense_text = ""
	npc_control[e].sense_range = 500
	npc_control[e].npc_will_flee = 2
	npc_control[e].idle_time = 3000
	npc_control[e].attack_range = 100
	npc_control[e].attack_interval = 1000
	npc_control[e].attack_damage = 10
	npc_control[e].random_damage = 1
	npc_control[e].npc_can_roam = 1
	npc_control[e].roam_range = 500
	npc_control[e].npc_anim_speed = 0.5
	npc_control[e].npc_move_speed = 100
	npc_control[e].npc_run_speed = 110
	npc_control[e].npc_turn_speed = 100
	npc_control[e].npc_can_shoot = 2
	npc_control[e].idle1_animation = ""
	npc_control[e].idle2_animation = ""
	npc_control[e].idle4_animation = ""
	npc_control[e].idle4_animation = ""
	npc_control[e].walk_animation = ""
	npc_control[e].run_animation = ""
	npc_control[e].threat_animation = ""
	npc_control[e].attack1_animation = ""
	npc_control[e].attack2_animation = ""
	npc_control[e].attack3_animation = ""
	npc_control[e].shoot_animation = ""
	npc_control[e].hurt_animation = ""
	npc_control[e].death1_animation = ""
	npc_control[e].death2_animation = ""
	npc_control[e].lastflag_animation = ""
	npc_control[e].lastflag_time = 3000
	npc_control[e].lastflag_loop = 2
	npc_control[e].force_move = 2
	npc_control[e].npc_tilting = 1
	npc_control[e].diagnostics = 0

	action_delay[e] = math.huge
	lastflag_delay[e] = math.huge
	anim_var[e] = 0
	senseonce[e] = 0
	issensed[e] = 0
	animonce[e] = 0
	idlemonce[e] = 0
	attkonce[e] = 0
	wandonce[e] = 0
	patrolonce[e] = 0
	shootonce[e] = 0
	hurtonce[e] = 0
	scareonce[e] = 0
	actiononce[e] = 0
	shooting[e] = 0
	plrseen[e] = 0
	heardistance[e] = 600
	lostsight[e] = 30
	currpause[e] = 0
	plrlost[e] = 0
	sayonce[e] = 0
	aggro[e] = 0
	scare[e] = 0
	wait[e] = math.huge
	status[e] = "init"
	state[e] = "flag_pathing"
	state_choice[e] = 0
	idle_delay[e] = math.huge
	root_time[e] =  math.huge	
	startx[e] = 0
	starty[e] = 0
	startz[e] = 0
	destx[e] = 0
	desty[e] = 0
	destz[e] = 0
	startfr[e] = 0
	finishfr[e] = 0
	pointcount[e] = 0
	pointcountp[e] = 0
	pathdelay[e] = math.huge
	patrolmode[e] = 0
	callhelponce[e] = 0
	regen[e] = math.huge
	start_sense_range[e] = 0
	closestflag[e] = 0
	previousflag[e] = 0
	name1[e] = name
	colobj[e] = 0
	allegiance[e] = 0
	avoidance[e] = 0	
	playgsound[e] = 0
	plrwithinmesh[e] = 1	
	svolume[e] = 0
	resetstate[e] = 0
end

function npc_control_main(e)

	CollisionOn(e)
	if status[e] == "init" then		
		allegiance[e] = GetEntityAllegiance(e) -- (0-enemy, 1-ally, 2-neutral)		
		SetAnimationSpeed(e,npc_control[e].npc_anim_speed)
		SetEntityMoveSpeed(e,npc_control[e].npc_move_speed)
		SetEntityTurnSpeed(e,npc_control[e].npc_turn_speed)
		SetPreExitValue(e,0)
		attack_delay[e] = npc_control[e].attack_interval
		if g_Entity[e]['health'] < 1000 then g_Entity[e]['health'] = g_Entity[e]['health'] + 1000 end
		start_health[e] = g_Entity[e]['health']
		SetEntityHealth(e,start_health[e])
		startx[e] = g_Entity[e]['x']
		starty[e] = g_Entity[e]['y']
		startz[e] = g_Entity[e]['z']
		start_sense_range[e] = npc_control[e].sense_range
		idle_delay[e] = g_Time + npc_control[e].idle_time
		pathdelay[e] = g_Time + 3000
		regen[e] = g_Time + 3000
		state_choice[e] = math.random(1,5)
		LoadGlobalSound("audiobank\\user\\" ..name1[e].. ".wav", g_Entity[e])
		status[e] = "endinit"		
	end

	-- Functions ------------------------------------------
	local function GetFlatDistance(e,x,z)
		local x1,z1 = 0,0
		x1,z1 = g_Entity[e]['x'],g_Entity[e]['z']
		local dx = x-x1
		local dz = z-z1
		local ddist = dx*dx+dz*dz
		return ddist
	end
	local function CheckDistance(e)
		CheckDX = (g_Entity[e]['x'] - startx[e])
		CheckDY = (g_Entity[e]['y'] - starty[e])
		CheckDZ = (g_Entity[e]['z'] - startz[e])
		if math.abs(CheckDY) > 100 then
			CheckDY = CheckDY * 4
		end
		return math.sqrt(math.abs(CheckDX*CheckDX)+math.abs(CheckDY*CheckDY)+math.abs(CheckDZ*CheckDZ));
	end
	local function GetFlatDistanceFromEntity(e,t)
		local x1,z1 = g_Entity[e].x,g_Entity[e].z
		local x2,z2 = g_Entity[t].x,g_Entity[t].z
		local dx = x2-x1
		local dz = z2-z1
		local ddist = dx*dx+dz*dz
		return ddist
	end
	local function GetNextFlag(e,gp)
		local tflag = -1
		for a = 0, 9 do
			local connectede = GetEntityRelationshipID(closestflag[e],a)
			if connectede > 0 then
				if GetEntityMarkerMode(connectede) == 11 then
					if connectede ~= previousflag[e] then
						tflag = connectede
						break
					end
				end
			end
		end
		-- If not a valid flag then end of the path go to the previous flag
		if tflag == -1 then
			if npc_control[e].lastflag_animation > "" then
				lastflag_delay[e] = g_Time + npc_control[e].lastflag_time
				state[e] = "lastflag"
			end
			tflag = previousflag[e]
		end
		-- Change our target flag
		local tflag2 = closestflag[e]
		closestflag[e] = tflag
		previousflag[e] = tflag2
		-- Get path to new flag
		local ex,ey,ez,ax,ay,az = GetEntityPosAng(e)
		local tx,ty,tz = GetEntityPosAng(closestflag[e])
		return tx,ty,tz
	end
	-------------------------------------------------------

	if state[e] == "idle" then
		StopSound(e,0)
		if idlemonce[e] == 0 then
			anim_var[e] = math.random(1,4)
			if anim_var[e] == 1 then SetAnimationName(e,npc_control[e].idle1_animation) end -- Idle1
			if anim_var[e] == 2 then SetAnimationName(e,npc_control[e].idle2_animation) end -- Idle2
			if anim_var[e] == 3 then SetAnimationName(e,npc_control[e].idle3_animation) end -- Idle3
			if anim_var[e] == 4 then SetAnimationName(e,npc_control[e].idle4_animation) end -- Idle4
			LoopAnimation(e)
			idlemonce[e] = 1
		end
		senseonce[e] = 0
		aggro[e] = 0
		scare[e] = 0
		wandonce[e] = 0
		animonce[e] = 0
		resetstate[e] = 0
		if g_Time > idle_delay[e] then
			state_choice[e] = math.random(1,5)
			if state_choice[e] < 5 then
				if npc_control[e].npc_can_roam == 2 then --No Roaming
					state[e] = "idle"
					idlemonce[e] = math.random(0,1)
				end
				if npc_control[e].npc_can_roam == 1 then --Can Roam
					state[e] = "idle"
				end
			end
			if state_choice[e] == 5 then
				if npc_control[e].npc_can_roam == 1 then --Can Roam
					state[e] = "roam"
					idlemonce[e] = 0
				end
			end
			idle_delay[e] = g_Time + npc_control[e].idle_time
		end
	end
	--------------------------------------------------------------------------------------------------------------------------------
	plrwithinmesh[e] = RDIsWithinMesh(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ) -- returns 1 if the XYZ coordinate is on the nav mesh
	allegiance[e] = GetEntityAllegiance(e)
	
	if GetPlayerDistance(e) > npc_control[e].sense_range and allegiance[e] ~= 0 then
		if npc_control[e].npc_can_roam == 1 then
			if g_Time > action_delay[e] then
				aggro[e] = 1
				state[e] = "idle"
				pathdelay[e] = g_Time + 100
				pointcount[e] = 0
				issensed[e] = 0
			end
		end
	end
	
	if GetPlayerDistance(e) <= npc_control[e].sense_range and aggro[e] == 0 and allegiance[e] > 2 then
		if npc_control[e].npc_can_roam == 1 then state[e] = "roam" end
	end

	if GetPlayerDistance(e) <= npc_control[e].sense_range and aggro[e] == 0 and allegiance[e] == 0 and plrwithinmesh[e] == 1 then
		state[e] = "sensed"
	end
	
	GetEntityPlayerVisibility(e)
	if GetPlayerDistance(e) > npc_control[e].sense_range and g_Entity[e]['plrvisible'] == 1 and aggro[e] == 0 and allegiance[e] == 0 and plrseen[e] == 0 and plrwithinmesh[e] == 1 then
		npc_control[e].sense_range = GetEntityViewRange(e)
		plrseen[e] = 1
		state[e] = "sensed"
	end	
	
	local flee_range = math.random(2,3)
	if GetPlayerDistance(e) <= npc_control[e].sense_range/flee_range and aggro[e] == 0 and allegiance[e] == 2 then
		if npc_control[e].npc_can_roam == 1 then
			if npc_control[e].npc_will_flee == 1 then
				scare[e] = 1
				state[e] = "roam"
			end
		end
	end
	if GetPlayerDistance(e) <= npc_control[e].sense_range*3 and g_PlayerGunFired == 1 and aggro[e] == 0 and allegiance[e] == 2 then
		if npc_control[e].npc_can_roam == 1 then
			if npc_control[e].npc_will_flee == 1 then
				scare[e] = 1
				RotateY(e,math.random(90,240))
				state[e] = "roam"
			end
		end
	end	
	---------------------------------------------------------------------------------------------------------------------------------
	if state[e] == "sensed" then
		GetEntityPlayerVisibility(e)		
		--StopSound(e,0)
		if GetPlayerDistance(e) < npc_control[e].sense_range or g_Entity[e]['plrvisible'] == 1 and allegiance[e] == 0 then
			issensed[e] = 1
			RotateToPlayerSlowly(e,GetEntityTurnSpeed(e)/2)
			if senseonce[e] == 0 then
				if npc_control[e].npc_can_roam == 1 or  npc_control[e].npc_can_roam == 2 then
					SetAnimationName(e,npc_control[e].threat_animation)
					ModulateSpeed(e,npc_control[e].npc_anim_speed)
					PlayAnimation(e)
					PlaySound(e,2)
					if sayonce[e] == 0 then
						PromptDuration(npc_control[e].sense_text,1000)
						sayonce[e] = 1
					end	
				end
				senseonce[e] = 1
				action_delay[e] = g_Time + 100
			end
			if npc_control[e].npc_will_flee == 2 and g_Time > action_delay[e] then
				if allegiance[e] == 0 then
					aggro[e] = 1
					scare[e] = 0
					hurtonce[e] = 0
					state[e] = "attack"
					pathdelay[e] = g_Time + 50
					pointcount[e] = 0
				end
			end
			if npc_control[e].npc_will_flee == 1 then
				aggro[e] = 0
				scare[e] = 1
			end
		end
	end
	if state[e] == "idle" or state[e] == "roam" or state[e] == "die" then
		--End Combat Music
		StopGlobalSound(g_Entity[e])
		playgsound[e] = 0
	end	
	
	if state[e] == "attack" then	
		-- Trigger Combat Music
		if playgsound[e] == 0 then			
			PlayGlobalSound(g_Entity[e])				
			playgsound[e] = 1
		end				
		-----------------------
		svolume[e] = (2000-GetPlayerDistance(e))/10		
		SetSoundVolume(svolume[e])
		if GetPlayerDistance(e) > npc_control[e].attack_range and aggro[e] == 1 then			
			if g_Time > pathdelay[e] or hurtonce[e] == 1 then
				local ex,ey,ez,ax,ay,az = GetEntityPosAng(e)
				destx[e] = g_PlayerPosX
				desty[e] = g_PlayerPosY
				destz[e] = g_PlayerPosZ
				RDFindPath(ex,ey,ez,destx[e],desty[e],destz[e])
				pointcount[e] = RDGetPathPointCount()
				if pointcount[e] > 0 then
					SetEntityPathRotationMode(e,1)
					if npc_control[e].npc_tilting == 1 then StartMoveAndRotateToXYZ(e,GetEntityMoveSpeed(e)/100,GetEntityTurnSpeed(e),1,0) end
					if npc_control[e].npc_tilting == 2 then StartMoveAndRotateToXYZ(e,GetEntityMoveSpeed(e)/100,GetEntityTurnSpeed(e),0,0) end
				end
				GetEntityPlayerVisibility(e)						
				if g_Entity[e]['plrvisible'] == 0 or plrwithinmesh[e] == 0 and plrlost[e] == 0 then
					if currpause[e] < lostsight[e] then
						currpause[e] = currpause[e] + 1
					end
					if currpause[e] >= lostsight[e] then
						plrlost[e] = 1
						currpause[e] = 0
					end	
				end	
				if g_Entity[e]['plrvisible'] == 1 then currpause[e] = 0 end
				
				if pointcount[e] == 0 or plrlost[e] == 1 then
					if resetstate[e] == 0 then
						local ex,ey,ez,ax,ay,az = GetEntityPosAng(e)
						destx[e] = ex
						desty[e] = ey
						destz[e] = ez
						RDFindPath(ex,ey,ez,destx[e],desty[e],destz[e])
						pointcount[e] = RDGetPathPointCount()
						SetEntityPathRotationMode(e,1)
						StartMoveAndRotateToXYZ(e,GetEntityMoveSpeed(e)/100,GetEntityTurnSpeed(e),1,0)
						npc_control[e].sense_range = start_sense_range[e]
						resetstate[e] = 1
						plrlost[e] = 0
					end					
					idlemonce[e] = 0
					aggro[e] = 0
					issensed[e] = 0
					plrseen[e] = 0
					state[e] = "idle"
				end
				pathdelay[e] = g_Time + 100 -- Check for new destination
			end
			if animonce[e] == 0 then
				SetAnimationName(e,npc_control[e].run_animation)
				LoopAnimation(e)
				LoopSound(e,0)
				animonce[e] = 1
				hurtonce[e] = 0
			end
			SetEntityMoveSpeed(e,npc_control[e].npc_run_speed)
			RDFindPath(ex,ey,ez,destx[e],desty[e],destz[e])
			MoveAndRotateToXYZ(e,GetEntityMoveSpeed(e)/100,GetEntityTurnSpeed(e))
			
			----------------------------------------------------------------
			local ex,ey,ez,eax,eay,eaz = GetEntityPosAng(e)
			local ox,oy,oz = U.Rotate3D(0,0,150, 0,math.rad(eay),0)
			colobj[e] = IntersectAll(ex,ey+35,ez,ex+ox,ey+oy+35,ez+oz,g_Entity[e]['obj'])
			if colobj[e] == nil then colobj[e] = 0 end
			if colobj[e] > 0 then
				wandonce[e] = 0		
				avoidance[e] = 1
				root_time[e] = g_Time + 800
				state[e] = "roam"
			end			
			----------------------------------------------------------------			
			if npc_control[e].force_move == 1 then
				local ex,ey,ez,eax,eay,eaz = GetEntityPosAng(e)
				local ox,oy,oz = U.Rotate3D(0,0,150, 0,math.rad(eay),0)
				colobj[e] = IntersectAll(ex,ey+35,ez,ex+ox,ey+oy+35,ez+oz,g_Entity[e]['obj'])
				if colobj[e] > 0 then
					MoveForward(e,GetEntityMoveSpeed(e)*0)
					RotateY(e,math.random(45,240))
				end
				MoveForward(e,GetEntityMoveSpeed(e)*2)
			end
			shooting[e] = 0
		else
			SetEntityMoveSpeed(e,npc_control[e].npc_move_speed)
		end
		if GetPlayerDistance(e) < npc_control[e].attack_range and g_Entity[e]['health'] > 0 and state[e] ~= "die" then
			animonce[e] = 0
			RotateToPlayer(e)
			if attkonce[e] == 0 then
				if npc_control[e].npc_can_shoot == 2 then
					SetAnimationName(e,npc_control[e].attack1_animation)
					ModulateSpeed(e,npc_control[e].npc_anim_speed)
					PlayAnimation(e)
					StopSound(e,0)
					PlaySound(e,1)
				end
				if npc_control[e].npc_can_shoot == 1 then --can Shoot
					SetAnimationName(e,npc_control[e].shoot_animation)
					ModulateSpeed(e,npc_control[e].npc_anim_speed)
					PlayAnimation(e)
					StopSound(e,0)
					LoopSound(e,1)
					if shooting[e] == 1 then FireWeapon(e) end
					if shooting[e] == 1 and npc_control[e].force_move == 1 then
						HurtPlayer(-1,npc_control[e].attack_damage)
						PlaySound(e,1)
					end
				end
				attkonce[e] = 1
				anim_var[e] = 0
			end
			if GetTimer(e) > attack_delay[e] then
				if anim_var[e] == 1 then  -- Attack1
					SetAnimationName(e,npc_control[e].attack1_animation)
					ModulateSpeed(e,npc_control[e].npc_anim_speed)
					PlayAnimation(e)
					StopSound(e,0)
					PlaySound(e,1)
					ForcePlayer(g_PlayerAngY-180,math.random(0.0,0.1))
					if npc_control[e].random_damage == 1 then HurtPlayer(-1,math.random(1,npc_control[e].attack_damage)) end
					if npc_control[e].random_damage == 2 then HurtPlayer(-1,npc_control[e].attack_damage) end
					if npc_control[e].npc_can_shoot == 2 then
						GamePlayerControlAddShakeTrauma(math.random(30.9,35.9))
						GamePlayerControlAddShakePeriod(90.0)
						GamePlayerControlAddShakeFade  (1.9)
						GamePlayerControlSetShakeTrauma(2.4)
						GamePlayerControlSetShakePeriod(100.0)
					end
					anim_var[e] = 0
				end
				if anim_var[e] == 2 then -- Attack2
					SetAnimationName(e,npc_control[e].attack2_animation)
					ModulateSpeed(e,npc_control[e].npc_anim_speed)
					PlayAnimation(e)
					StopSound(e,0)
					PlaySound(e,1)
					ForcePlayer(g_PlayerAngY-180,math.random(0.0,0.1))
					if npc_control[e].random_damage == 1 then HurtPlayer(-1,math.random(1,npc_control[e].attack_damage)) end
					if npc_control[e].random_damage == 2 then HurtPlayer(-1,npc_control[e].attack_damage) end
					if npc_control[e].npc_can_shoot == 2 then
						GamePlayerControlAddShakeTrauma(math.random(30.9,35.9))
						GamePlayerControlAddShakePeriod(90.0)
						GamePlayerControlAddShakeFade  (1.9)
						GamePlayerControlSetShakeTrauma(2.4)
						GamePlayerControlSetShakePeriod(100.0)
					end
					anim_var[e] = 0
				end
				if anim_var[e] == 3 then -- Attack3
					SetAnimationName(e,npc_control[e].attack3_animation)
					ModulateSpeed(e,npc_control[e].npc_anim_speed)
					PlayAnimation(e)
					StopSound(e,0)
					PlaySound(e,1)
					ForcePlayer(g_PlayerAngY-180,math.random(0.0,0.1))
					if npc_control[e].random_damage == 1 then HurtPlayer(-1,math.random(1,npc_control[e].attack_damage)) end
					if npc_control[e].random_damage == 2 then HurtPlayer(-1,npc_control[e].attack_damage) end
					if npc_control[e].npc_can_shoot == 2 then
						GamePlayerControlAddShakeTrauma(math.random(30.9,35.9))
						GamePlayerControlAddShakePeriod(90.0)
						GamePlayerControlAddShakeFade  (1.9)
						GamePlayerControlSetShakeTrauma(2.4)
						GamePlayerControlSetShakePeriod(100.0)
					end
					anim_var[e] = 0
				end
				if anim_var[e] == 4 then -- Threat
					SetAnimationName(e,npc_control[e].threat_animation)
					ModulateSpeed(e,npc_control[e].npc_anim_speed)
					PlayAnimation(e)
					StopSound(e,0)
					PlaySound(e,2)
					anim_var[e] = 0
				end
				if anim_var[e] == 5 then
					SetAnimationName(e,npc_control[e].shoot_animation)
					ModulateSpeed(e,npc_control[e].npc_anim_speed)
					PlayAnimation(e)
					StopSound(e,0)
					if shooting[e] == 1 then FireWeapon(e) end
					if shooting[e] == 1 and npc_control[e].force_move == 1 then
						HurtPlayer(-1,npc_control[e].attack_damage)
						PlaySound(e,1)
					end
					anim_var[e] = 0
				end
				attack_delay[e] = GetTimer(e) + npc_control[e].attack_interval
				anim_var[e] = math.random(1,3)  -- Attack random animation variation
				if npc_control[e].npc_can_shoot == 1 then
					anim_var[e] = 5
					shooting[e] = 1
					if GetPlayerDistance(e) < 90 then anim_var[e] = 1 end
				end
				animonce[e] = 0
			end
		end
	end

	if state[e] == "roam" then
		svolume[e] = (2000-GetPlayerDistance(e))/10
		SetSoundVolume(svolume[e])
		local dist = 0
		if wandonce[e] == 0 then -- get a random point on a circle around the current location
			if avoidance[e] == 0 then
				local ex,ey,ez,eax,eay,eaz = GetEntityPosAng(e)
				local ang = math.rad(math.random(1,360))
				dist = npc_control[e].roam_range
				if scare[e] == 0 then dist = npc_control[e].roam_range end
				if scare[e] == 1 then dist = (npc_control[e].roam_range*2) end
				destx[e] = startx[e] + math.cos(ang) * dist
				desty[e] = starty[e]
				destz[e] = startz[e] + math.sin(ang) * dist
			end	
			if avoidance[e] == 1 then
				local ex,ey,ez,eax,eay,eaz = GetEntityPosAng(e)
				local ang = math.rad(math.random(-10,10))
				dist = (npc_control[e].roam_range/90)
				destx[e] = startx[e] + math.cos(ang) * dist
				desty[e] = starty[e]
				destz[e] = startz[e] + math.sin(ang) * dist
			end
			----------------------------------------------------------------
			local ex,ey,ez,eax,eay,eaz = GetEntityPosAng(e)
			local ox,oy,oz = U.Rotate3D(0,0,150, 0,math.rad(eay),0)
			colobj[e] = IntersectAll(ex,ey+35,ez,ex+ox,ey+oy+35,ez+oz,g_Entity[e]['obj'])
			if colobj[e] == nil then colobj[e] = 0 end
			if colobj[e] > 0 then
				local ex,ey,ez,eax,eay,eaz = GetEntityPosAng(e)
				local ang = math.rad(math.random(-10,10))
				dist = (npc_control[e].roam_range/90)
				destx[e] = startx[e] + math.cos(ang) * dist
				desty[e] = starty[e]
				destz[e] = startz[e] + math.sin(ang) * dist
				avoidance[e] = 1
			end			
			----------------------------------------------------------------			
			RDFindPath(ex,ey,ez,destx[e],desty[e],destz[e])
			local result = RDIsWithinMesh(destx[e],desty[e],destz[e])
			if result == 0 then dist = dist-50 end
			pointcount[e] = RDGetPathPointCount()
			if pointcount[e] > 0 then
				if npc_control[e].npc_tilting == 1 then StartMoveAndRotateToXYZ(e,GetEntityMoveSpeed(e)/100,GetEntityTurnSpeed(e),1,0) end
				if npc_control[e].npc_tilting == 2 then StartMoveAndRotateToXYZ(e,GetEntityMoveSpeed(e)/100,GetEntityTurnSpeed(e),0,0) end
			end
			if scare[e] == 0 then
				ModulateSpeed(e,npc_control[e].npc_anim_speed)
				SetAnimationName(e,npc_control[e].walk_animation)
				SetEntityMoveSpeed(e,npc_control[e].npc_move_speed)
				LoopAnimation(e)
				LoopSound(e,0)
				wandonce[e] = 1
				hurtonce[e] = 0
			end
			if scare[e] == 1 then
				ModulateSpeed(e,npc_control[e].npc_anim_speed)
				SetAnimationName(e,npc_control[e].run_animation)
				SetEntityMoveSpeed(e,npc_control[e].npc_run_speed)
				LoopAnimation(e)
				PlaySound(e,0)
				wandonce[e] = 1
				hurtonce[e] = 0
			end
		end
		RDFindPath(ex,ey,ez,destx[e],desty[e],destz[e])
		MoveAndRotateToXYZ(e,GetEntityMoveSpeed(e)/100,GetEntityTurnSpeed(e))

		if npc_control[e].force_move == 1 then
			local ex,ey,ez,eax,eay,eaz = GetEntityPosAng(e)
			local ox,oy,oz = U.Rotate3D(0,0,150, 0,math.rad(eay),0)
			colobj[e] = IntersectAll(ex,ey+35,ez,ex+ox,ey+oy+35,ez+oz,g_Entity[e]['obj'])
			if colobj[e] > 0 then RotateY(e,math.random(90,240)) end
			if scare[e] == 0 then
				ModulateSpeed(e,npc_control[e].npc_anim_speed)
				MoveForward(e,GetEntityMoveSpeed(e)/2)
			end
			if scare[e] == 1 then
				if scareonce[e] == 0 then
					RotateY(e,math.random(100,240))
					scareonce[e] = 1
				end
				ModulateSpeed(e,npc_control[e].npc_anim_speed*2)
				MoveForward(e,GetEntityMoveSpeed(e)*2)
			end
			if CheckDistance(e) > npc_control[e].roam_range or colobj[e] > 0 then
				RotateY(e,math.random(45,240))
				state_choice[e] = math.random(1,2)
				if state_choice[e] == 1 then
					state[e] = "idle"
					idle_delay[e] = g_Time + npc_control[e].idle_time
					idlemonce[e] = 0
					state_choice[e] = 0
					wandonce[e] = 0
					hurtonce[e] = 0
					scareonce[e] = 0
					avoidance[e] = 0
					scare[e] = 0
				end
			end
		end
		if GetPlayerDistance(e) <= npc_control[e].sense_range/2 and allegiance[e] == 2 and npc_control[e].npc_will_flee == 1 then
			scare[e] = 1
			RotateY(e,math.random(90,240))
			if scareonce[e] == 0 then
				wandonce[e] = 0
				scareonce[e] = 1
				avoidance[e] = 0
				state[e] = "idle"
			end
		end
		if GetPlayerDistance(e) <= npc_control[e].sense_range/2 and allegiance[e] == 2 and npc_control[e].npc_will_flee == 2 then
			RotateY(e,math.random(90,240))
		end
		
		if avoidance[e] == 1 and g_Time > root_time[e] then
			aggro[e] = 1
			state[e] = "attack"
		end

		if GetFlatDistance(e,destx[e],destz[e]) <= 150 then
			idlemonce[e] = 0
			state_choice[e] = 0
			wandonce[e] = 0
			animonce[e] = 0
			hurtonce[e] = 0
			scareonce[e] = 0
			scare[e] = 0
			--avoidance[e] = 0
			state[e] = "idle"
			idle_delay[e] = g_Time + npc_control[e].idle_time
		end

		if GetPlayerDistance(e) < 100 then RotateY(e,math.random(1,240)) end  -----------------------------------
	else
		MoveAndRotateToXYZ(e,GetEntityMoveSpeed(e)/100,GetEntityTurnSpeed(e))
	end

	if state[e] == "flag_pathing" then
		if closestflag[e] < 1 then
			patrolmode[e] = 0
			for a = 0, 9 do
				local connectede = GetEntityRelationshipID(e,a)
				if connectede > 0 then
					if GetEntityMarkerMode(connectede) == 11 then
						patrolmode[e] = 1
						closestflag[e] = connectede
						previousflag[e] = connectede
						break
					end
				end
			end
			if patrolmode[e] > 0 then
				local ex,ey,ez,ax,ay,az = GetEntityPosAng(e)
				local tx,ty,tz = GetEntityPosAng(closestflag[e])
				destx[e] = tx
				desty[e] = ty
				destz[e] = tz
				state[e] = "patrol_flags"
			else
				patrolmode[e] = 0
				state[e] = "idle"
				idle_delay[e] = g_Time + npc_control[e].idle_time
			end
		end
	end

	if state[e] == "patrol_flags" then
		svolume[e] = (2000-GetPlayerDistance(e))/10
		SetSoundVolume(svolume[e])
		if patrolonce[e] == 0 then
			ModulateSpeed(e,npc_control[e].npc_anim_speed)
			SetAnimationName(e,npc_control[e].walk_animation)
			LoopAnimation(e)
			LoopSound(e,0)
			patrolonce[e] = 1
		end
		local ex,ey,ez,eax,eay,eaz = GetEntityPosAng(e)
		RDFindPath(ex,ey,ez, destx[e],desty[e],destz[e])
		pointcountp[e] = RDGetPathPointCount()
		if pointcountp[e] > 0 then
			if npc_control[e].npc_tilting == 1 then StartMoveAndRotateToXYZ(e,GetEntityMoveSpeed(e)/100,GetEntityTurnSpeed(e),1,0) end
			if npc_control[e].npc_tilting == 2 then StartMoveAndRotateToXYZ(e,GetEntityMoveSpeed(e)/100,GetEntityTurnSpeed(e),0,0) end
		end
		if npc_control[e].force_move == 1 then MoveForward(e,GetEntityMoveSpeed(e)/2) end
		MoveAndRotateToXYZ(e,GetEntityMoveSpeed(e)/100,GetEntityTurnSpeed(e),0)
		if GetFlatDistanceFromEntity(e,closestflag[e]) < 200 then
			destx[e],desty[e],destz[e] = GetNextFlag(e,g_Entity[e])
		end
		if GetPlayerDistance(e) < 100 then RotateY(e,math.random(1,240)) end
	end

	if state[e] == "lastflag" then
		if actiononce[e] == 0 then
			SetAnimationName(e,npc_control[e].lastflag_animation)
			LoopAnimation(e)
			actiononce[e] = 1
		end
		if actiononce[e] == 2 then
			SetAnimationName(e,npc_control[e].idle1_animation)
			LoopAnimation(e)
			actiononce[e] = 2
		end
		if g_Time > lastflag_delay[e] then
			if actiononce[e] == 1 or actiononce[e] == 2 then
				SetAnimationName(e,npc_control[e].walk_animation)
				LoopAnimation(e)
				patrolmode[e] = 1
				if npc_control[e].lastflag_loop == 1 then actiononce[e] = 0 end
				if npc_control[e].lastflag_loop == 2 then actiononce[e] = 2 end
				if previousflag[e] == closestflag[e] then state[e] = "roam" end
				if previousflag[e] ~= closestflag[e] then state[e] = "patrol_flags" end
			end
		end
	end

	if state[e] == "die" then
		if animonce[e] == 0 then
			anim_var[e] = math.random(1,2)  -- Death random animation variation
			if anim_var[e] == 1 then SetAnimationName(e,npc_control[e].death1_animation) end
			if anim_var[e] == 2 then SetAnimationName(e,npc_control[e].death2_animation) end
			ModulateSpeed(e,npc_control[e].npc_anim_speed)
			PlayAnimation(e)
			animonce[e] = 1
		end
		SetEntityHealth(e,999)
		if g_Entity[e]['animating'] == 0 then
			SetEntityHealth(e,-1)
			CollisionOff(e)
			animonce[e] = 1
			state[e] = "dead"
			StopSound(e,0)
			StopSound(e,1)
			StopSound(e,2)
			StopSound(e,3)
			--Destroy(e)
			SetPreExitValue(e,2)
			SwitchScript(e,"no_behavior_selected.lua")
		end
	end

	--- Check Health and Hurt Animation -------------------------------------
	
	if g_Time > regen[e] and g_Entity[e]['health'] > 1000 and g_Entity[e]['health'] < 1500 then
		SetEntityHealth(e,g_Entity[e]['health']+1)
		regen[e] = g_Time + 2000
	end
	
	if g_Entity[e]['health'] < 1000 and state[e] ~= "die" then
		local ex,ey,ez,ax,ay,az = GetEntityPosAng(e)
		destx[e] = ex
		desty[e] = ey
		destz[e] = ez
		RDFindPath(ex,ey,ez,destx[e],desty[e],destz[e])
		pointcount[e] = RDGetPathPointCount()
		SetEntityMoveSpeed(e,0)	
		animonce[e] = 0
		StopSound(e,0)
		PlaySound(e,3)
		state[e] = "die"
	end		
	
	if g_Entity[e]['health'] > 1000 then	
		if g_Entity[e]['health'] < start_health[e] then			
			SetEntityMoveSpeed(e,0)	
			if hurtonce[e] == 0 then
				if g_Entity[e]['animating'] == 0 then MoveForward(e,-5) end
				StopAnimation(e)
				SetAnimationName(e,npc_control[e].hurt_animation)
				StopSound(e,0)				
				PlaySound(e,2)
				PlayAnimation(e)
				LookAtPlayer(e)
				hurtonce[e] = 1
				allegiance[e] = 0
			end
			if GetObjectAnimationFinished(e) == 0 then g_Entity[e]['animating'] = 0	end
			if GetObjectAnimationFinished(e) == 1 then g_Entity[e]['animating'] = 1 end
			if g_Entity[e]['animating'] == 1 then
				start_health[e] = g_Entity[e]['health']
				SetEntityHealth(e,start_health[e])
				attkonce[e] = 0
				animonce[e] = 0
				wandonce[e] = 0
				idlemonce[e] = 0
				senseonce[e] = 0
				anim_var[e] = 0
				if aggro[e] == 1 then
					state_choice[e] = 2
				else
					state_choice[e] = math.random(1,2)
				end
				if state_choice[e] == 1 then
					aggro[e] = 0
					scare[e] = 1
					hurtonce[e] = 0
					if npc_control[e].npc_can_roam == 1 then state[e] = "roam" end
					if npc_control[e].npc_can_roam == 2 then state[e] = "idle" end
					state_choice[e] = 0
				end
				if state_choice[e] == 2 then
					if allegiance[e] > 0 then
						aggro[e] = 0
						scare[e] = 1
						hurtonce[e] = 0
						state[e] = "roam"
					end
					if allegiance[e] == 0 then
						aggro[e] = 1
						scare[e] = 0
						state[e] = "idle"
						pathdelay[e] = g_Time + 10
						animonce[e] = 0
					end
					state_choice[e] = 0
				end
			end
		end		
	end

	--Diagnostic text -----------------------------------
	if npc_control[e].diagnostics == 1 then
		local ex,ey,ez,ax,ay,az = GetEntityPosAng(e)
		Text(2,46,3,"Entity : " ..name1[e])
		if allegiance[e] == 0 then Text(2,50,3,"Allegiance : Enemy") end
		if allegiance[e] == 1 then Text(2,50,3,"Allegiance : Ally") end
		if allegiance[e] == 2 then Text(2,50,3,"Allegiance : Neutral") end
		Text(2,52,3,"Anim Speed   : " ..GetAnimationSpeed(e))
		Text(2,54,3,"Move Speed   : " ..GetEntityMoveSpeed(e))
		Text(2,56,3,"Turn Speed   : " ..GetEntityTurnSpeed(e))
		Text(2,58,3,"Current State: " ..state[e])
		Text(2,60,3,"Aggro: " ..aggro[e])
		Text(2,62,3,"Scared: " ..scare[e])
		Text(2,64,3,"Dest Flag #  : " ..closestflag[e])
		Text(2,66,3,"Entity PosX  : " ..ex)
		Text(2,68,3,"Entity PosZ  : " ..ez)
		Text(2,70,3,"Destination X: " ..destx[e])
		Text(2,72,3,"Destination Z: " ..destz[e])
		Text(2,74,3,"Current Health : " ..g_Entity[e]['health']-1000)
		Text(2,76,3,"Roaming Range : " ..npc_control[e].roam_range)
		if plrwithinmesh[e] == 0 then Text(2,78,3,"Player OUTSIDE Navmesh") end
		if plrwithinmesh[e] == 1 then Text(2,78,3,"Player WITHIN Navmesh") end
		Text(2,80,3,"Combat Music : audiobank\\user\\" ..name1[e].. ".wav")
	end
end

function npc_control_preexit(e)
end

function npc_control_exit(e)
end