-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Object Mover v7 by Necrym59
-- DESCRIPTION: This will move an object by a specified amount either by player or a switch/zone and can be used as a switch for activating other objects or game elements. Set Physics=ON, Always Active = ON, IsImobile=ON
-- DESCRIPTION: [PROMPT_TEXT$ = "E to move"]
-- DESCRIPTION: [USE_RANGE=90(1,500)]
-- DESCRIPTION: [@MOVEMENT_TYPE=1(1=Push, 2=Pull, 3=Slide-Left, 4=Slide-Right, 5=Slide-Up, 6=Slide-Down, 7=Rotate X , 8=Rotate Y , 9=Rotate Z)]
-- DESCRIPTION: [MOVEMENT=100(1,500)]
-- DESCRIPTION: [@USE_AS_SWITCH=1(1=No, 2=Yes)]
-- DESCRIPTION: [MULTI_USE!=0]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline)] Use emmisive color for shape option
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: <Sound0> Loop when used
-- DESCRIPTION: <Sound1> Plays when finished

local U = require "scriptbank\\utillib"
local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local objectmover 	= {}
local prompt_text 	= {}
local use_range 	= {}
local movement_type = {}
local movement 		= {}
local status 		= {}
local use_as_switch	= {}
local multi_use		= {}
local item_highlight= {}
local prompt_display = {}

local objrotval 	= {}
local objpushval 	= {}
local objpullval 	= {}
local objslideval 	= {}
local tEnt 			= {}
local selectobj 	= {}
local pressed		= {}
local rotatedx		= {}
local rotatedy		= {}
local rotatedz		= {}

function object_mover_properties(e, prompt_text, use_range, movement_type, movement, use_as_switch, multi_use, item_highlight, prompt_display)
	objectmover[e].prompt_text = prompt_text
	objectmover[e].use_range = use_range
	objectmover[e].movement_type = movement_type
	objectmover[e].movement = movement
	objectmover[e].use_as_switch = use_as_switch or 2
	objectmover[e].multi_use = multi_use or 0
	objectmover[e].item_highlight = item_highlight or 0
	objectmover[e].prompt_display = prompt_display or 1
end 

function object_mover_init(e)
	objectmover[e] = {}
	objectmover[e].prompt_text = " E to move"
	objectmover[e].use_range = 90
	objectmover[e].movement_type = 1
	objectmover[e].movement = 30
	objectmover[e].use_as_switch = 1
	objectmover[e].multi_use = 0
	objectmover[e].item_highlight = 0
	objectmover[e].prompt_display =	1
	
	status[e] = "waiting"
	objrotval[e] = 0
	objpushval[e] = 0
	objpullval[e] = 0
	objslideval[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	pressed[e] = 0
	selectobj[e] = 0
	rotatedx[e] = 0
	rotatedy[e] = 0
	rotatedz[e] = 0	
end

function object_mover_main(e)

	local PlayerDist = GetPlayerDistance(e)	
	if PlayerDist < objectmover[e].use_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,objectmover[e].use_range,objectmover[e].item_highlight)
		tEnt[e] = g_tEnt
		--end pinpoint select object--	
	end
	
	if PlayerDist < objectmover[e].use_range and tEnt[e] ~= 0 then
		if status[e] == "waiting" then  
			if objectmover[e].prompt_display == 1 then PromptLocal(e,objectmover[e].prompt_text) end
			if objectmover[e].prompt_display == 2 then Prompt(objectmover[e].prompt_text) end	
			if g_KeyPressE == 1 and pressed[e] == 0	then
				pressed[e] = 1
				status[e] = "moving" 
			end			
		end
	end
			
	if g_Entity[e].activated == 1 or pressed[e] == 1 then							
		if objectmover[e].movement_type == 1 then	--Push
			if objpushval[e] < objectmover[e].movement then							
				GravityOff(e)
				CollisionOff(e)
				local ox,oy,oz = U.Rotate3D(-0.1,0,0, math.rad(g_Entity[e].anglex), math.rad(g_Entity[e].angley), math.rad(g_Entity[e].anglez))
				local forwardposx, forwardposy, forwardposz = g_Entity[e].x + ox, g_Entity[e].y + oy, g_Entity[e].z + oz				
				ResetPosition(e,forwardposx,forwardposy,forwardposz)
				objpushval[e] = objpushval[e] + 0.1	
				LoopSound(e,0)
				CollisionOn(e)					
			end
			if objpushval[e] >= objectmover[e].movement then
				objpushval[e] = objectmover[e].movement
				StopSound(e,0)
				PlaySound(e,1)
				if objectmover[e].use_as_switch == 2 then
					ActivateIfUsed(e)						
					PerformLogicConnections(e)
				end
				pressed[e] = 0
				objectmover[e].movement_type = 2
				SetActivated(e,0)
				objpushval[e] = 0				
				status[e] = "waiting"
				if objectmover[e].multi_use == 0 then SwitchScript(e,"no_behavior_selected.lua") end
			end
		end
		if objectmover[e].movement_type == 2 then	--Pull
			if objpullval[e] < objectmover[e].movement then
				GravityOff(e)
				CollisionOff(e)
				local ox,oy,oz = U.Rotate3D(0.1,0,0, math.rad(g_Entity[e].anglex), math.rad(g_Entity[e].angley), math.rad(g_Entity[e].anglez))
				local forwardposx, forwardposy, forwardposz = g_Entity[e].x + ox, g_Entity[e].y + oy, g_Entity[e].z + oz				
				ResetPosition(e,forwardposx,forwardposy,forwardposz)													
				objpullval[e] = objpullval[e] + 0.1
				CollisionOn(e)
				LoopSound(e,0)
			end
			if objpullval[e] >= objectmover[e].movement then
				objpullval[e] = objectmover[e].movement
				StopSound(e,0)
				PlaySound(e,1)
				if objectmover[e].use_as_switch == 2 then
					ActivateIfUsed(e)						
					PerformLogicConnections(e)
				end
				pressed[e] = 0
				objectmover[e].movement_type = 1
				SetActivated(e,0)
				objpullval[e] = 0
				status[e] = "waiting"
				if objectmover[e].multi_use == 0 then SwitchScript(e,"no_behavior_selected.lua") end
			end
		end
		if objectmover[e].movement_type == 3 then	--Slide Left
			if objslideval[e] < objectmover[e].movement then
				GravityOff(e)
				CollisionOff(e)
				local ox,oy,oz = U.Rotate3D(0,0,-0.1, math.rad(g_Entity[e].anglex), math.rad(g_Entity[e].angley), math.rad(g_Entity[e].anglez))
				local forwardposx, forwardposy, forwardposz = g_Entity[e].x + ox, g_Entity[e].y + oy, g_Entity[e].z + oz
				ResetPosition(e,forwardposx,forwardposy,forwardposz)							
				objslideval[e] = objslideval[e] + 0.1
				CollisionOn(e)
				LoopSound(e,0)							
			end
			if objslideval[e] >= objectmover[e].movement then
				objslideval[e] = objectmover[e].movement
				StopSound(e,0)
				PlaySound(e,1)
				if objectmover[e].use_as_switch == 2 then
					ActivateIfUsed(e)						
					PerformLogicConnections(e)
				end
				pressed[e] = 0
				objectmover[e].movement_type = 4
				SetActivated(e,0)
				objslideval[e] = 0
				status[e] = "waiting"
				if objectmover[e].multi_use == 0 then SwitchScript(e,"no_behavior_selected.lua") end				
			end						
		end
		if objectmover[e].movement_type == 4 then	--Slide Right
			if objslideval[e] < objectmover[e].movement then
				GravityOff(e)
				CollisionOff(e)
				local ox,oy,oz = U.Rotate3D(0,0,0.1, math.rad(g_Entity[e].anglex), math.rad(g_Entity[e].angley), math.rad(g_Entity[e].anglez))
				local forwardposx, forwardposy, forwardposz = g_Entity[e].x + ox, g_Entity[e].y + oy, g_Entity[e].z + oz
				ResetPosition(e,forwardposx,forwardposy,forwardposz)							
				objslideval[e] = objslideval[e] + 0.1
				CollisionOn(e)
				LoopSound(e,0)
			end
			if objslideval[e] >= objectmover[e].movement then							
				StopSound(e,0)
				PlaySound(e,1)
				if objectmover[e].use_as_switch == 2 then
					ActivateIfUsed(e)							
					PerformLogicConnections(e)
				end
				pressed[e] = 0
				objectmover[e].movement_type = 3
				SetActivated(e,0)
				objslideval[e] = 0
				status[e] = "waiting"
				if objectmover[e].multi_use == 0 then SwitchScript(e,"no_behavior_selected.lua") end
			end						
		end
		if objectmover[e].movement_type ==  5 then	--Slide Up
			if objslideval[e] < objectmover[e].movement then
				GravityOff(e)
				CollisionOff(e)
				local ox,oy,oz = U.Rotate3D(0,0.1,0, math.rad(g_Entity[e].anglex), math.rad(g_Entity[e].angley), math.rad(g_Entity[e].anglez))
				local forwardposx, forwardposy, forwardposz = g_Entity[e].x + ox, g_Entity[e].y + oy, g_Entity[e].z + oz
				ResetPosition(e,forwardposx,forwardposy,forwardposz)							
				objslideval[e] = objslideval[e] + 0.1
				CollisionOn(e)
				LoopSound(e,0)
			end
			if objslideval[e] >= objectmover[e].movement then
				objslideval[e] = objectmover[e].movement			
				StopSound(e,0)
				PlaySound(e,1)
				if objectmover[e].use_as_switch == 2 then
					ActivateIfUsed(e)							
					PerformLogicConnections(e)
				end
				pressed[e] = 0
				objectmover[e].movement_type = 6
				SetActivated(e,0)
				objslideval[e] = 0
				status[e] = "waiting"
				if objectmover[e].multi_use == 0 then SwitchScript(e,"no_behavior_selected.lua") end
			end						
		end
		if objectmover[e].movement_type == 6 then	--Slide Down
			if objslideval[e] < objectmover[e].movement then							
				GravityOff(e)
				CollisionOff(e)
				local ox,oy,oz = U.Rotate3D(0,-0.1,0, math.rad(g_Entity[e].anglex), math.rad(g_Entity[e].angley), math.rad(g_Entity[e].anglez))
				local forwardposx, forwardposy, forwardposz = g_Entity[e].x + ox, g_Entity[e].y + oy, g_Entity[e].z + oz
				ResetPosition(e,forwardposx,forwardposy,forwardposz)							
				objslideval[e] = objslideval[e] + 0.1
				CollisionOn(e)
				LoopSound(e,0)
			end
			if objslideval[e] >= objectmover[e].movement then
				objslideval[e] = objectmover[e].movement
				StopSound(e,0)
				PlaySound(e,1)
				if objectmover[e].use_as_switch == 2 then
					ActivateIfUsed(e)							
					PerformLogicConnections(e)
				end
				pressed[e] = 0
				objectmover[e].movement_type = 5
				SetActivated(e,0)
				objslideval[e] = 0
				status[e] = "waiting"
				if objectmover[e].multi_use == 0 then SwitchScript(e,"no_behavior_selected.lua") end
			end						
		end
		if objectmover[e].movement_type == 7 then	--Rotate X
			if objrotval[e] < objectmover[e].movement then
				GravityOff(e)
				CollisionOff(e)							
				RotateX(e,objrotval[e])
				if rotatedx[e] == 0 then objrotval[e] = objrotval[e] + 0.1 end
				if rotatedx[e] == 1 then objrotval[e] = objrotval[e] - 0.1 end				
				CollisionOn(e)
				LoopSound(e,0)
			end
			if objrotval[e] >= objectmover[e].movement then
				objrotval[e] = -objectmover[e].movement
				StopSound(e,0)
				PlaySound(e,1)
				if objectmover[e].use_as_switch == 2 then
					ActivateIfUsed(e)						
					PerformLogicConnections(e)
				end
				pressed[e] = 0
				objectmover[e].movement_type = 7
				rotatedx[e] = 1	
				objrotval[e] = 0				
				SetActivated(e,0)
				status[e] = "waiting"
				if objectmover[e].multi_use == 0 then SwitchScript(e,"no_behavior_selected.lua") end
			end
			if objrotval[e] <= -objectmover[e].movement then
				objrotval[e] = -objectmover[e].movement
				StopSound(e,0)
				PlaySound(e,1)
				if objectmover[e].use_as_switch == 2 then
					ActivateIfUsed(e)						
					PerformLogicConnections(e)
				end
				pressed[e] = 0
				objectmover[e].movement_type = 7
				rotatedx[e] = 0	
				objrotval[e] = 0				
				SetActivated(e,0)
				status[e] = "waiting"
				if objectmover[e].multi_use == 0 then SwitchScript(e,"no_behavior_selected.lua") end
			end			
		end
		if objectmover[e].movement_type == 8 then	--Rotate Y
			if objrotval[e] < objectmover[e].movement then
				GravityOff(e)
				CollisionOff(e)							
				RotateY(e,objrotval[e])
				if rotatedy[e] == 0 then objrotval[e] = objrotval[e] + 0.1 end
				if rotatedy[e] == 1 then objrotval[e] = objrotval[e] - 0.1 end	
				CollisionOn(e)
				LoopSound(e,0)
			end
			if objrotval[e] >= objectmover[e].movement or objrotval[e] == 0 then
				objrotval[e] = objectmover[e].movement
				StopSound(e,0)
				PlaySound(e,1)
				if objectmover[e].use_as_switch == 2 then
					ActivateIfUsed(e)							
					PerformLogicConnections(e)
				end
				pressed[e] = 0
				objectmover[e].movement_type = 8
				rotatedy[e] = 1	
				objrotval[e] = 0				
				SetActivated(e,0)
				status[e] = "waiting"
				if objectmover[e].multi_use == 0 then SwitchScript(e,"no_behavior_selected.lua") end
			end
			if objrotval[e] <= -objectmover[e].movement then
				objrotval[e] = -objectmover[e].movement
				StopSound(e,0)
				PlaySound(e,1)
				if objectmover[e].use_as_switch == 2 then
					ActivateIfUsed(e)						
					PerformLogicConnections(e)
				end
				pressed[e] = 0
				objectmover[e].movement_type = 8
				rotatedy[e] = 0	
				objrotval[e] = 0				
				SetActivated(e,0)
				status[e] = "waiting"
				if objectmover[e].multi_use == 0 then SwitchScript(e,"no_behavior_selected.lua") end
			end
		end
		if objectmover[e].movement_type == 9 then	--Rotate Z
			if objrotval[e] < objectmover[e].movement then			
				GravityOff(e)
				CollisionOff(e)							
				RotateZ(e,objrotval[e])
				if rotatedz[e] == 0 then objrotval[e] = objrotval[e] + 0.1 end
				if rotatedz[e] == 1 then objrotval[e] = objrotval[e] - 0.1 end				
				CollisionOn(e)
				LoopSound(e,0)
			end
			if objrotval[e] >= objectmover[e].movement then							
				objrotval[e] = objectmover[e].movement
				StopSound(e,0)
				PlaySound(e,1)
				if objectmover[e].use_as_switch == 2 then
					ActivateIfUsed(e)							
					PerformLogicConnections(e)
				end
				pressed[e] = 0
				objectmover[e].movement_type = 9
				rotatedz[e] = 1	
				objrotval[e] = 0				
				SetActivated(e,0)
				status[e] = "waiting"
				if objectmover[e].multi_use == 0 then SwitchScript(e,"no_behavior_selected.lua") end				
			end
			if objrotval[e] <= -objectmover[e].movement then
				objrotval[e] = -objectmover[e].movement
				StopSound(e,0)
				PlaySound(e,1)
				if objectmover[e].use_as_switch == 2 then
					ActivateIfUsed(e)						
					PerformLogicConnections(e)
				end
				pressed[e] = 0
				objectmover[e].movement_type = 9
				rotatedz[e] = 0	
				objrotval[e] = 0				
				SetActivated(e,0)
				status[e] = "waiting"
				if objectmover[e].multi_use == 0 then SwitchScript(e,"no_behavior_selected.lua") end
			end			
		end	
	end
end