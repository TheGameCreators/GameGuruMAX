-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Task Object v4 by Necrym59
-- DESCRIPTION: This object will be treated as a switch object for activating other objects or game elements. Set Always Active = On
-- DESCRIPTION: [TASK_TEXT$ = "is required to use"], [USE_RANGE=80(1,100)],[@TOOL_REQUIRED=1(1=Crowbar, 2=Screwdriver, 3=Spanner, 4=Cutter, 5=Named Tool)], [TOOL_NAME$="Named Tool"], [TASK_USE_TEXT$="E to Use"], [TASK_DONE_TEXT$="Task Completed"]
-- DESCRIPTION: Play <Sound0> when using tool.
-- DESCRIPTION: Play <Sound1> task completed.

	g_tasktool = {}
	local taskobject = {}
	local task_text = {}
	local use_range = {}
	local tool_required = {}
	local tool_name = {}
	local task_use_text = {}
	local task_done_text = {}
	local status = {}
	local activated = {}
	local played = {}
	local state = {}

function task_object_properties(e, task_text, use_range, tool_required, tool_name, task_use_text, task_done_text, status)
	taskobject[e] = g_Entity[e]
	taskobject[e].task_text = task_text
	taskobject[e].use_range = use_range
	taskobject[e].tool_required = tool_required
	taskobject[e].tool_name = tool_name
	taskobject[e].task_use_text = task_use_text
	taskobject[e].task_done_text = task_done_text
	taskobject[e].status = 1
end 

function task_object_init(e)
	taskobject[e] = g_Entity[e]
	taskobject[e].task_text = ""	
	taskobject[e].use_range = 80
	taskobject[e].tool_required = 1
	taskobject[e].tool_name = ""
	taskobject[e].task_use_text = ""
	taskobject[e].task_done_text = ""
	taskobject[e].status = 1
	state[e] = "init"		
	activated[e] = 0	
	played = 0	
end

function task_object_main(e)
	taskobject[e] = g_Entity[e]	
	if state[e] == "init" then
		if g_tasktool == nil then g_tasktool = 0 end
		if taskobject[e].tool_required == 1 then taskobject[e].tool_name = "Crowbar" end
		if taskobject[e].tool_required == 2 then taskobject[e].tool_name = "Screwdriver" end
		if taskobject[e].tool_required == 3 then taskobject[e].tool_name = "Spanner" end
		if taskobject[e].tool_required == 4 then taskobject[e].tool_name = "Cutter" end
		if taskobject[e].tool_required == 5 then taskobject[e].tool_name = taskobject[e].tool_name end
		state[e] = "endinit"
	end
	
	local PlayerDist = GetPlayerDistance(e)
		
	if PlayerDist < taskobject[e].use_range and g_PlayerHealth > 0 then
		local LookingAt = GetPlrLookingAtEx(e,1)
		
		if taskobject[e].status == 1 then  --Disabled		
			if LookingAt == 1 then
				if g_tasktool ~= taskobject[e].tool_required then 
					if GetGamePlayerStateXBOX() == 1 then
						PromptLocalForVR(e,"Y Button " ..taskobject[e].tool_name.. " " ..taskobject[e].task_text,3)
					else
						if GetHeadTracker() == 1 then
							PromptLocalForVR(e,"Trigger " ..taskobject[e].tool_name.. " " ..taskobject[e].task_text,3)
						else
							PromptLocalForVR(e,taskobject[e].tool_name.. " " ..taskobject[e].task_text,3)
						end					
					end
				end
				if g_tasktool == taskobject[e].tool_required then 
					if GetGamePlayerStateXBOX() == 1 then
						PromptLocalForVR(e,"Y Button " ..taskobject[e].task_use_text,3)
					else
						if GetHeadTracker() == 1 then
							PromptLocalForVR(e,"Trigger " ..taskobject[e].task_use_text,3)
						else
							PromptLocalForVR(e,taskobject[e].task_use_text,3)
						end					
					end
				end
				
				if g_KeyPressE == 1 then
					if g_tasktool == taskobject[e].tool_required then
							PlaySound(e,0)						
							taskobject[e].status = 2
						else						
							activated[e] = 0
							PromptLocalForVR(e,taskobject[e].tool_name.. " " ..taskobject[e].task_text,3)
							taskobject[e].status = 1
						end
					end
				if g_KeyPressE == 0 then played = 0 end					
			end			
		end
		
		if taskobject[e].status == 2 then --Enabled
			
			if g_KeyPressE == 1 and activated[e] == 0 then
				PromptLocalForVR(e,taskobject[e].task_done_text,3)
				SetActivatedWithMP(e,201)
				activated[e] = 1
				PlaySound(e,1)
				SetAnimationName(e,"on")
				PlayAnimation(e)				
				PerformLogicConnections(e)
				g_tasktool = 0
			end			
		end		
	end
end
