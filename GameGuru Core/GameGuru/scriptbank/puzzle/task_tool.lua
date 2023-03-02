-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Task Tool v4 by Necrym59
-- DESCRIPTION: This object will give the player a designated task-tool if collected.
-- DESCRIPTION: [PROMPT_TEXT$="E to collect"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [AUTO_PICKUP!=0]
-- DESCRIPTION: [@TOOL_TYPE=1(1=Crowbar, 2=Screwdriver, 3=Spanner, 4=Cutter, 5=Named Tool)] [TOOL_NAME$="Named Tool"]
-- DESCRIPTION: Play the audio <Sound0> when picked up.

	g_tasktool = {}
	local tasktool = {}
	local prompt_text = {}
	local pickup_range = {}
	local auto_pickup = {}
	local tool_type = {}
	local tool_name = {}
	local collected = {}
	local state = {}

function task_tool_properties(e, prompt_text, pickup_range, auto_pickup, tool_type, tool_name)
	tasktool[e] = g_Entity[e]
	tasktool[e].prompt_text = prompt_text
	tasktool[e].pickup_range = pickup_range
	tasktool[e].auto_pickup = auto_pickup
	tasktool[e].tool_type = tool_type
	tasktool[e].tool_name = tool_name
end

function task_tool_init(e)	
	tasktool[e] = g_Entity[e]	
	tasktool[e].prompt_text = "to collect"
	tasktool[e].pickup_range = 80
	tasktool[e].auto_pickup = 0
	tasktool[e].tool_type = 1
	tasktool[e].tool_name = ""
	g_tasktool = 0
	collected[e] = 0
	state[e] = "init"
end

function task_tool_main(e)
	tasktool[e] = g_Entity[e]
	if state[e] == "init" then	
		if tasktool[e].tool_type == 1 then tasktool[e].tool_name = "Crowbar" end
		if tasktool[e].tool_type == 2 then tasktool[e].tool_name = "Screwdriver" end
		if tasktool[e].tool_type == 3 then tasktool[e].tool_name = "Spanner" end
		if tasktool[e].tool_type == 4 then tasktool[e].tool_name = "Cutter" end
		if tasktool[e].tool_type == 5 then tasktool[e].tool_name = tasktool[e].tool_name end
		state[e] = "endinit"
	end
	local PlayerDist = GetPlayerDistance(e)
	
	if PlayerDist < 100 and g_PlayerHealth > 0 and collected[e] == 0 then
		if tasktool[e].auto_pickup == 0 then 
			local LookingAt = GetPlrLookingAtEx(e,1)			
			if LookingAt == 1 then
				if GetGamePlayerStateXBOX() == 1 then
					PromptLocalForVR(e,"Y Button " ..tasktool[e].prompt_text,3)
				else
					if GetHeadTracker() == 1 then
						PromptLocalForVR(e,"Trigger " ..tasktool[e].prompt_text,3)
					else
						PromptLocalForVR(e,tasktool[e].prompt_text,3)
					end					
				end			
				if g_KeyPressE == 1 then
					PlaySound(e,0)
					PromptLocalForVR(e,tasktool[e].tool_name.. " collected",3)
					PerformLogicConnections(e)
					g_tasktool = tasktool[e].tool_type
					collected[e] = 1
					Destroy(e)
				end
			end
		end
		if tasktool[e].auto_pickup == 1 then 
			PlaySound(e,0)
			PromptLocalForVR(e,tasktool[e].tool_name.. " collected",3)
			PerformLogicConnections(e)
			g_tasktool = tasktool[e].tool_type
			collected[e] = 1
			Destroy(e)			
		end
	end
end

