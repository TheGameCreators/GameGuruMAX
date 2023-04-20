-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Weight Switch v9: by Necrym59
-- DESCRIPTION: Attach to the Weight Switch Object? This object will be treated as a switch object for activating other objects or game elements.
-- DESCRIPTION: Set Object to Physics=ON, Collision=BOX, IsImobile=ON. Use AlphaClipping to make invisible if required.
-- DESCRIPTION: [PROMPT_TEXT$="Weight needed to activate"]
-- DESCRIPTION: [ACTIVATION_TEXT$="You have activated the switch"]
-- DESCRIPTION: [ACTIVATION_WEIGHT=300(0,1000)]
-- DESCRIPTION: [PLAYER_WEIGHT=100(0,100)]
-- DESCRIPTION: [MOVEMENT=3(1,10)]
-- DESCRIPTION: Play <Sound0> when used.

	local U = require "scriptbank\\utillib"

	g_carryingweight		= {}
	local weightswitch 		= {}
	local prompt_text 		= {}	
	local activation_text 	= {}
	local activation_weight	= {}
	local player_weight 	= {}
	local switch_movement 	= {}

	local sense_area 		= {}
	local status 			= {}
	local activated 		= {}
	local accrued_weight	= {}
	local playeronswitch 	= {}
	local objectonswitch 	= {}
	local max_height 		= {}
	local min_height 		= {}
	local pEnt				= {}
	
function weight_switch_properties(e, prompt_text, activation_text, activation_weight, player_weight, switch_movement)
	weightswitch[e] = g_Entity[e]
	weightswitch[e].prompt_text = prompt_text	
	weightswitch[e].activation_text = activation_text
	weightswitch[e].activation_weight = activation_weight
	weightswitch[e].player_weight = player_weight
	weightswitch[e].switch_movement = switch_movement
end 

function weight_switch_init(e)
	weightswitch[e] = g_Entity[e]
	weightswitch[e].prompt_text = "Weight needed to activate"
	weightswitch[e].activation_text = "You have activated the switch"
	weightswitch[e].activation_weight = 300
	weightswitch[e].player_weight = 100
	weightswitch[e].switch_movement = 3
	status[e] = 'init'
	playeronswitch[e] = 0
	objectonswitch[e] = 0
	activated[e] = 0
	accrued_weight[e] = 0
	g_carryingweight = 0
end 

function weight_switch_main(e)	
	weightswitch[e] = g_Entity[e]	
	-- init -------------------------------------------------------------------------------------
	if status[e] == 'init' then	
		local xmin, ymin, zmin, xmax, ymax, zmax = GetObjectColBox(g_Entity[e]['obj'])
		local sx, sy, sz = GetObjectScales(g_Entity[e]['obj'])
		local w, h, l = (xmax - xmin) * sx, (ymax - ymin) * sy, (zmax - zmin) * sz		
		if w > l then sense_area[e] = l end
		if w == l then sense_area[e] = w end
		if w < l then sense_area[e] = w end		
		max_height[e] = g_Entity[e]['y']
		min_height[e] = (g_Entity[e]['y']-weightswitch[e].switch_movement)		
		StartTimer(e)		
		status[e] = 'checkweight'
	end	
	-- Check weight  ----------------------------------------------------------------------------
	if status[e] == 'checkweight' then
		for a = 1, g_EntityElementMax do
			if a ~= nil and g_Entity[a] ~= nil then			
				local tpx, tpy, tpz, tax, tay, taz = GetObjectPosAng(g_Entity[a]['obj'])
				if g_Entity[a]['y'] < g_Entity[e]['y'] + 5 then
					if U.CloserThan(tpx, tpy, tpz, g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'],sense_area[e]) then						
						if g_KeyPressE == 0 then objectonswitch[e]=IntersectAll(g_Entity[a]['x'],g_Entity[a]['y'],g_Entity[a]['z'],g_Entity[e]['x'],-1,g_Entity[e]['z'],0) end
						if objectonswitch[e] > 0 then
							pEnt[e] = a
							accrued_weight[e] = accrued_weight[e] + GetEntityWeight(pEnt[e])					
						end					
					end
				end
			end		
		end
		if U.PlayerCloserThanPos(g_Entity[e]['x'], g_PlayerPosY, g_Entity[e]['z'],sense_area[e]) then
			playeronswitch[e]=IntersectAll(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,g_Entity[e]['x'],-1,g_Entity[e]['z'],0)
			if playeronswitch[e] > 0 then 
				accrued_weight[e] = accrued_weight[e] + (weightswitch[e].player_weight + g_carryingweight)
			else
				accrued_weight[e] = accrued_weight[e]
			end					
		end
		status[e] = 'active'
	end
	-- Weight Switch  ----------------------------------------------------------------------------	
			
	if status[e] == 'active' then			
		if accrued_weight[e] < weightswitch[e].activation_weight then			
			if playeronswitch[e] > 1 then Prompt(weightswitch[e].prompt_text) end
			playeronswitch[e] = 0
			status[e] = 'checkweight'
			accrued_weight[e] = 0
		end	
		if accrued_weight[e] >= weightswitch[e].activation_weight then
			if activated[e] == 0 then
				PromptDuration(weightswitch[e].activation_text,3000)
				ActivateIfUsed(e)				
				PerformLogicConnections(e)
				PlaySound(e,0)
				SetPosition(e, g_Entity[e]['x'],min_height[e],g_Entity[e]['z'])
				ResetPosition(e, g_Entity[e]['x'],min_height[e],g_Entity[e]['z'])
				activated[e] = 1
				status[e] = "activated"
			end
		end			
	end
end


function weight_switch_exit(e)
end