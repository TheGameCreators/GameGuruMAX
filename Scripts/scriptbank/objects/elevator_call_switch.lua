-- Elevator Call Switch v4 by Necrym59
-- DESCRIPTION: Calls an Elevator to level
-- DESCRIPTION: Attach switch to activate
-- DESCRIPTION: [USE_RANGE=80(1,200)]
-- DESCRIPTION: [USE_MESSAGE$="Press E to call Elevator"]
-- DESCRIPTION: [CALL_MESSAGE$="Calling Elevator.."]
-- DESCRIPTION: [ELEVATOR_LEVEL_NO=0(0,100)]
-- DESCRIPTION: [ELEVATOR_NAME$="Elevator1"]
-- DESCRIPTION: [TRIGGER_PROXIMITY=120(1,300)]
-- DESCRIPTION: <Sound0> when activated

local U = require "scriptbank\\utillib"

g_elevator_calledno = {}
g_elevator_calledlevel = {}
local lower = string.lower

local elcallswitch 		= {}
local use_range 		= {}
local call_message 		= {}
local elevator_level_no	= {}
local elevator_name		= {}
local trigger_proximity = {}

local played			= {}
local proximity			= {}
local doonce			= {}
local wait				= {}
local animonce			= {}
local tEnt 				= {}
local selectobj 		= {}
local status			= {}

function elevator_call_switch_properties(e, use_range, use_message, call_message, elevator_level_no, elevator_name, trigger_proximity)
	elcallswitch[e] = g_Entity[e]
	elcallswitch[e].use_range = use_range
	elcallswitch[e].use_message = use_message
	elcallswitch[e].call_message = call_message
	elcallswitch[e].elevator_level_no = elevator_level_no
	elcallswitch[e].elevator_name = lower(elevator_name)
	elcallswitch[e].trigger_proximity = trigger_proximity
end

function elevator_call_switch_init(e)
	elcallswitch[e] = {}
	elcallswitch[e].use_range = 80
	elcallswitch[e].use_message = "Press E to call Elevator"
	elcallswitch[e].call_message = "Calling Elevator.."
	elcallswitch[e].elevator_level_no = 0
	elcallswitch[e].elevator_name = "Elevator1"
	elcallswitch[e].trigger_proximity = 120

	played[e] = 0
	doonce[e] = 0
	proximity[e] = 0
	animonce[e] = 0
	tEnt[e] = 0
	selectobj[e] = 0
	wait[e] = math.huge
	status[e] = "init"
end

function elevator_call_switch_main(e)

	if status[e] == "init" then
		proximity[e] = 0
		status[e] = "waiting"
	end

	if status[e] == "waiting" then
		local PlayerDist = GetPlayerDistance(e)
		if PlayerDist < elcallswitch[e].use_range then
			-- pinpoint select object--
			local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
			local rayX, rayY, rayZ = 0,0,elcallswitch[e].use_range
			local paX, paY, paZ = math.rad(GetCameraAngleX(0)), math.rad(GetCameraAngleY(0)), math.rad(GetCameraAngleZ(0))
			rayX, rayY, rayZ = U.Rotate3D(rayX, rayY, rayZ, paX, paY, paZ)
			selectobj[e]=IntersectAll(px,py,pz, px+rayX, py+rayY, pz+rayZ,e)
			if selectobj[e] ~= 0 or selectobj[e] ~= nil then
				if g_Entity[e].obj == selectobj[e] then
					TextCenterOnXColor(50-0.01,50,3,"+",255,255,255) --highliting (with crosshair at present)
					tEnt[e] = e
				else
					tEnt[e] = 0
				end
				if g_Entity[e]['obj'] ~= selectobj[e] then selectobj[e] = 0 end
			end
			if selectobj[e] == 0 or selectobj[e] == nil then
				tEnt[e] = 0
				TextCenterOnXColor(50-0.01,50,3,"+",155,155,155) --highliting (with crosshair at present)
			end
			--end pinpoint select object--
		end
		if PlayerDist < elcallswitch[e].use_range and tEnt[e] ~= 0 then
			PromptLocal(e,elcallswitch[e].use_message)
			if g_KeyPressE == 1 then
				status[e] = "calling"				
				SetActivated(e,1)				
			end
		end
	end

	if g_Entity[e]['activated'] == 1 then
		if status[e] == "calling" then
			if doonce[e] == 0 then
				if elcallswitch[e].elevator_name ~= "" or elcallswitch[e].elevator_name ~= nil then
					for n = 1, g_EntityElementMax do
						if n ~= nil and g_Entity[n] ~= nil then
							if lower(GetEntityName(n)) == elcallswitch[e].elevator_name then
								g_elevator_calledno = n
								g_elevator_calledlevel = elcallswitch[e].elevator_level_no
								break
							end
						end
					end
				end
			end
			if played[e] == 0 then
				PromptLocal(e,elcallswitch[e].call_message)
				SetAnimationName(e,"on")
				PlayAnimation(e)
				PlaySound(e,0)				
				played[e] = 1
			end
			proximity[e] = GetDistance(e,g_elevator_calledno)
			if proximity[e] < elcallswitch[e].trigger_proximity then
				if doonce[e] == 0 then
					SetAnimationName(e,"off")
					PlayAnimation(e)
					PerformLogicConnections(e)
					doonce[e] = 1
					wait[e] = g_Time + 5000
					SetActivated(e,0)
				end
			end
		end
	end
	
	if g_Entity[e]['activated'] == 0 then
		if g_Time > wait[e] then
			doonce[e] = 0
			played[e] = 0
			if animonce[e] == 1 then
				SetAnimationName(e,"off")				
				PlayAnimation(e)
				animonce[e] = 0
				PerformLogicConnections(e)
			end	
			status[e] = "waiting"
		end
	end
	
end

function GetDistance(e,v)
	if g_Entity[e] ~= nil and g_Entity[e] ~= 0 and g_Entity[v] ~= nil and g_Entity[v] ~= 0 then
		local disx = g_Entity[e]['x'] - g_Entity[v]['x']
		local disz = g_Entity[e]['z'] - g_Entity[v]['z']
		local disy = g_Entity[e]['y'] - g_Entity[v]['y']
		return math.sqrt(disx^2 + disz^2 + disy^2)
	end
end