-- Elevator Call Switch v7 by Necrym59
-- DESCRIPTION: Calls an Elevator to level
-- DESCRIPTION: Attach switch to activate
-- DESCRIPTION: [USE_RANGE=90(1,200)]
-- DESCRIPTION: [USE_MESSAGE$="Press E to call Elevator"]
-- DESCRIPTION: [CALL_MESSAGE$="Calling Elevator.."]
-- DESCRIPTION: [ELEVATOR_LEVEL_NO=0(0,100)]
-- DESCRIPTION: [ELEVATOR_NAME$="Elevator1"]
-- DESCRIPTION: [TRIGGER_PROXIMITY=120(1,300)]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\press.png"]
-- DESCRIPTION: <Sound0> when activated

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}
g_elevator_calledno = {}
g_elevator_calledlevel = {}

local lower = string.lower

local elcallswitch 		= {}
local use_range 		= {}
local call_message 		= {}
local elevator_level_no	= {}
local elevator_name		= {}
local trigger_proximity = {}
local prompt_display	= {}
local item_highlight	= {}
local highlight_icon	= {}

local played			= {}
local proximity			= {}
local doonce			= {}
local wait				= {}
local animonce			= {}
local tEnt 				= {}
local selectobj 		= {}
local status			= {}
local hl_icon			= {}
local hl_imgwidth		= {}
local hl_imgheight		= {}


function elevator_call_switch_properties(e, use_range, use_message, call_message, elevator_level_no, elevator_name, trigger_proximity, prompt_display, item_highlight, highlight_icon_imagefile)
	elcallswitch[e].use_range = use_range or 90
	elcallswitch[e].use_message = use_message
	elcallswitch[e].call_message = call_message
	elcallswitch[e].elevator_level_no = elevator_level_no
	elcallswitch[e].elevator_name = lower(elevator_name)
	elcallswitch[e].trigger_proximity = trigger_proximity
	elcallswitch[e].prompt_display = prompt_display
	elcallswitch[e].item_highlight = item_highlight
	elcallswitch[e].highlight_icon = highlight_icon_imagefile	
end

function elevator_call_switch_init(e)
	elcallswitch[e] = {}
	elcallswitch[e].use_range = 90
	elcallswitch[e].use_message = "Press E to call Elevator"
	elcallswitch[e].call_message = "Calling Elevator.."
	elcallswitch[e].elevator_level_no = 0
	elcallswitch[e].elevator_name = "Elevator1"
	elcallswitch[e].trigger_proximity = 120
	elcallswitch[e].prompt_display = 1
	elcallswitch[e].item_highlight = 0
	elcallswitch[e].highlight_icon = "imagebank\\icons\\press.png"
	
	played[e] = 0
	doonce[e] = 0
	proximity[e] = 0
	animonce[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0
	wait[e] = math.huge
	status[e] = "init"
end

function elevator_call_switch_main(e)

	if status[e] == "init" then
		if elcallswitch[e].item_highlight == 3 and elcallswitch[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(elcallswitch[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(elcallswitch[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(elcallswitch[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end	
		proximity[e] = 0
		status[e] = "waiting"
	end

	if status[e] == "waiting" then
		local PlayerDist = GetPlayerDistance(e)
		if PlayerDist < elcallswitch[e].use_range then
			--pinpoint select object--
			module_misclib.pinpoint(e,elcallswitch[e].use_range,elcallswitch[e].item_highlight,hl_icon[e])
			tEnt[e] = g_tEnt
			--end pinpoint select object--
		end
		if PlayerDist < elcallswitch[e].use_range and tEnt[e] == e then
			if elcallswitch[e].prompt_display == 1 then PromptLocal(e,elcallswitch[e].use_message) end
			if elcallswitch[e].prompt_display == 2 then Prompt(elcallswitch[e].use_message) end		
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
				if elcallswitch[e].prompt_display == 1 then PromptLocal(e,elcallswitch[e].call_message) end
				if elcallswitch[e].prompt_display == 2 then Prompt(elcallswitch[e].call_message) end			
				SetAnimationName(e,"on")
				PlayAnimation(e)
				PlaySound(e,0)				
				played[e] = 1
			end
			proximity[e] = GetDistance(e,g_elevator_calledno)
			if proximity[e] <= elcallswitch[e].trigger_proximity then
				if doonce[e] == 0 then
					SetAnimationName(e,"off")
					PlayAnimation(e)
					PerformLogicConnections(e)
					wait[e] = g_Time + 5000
					SetActivated(e,0)
					doonce[e] = 1	
				end				
			end
		end
	end
	
	if g_Entity[e]['activated'] == 0 then
		doonce[e] = 0
		played[e] = 0
		if g_Time >= wait[e] then
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