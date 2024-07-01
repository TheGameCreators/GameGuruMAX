-- Action Plate v6: by Necrym59
-- DESCRIPTION: Attach to the Action Plate Object? This object will be treated as a switch object for activating other objects or game elements.
-- DESCRIPTION: Set Object to Physics=ON, Collision=BOX, IsImobile=ON. Use AlphaClipping to make invisible if required.
-- DESCRIPTION: Set [ACTIVATION_TEXT$="You have activated a panel"]
-- DESCRIPTION: Plate [@ACTIVATION_TYPE=1(1=Single Use, 2=Multi Use)]
-- DESCRIPTION: Plate [MOVEMENT=3(0,100)]
-- DESCRIPTION: Play <Sound0> when used.

local U = require "scriptbank\\utillib"	
local actionplate 		= {}	
local activation_text 	= {}
local activation_type	= {}	
local movement 			= {}
local status 			= {}
local activated 		= {}
local onactionplate 	= {}
local max_height 		= {}
local min_height 		= {}
local sense_area 		= {}
local wait				= {}
local heightY			= {}

	
function actionplate_properties(e, activation_text, activation_type, movement)
	actionplate[e].activation_text = activation_text
	actionplate[e].activation_type = activation_type
	actionplate[e].movement = movement
end 

function actionplate_init(e)
	actionplate[e] = {}
	actionplate[e].activation_text = "You have activated a panel"
	actionplate[e].activation_type = 1
	actionplate[e].movement = 3
	status[e] = "init"
	heightY[e] = 0
	wait[e] = math.huge
	onactionplate[e] = 0
	activated[e] = 0
	GravityOff(e)
	heightY[e] = GetSurfaceHeight(g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'])
end 

function actionplate_main(e)	
	
	-- Action Plate init ----------------------------------------------------------------
	if status[e] == "init" then		
		local xmin, ymin, zmin, xmax, ymax, zmax = GetObjectColBox(g_Entity[e]['obj'])
		local sx, sy, sz = GetObjectScales(g_Entity[e]['obj'])
		local w, h, l = (xmax - xmin) * sx, (ymax - ymin) * sy, (zmax - zmin) * sz
		if w > l then sense_area[e] = l end
		if w == l then sense_area[e] = w end
		if w < l then sense_area[e] = w end
		max_height[e] = heightY[e]
		min_height[e] = (heightY[e]-actionplate[e].movement)
		SetPosition(e, g_Entity[e]['x'],max_height[e],g_Entity[e]['z'])
		GravityOn(e)
		status[e] = "endinit"
	end	
		
	-- Action Plate  ----------------------------------------------------------------------------	
	if U.PlayerCloserThanPos(g_Entity[e]['x'], g_PlayerPosY, g_Entity[e]['z'],sense_area[e]) then
		onactionplate[e]=IntersectAll(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,actionplate[e]['x'],-1,actionplate[e]['z'],0)
	else
		onactionplate[e] = 0
	end
	
	if onactionplate[e] == 0 then
		if status[e] == "reset" then
			if g_Time > wait[e] then
				activated[e] = 0
				ResetPosition(e, g_Entity[e]['x'],max_height[e],g_Entity[e]['z'])	
			end		
		end
		if status[e] == "finished" then
			onactionplate[e] = 0
			activated[e] = 1
		end
	end
	
	if onactionplate[e] > 0 then	
		if activated[e] == 0 then
			ActivateIfUsed(e)
			PerformLogicConnections(e)
			PlaySound(e,0)
			if actionplate[e].movement > 0 then ResetPosition(e, g_Entity[e]['x'],min_height[e],g_Entity[e]['z']) end
			PromptDuration(actionplate[e].activation_text,1000)
			activated[e] = 1
			if actionplate[e].activation_type == 1 then status[e] = "finished" end
			if actionplate[e].activation_type == 2 then
				status[e] = "reset"
				wait[e] = g_Time + 2000
			end								
		end
	end
end
	
function actionplate_exit(e)	
end