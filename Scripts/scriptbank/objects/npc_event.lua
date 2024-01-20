-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- NPC Event v9 by Necrym59
-- DESCRIPTION: Triggers npc animation event then removes npc after set duration.
-- DESCRIPTION: Attach to a character. Trigger by linked switch or zone or range.
-- DESCRIPTION: Zone is destroyed after event.
-- DESCRIPTION: [EVENT_DURATION=10(1,100)] seconds
-- DESCRIPTION: [@IDLE_ANIMATION$=-1(0=AnimSetList)]
-- DESCRIPTION: [@EVENT_ANIMATION$=-1(0=AnimSetList)]
-- DESCRIPTION: [@END_TRIGGER=2(1=On, 2=Off)]
-- DESCRIPTION: [#FADE_SPEED=0.5(0.01,10.00)]
-- DESCRIPTION: [@EVENT_TRIGGER=1(1=Zone/Switch, 2=Range)]
-- DESCRIPTION: [RANGE = 500(0,3000)]
-- DESCRIPTION: [!INSTANT_RAGDOLL=0]
-- DESCRIPTION: <Sound0> for event 

	local npcevent 				= {}
	local event_trigger 		= {}	
	local event_duration 		= {}	
	local idle_animation		= {}
	local event_animation		= {}
	local end_trigger			= {}	

	local status 				= {}
	local anim_idle				= {}
	local anim_event			= {}
	local eventduration			= {}
	local fade_level			= {}

function npc_event_properties(e, event_duration, idle_animation, event_animation, end_trigger, fade_speed, event_trigger, range, instant_ragdoll)
	npcevent[e] = g_Entity[e]
	npcevent[e].event_duration = event_duration
	npcevent[e].idle_animation = "=" .. tostring(idle_animation)
	npcevent[e].event_animation = "=" .. tostring(event_animation)
	npcevent[e].end_trigger = end_trigger
	npcevent[e].fade_speed = fade_speed
	npcevent[e].event_trigger = event_trigger
	npcevent[e].range = range	
	npcevent[e].instant_ragdoll = instant_ragdoll
end

function npc_event_init(e)
	npcevent[e] = {}
	npcevent[e].event_duration = 10
	npcevent[e].idle_animation = ""
	npcevent[e].event_animation = ""
	npcevent[e].end_trigger =	2
	npcevent[e].fade_speed = 0.5
	npcevent[e].event_trigger = 1
	npcevent[e].range = 500	
	npcevent[e].instant_ragdoll = 0
	
	anim_idle[e] = 0
	anim_event[e] = 0
	eventduration[e] = math.huge
	status[e] = "init"	
end

function npc_event_main(e)	
	
	if status[e] =="init" then
		SetEntityBaseAlpha(e,100)
		SetEntityTransparency(e,1)
		fade_level[e] = GetEntityBaseAlpha(e)
		eventduration[e] = g_Time + (npcevent[e].event_duration*1000)
		if anim_idle[e] == 0 then	
			SetAnimationName(e,npcevent[e].idle_animation)
			LoopAnimation(e)
			anim_idle[e] = 1
		end				
		status[e] ="endinit"
	end		
	
	local PlayerDist = GetPlayerDistance(e)	
	if npcevent[e].event_trigger == 2 then
		if PlayerDist < npcevent[e].range then SetActivated(e,1) end
	end	
	
	if g_Entity[e]['activated'] == 1 then
		if 	npcevent[e].instant_ragdoll == 1 then
			PlaySound(e,0)
			StopAnimation(e)
			SetEntityHealth(e,0)
		end
		if anim_event[e] == 0 and npcevent[e].instant_ragdoll == 0 then
			SetAnimationName(e,npcevent[e].event_animation)
			PlayAnimation(e)
			PlaySound(e,0)
			eventduration[e] = g_Time + (npcevent[e].event_duration*1000)
			anim_event[e] = 1
		end				
	end
	if g_Time > eventduration[e] then		
		if fade_level[e] > 0 then
			SetEntityBaseAlpha(e,fade_level[e])
			fade_level[e] = fade_level[e]-npcevent[e].fade_speed
		end
		if fade_level[e] <= 0 then
			if npcevent[e].end_trigger == 1 then					
				ActivateIfUsed(e)
				PerformLogicConnections(e)
			end
			CollisionOff(e)
			Hide(e)	
			SetEntityBaseAlpha(e,100)
			Destroy(e)
			SwitchScript(e,"no_behavior_selected.lua")
		end
	end
end


