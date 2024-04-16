-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Task Object v10 by Necrym59
-- DESCRIPTION: This object will be treated as a switch object for activating other objects or game elements. Set Always Active = On
-- DESCRIPTION: [TASK_TEXT$ = "Tool is required to use"]
-- DESCRIPTION: [USE_RANGE=80(1,100)]
-- DESCRIPTION: [@TOOL_REQUIRED=1(1=Crowbar, 2=Screwdriver, 3=Spanner, 4=Cutter, 5=Named Tool)],
-- DESCRIPTION: [TOOL_NAME$="Named Tool"],
-- DESCRIPTION: [TASK_USE_TEXT$="E to use the tool to activate"]
-- DESCRIPTION: [TASK_DONE_TEXT$="Task Completed"]
-- DESCRIPTION: [@VISIBLE=1(1=Yes, 2=No)]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [SWITCH_VALUE=0(0,99)] success result for use with combo switches
-- DESCRIPTION: Play <Sound0> when using tool.
-- DESCRIPTION: Play <Sound1> task completed.

g_tasktool 				= {}
g_tasktoolname 			= {}
g_swcvalue 				= {}

local taskobject 		= {}
local task_text 		= {}
local use_range 		= {}
local tool_required 	= {}
local tool_name 		= {}
local task_use_text		= {}
local task_done_text 	= {}
local visible 			= {}
local prompt_display 	= {}
local switch_value 		= {}

local unlocked 			= {}
local usereset			= {}
local doonce			= {}
local played			= {}
local wait 				= {}
local status 			= {}

function task_object_properties(e, task_text, use_range, tool_required, tool_name, task_use_text, task_done_text, visible, prompt_display, switch_value)
	taskobject[e].task_text = task_text
	taskobject[e].use_range = use_range
	taskobject[e].tool_required = tool_required
	taskobject[e].tool_name = tool_name
	taskobject[e].task_use_text = task_use_text
	taskobject[e].task_done_text = task_done_text
	taskobject[e].visible = visible
	taskobject[e].prompt_display = prompt_display
	taskobject[e].switch_value = switch_value
end

function task_object_init(e)
	taskobject[e] = {}
	taskobject[e].task_text = ""
	taskobject[e].use_range = 80
	taskobject[e].tool_required = 1
	taskobject[e].tool_name = ""
	taskobject[e].task_use_text = ""
	taskobject[e].task_done_text = ""
	taskobject[e].visible = 1
	taskobject[e].prompt_display = 1
	taskobject[e].switch_value = 0

	status[e] = "init"
	unlocked[e] = 0
	usereset[e] = 0
	doonce[e] = 0
	played[e] = 0
	wait[e] = math.huge
	g_tasktool = 0
	g_swcvalue = 0
end

function task_object_main(e)

	if status[e] == "init" then
		if g_tasktool == nil then g_tasktool = 0 end
		if taskobject[e].tool_required == 1 then taskobject[e].tool_name = "Crowbar" end
		if taskobject[e].tool_required == 2 then taskobject[e].tool_name = "Screwdriver" end
		if taskobject[e].tool_required == 3 then taskobject[e].tool_name = "Spanner" end
		if taskobject[e].tool_required == 4 then taskobject[e].tool_name = "Cutter" end
		if taskobject[e].tool_required == 5 then taskobject[e].tool_name = taskobject[e].tool_name end
		if taskobject[e].visible == 1 then
			Show(e)
			CollisionOn(e)
		end
		if taskobject[e].visible == 2 then
			Hide(e)
			CollisionOff(e)
		end
		if taskobject[e].switch_value >= 99 then taskobject[e].switch_value = 98 end
		status[e] = "endinit"
	end

	local PlayerDist = GetPlayerDistance(e)

	if PlayerDist < taskobject[e].use_range then
		local LookingAt = GetPlrLookingAtEx(e,1)
		if LookingAt == 1 then
			if g_tasktoolname ~= taskobject[e].tool_name and unlocked[e] == 0 then
				if taskobject[e].prompt_display == 1 then PromptLocal(e,taskobject[e].task_text) end
				if taskobject[e].prompt_display == 2 then Prompt(taskobject[e].task_text) end
			end
			if g_tasktoolname ~= taskobject[e].tool_name and unlocked[e] == 1 then
				if taskobject[e].prompt_display == 1 then PromptLocal(e,taskobject[e].task_done_text) end
				if taskobject[e].prompt_display == 2 then Prompt(taskobject[e].task_done_text) end
			end
			if g_tasktoolname == taskobject[e].tool_name and usereset[e] == 0 then
				if taskobject[e].prompt_display == 1 then PromptLocal(e,taskobject[e].task_use_text) end
				if taskobject[e].prompt_display == 2 then Prompt(taskobject[e].task_use_text) end
			end

			if g_KeyPressE == 1 then
				if g_tasktool == taskobject[e].tool_required then
					if g_tasktoolname == taskobject[e].tool_name then
						Show(e)
						CollisionOn(e)
						if played[e] == 0 then
							PlaySound(e,0)
							played[e] = 1
						end
						if taskobject[e].prompt_display == 1 then PromptLocal(e,taskobject[e].task_done_text) end
						if taskobject[e].prompt_display == 2 then Prompt(taskobject[e].task_done_text) end
						SetActivated(e,1)
						SetActivatedWithMP(e,201)
						SetAnimationName(e,"on")
						PlayAnimation(e)
						PerformLogicConnections(e)
						ActivateIfUsed(e)
						usereset[e] = 1
						wait[e] = g_Time + 1000
						g_tasktool = 0
						g_tasktoolname = ""
						unlocked[e] = 1
						played[e] = 0
						g_swcvalue = g_swcvalue + taskobject[e].switch_value
					end
				end
				if unlocked[e] == 1 then
					Show(e)
					CollisionOn(e)
					if taskobject[e].prompt_display == 1 then PromptLocal(e,taskobject[e].task_done_text) end
					if taskobject[e].prompt_display == 2 then Prompt(taskobject[e].task_done_text) end
					SetActivated(e,1)
					unlocked[e] = 1
					SetAnimationName(e,"on")
					PlayAnimation(e)
					PerformLogicConnections(e)
					ActivateIfUsed(e)
					usereset[e] = 1
					played[e] = 0
					wait[e] = g_Time + 1000					
				end
				if g_tasktool ~= taskobject[e].tool_required and unlocked[e] == 0 then
					if taskobject[e].prompt_display == 1 then PromptLocal(e,taskobject[e].task_text) end
					if taskobject[e].prompt_display == 2 then Prompt(taskobject[e].task_text) end
				end
			end
		end
	end
	if g_Entity[e]['activated'] == 1 and g_Time > wait[e] then
		if doonce[e] == 0 then			
			Show(e)
			CollisionOn(e)
			if played[e] == 0 then
				PlaySound(e,1)
				played[e] = 1
			end
			if taskobject[e].prompt_display == 1 then PromptLocal(e,taskobject[e].task_done_text) end
			if taskobject[e].prompt_display == 2 then Prompt(taskobject[e].task_done_text) end
			unlocked[e] = 1		
			SetAnimationName(e,"on")
			PlayAnimation(e)
			usereset[e] = 1
			wait[e] = g_Time + 3000
			SetActivated(e,0)			
			doonce[e] = 1
		end
	end

	if g_Time > wait[e] and unlocked[e] == 1 then -- Reset
		usereset[e] = 0
		played[e] = 0
		doonce[e] = 0
	end
end