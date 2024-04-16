-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Timed Event v5 - by Necrym59
-- DESCRIPTION: Will activate timed delayed linked events.
-- DESCRIPTION: Attach this behavior to an object. Link a zone or switch to activate this object.
-- DESCRIPTION: Link from this object to other entities for delayed single or multiple activation.
-- DESCRIPTION: Edit the [EVENT_TEXT$="Event Underway"]
-- DESCRIPTION: Number of [EVENT_COUNT=1(1,10)]
-- DESCRIPTION: [EVENT_DELAY=5(1,100)] in seconds
-- DESCRIPTION: [@VISIBILITY=2(1=Hide, 2=Show)]
-- DESCRIPTION: <Sound0> plays for each event


local timedevent 		= {}
local event_text 		= {}
local event_count		= {}
local event_delay		= {}
local wait 				= {}
local delaytime 		= {}
local eventcount		= {}
local status 			= {}
local doonece 			= {}

function timed_event_properties(e, event_text, event_count, event_delay, visibility)
	timedevent[e] = g_Entity[e]
	timedevent[e].event_text = event_text
	timedevent[e].event_count = event_count
	timedevent[e].event_delay = event_delay
	timedevent[e].visibility = visibility
end

function timed_event_init(e)
	timedevent[e] = g_Entity[e]
	timedevent[e].event_text = ""
	timedevent[e].event_count = 1
	timedevent[e].event_delay = 5
	timedevent[e].visibility = 2
	wait[e] = math.huge
	delaytime[e] = 0
	eventcount[e] = 0
	doonece[e] = 0
	status[e] = "init"
end

function timed_event_main(e)
	timedevent[e] = g_Entity[e]

	if status[e] == "init" then
		eventcount[e] = timedevent[e].event_count
		delaytime[e] = timedevent[e].event_delay * 1000
		status[e] = "endinit"
	end

	if g_Entity[e]['activated'] == 1 then
		if timedevent[e].visibility == 1 then
			Hide(e)
			CollisionOff(e)
		end
		if doonece[e] == 0 then
			wait[e] = g_Time + delaytime[e]
			doonece[e] = 1
			status[e] = 'process'
		end
	end
	if status[e] == 'process' then
		if g_Time > wait[e] then
			if eventcount[e] > 0 then
				PromptDuration(timedevent[e].event_text,1000)
				PlaySound(e,0)
				SetActivatedWithMP(e,201)
				PerformLogicConnections(e)
				eventcount[e] = eventcount[e]-1
				if eventcount[e] == 0 then
					status[e] = "finished"
				end
				wait[e] = g_Time + delaytime[e]
			end
		end
	end
	if status[e] == "finished" then
		SetActivated(e,0)
	end
end