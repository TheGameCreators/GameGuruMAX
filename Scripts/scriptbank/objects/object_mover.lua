-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Object Mover v4   by Necrym59
-- DESCRIPTION: This will move an object by a specified amount and can be used as a switch object for activating other objects or game elements. Set Physics=ON, Always Active = ON, IsImobile=ON
-- DESCRIPTION: Customise the [PROMPT_TEXT$ = "E to move"] [USE_RANGE=90(1,500)]
-- DESCRIPTION: [@MOVEMENT_TYPE=1(1=Push, 2=Pull, 3=Slide-Left, 4=Slide-Right, 5=Slide-Up, 6=Slide-Down, 7=Rotate X , 8=Rotate Y , 9=Rotate Z)]
-- DESCRIPTION: [MOVEMENT=100(1,500)]
-- DESCRIPTION: [@USE_AS_SWITCH=1(1=No, 2=Yes)]
-- DESCRIPTION: Loop <Sound0> when used
-- DESCRIPTION: Play <Sound1> when finished
local U = require "scriptbank\\utillib"

g_objectmover 		= {}
local prompt_text 	= {}
local use_range 	= {}
local movement_type = {}
local movement 		= {}
local status 		= {}
local use_as_switch	= {}
local objrotval 	= {}
local objpushval 	= {}
local objpullval 	= {}
local objslideval 	= {}

function object_mover_properties(e, prompt_text, use_range, movement_type, movement, use_as_switch)
	g_objectmover[e] = g_Entity[e]
	g_objectmover[e]['prompt_text'] = prompt_text
	g_objectmover[e]['use_range'] = use_range
	g_objectmover[e]['movement_type'] = movement_type
	g_objectmover[e]['movement'] =	movement
	g_objectmover[e]['use_as_switch'] =	use_as_switch
end 

function object_mover_init(e)
	g_objectmover[e] = g_Entity[e]
	g_objectmover[e]['prompt_text'] = " E to move"
	g_objectmover[e]['use_range'] = 90
	g_objectmover[e]['movement_type'] = 1
	g_objectmover[e]['movement'] = 30
	g_objectmover[e]['use_as_switch'] = 1
	status[e] = "unmoved"
	objrotval[e] = 0
	objpushval[e] = 0
	objpullval[e] = 0
	objslideval[e] = 0
end

function object_mover_main(e)
	g_objectmover[e] = g_Entity[e]
	local PlayerDist = GetPlayerDistance(e)
	
	if PlayerDist < g_objectmover[e]['use_range'] and g_PlayerHealth > 0 then
		local LookingAt = GetPlrLookingAtEx(e,1)
		
		if status[e] == "unmoved" then  --Unmoved
			if LookingAt == 1 then
				if status[e] ~= "moved" then PromptLocal(e,g_objectmover[e]['prompt_text']) end
				if g_KeyPressE == 1 then
							
					if g_objectmover[e]['movement_type'] == 1 then	--Push
						if objpushval[e] < g_objectmover[e]['movement'] then							
							GravityOff(e)
							CollisionOff(e)
							local ox,oy,oz = U.Rotate3D(-0.1,0,0, math.rad(g_Entity[e]['anglex']), math.rad(g_Entity[e]['angley']), math.rad(g_Entity[e]['anglez']))
							local forwardposx, forwardposy, forwardposz = g_Entity[e]['x'] + ox, g_Entity[e]['y'] + oy, g_Entity[e]['z'] + oz				
							ResetPosition(e,forwardposx,forwardposy,forwardposz)
							objpushval[e] = objpushval[e] + 0.1	
							LoopSound(e,0)
							CollisionOn(e)					
						end
						if objpushval[e] >= g_objectmover[e]['movement'] then							
							StopSound(e,0)
							PlaySound(e,1)
							if g_objectmover[e]['use_as_switch'] == 2 then
								SetActivatedWithMP(e,201)							
								PerformLogicConnections(e)
							end
							status[e] = "moved"
						end
					end
					if g_objectmover[e]['movement_type'] == 2 then	--Pull
						if objpullval[e] < g_objectmover[e]['movement'] then
							GravityOff(e)
							CollisionOff(e)
							local ox,oy,oz = U.Rotate3D(0.1,0,0, math.rad(g_Entity[e]['anglex']), math.rad(g_Entity[e]['angley']), math.rad(g_Entity[e]['anglez']))
							local forwardposx, forwardposy, forwardposz = g_Entity[e]['x'] + ox, g_Entity[e]['y'] + oy, g_Entity[e]['z'] + oz				
							ResetPosition(e,forwardposx,forwardposy,forwardposz)													
							objpullval[e] = objpullval[e] + 0.1
							CollisionOn(e)
							LoopSound(e,0)
						end
						if objpullval[e] >= g_objectmover[e]['movement'] then							
							StopSound(e,0)
							PlaySound(e,1)
							if g_objectmover[e]['use_as_switch'] == 2 then
								SetActivatedWithMP(e,201)							
								PerformLogicConnections(e)
							end
							status[e] = "moved"
						end
					end
					if g_objectmover[e]['movement_type'] == 3 then	--Slide Left
						if objslideval[e] < g_objectmover[e]['movement'] then
							GravityOff(e)
							CollisionOff(e)
							local ox,oy,oz = U.Rotate3D(0,0,-0.1, math.rad(g_Entity[e]['anglex']), math.rad(g_Entity[e]['angley']), math.rad(g_Entity[e]['anglez']))
							local forwardposx, forwardposy, forwardposz = g_Entity[e]['x'] + ox, g_Entity[e]['y'] + oy, g_Entity[e]['z'] + oz
							ResetPosition(e,forwardposx,forwardposy,forwardposz)							
							objslideval[e] = objslideval[e] + 0.1
							CollisionOn(e)
							LoopSound(e,0)							
						end
						if objslideval[e] >= g_objectmover[e]['movement'] then							
							StopSound(e,0)
							PlaySound(e,1)
							if g_objectmover[e]['use_as_switch'] == 2 then
								SetActivatedWithMP(e,201)							
								PerformLogicConnections(e)
							end
							status[e] = "moved"
						end						
					end
					if g_objectmover[e]['movement_type'] == 4 then	--Slide Right
						if objslideval[e] < g_objectmover[e]['movement'] then
							GravityOff(e)
							CollisionOff(e)
							local ox,oy,oz = U.Rotate3D(0,0,0.1, math.rad(g_Entity[e]['anglex']), math.rad(g_Entity[e]['angley']), math.rad(g_Entity[e]['anglez']))
							local forwardposx, forwardposy, forwardposz = g_Entity[e]['x'] + ox, g_Entity[e]['y'] + oy, g_Entity[e]['z'] + oz
							ResetPosition(e,forwardposx,forwardposy,forwardposz)							
							objslideval[e] = objslideval[e] + 0.1
							CollisionOn(e)
							LoopSound(e,0)
						end
						if objslideval[e] >= g_objectmover[e]['movement'] then							
							StopSound(e,0)
							PlaySound(e,1)
							if g_objectmover[e]['use_as_switch'] == 2 then
								SetActivatedWithMP(e,201)							
								PerformLogicConnections(e)
							end
							status[e] = "moved"
						end						
					end
					if g_objectmover[e]['movement_type'] ==  5 then	--Slide Up
						if objslideval[e] < g_objectmover[e]['movement'] then
							GravityOff(e)
							CollisionOff(e)
							local ox,oy,oz = U.Rotate3D(0,0.1,0, math.rad(g_Entity[e]['anglex']), math.rad(g_Entity[e]['angley']), math.rad(g_Entity[e]['anglez']))
							local forwardposx, forwardposy, forwardposz = g_Entity[e]['x'] + ox, g_Entity[e]['y'] + oy, g_Entity[e]['z'] + oz
							ResetPosition(e,forwardposx,forwardposy,forwardposz)							
							objslideval[e] = objslideval[e] + 0.1
							CollisionOn(e)
							LoopSound(e,0)
						end
						if objslideval[e] >= g_objectmover[e]['movement'] then							
							StopSound(e,0)
							PlaySound(e,1)
							if g_objectmover[e]['use_as_switch'] == 2 then
								SetActivatedWithMP(e,201)							
								PerformLogicConnections(e)
							end
							status[e] = "moved"
						end						
					end
					if g_objectmover[e]['movement_type'] == 6 then	--Slide Down
						if objslideval[e] < g_objectmover[e]['movement'] then							
							GravityOff(e)
							CollisionOff(e)
							local ox,oy,oz = U.Rotate3D(0,-0.1,0, math.rad(g_Entity[e]['anglex']), math.rad(g_Entity[e]['angley']), math.rad(g_Entity[e]['anglez']))
							local forwardposx, forwardposy, forwardposz = g_Entity[e]['x'] + ox, g_Entity[e]['y'] + oy, g_Entity[e]['z'] + oz
							ResetPosition(e,forwardposx,forwardposy,forwardposz)							
							objslideval[e] = objslideval[e] + 0.1
							CollisionOn(e)
							LoopSound(e,0)
						end
						if objslideval[e] >= g_objectmover[e]['movement'] then					
							StopSound(e,0)
							PlaySound(e,1)
							if g_objectmover[e]['use_as_switch'] == 2 then
								SetActivatedWithMP(e,201)							
								PerformLogicConnections(e)
							end
							status[e] = "moved"	
						end						
					end
					if g_objectmover[e]['movement_type'] == 7 then	--Rotate X
						if objrotval[e] < g_objectmover[e]['movement'] then
							GravityOff(e)
							CollisionOff(e)							
							RotateX(e,objrotval[e])
							objrotval[e] = objrotval[e] + 0.1
							CollisionOn(e)
							LoopSound(e,0)
						end
						if objrotval[e] >= g_objectmover[e]['movement'] then							
							StopSound(e,0)
							PlaySound(e,1)
							if g_objectmover[e]['use_as_switch'] == 2 then
								SetActivatedWithMP(e,201)							
								PerformLogicConnections(e)
							end
							status[e] = "moved"
						end
					end
					if g_objectmover[e]['movement_type'] == 8 then	--Rotate Y
						if objrotval[e] < g_objectmover[e]['movement'] then
							GravityOff(e)
							CollisionOff(e)							
							RotateY(e,objrotval[e])
							objrotval[e] = objrotval[e] + 0.1
							CollisionOn(e)
							LoopSound(e,0)
						end
						if objrotval[e] >= g_objectmover[e]['movement'] then
							StopSound(e,0)
							PlaySound(e,1)
							if g_objectmover[e]['use_as_switch'] == 2 then
								SetActivatedWithMP(e,201)							
								PerformLogicConnections(e)
							end
							status[e] = "moved"
						end
					end
					if g_objectmover[e]['movement_type'] == 9 then	--Rotate Z
						if objrotval[e] < g_objectmover[e]['movement'] then
							GravityOff(e)
							CollisionOff(e)							
							RotateZ(e,objrotval[e])
							objrotval[e] = objrotval[e] + 0.1
							CollisionOn(e)
							LoopSound(e,0)
						end
						if objrotval[e] >= g_objectmover[e]['movement'] then							
							StopSound(e,0)
							PlaySound(e,1)
							if g_objectmover[e]['use_as_switch'] == 2 then
								SetActivatedWithMP(e,201)							
								PerformLogicConnections(e)
							end
							status[e] = "moved"
						end
					end					
				end
				if g_KeyPressE == 0 then StopSound(e,0) end
			end
		end
	else
		StopSound(e,0)
	end		
end