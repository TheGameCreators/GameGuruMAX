-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Target v15 by Necrym59 and BloodMoon
-- DESCRIPTION: Allows a target object to be shot and reset and can activate Linked or IfUsed entities.
-- DESCRIPTION: Attach to an object/entity and set AlwaysActive=ON, IsImobile=YES
-- DESCRIPTION: [HIT_TEXT$="Target Hit"]
-- DESCRIPTION: [@TARGET_TYPE=1(1=Stationary, 2=Moving, 3=Spinning, 4=Random Flying, 5=Scenery)]
-- DESCRIPTION: [TARGET_MOVE_X=0(0,1000)]
-- DESCRIPTION: [TARGET_MOVE_Y=0(0,1000)]
-- DESCRIPTION: [TARGET_MOVE_Z=0(0,1000)]
-- DESCRIPTION: [#TARGET_MOVE_SPEED=0.50(0.0,2.0)]
-- DESCRIPTION: [TARGET_RESET=10(0,100)] Seconds
-- DESCRIPTION: [@TARGET_TRIGGER=2(1=Yes, 2=No, 3=Lose Game, 4=Win Game)]
-- DESCRIPTION: [TARGET_POINTS=1(1,100)]
-- DESCRIPTION: [@POINTS_ISSUANCE=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyPointTally"]
-- DESCRIPTION: [TARGET_MOVE_DELAY=0(0,100)] Seconds
-- DESCRIPTION: [@TARGET_FACING=1(1=Fixed, 2=Random)]
-- DESCRIPTION: <Sound0> - Target Hit
-- DESCRIPTION: <Sound1> - Target Reset
-- DESCRIPTION: <Sound2> - Target Moving
-- DESCRIPTION: <Sound3> - Hit Trigger

local U = require "scriptbank\\utillib"
local rad = math.rad

local target 					= {}
local hit_text 					= {}
local target_type				= {}
local target_move_x				= {}
local target_move_y				= {}
local target_move_z				= {}
local target_move_speed			= {}
local target_reset				= {}
local target_trigger			= {}
local target_points				= {}
local points_issuance			= {}
local user_global_affected		= {}
local target_move_delay			= {}
local target_facing				= {}

local targetxpos		= {}
local targetypos		= {}
local targetzpos		= {}
local targetxang		= {}
local targetyang		= {}
local targetzang		= {}
local startxang 		= {}
local startyang 		= {}
local startzang 		= {}
local doonce			= {}
local played			= {}
local triggered			= {}
local moved				= {}
local movestate			= {}
local reached			= {}
local movedir			= {}
local status			= {}
local state				= {}
local currentvalue		= {}
local reset1			= {}
local reset2			= {}
local facing			= {}
local fside				= {}
local newyang			= {}
local starthealth		= {}
local surfaceheight		= {}
local rotation_movement	= {}
local vertical_movement	= {}
local flyonce			= {}
local flymode			= {}
local randomonce		= {}
	
function target_properties(e, hit_text, target_type, target_move_x, target_move_y, target_move_z, target_move_speed, target_reset, target_trigger, target_points, points_issuance, user_global_affected, target_move_delay, target_facing)
	target[e] = g_Entity[e]
	target[e].hit_text = hit_text
	target[e].target_type = target_type
	target[e].target_move_x = target_move_x	target[e].target_move_y = target_move_y
	target[e].target_move_z = target_move_z
	target[e].target_move_speed = target_move_speed	
	target[e].target_reset = target_reset
	target[e].target_trigger = target_trigger
	target[e].target_points = target_points
	target[e].points_issuance = points_issuance
	target[e].user_global_affected = user_global_affected
	target[e].target_move_delay = target_move_delay
	target[e].target_facing = target_facing
end
 
function target_init(e)
	target[e] = {}
	target[e].hit_text = hit_text
	target[e].target_type = 1
	target[e].target_move_x = 0
	target[e].target_move_y = 0
	target[e].target_move_z = 0	
	target[e].target_move_speed = 1	
	target[e].target_reset = 10
	target[e].target_trigger = 2
	target[e].target_points = 1
	target[e].points_issuance = 1
	target[e].user_global_affected = "MyPointTally"
	target[e].target_move_delay = 0
	target[e].target_facing = 1
	
	played[e] = 0
	moved[e] = 0
	reached[e] = 0
	movedir[e] = 1
	movestate[e] = 0	
	doonce[e] = 0
	flyonce[e] = 0
	flymode[e] = math.huge
	state[e] = 0
	currentvalue[e] = 0
	starthealth[e] = 0
	triggered[e] = 0
	facing[e] = 1
	fside[e] = 1
	reset1[e] = math.huge
	reset2[e] = math.huge
	randomonce[e] = 0
	_,_,_,startxang[e],startyang[e],startzang[e] = GetEntityPosAng(e)
	reset2[e] = g_Time + (target[e].target_move_delay*1000)
	if g_Entity[e]['health'] < 100 then SetEntityHealth(e,g_Entity[e]['health']+100) end
	status[e] = "init"
end
 
function target_main(e)	
	
	if status[e] == "init" then
		local x,y,z,Ax,Ay,Az = GetEntityPosAng(e)
		targetxpos[e] = x
		targetypos[e] = y
		targetzpos[e] = z
		targetxang[e] = Ax
		targetyang[e] = Ay
		targetzang[e] = Az
		if g_Entity[e]['health'] < 100 then SetEntityHealth(e,g_Entity[e]['health']+100) end		
		starthealth[e] = g_Entity[e]['health']
		if target[e].target_type == 5 then
			if SetCharacterMode(e,0) == 1 then end
			GravityOff(e)
			CollisionOff(e)
		end		
		status[e] = "endinit"
	end
	
	if target[e].target_type == 1 or target[e].target_type == 5 then
		target[e].target_move_x = 0
		target[e].target_move_y = 0
		target[e].target_move_z = 0
	end
	
	if target[e].target_type == 2 then
		local x,y,z,Ax,Ay,Az = GetEntityPosAng(e)
		local ox,oy,oz = U.Rotate3D(target[e].target_move_x, target[e].target_move_y, target[e].target_move_z, rad(startxang[e]),rad(startyang[e]),rad(startzang[e]))	
		targetxpos[e] = x + ox * target[e].target_move_speed/100 * movedir[e]
		targetypos[e] = y + oy * target[e].target_move_speed/100 * movedir[e]
		targetzpos[e] = z + oz * target[e].target_move_speed/100 * movedir[e]
		targetxang[e] = Ax
		targetyang[e] = Ay
		targetzang[e] = Az

		if g_Time > reset2[e] and reached[e] == 0 and movestate[e] == 0 then
			GravityOff(e)
			CollisionOff(e)
			SetPosition(e,targetxpos[e],targetypos[e],targetzpos[e])		
			CollisionOn(e)
			LoopSound(e,2)
			moved[e] = moved[e] + 1
			if moved[e] == target[e].target_move_x then
				reached[e] = 1
				movedir[e] = movedir[e]* -1
				reset2[e] = g_Time + (target[e].target_move_delay*1000)
			end
			if moved[e] == target[e].target_move_y then
				reached[e] = 1
				movedir[e] = movedir[e]* -1
				reset2[e] = g_Time + (target[e].target_move_delay*1000)
			end
			if moved[e] == target[e].target_move_z then
				reached[e] = 1
				movedir[e] = movedir[e]* -1
				reset2[e] = g_Time + (target[e].target_move_delay*1000)
			end
		end

		if g_Time > reset2[e] and reached[e] == 1 and movestate[e] == 0 then
			GravityOff(e)
			CollisionOff(e)
			SetPosition(e,targetxpos[e],targetypos[e],targetzpos[e])
			CollisionOn(e)
			LoopSound(e,2)
			moved[e] = moved[e] - 1
			if moved[e] == 0 then
				reached[e] = 0
				movedir[e] = 1
				reset2[e] = g_Time + (target[e].target_move_delay*1000)
			end
		end		
	end
	if target[e].target_type == 3 and state[e] == 0 then
		GravityOff(e)
		RotateY(e,GetAnimationSpeed(e)*(target[e].target_move_speed*200))
	end
	if target[e].target_type == 4 and g_Entity[e]['health'] > 100 then
		if flyonce[e] == 0 then
			SetAnimationName(e,"fly")
			LoopAnimation(e)
			ModulateSpeed(e,1.5)
			rotation_movement[e] = math.random(-160,200)
			vertical_movement[e] = math.random(-30,30)
			flymode[e] = g_Time + 2000
			flyonce[e] = 1
		end
		if g_Time >= flymode[e] then
			rotation_movement[e] = math.random(-160,200)
			vertical_movement[e] = math.random(-30,30)
			flymode[e] = g_Time + 2000
		end	
		CollisionOff(e)
		MoveForward(e,50)
		MoveUp(e,vertical_movement[e])
		if g_Entity[e]['y'] < (targetypos[e] + 100) and g_Entity[e]['y'] > (targetypos[e] + 10) then	
			MoveUp(e,vertical_movement[e])
		elseif g_Entity[e]['y'] >= (targetypos[e] + 100) then
			MoveUp(e,-vertical_movement[e])
		elseif g_Entity[e]['y'] <= (targetypos[e] + 10) then
			MoveUp(e,20)
		end
		RotateY(e,rotation_movement[e])
		if g_Entity[e]['x'] > targetxpos[e] + 500 or g_Entity[e]['z'] > targetzpos[e] + 500 then
			RotateY(e,150)
		end
		CollisionOn(e)
	end
	
	if target[e].target_facing == 2 and target[e].target_reset > 0 then
		if randomonce[e] == 0 then
			reset1[e] = g_Time + math.random(2000, 10000)			
			randomonce[e] = 1
		end
		if g_Time > reset1[e] and randomonce[e] == 1 then
			SetEntityHealth(e,0)
			state[e] = 1
			randomonce[e] = 0
		end	
	end
	
	if g_Entity[e]['health'] < 100 and state[e] == 0 then
		if doonce[e] == 0 then
			StopSound(e,2)
			PromptDuration(target[e].hit_text,2000)
			if target[e].user_global_affected > "" then
				if fside[e] == 1 then 
					if _G["g_UserGlobal['"..target[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..target[e].user_global_affected.."']"] end
					if target[e].points_issuance == 1 then _G["g_UserGlobal['"..target[e].user_global_affected.."']"] = currentvalue[e] + target[e].target_points end
					if target[e].points_issuance == 2 then _G["g_UserGlobal['"..target[e].user_global_affected.."']"] = currentvalue[e] - target[e].target_points end
				end
				if fside[e] == 2 then 
					if _G["g_UserGlobal['"..target[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..target[e].user_global_affected.."']"] end
					if target[e].points_issuance == 1 then _G["g_UserGlobal['"..target[e].user_global_affected.."']"] = currentvalue[e] - target[e].target_points end
					if target[e].points_issuance == 2 then _G["g_UserGlobal['"..target[e].user_global_affected.."']"] = currentvalue[e] - target[e].target_points end
				end				
			end
			if played[e] == 0 then
				PlaySound(e,0)
				played[e] = 1
			end
			if target[e].target_type == 1 then
				ResetRotation(e,targetxang[e],targetyang[e]-90,targetzang[e])
			end
			if target[e].target_type == 2 then
				StopSound(e,2)
				movestate[e] = 1
				Hide(e)
			end
			if target[e].target_type == 3 then
				StopSound(e,2)
				Hide(e)
			end
			if target[e].target_type == 4 then
				StopSound(e,2)
				StopAnimation(e)
				Hide(e)
			end
			---------------------------------
			if target[e].target_trigger == 1 then
				if triggered[e] == 0 then
					PlaySound(e,3)
					triggered[e] = 1
				end					
				PerformLogicConnections(e)
				ActivateIfUsed(e)
			end
			if target[e].target_trigger == 3 then
				LoseGame()
			end	
			if target[e].target_trigger == 4 then
				WinGame()
			end			
			if target[e].target_reset > 0 then 
				reset1[e] = g_Time + (target[e].target_reset*1000)
				state[e] = 1
			else
				state[e] = 0
			end			
			doonce[e] = 1
		end		
	end

	if g_Time > reset1[e] and state[e] == 1 then		
		SetEntityHealth(e,starthealth[e])
		if target[e].target_facing == 1 then 
			ResetRotation(e,targetxang[e],targetyang[e],targetzang[e])
			if target[e].target_type == 2 or target[e].target_type == 3 or target[e].target_type == 4 then
				ResetPosition(e,targetxpos[e],targetypos[e],targetzpos[e])
				Show(e)
			end			
			if played[e] == 1 then
				StopSound(e,0)			
				PlaySound(e,1)
				played[e] = 0
			end			
		end
		if target[e].target_facing == 2 then
			facing[e] =  math.random(1,2)
			if target[e].target_type == 1 then
				if facing[e] == 1 then ResetRotation(e,targetxang[e],targetyang[e],targetzang[e]) end
				if facing[e] == 2 then ResetRotation(e,targetxang[e],-90,targetzang[e]) end
			end	
			if target[e].target_type == 2 or target[e].target_type == 3 or target[e].target_type == 4 then	
				if facing[e] == 1 then ResetRotation(e,targetxang[e],targetyang[e],targetzang[e]) end
				if facing[e] == 2 then ResetRotation(e,targetxang[e],targetyang[e]-180,targetzang[e]) end
				ResetPosition(e,targetxpos[e],targetypos[e],targetzpos[e])
				Show(e)
			end			
			if played[e] == 1 then
				StopSound(e,0)
				PlaySound(e,1)
				played[e] = 0
			end
			local x,y,z,Ax,Ay,Az = GetEntityPosAng(e)
			newyang[e] = Ay
			if newyang[e] == startyang[e] then fside[e] = 1 end
			if newyang[e] ~= startyang[e] then fside[e] = 2 end
		end		
 		triggered[e] = 0
		state[e] = 0
		movestate[e] = 0		
		doonce[e] = 0
		flyonce[e] = 0
	end	
end
 
function target_exit(e)	
end