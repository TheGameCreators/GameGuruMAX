-- Collection Control v9 by Necrym59
-- DESCRIPTION: This behavior allows for configuration for control of a collection count.
-- DESCRIPTION: Use the Collection Count behavior for the pickup items.
-- DESCRIPTION: [OBJECTIVES=6] to collect to win
-- DESCRIPTION: [COLLECTION_TIME=60(10,500)] total in seconds
-- DESCRIPTION: [@ON_COMPLETION=1(1=Next Level, 2=Activate Logic links, 3=Activate IfUsed, 4=Win Game)] controls whether to end level or activate links.
-- DESCRIPTION: [@ON_FAILURE=2(1=Nothing, 2=Lose Game)]
-- DESCRIPTION: [DISPLAY_X=50]
-- DESCRIPTION: [DISPLAY_Y=10]
-- DESCRIPTION: [DISPLAY_SIZE=3(1,5)]
-- DESCRIPTION: [@GoToLevelMode=1(1=Use Storyboard Logic,2=Go to Specific Level)] controls whether to load the next level in the Storyboard, or a specific level.
-- DESCRIPTION: <Sound0> plays when objectives completed

g_collection_objectives	= {}
g_collection_time		= {}
g_collection_counted	= 0

local cc_control		= {}
local objectives 		= {}
local collection_time	= {}
local on_completion		= {}
local on_failure		= {}
local display_x			= {}
local display_y			= {}
local display_size		= {}
local status			= {}
local doonce			= {}
local doend				= {}
local timeleft			= {}

function collection_control_properties(e, objectives, collection_time, on_completion, on_failure, display_x, display_y, display_size)
	cc_control[e].objectives = objectives
	cc_control[e].collection_time = collection_time
	cc_control[e].on_completion = on_completion
	cc_control[e].on_failure = on_failure	
	cc_control[e].display_x = display_x
	cc_control[e].display_y = display_y	
	cc_control[e].display_size = display_size
end

function collection_control_init(e)
	cc_control[e] = {}
	cc_control[e].objectives = 6
	cc_control[e].collection_time = 60
	cc_control[e].on_completion = 1
	cc_control[e].on_failure = 2
	cc_control[e].display_x = 50
	cc_control[e].display_y = 10
	cc_control[e].display_size = 3	
	status[e] = "init"
	doonce[e] = 0
	doend[e] = 0
	timeleft[e] = 0
end

function collection_control_main(e)
	if status[e] == "init" then	
		g_collection_objectives = cc_control[e].objectives
		g_collection_time = cc_control[e].collection_time * 1000		
		g_collection_counted = 0
		timeleft[e] = g_collection_time
		status[e] = "endinit"
	end

	if g_collection_counted >= cc_control[e].objectives then
		if doend[e] == 0 then
			PlaySound(e,0)
			if cc_control[e].on_completion == 1 then JumpToLevelIfUsed(e) end
			if cc_control[e].on_completion == 2 then PerformLogicConnections(e) end
			if cc_control[e].on_completion == 3 then ActivateIfUsed(e) end	
			if cc_control[e].on_completion == 4 then WinGame() end			
			doend[e] = 1
			SwitchScript(e,"no_behavior_selected.lua")
		end
	end
	if g_collection_counted > 0 and doonce[e] == 0 then
		StartTimer(e)
		doonce[e] = 1
	end			
	if timeleft[e] > 0 and g_collection_counted > 0 and g_collection_counted < cc_control[e].objectives then
		timeleft[e] = math.floor(g_collection_time/1000)-math.floor(GetTimer(e)/1000)
		TextCenterOnX(cc_control[e].display_x,cc_control[e].display_y,cc_control[e].display_size,g_collection_counted.."/"..cc_control[e].objectives	.." completed " ..timeleft[e].. " seconds left")
	end
	if timeleft[e] <= 0 then
		if cc_control[e].on_failure == 1 then end
		if cc_control[e].on_failure == 2 then LoseGame() end
	end
end
