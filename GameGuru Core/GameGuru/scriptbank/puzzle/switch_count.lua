-- Switch_Count v2
-- DESCRIPTION: Activate all [SWITCHES=4] to complete.
-- DESCRIPTION: Switch [@SWITCH_STATE=1(1=On, 2=Off)].
-- DESCRIPTION: Set [USE_RANGE=60 (1,100)] distance
-- DESCRIPTION: Set [USE_PROMPT$="E to use"]
-- DESCRIPTION: Count [MESSAGE_DURATION=2 (1,5)] seconds.
-- DESCRIPTION: Play <Sound0> when switch activated
-- DESCRIPTION: Play <Sound1> when completed
-- DESCRIPTION: Select [@COMPLETION=1(1=End Level, 2=Activate if Used)] controls whether to end level or activate if used object.

g_switch_count = {}
g_switch_count_counts = 0

local switch_count_active = {}
local doonce = {}

function switch_count_properties(e, switches, state, use_range, use_prompt, message_duration, completion)
	g_switch_count[e]['switches'] = switches
	g_switch_count[e]['state'] = state
	g_switch_count[e]['use_range'] = use_range
	g_switch_count[e]['use_prompt'] = use_prompt
	g_switch_count[e]['message_duration'] = message_duration
	g_switch_count[e]['completion'] = completion
end 

function switch_count_init(e)
	g_switch_count[e] = {}
	g_switch_count[e]['switches'] = 4
	g_switch_count[e]['state'] = 1
	g_switch_count[e]['use_range'] = 70
	g_switch_count[e]['use_prompt'] = "E to use"
	g_switch_count[e]['message_duration'] = 1
	g_switch_count[e]['completion'] = 1
	switch_count_active[e] = 1
	doonce[e] = 0
	if g_switch_count[e]['state'] == 1 then
		SetAnimationName(e,"off")
		PlayAnimation(e)
		StopAnimation(e)
		g_switch_count[e]['state'] = 2
	else
		SetAnimationName(e,"on")
		PlayAnimation(e)
		StopAnimation(e)
		g_switch_count[e]['state'] = 1
	end
end
	 
function switch_count_main(e)
	if switch_count_active[e] == nil then
		switch_count_active[e] = 1
	end
		
	if switch_count_active[e] == 1 then
		local PlayerDist = GetPlayerDistance(e)
		if PlayerDist <= g_switch_count[e]['use_range'] then
			Prompt(g_switch_count[e]['use_prompt'])
			if g_KeyPressE == 1 then
				g_switch_count_counts = g_switch_count_counts + 1				
				switch_count_active[e] = 0
				if doonce[e] == 0 then 
					SetActivated(e,1)
					if g_switch_count[e]['state'] == 1 then SetAnimationName(e,"on") end
					if g_switch_count[e]['state'] == 2 then SetAnimationName(e,"off") end
					PlayAnimation(e)					
					StopAnimation(e)
					PlaySound(e,0)
					PerformLogicConnections(e)
					doonce[e] = 1
				end
				PromptDuration(g_switch_count_counts.."/"..g_switch_count[e]['switches']	.." complete", g_switch_count[e]['message_duration']*1000)
			end			
		end
		if g_switch_count_counts >= g_switch_count[e]['switches'] then
			PlaySound(e,1)
			if g_switch_count[e]['completion'] == 1 then JumpToLevelIfUsed(e) end
			if g_switch_count[e]['completion'] == 2 then ActivateIfUsed(e) end
		end
	end
end
