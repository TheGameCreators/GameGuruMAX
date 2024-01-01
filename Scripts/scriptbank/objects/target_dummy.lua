-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Target Dummy v6   by Necrym59
-- DESCRIPTION: Allows a target dummy to be shot and reset
-- DESCRIPTION: Attach to an object/entity and set AlwaysActive=ON, IsImobile=YES
-- DESCRIPTION: [HIT_TEXT$="Target Hit"]
-- DESCRIPTION: [@TARGET_TYPE=1(1=Flip, 2=Rotate, 3=Side Move+Flip, 3=Vertical Move+Hide)]
-- DESCRIPTION: [TARGET_MOVE_DISTANCE=250(0,500)]
-- DESCRIPTION: [#TARGET_MOVE_SPEED=0.50(0.0,2.0)]
-- DESCRIPTION: [TARGET_RESET=10(0,100)] Seconds
-- DESCRIPTION: [@TARGET_TRIGGER=2(1=Yes, 2=No)]
-- DESCRIPTION: <Sound0> - Target Hit
-- DESCRIPTION: <Sound1> - Target Reset
-- DESCRIPTION: <Sound2> - Target Moving

local target 					= {}
local hit_text 					= {}
local target_type				= {}
local target_move_distance		= {}
local target_move_speed			= {}
local target_reset				= {}
local target_trigger			= {}

local targetxpos		= {}
local targetypos		= {}
local targetzpos		= {}
local targetxang		= {}
local targetyang		= {}
local targetzang		= {}
local doonce			= {}
local played			= {}
local moved				= {}
local movestate			= {}
local status			= {}
local state				= {}
local reset				= {}

	
function target_dummy_properties(e, hit_text, target_type, target_move_distance, target_move_speed, target_reset, target_trigger)
	target[e] = g_Entity[e]
	target[e].hit_text = hit_text
	target[e].target_type = target_type
	target[e].target_move_distance = target_move_distance
	target[e].target_move_speed = target_move_speed	
	target[e].target_reset = target_reset
	target[e].target_trigger = target_trigger
end
 
function target_dummy_init(e)
	target[e] = {}
	target[e].hit_text = hit_text
	target[e].target_type = 1
	target[e].target_move_distance = 300
	target[e].target_move_speed = 1	
	target[e].target_reset = 10
	target[e].target_trigger = 2
	
	played[e] = 0
	moved[e] = 0
	movestate[e] = 0	
	doonce[e] = 0
	state[e] = 0	
	reset[e] = math.huge
	status[e] = "init"
end
 
function target_dummy_main(e)	
	
	if status[e] == "init" then
		if g_Entity[e]['health'] < 100 then SetEntityHealth(e,101) end
		x,y,z,Ax,Ay,Az = GetEntityPosAng(e)
		targetxpos[e] = x
		targetypos[e] = y
		targetzpos[e] = z
		targetxang[e] = Ax
		targetyang[e] = Ay
		targetzang[e] = Az
		status[e] = "endinit"
	end
	
	if target[e].target_type == 3 then
		if moved[e] < target[e].target_move_distance and movestate[e] == 0 then
			CollisionOff(e)
			ResetPosition(e,targetxpos[e],targetypos[e],targetzpos[e]+moved[e])
			moved[e] = moved[e] + target[e].target_move_speed
			CollisionOn(e)
			LoopSound(e,2)
			if moved[e] >= target[e].target_move_distance then
				moved[e] = target[e].target_move_distance
				movestate[e] = 1
				StopSound(e,2)
			end	
		end	
		if moved[e] > 0 and movestate[e] == 1 then
			CollisionOff(e)
			ResetPosition(e,targetxpos[e],targetypos[e],targetzpos[e]+moved[e])
			moved[e] = moved[e] - target[e].target_move_speed
			CollisionOn(e)
			LoopSound(e,2)
			if moved[e] <= 0 then
				moved[e] = 0
				movestate[e] = 0
				StopSound(e,2)
			end
		end	
	end
	
	if target[e].target_type == 4 then
		if moved[e] < target[e].target_move_distance and movestate[e] == 0 then
			GravityOff(e)
			CollisionOff(e)
			ResetPosition(e,targetxpos[e],targetypos[e]+moved[e],targetzpos[e])
			moved[e] = moved[e] + target[e].target_move_speed
			CollisionOn(e)
			LoopSound(e,2)
			if moved[e] >= target[e].target_move_distance then
				moved[e] = target[e].target_move_distance
				movestate[e] = 1
				StopSound(e,2)
			end	
		end	
		if moved[e] > 0 and movestate[e] == 1 then
			GravityOff(e)
			CollisionOff(e)
			ResetPosition(e,targetxpos[e],targetypos[e]+moved[e],targetzpos[e])
			moved[e] = moved[e] - target[e].target_move_speed
			CollisionOn(e)
			LoopSound(e,2)
			if moved[e] <= 0 then
				moved[e] = 0
				movestate[e] = 0
				StopSound(e,2)
			end
		end	
	end

	if g_Entity[e]['health'] < 100 and state[e] == 0 then
		if doonce[e] == 0 then	
			PromptDuration(target[e].hit_text,2000)
			if played[e] == 0 then
				PlaySound(e,0)
				played[e] = 1
			end	
			if target[e].target_type == 1 then
				ResetRotation(e,targetxang[e],targetyang[e],targetzang[e]+90)
			end
			if target[e].target_type == 2 then
				ResetRotation(e,targetxang[e],targetyang[e]-90,targetzang[e])
			end
			if target[e].target_type == 3 then
				StopSound(e,2)
				movestate[e] = -1				
				ResetRotation(e,targetxang[e],targetyang[e],targetzang[e]+90)
			end
			if target[e].target_type == 4 then
				StopSound(e,2)
				movestate[e] = -1				
				Hide(e)
			end
			
			if target[e].target_trigger == 1 then
				PerformLogicConnections(e)
			end			
			if target[e].target_reset > 0 then 
				reset[e] = g_Time + (target[e].target_reset*1000)
				state[e] = 1
			else
				state[e] = 0
			end
			doonce[e] = 1
		end		
	end
	
	if g_Time > reset[e] and state[e] == 1 then
		SetEntityHealth(e,100)
		ResetRotation(e,targetxang[e],targetyang[e],targetzang[e])
		if target[e].target_type == 4 then
			ResetPosition(e,targetxpos[e],targetypos[e],targetzpos[e])
			Show(e)
		end			
		if played[e] == 1 then
			StopSound(e,0)
			PlaySound(e,1)
			played[e] = 0
		end			
		state[e] = 0
		doonce[e] = 0
		movestate[e] = 0
	end
end
 
function target_dummy_exit(e)	
end
