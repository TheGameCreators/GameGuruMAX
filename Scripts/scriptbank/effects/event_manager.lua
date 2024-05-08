-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Event Manager v3 - by Necrym59
-- DESCRIPTION: A global behavior that will activate timed interval delayed or random linked events plus an IfUsed link.
-- DESCRIPTION: Attach this behavior to an object. Link from this object to other entities for activations.
-- DESCRIPTION: IfUsed activation is once only after set delay.
-- DESCRIPTION: [@EVENT_MODE=1(1=Timed, 2=Random)]
-- DESCRIPTION: [EVENT0_LINK=0] Link Number 0 to 9
-- DESCRIPTION: [EVENT0_DELAY=0]
-- DESCRIPTION: [EVENT1_LINK=1] Link Number 0 to 9
-- DESCRIPTION: [EVENT1_DELAY=0]
-- DESCRIPTION: [EVENT2_LINK=2] Link Number 0 to 9
-- DESCRIPTION: [EVENT2_DELAY=0]
-- DESCRIPTION: [EVENT3_LINK=3] Link Number 0 to 9
-- DESCRIPTION: [EVENT3_DELAY=0]
-- DESCRIPTION: [EVENT4_LINK=4] Link Number 0 to 9
-- DESCRIPTION: [EVENT4_DELAY=0]
-- DESCRIPTION: [EVENT5_LINK=5] Link Number 0 to 9
-- DESCRIPTION: [EVENT5_DELAY=0]
-- DESCRIPTION: [EVENT6_LINK=6] Link Number 0 to 9
-- DESCRIPTION: [EVENT6_DELAY=0]
-- DESCRIPTION: [EVENT7_LINK=7] Link Number 0 to 9
-- DESCRIPTION: [EVENT7_DELAY=0]
-- DESCRIPTION: [EVENT8_LINK=8] Link Number 0 to 9
-- DESCRIPTION: [EVENT8_DELAY=0]
-- DESCRIPTION: [EVENT9_LINK=9] Link Number 0 to 9
-- DESCRIPTION: [EVENT9_DELAY=0]
-- DESCRIPTION: [REPEAT_EVENTS=1(1,10)] Number of repeats of the set of event sequence 
-- DESCRIPTION: [REPEAT_DELAY=0] Delay before event sequence repeats begin
-- DESCRIPTION: [RAMDOM_MODIFIER=1(1,100)] Modifies ramdom interval
-- DESCRIPTION: [@IFUSED_EVENT=2(1=On, 2=Off))] Activate an IfUsed link
-- DESCRIPTION: [IFUSED_DELAY=0] Delay before IfUsed is activated
-- DESCRIPTION: [!ACTIVATED=1] if unchecked use a switch/zone other trigger to activate this object.
-- DESCRIPTION: <Sound0> played for each event


local evm 				= {}
local event_mode		= {}
local event0_link 		= {}
local event0_delay		= {}
local event1_link 		= {}
local event1_delay		= {}
local event2_link 		= {}
local event2_delay		= {}
local event3_link 		= {}
local event3_delay		= {}
local event4_link 		= {}
local event4_delay		= {}
local event5_link 		= {}
local event5_delay		= {}
local event6_link 		= {}
local event6_delay		= {}
local event7_link 		= {}
local event7_delay		= {}
local event8_link 		= {}
local event8_delay		= {}
local event9_link 		= {}
local event9_delay		= {}
local repeat_events 	= {}
local repeat_delay		= {}
local random_modifier 	= {}
local ifused_event 		= {}
local ifused_delay		= {}

local delaytimer 		= {}
local ifusedtime		= {}
local eventcount		= {}
local ifuseddone		= {}
local status 			= {}
local doonce 			= {}

function event_manager_properties(e, event_mode, event0_link, event0_delay, event1_link, event1_delay, event2_link, event2_delay, event3_link, event3_delay, event4_link, event4_delay, event5_link, event5_delay, event6_link, event6_delay, event7_link, event7_delay, event8_link, event8_delay, event9_link, event9_delay, repeat_events, repeat_delay, random_modifier, ifused_event, ifused_delay, activated)
	evm[e].event_mode = event_mode
	evm[e].event0_link = event0_link
	evm[e].event0_delay = event0_delay	
	evm[e].event1_link = event1_link
	evm[e].event1_delay = event1_delay
	evm[e].event2_link = event2_link
	evm[e].event2_delay = event2_delay
	evm[e].event3_link = event3_link
	evm[e].event3_delay = event3_delay
	evm[e].event4_link = event4_link
	evm[e].event4_delay = event4_delay
	evm[e].event5_link = event5_link
	evm[e].event5_delay = event5_delay
	evm[e].event6_link = event6_link
	evm[e].event6_delay = event6_delay
	evm[e].event7_link = event7_link
	evm[e].event7_delay = event7_delay
	evm[e].event8_link = event8_link
	evm[e].event8_delay = event8_delay
	evm[e].event9_link = event9_link
	evm[e].event9_delay = event9_delay
	evm[e].repeat_events = repeat_events or 1
	evm[e].repeat_delay = repeat_delay
	evm[e].random_modifier = random_modifier
	evm[e].ifused_event = ifused_event or 2
	evm[e].ifused_delay = ifused_delay	
	evm[e].activated = activated or 1
end 

function event_manager_init(e)
	evm[e] = {}
	evm[e].event_mode = 1
	evm[e].event0_link = 0
	evm[e].event0_delay = 0	
	evm[e].event1_link = 0
	evm[e].event1_delay = 0
	evm[e].event2_link = 2
	evm[e].event2_delay = 0
	evm[e].event3_link = 3
	evm[e].event3_delay = 0
	evm[e].event4_link = 4
	evm[e].event4_delay = 0
	evm[e].event5_link = 5
	evm[e].event5_delay = 0
	evm[e].event6_link = 6
	evm[e].event6_delay = 0
	evm[e].event7_link = 7
	evm[e].event7_delay = 0
	evm[e].event8_link = 9
	evm[e].event8_delay = 0
	evm[e].event9_link = 9
	evm[e].event9_delay = 0
	evm[e].repeat_events = 1
	evm[e].repeat_delay = 0
	evm[e].random_modifier = 1
	evm[e].ifused_event = 2
	evm[e].ifused_delay = 0	
	evm[e].activated = 1	
	
	delaytimer[e] = math.huge
	ifusedtime[e] = math.huge
	eventcount[e] = 0
	ifuseddone[e] = 0
	doonce[e] = 0
	status[e] = "init"
end

function event_manager_main(e)
	
	if status[e] == "init" then
		if evm[e].activated == 1 then SetActivated(e,1) end
		if evm[e].activated == 0 then SetActivated(e,0) end		
		eventcount[e] = 0		
		status[e] = "endinit"
	end
	
	if g_Entity[e]['activated'] == 1 then
	
		if doonce[e] == 0 then
			delaytimer[e] = g_Time + (evm[e].event0_delay * 1000)
			ifusedtime[e] = g_Time + (evm[e].ifused_delay * 1000)
			doonce[e] = 1
		end	
	
		if evm[e].event_mode == 1 then -- Timed event mode
		
			if g_Time > delaytimer[e] and eventcount[e] == 0 then
				PlaySound(e,0)
				PerformLogicConnectionNumber(e,evm[e].event0_link)			
				eventcount[e] = eventcount[e] + 1
				delaytimer[e] = g_Time + evm[e].event1_delay * 1000				
			end		
			if g_Time > delaytimer[e] and eventcount[e] == 1 then
				PlaySound(e,0)
				PerformLogicConnectionNumber(e,evm[e].event1_link)			
				eventcount[e] = eventcount[e] + 1
				delaytimer[e] = g_Time + evm[e].event2_delay * 1000				
			end
			if g_Time > delaytimer[e] and eventcount[e] == 2 then
				PlaySound(e,0)
				PerformLogicConnectionNumber(e,evm[e].event2_link)
				eventcount[e] = eventcount[e] + 1
				delaytimer[e] = g_Time + evm[e].event3_delay * 1000
			end
			if g_Time > delaytimer[e] and eventcount[e] == 3 then
				PlaySound(e,0)
				PerformLogicConnectionNumber(e,evm[e].event3_link)
				eventcount[e] = eventcount[e] + 1
				delaytimer[e] = g_Time + evm[e].event4_delay * 1000
			end
			if g_Time > delaytimer[e] and eventcount[e] == 4 then
				PlaySound(e,0)
				PerformLogicConnectionNumber(e,evm[e].event4_link)
				eventcount[e] = eventcount[e] + 1
				delaytimer[e] = g_Time + evm[e].event5_delay * 1000
			end
			if g_Time > delaytimer[e] and eventcount[e] == 5 then
				PlaySound(e,0)
				PerformLogicConnectionNumber(e,evm[e].event5_link)
				eventcount[e] = eventcount[e] + 1
				delaytimer[e] = g_Time + evm[e].event6_delay * 1000
			end
			if g_Time > delaytimer[e] and eventcount[e] == 6 then
				PlaySound(e,0)
				PerformLogicConnectionNumber(e,evm[e].event6_link)
				eventcount[e] = eventcount[e] + 1
				delaytimer[e] = g_Time + evm[e].event7_delay * 1000
			end
			if g_Time > delaytimer[e] and eventcount[e] == 7 then
				PlaySound(e,0)
				PerformLogicConnectionNumber(e,evm[e].event7_link)
				eventcount[e] = eventcount[e] + 1
				delaytimer[e] = g_Time + evm[e].event8_delay * 1000
			end
			if g_Time > delaytimer[e] and eventcount[e] == 8 then
				PlaySound(e,0)
				PerformLogicConnectionNumber(e,evm[e].event8_link)
				eventcount[e] = eventcount[e] + 1
				delaytimer[e] = g_Time + evm[e].event9_delay * 1000
			end
			if g_Time > delaytimer[e] and eventcount[e] == 9 then
				PlaySound(e,0)
				PerformLogicConnectionNumber(e,evm[e].event9_link)
				delaytimer[e] = g_Time + evm[e].repeat_delay * 1000				
				eventcount[e] = eventcount[e] + 1
			end	
		end
		if evm[e].event_mode == 2 then -- Random event mode
			local maxrand = 0
			for i = 0, 9 do
				local elink = GetEntityRelationshipID(e,i)
				if elink > 0 then
					maxrand = maxrand + evm[e].random_modifier
				end	
			end
			PerformLogicConnectionNumber(e,math.random(0,maxrand))
			StartTimer(e)			
		end		
		if evm[e].repeat_events > 1 and eventcount[e] == 10 then
			if g_Time > delaytimer[e] then
				eventcount[e] = 0
				evm[e].repeat_events = evm[e].repeat_events - 1
			end	
		end		
		if evm[e].ifused_event == 1 and g_Time > ifusedtime[e] and ifuseddone[e] == 0 then
			ActivateIfUsed(e)
			ifuseddone[e] = 1
			ifusedtime[e] = math.huge
		end	
	end
end
