-- Hover v8
-- DESCRIPTION: The object will hover up and down it`s Y axis by [HoverHeight=20(1,100)] from it's [LowerHeight=0(0,100)] units at [Speed=100(1,200)].
-- DESCRIPTION: [ActiveAtStart!=1] if unchecked use a switch or zone trigger to activate.
-- DESCRIPTION: [@USE_VARIABLE_SWITCH=2(1=Yes,2=No)]
-- DESCRIPTION: [@@VARIABLE_SWITCH_USER_GLOBAL$=""(0=globallist)] The unique user global for this switch (eg; Variable_Switch1)

g_hover_heightangle 	= {}
g_hover 				= {}

local hoverheight
local lowerheight
local speed
local ActivateAtStart	= {}
local use_variable_switch 			= {}
local variable_switch_user_global	= {}

local status			= {}
local currentY			= {}
local currentvalue = {}
local variablevalue = {}

function hover_properties(e,hoverheight, lowerheight, speed, ActivateAtStart, use_variable_switch, variable_switch_user_global)
	g_hover[e]['hoverheight'] = hoverheight
	g_hover[e]['lowerheight'] = lowerheight
	g_hover[e]['speed'] = speed
	g_hover[e]['ActivateAtStart'] = ActivateAtStart
	g_hover[e]['use_variable_switch'] = use_variable_switch
	g_hover[e]['variable_switch_user_global'] = variable_switch_user_global
end 

function hover_init(e)
	g_hover[e] = {}
	g_hover[e]['hoverheight'] = 20
	g_hover[e]['lowerheight'] = 0
	g_hover[e]['speed'] = 100
	g_hover[e]['ActivateAtStart'] = 1
	g_hover[e]['use_variable_switch'] = 2
	g_hover[e]['variable_switch_user_global'] = ""
	
	CollisionOff(e)
	GravityOff(e)
	currentvalue[e] = 0
	variablevalue[e]=0
	status[e] = "init"
	currentY[e] = g_hover[e]['lowerheight']
	g_hover_heightangle[e] = 0
end
function hover_main(e)
	if status[e] == "init" then
		if g_hover[e]['ActivateAtStart'] == 1 then SetActivated(e,1) end
		if g_hover[e]['ActivateAtStart'] == 0 then SetActivated(e,0) end
		status[e] = "start"
	end	

	if g_Entity[e]['activated'] == 1 then
		if g_hover[e]['use_variable_switch'] == 1 then
			if g_hover[e]['variable_switch_user_global'] ~= "" then
				if _G["g_UserGlobal['"..g_hover[e]['variable_switch_user_global'].."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..g_hover[e]['variable_switch_user_global'].."']"] end
				variablevalue[e] = _G["g_UserGlobal['"..g_hover[e]['variable_switch_user_global'].."']"]
				if variablevalue[e] >= 199 then variablevalue[e] = 199 end
				g_hover[e]['speed'] = 200 - variablevalue[e]
			end
		end
		if status[e] == "start" then
			local nhoverheight = g_hover[e]['hoverheight']
			local lhoverheight = g_hover[e]['lowerheight']
			if currentY[e] < g_hover[e]['hoverheight'] then
				g_hover_heightangle[e] = g_hover_heightangle[e] + (GetAnimationSpeed(e)/g_hover[e]['speed'])
				local fFinalY = (GetGroundHeight(g_Entity[e]['x'],g_Entity[e]['z'])+ lhoverheight) + nhoverheight - (math.cos(g_hover_heightangle[e])*nhoverheight)
				CollisionOff(e)
				ResetPosition(e,g_Entity[e]['x'],fFinalY,g_Entity[e]['z'])
				CollisionOn(e)
			end
			if currentY[e] >= g_hover[e]['hoverheight'] then status[e] = "hovering" end
		end
		if status[e] == "hovering" then
			local nhoverheight = g_hover[e]['hoverheight']
			local lhoverheight = g_hover[e]['lowerheight']
			g_hover_heightangle[e] = g_hover_heightangle[e] + (GetAnimationSpeed(e)/g_hover[e]['speed'])
			local fFinalY = (GetGroundHeight(g_Entity[e]['x'],g_Entity[e]['z'])+ lhoverheight) + nhoverheight + (math.cos(g_hover_heightangle[e])*nhoverheight)
			CollisionOff(e)
			ResetPosition(e,g_Entity[e]['x'],fFinalY,g_Entity[e]['z'])
			CollisionOn(e)
		end		
	end	
end