-- DESCRIPTION: The object will hover up and down it`s Y axis by [HoverHeight=20(1,100)] units.

g_hover_heightangle = {}
g_hover = {}

function hover_properties(e,hoverheight)
	g_hover[e]['hoverheight'] = hoverheight
end 

function hover_init(e)
	g_hover[e] = {}
	g_hover[e]['hoverheight'] = 20
	CollisionOff(e)
	GravityOff(e)
	g_hover_heightangle[e] = 0
end
function hover_main(e)
	local nhoverheight = g_hover[e]['hoverheight']
	g_hover_heightangle[e] = g_hover_heightangle[e] + (GetAnimationSpeed(e)/100.0)
	local fFinalY = GetGroundHeight(g_Entity[e]['x'],g_Entity[e]['z']) + nhoverheight + (math.cos(g_hover_heightangle[e])*nhoverheight)
	SetPosition(e,g_Entity[e]['x'],fFinalY,g_Entity[e]['z'])
end
