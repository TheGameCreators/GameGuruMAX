-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Win Kill v6 by Necrym59
-- DESCRIPTION: When all enemies have been killed, level is won and changes to next or selected level.
-- DESCRIPTION: [@ENEMY_COUNTER=1(1=Off, 2=On)]
-- DESCRIPTION: [COUNTER_TEXT$="Targets Remaining"]
-- DESCRIPTION: [END_TEXT$="All Targets Eliminated"]
-- DESCRIPTION: [END_DELAY=1(1,60)] seconds
-- DESCRIPTION: [@GoToLevelMode=1(1=Use Storyboard Logic,2=Go to Specific Level)] controls whether the next level in the Storyboard, or another level is loaded.
-- DESCRIPTION: [ResetStates!=0] when entering the new level
-- DESCRIPTION: <Sound0> will play and the level is complete.

local winkill 		= {}
local enemy_counter	= {}
local counter_text	= {}
local end_text 		= {}
local end_delay 	= {}
local resetstates	= {}

local pEntno		= {}
local pEntalive 	= {}
local pEnthealth	= {}
local status	 	= {}
local wait			= {}

function winkill_properties(e, enemy_counter, counter_text, end_text, end_delay, resetstates)
	winkill[e].enemy_counter = enemy_counter
	winkill[e].counter_text = counter_text
	winkill[e].end_text = end_text
	winkill[e].resetstates = resetstates
	winkill[e].end_delay = end_delay or 1
end 

function winkill_init(e)
	winkill[e] = {}
	winkill[e].enemy_counter = 1
	winkill[e].counter_text = "Targets Remaining"
	winkill[e].end_text = "All Targets Eliminated"
	winkill[e].resetstates = 0
	winkill[e].end_delay = 1
	
	pEntno = 0
	pEntalive[e] = 0
	pEnthealth[e] = 0
	status[e] = 'init'
	wait[e] = math.huge
end

function winkill_main(e)
	if status[e] == 'init' then
		for a = 1, g_EntityElementMax do
			if a ~= nil and g_Entity[a] ~= nil then 
				local allegiance = GetEntityAllegiance(a)				
				if allegiance == 0 then
					if g_Entity[a]['health'] > 1 then
						pEntno = a
						pEntalive[e] = pEntalive[e] + 1
					end
				end
			end				
		end		
		status[e] = 'run'
	end
	
	if status[e] == 'run' then
		if winkill[e].enemy_counter == 2 then TextCenterOnX(50,95,3,winkill[e].counter_text.. "  " ..pEntalive[e]) end
		if pEntalive[e] == 0 then
			TextCenterOnX(50,50,5,winkill[e].end_text)
			PlaySound(e,0)
			PerformLogicConnections(e)
			wait[e] = g_Time + (winkill[e].end_delay * 1000)
			status[e] = 'endlevel'
		end
		if pEntalive[e] > 0 then		
			pEntalive[e] = 0
			status[e] = 'init'
		end
	end
	
	if g_Time >= wait[e] and status[e] == 'endlevel' then				
		JumpToLevelIfUsedEx(e,winkill[e].resetstates)
	end
end

