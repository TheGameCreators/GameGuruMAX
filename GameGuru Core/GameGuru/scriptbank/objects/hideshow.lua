-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- HideShow v9
-- DESCRIPTION: Will Hide or Show an object activated from a linked Trigger Zone or Switch
-- DESCRIPTION: Attach to the object(s) to be hidden/shown. Physics=ON, IsImobile=YES
-- DESCRIPTION: Change the [PROMPT_TEXT$="What was that"], Object [@MODE=1(1=Hide, 2=Show)], [DELAY=0(0,30)] in seconds.
-- DESCRIPTION: play <Sound0>

	local hsobject = {}
	local prompt_text = {}
	local mode = {}
	local delay = {}
	local status = {}
	local hs_time = {}
	local played = {}
	
function hideshow_properties(e, prompt_text, mode, delay)
	hsobject[e] = g_Entity[e]
	hsobject[e].prompt_text = prompt_text
	hsobject[e].mode = mode	
	hsobject[e].delay = delay
end
 
function hideshow_init(e)
	hsobject[e] = g_Entity[e]
	hsobject[e].prompt_text = ""
	hsobject[e].mode = 1
	hsobject[e].delay = 1
	status[e] = "prompt"
	hs_time[e] = 0
	played[e] = 0
	StartTimer(e)
end
 
function hideshow_main(e)
	hsobject[e] = g_Entity[e]
	if g_Entity[e]['activated'] == 0 then played[e] = 0 end
	if hsobject[e].mode == 1 then
		Show(e)
		CollisionOn(e)
	end
	if hsobject[e].mode == 2 then
		Hide(e)
		CollisionOff(e)
	end
		
	if g_Entity[e]['activated'] == 1 then
		if hs_time[e] == 0 then
			hs_time[e] = GetTimer(e) + (hsobject[e].delay * 1000)
		end
		
		if hsobject[e].mode == 1 then			
			if GetTimer(e) >= hs_time[e] then
				Hide(e)
				CollisionOff(e)
				if status[e] == "prompt" then
					PromptDuration(hsobject[e].prompt_text,2000)
					status[e] = "done"
				end
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
				g_Entity[e]['activated'] = 0
			end
		end		
		if hsobject[e].mode == 2 then					
			if GetTimer(e) >= hs_time[e] then
				Show(e)			
				CollisionOn(e)
				if status[e] == "prompt" then
					PromptDuration(hsobject[e].prompt_text,2000)
					status[e] = "done"
				end	
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end				
				g_Entity[e]['activated'] = 0
			end
		end			
	end	
end 
 
function hideshow_exit(e) 
end