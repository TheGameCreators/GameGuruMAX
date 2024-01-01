-- Hover v6
-- DESCRIPTION: The object will hover up and down it`s Y axis by [HoverHeight=20(1,100)] from it's [LowerHeight=0(0,100)] units at [Speed=100(1,200)].
-- DESCRIPTION: [ActiveAtStart!=1] if unchecked use a switch or zone trigger to activate.

g_hover_heightangle 	= {}
g_hover 				= {}

local ActivateAtStart	= {}
local status			= {}
local currentY			= {}

function hover_properties(e,hoverheight, lowerheight, speed, ActivateAtStart)
	g_hover[e]['hoverheight'] = hoverheight
	g_hover[e]['lowerheight'] = lowerheight
	g_hover[e]['speed'] = speed
	g_hover[e]['ActivateAtStart'] = ActivateAtStart
end 

function hover_init(e)
	g_hover[e] = {}
	g_hover[e]['hoverheight'] = 20
	g_hover[e]['lowerheight'] = 0
	g_hover[e]['speed'] = 100
	g_hover[e]['ActivateAtStart'] = 1
	CollisionOff(e)
	GravityOff(e)
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