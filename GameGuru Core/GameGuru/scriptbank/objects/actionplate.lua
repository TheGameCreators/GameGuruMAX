-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Action Plate v2: by Necrym59
-- DESCRIPTION: Attach to the Action Plate Object? This object will be treated as a switch object for activating other objects or game elements.
-- DESCRIPTION: Set Object to Physics=ON, Collision=BOX, IsImobile=ON. Use AlphaClipping to make invisible if required.
-- DESCRIPTION: Set [ACTIVATION_TEXT$="You have activated a panel"]
-- DESCRIPTION: Plate [@ACTIVATION_TYPE=1(1=Single Use, 2=Multi Use)]
-- DESCRIPTION: Plate [MOVEMENT=3(1,10)]
-- DESCRIPTION: Play <Sound0> when used.
	
	local actionplate 		= {}	
	local activation_text 	= {}
	local activation_type	= {}
	local movement 			= {}
	local status 			= {}
	local activated 		= {}
	local onactionplate 	= {}
	local max_height 		= {}
	local min_height 		= {}

	
function actionplate_properties(e, activation_text, activation_type, movement)
	actionplate[e] = g_Entity[e]	
	actionplate[e].activation_text = activation_text
	actionplate[e].activation_type = activation_type
	actionplate[e].movement = movement
end 

function actionplate_init(e)
	actionplate[e] = g_Entity[e]
	actionplate[e].activation_text = "You have activated a panel"
	actionplate[e].activation_type = 1
	actionplate[e].movement = 3
	status[e] = 'init'
	onactionplate[e] = 0
	activated[e] = 0
end 

function actionplate_main(e)	
	actionplate[e] = g_Entity[e]	
	-- Action Plate init ----------------------------------------------------------------
	if status[e] == 'init' then
		max_height[e] = g_Entity[e]['y']
		min_height[e] = (g_Entity[e]['y']-actionplate[e].movement)
		status[e] = 'active'
	end	
	
	-- Action Plate  ----------------------------------------------------------------------------	
	onactionplate[e]=IntersectAll(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,g_Entity[e]['x'],-1,g_Entity[e]['z'],0)
	
	if onactionplate[e] == 0 and actionplate[e].activation_type == 2 then		
		SetPosition(e, g_Entity[e]['x'],max_height[e],g_Entity[e]['z'])
		ResetPosition(e, g_Entity[e]['x'],max_height[e],g_Entity[e]['z'])
	end
	if onactionplate[e] > 0 then
	
		if activated[e] == 0 and status[e] == "active" then
			ActivateIfUsed(e)				
			PerformLogicConnections(e)
			PlaySound(e,0)
			SetPosition(e, g_Entity[e]['x'],min_height[e],g_Entity[e]['z'])
			ResetPosition(e, g_Entity[e]['x'],min_height[e],g_Entity[e]['z'])
			Prompt(actionplate[e].activation_text)
			activated[e] = 1
			if actionplate[e].activation_type == 1 then status[e] = "finished" end
			if actionplate[e].activation_type == 2 then
				status[e] = "inactive"
				StartTimer(e)
			end								
		end
		if activated[e] == 1 and status[e] == "active" then
			ActivateIfUsed(e)				
			PerformLogicConnections(e)
			PlaySound(e,0)			
			SetPosition(e, g_Entity[e]['x'],min_height[e],g_Entity[e]['z'])
			ResetPosition(e, g_Entity[e]['x'],min_height[e],g_Entity[e]['z'])			
			Prompt(actionplate[e].activation_text)
			activated[e] = 0
			status[e] = "inactive"
			StartTimer(e)
		end		
	end
	if status[e] == "inactive" and onactionplate[e] == 0 then
		if GetTimer(e) > 1000 then status[e] = "active" end		
	end	
	if status[e] == "finiished" then
		onactionplate[e] = 0		
	end
end
	
function actionplate_exit(e)	
end