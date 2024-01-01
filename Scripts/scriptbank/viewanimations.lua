-- DESCRIPTION: Allows you to view an object's animations in game when player within [RANGE=1000] distance. 
-- DESCRIPTION: Use 'Left Mouse Button' to cycle forwards (and 'Right Mouse Button' to reverse) by 1 frame.
-- DESCRIPTION: Hold 'Shift' and 'Left Mouse Button' (or 'Right Mouse Button') to play the animation (while held).
-- DESCRIPTION: Cycle through animation range from [STARTFRAME=0] to [ENDFRAME=1000].

viewanimations_frm = 0
viewanimations_pressed = 0

g_viewanimations = {}

function viewanimations_properties(e, range, startframe, endframe)
	g_viewanimations[e]['range'] = range 
	g_viewanimations[e]['startframe'] = startframe
	g_viewanimations[e]['endframe'] = endframe
	SetAnimationFrames(e,g_viewanimations[e]['startframe'],g_viewanimations[e]['endframe'])
end 

function viewanimations_init(e)
	g_viewanimations[e] = {}
	g_viewanimations[e]['range'] = 1000
	g_viewanimations[e]['startframe'] = 0
	g_viewanimations[e]['endframe'] = 10000
 CharacterControlLimbo(e)
 SetAnimationFrames(e,g_viewanimations[e]['startframe'],g_viewanimations[e]['endframe'])
 LoopAnimation(e)
end

function viewanimations_main(e)
 
 CharacterControlLimbo(e)
 if GetPlayerDistance(e) < g_viewanimations[e]['range'] then 
	 RotateToPlayer(e)
	 if g_MouseClick==0 then
		viewanimations_pressed = 0
	 end
	 if g_KeyPressSHIFT==1 or viewanimations_pressed==0 then
		if g_MouseClick==1 then
		 StopAnimation(e)
		 viewanimations_frm = viewanimations_frm + 1
		 if viewanimations_frm > g_viewanimations[e]['endframe'] then viewanimations_frm = g_viewanimations[e]['startframe'] end 
		 viewanimations_pressed = 1
		end
		if g_MouseClick==2 then
		 StopAnimation(e)
		 viewanimations_frm = viewanimations_frm - 1
		 if viewanimations_frm < g_viewanimations[e]['startframe'] then viewanimations_frm = g_viewanimations[e]['endframe'] end 
		 if viewanimations_frm < 0 then viewanimations_frm = 0 end --no negative frames
		 viewanimations_pressed = 1
		end
	 end
	 SetAnimationFrame(e,viewanimations_frm)
	 Prompt ( "viewanimations_frm  =  " .. viewanimations_frm )
	end 
end