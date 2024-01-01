-- Switch_Count v7
-- DESCRIPTION: Will create a switch series to ActivateIfUsed when completed.
-- DESCRIPTION: Activate all [SWITCHES=4] to complete.
-- DESCRIPTION: Switch [@SWITCH_STATE=1(1=On, 2=Off)].
-- DESCRIPTION: Set [USE_RANGE=60 (1,100)] distance
-- DESCRIPTION: Set [USE_PROMPT$="E to use"]
-- DESCRIPTION: Count [MESSAGE_DURATION=2 (1,5)] seconds.
-- DESCRIPTION: Play <Sound0> when switch activated
-- DESCRIPTION: Play <Sound1> when completed
-- DESCRIPTION: Select [@COMPLETION=1(1=End Level, 2=Activate if Used)] controls whether to end level or activate if used object.

g_SwitchesActivated = 0
local switch_count = {}
local U = require "scriptbank\\utillib"
local switch_count_active 	= {}
local doonce 				= {}
local tEnt 					= {}
local selectobj 			= {}

function switch_count_properties(e, switches, state, use_range, use_prompt, message_duration, completion)
	switch_count[e] = g_Entity[e]
	switch_count[e].switches = switches
	switch_count[e].state = state
	switch_count[e].use_range = use_range
	switch_count[e].use_prompt = use_prompt
	switch_count[e].message_duration = message_duration
	switch_count[e].completion = completion
end 

function switch_count_init(e)
	switch_count[e] = {}
	switch_count[e].switches = 4
	switch_count[e].state = 1
	switch_count[e].use_range = 70
	switch_count[e].use_prompt = "E to use"
	switch_count[e].message_duration = 1
	switch_count[e].completion = 1
	switch_count_active[e] = 1
	doonce[e] = 0
	tEnt[e] = 0
	selectobj[e] = 0
	if switch_count[e].state == 1 then
		SetAnimationName(e,"off")
		PlayAnimation(e)
		StopAnimation(e)
		switch_count[e].state = 2
	else
		SetAnimationName(e,"on")
		PlayAnimation(e)
		StopAnimation(e)
		switch_count[e].state = 1
	end
end
	 
function switch_count_main(e)
	if switch_count_active[e] == nil then
		switch_count_active[e] = 1
	end
		
	if switch_count_active[e] == 1 then
		local PlayerDist = GetPlayerDistance(e)
		if PlayerDist <= switch_count[e].use_range then
			-- pinpoint select object--				
			local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
			local rayX, rayY, rayZ = 0,0,switch_count[e].use_range
			local paX, paY, paZ = math.rad(GetCameraAngleX(0)), math.rad(GetCameraAngleY(0)), math.rad(GetCameraAngleZ(0))
			rayX, rayY, rayZ = U.Rotate3D(rayX, rayY, rayZ, paX, paY, paZ)
			selectobj[e]=IntersectAll(px,py,pz, px+rayX, py+rayY, pz+rayZ,e)
			if selectobj[e] ~= 0 or selectobj[e] ~= nil then
				if g_Entity[e].obj == selectobj[e] then
					TextCenterOnXColor(50-0.01,50,3,"+",255,255,255) --highliting (with crosshair at present)
					tEnt[e] = e
				else 
					tEnt[e] = 0			
				end
				if g_Entity[e].obj ~= selectobj[e] then selectobj[e] = 0 end
			end
			if selectobj[e] == 0 or selectobj[e] == nil then
				tEnt[e] = 0
				TextCenterOnXColor(50-0.01,50,3,"+",155,155,155) --highliting (with crosshair at present)
			end
			--end pinpoint select object--		
		end	
		if PlayerDist <= switch_count[e].use_range and tEnt[e] ~= 0 then
			Prompt(switch_count[e].use_prompt)
			if g_KeyPressE == 1 then
				g_SwitchesActivated = g_SwitchesActivated + 1				
				switch_count_active[e] = 0
				if doonce[e] == 0 then 
					SetActivated(e,1)
					if switch_count[e].state == 1 then SetAnimationName(e,"on") end
					if switch_count[e].state == 2 then SetAnimationName(e,"off") end
					PlayAnimation(e)					
					StopAnimation(e)
					PlaySound(e,0)
					PerformLogicConnections(e)
					doonce[e] = 1
				end
				PromptDuration(g_SwitchesActivated.."/"..switch_count[e].switches	.." complete", switch_count[e].message_duration*1000)
			end			
		end
		if g_SwitchesActivated >= switch_count[e].switches then
			PlaySound(e,1)
			if switch_count[e].completion == 1 then JumpToLevelIfUsed(e) end
			if switch_count[e].completion == 2 then ActivateIfUsed(e) end
			g_SwitchesActivated = 0
		end
	end
end
