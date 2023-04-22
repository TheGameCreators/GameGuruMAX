-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Kill Switch v6 by Necrym59
-- DESCRIPTION: Attach this behaviour to an object. Link to be activated by switch or zone.
-- DESCRIPTION: When all enemies have been killed it then activates another linked object or item.
-- DESCRIPTION: [EVENT_TEXT$="Kill all Targets in Area"]
-- DESCRIPTION: [ENEMY_RANGE=1000(100, 3000)]
-- DESCRIPTION: [@ENEMY_COUNTER=1(1=Off, 2=On)]
-- DESCRIPTION: [COUNTER_TEXT$="Targets in Area Remaining"]
-- DESCRIPTION: [END_TEXT$="All Targets in Area Eliminated"]
-- DESCRIPTION: [END_TEXT_DURATION=2(0,10)] Seconds
-- DESCRIPTION: <Sound0> will play at event start.
-- DESCRIPTION: <Sound1> will play at event end.


local killswitch 	= {}
local range 		= {}
local enemy_counter	= {}
local counter_text	= {}
local end_text 		= {}

local pEntno		= {}
local pEntalive 	= {}
local pEnthealth	= {}
local pEntrange		= {}
local doonce		= {}
local doend			= {}
local status	 	= {}

function kill_switch_properties(e, event_text, enemy_range, enemy_counter, counter_text, end_text, end_text_duration)
	 killswitch[e] = g_Entity[e]
	 killswitch[e].event_text = event_text
	 killswitch[e].enemy_range = enemy_range
	 killswitch[e].enemy_counter = enemy_counter
	 killswitch[e].counter_text = counter_text
	 killswitch[e].end_text = end_text
	 killswitch[e].end_text_duration = end_text_duration
end 

function kill_switch_init(e)
	killswitch[e] = g_Entity[e]
	killswitch[e].event_text = "Kill all Targets in Area"
	killswitch[e].enemy_range = 1000
	killswitch[e].enemy_counter = 1
	killswitch[e].counter_text = "Targets in Area Remaining"
	killswitch[e].end_text = "All Targets in Area Eliminated"
	killswitch[e].end_text_duration = 2
	
	pEntno[e] = 0
	pEntalive[e] = 0
	pEnthealth[e] = 0
	pEntrange[e] = 0
	doonce[e] = 0
	doend[e] = 0
	status[e] = 'init'
end

function kill_switch_main(e)
	killswitch[e] = g_Entity[e]

	if g_Entity[e]['activated'] == 1 then		
		
		if status[e] == 'init' then
			if doonce[e] == 0 then
				PromptDuration(killswitch[e].event_text,2000)	
				PlaySound(e,0)
				doonce[e] = 1
			end	
			for a = 1, g_EntityElementMax do
				if a ~= nil and g_Entity[a] ~= nil then 
					local allegiance = GetEntityAllegiance(a)				
					if allegiance == 0 then
						pEntrange[e] = math.ceil(GetFlatDistance(e,a))
						if g_Entity[a]['health'] > 1 and pEntrange[e] < killswitch[e].enemy_range then
							pEntno[e] = a
							pEntalive[e] = pEntalive[e] + 1
						end
					end
				end				
			end			
			status[e] = 'run'
		end
		if status[e] == 'run' then
			if killswitch[e].enemy_counter == 2 then TextCenterOnX(50,95,3,killswitch[e].counter_text.. "  " ..pEntalive[e]) end
			if pEntalive[e] == 0 then status[e] = 'endevent' end
			if pEntalive[e] > 0 then		
				pEntalive[e] = 0
				status[e] = 'init'
			end
		end
		if status[e] == 'endevent' then	
			if doend[e] == 0 then
				PromptDuration(killswitch[e].end_text,killswitch[e].end_text_duration*1000)
				PlaySound(e,1)
				ActivateIfUsed(e)			
				PerformLogicConnections(e)
				doend[e] = 1
				status[e] = 'eventended'
				Destroy(e)
			end
		end	
	end	
end

function GetFlatDistance(e,v)
	if g_Entity[e] ~= nil and g_Entity[v] ~= nil then
	  local distDX = g_Entity[e]['x'] - g_Entity[v]['x']
	  local distDZ = g_Entity[e]['z'] - g_Entity[v]['z']
	  return math.sqrt((distDX*distDX)+(distDZ*distDZ));
	end
end

