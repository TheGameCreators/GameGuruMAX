-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- HideShow v12
-- DESCRIPTION: Will Hide or Show an object activated from a linked Trigger Zone or Switch
-- DESCRIPTION: Attach to the object(s) to be hidden/shown. Physics=ON, IsImobile=YES
-- DESCRIPTION: Change the [PROMPT_TEXT$="What was that"], Object [@MODE=1(1=Hide, 2=Show, 3=Fade-Hide, 4=Fade-Show)],
-- DESCRIPTION: [DELAY=0(0,30)] in seconds, fade fade_speed [#FADE_SPEED=0.02(0.00,10.00)]
-- DESCRIPTION: play <Sound0>

	local hsobject 		= {}
	local prompt_text 	= {}
	local mode			= {}
	local delay			= {}
	local status		= {}
	local hs_time		= {}
	local fade_speed	= {}
	local current_level = {}
	local played		= {}
	local doonce		= {}
	
function hideshow_properties(e, prompt_text, mode, delay, fade_speed)
	hsobject[e] = g_Entity[e]
	hsobject[e].prompt_text = prompt_text
	hsobject[e].mode = mode	
	hsobject[e].delay = delay
	hsobject[e].fade_speed = fade_speed
end
 
function hideshow_init(e)
	hsobject[e] = {}
	hsobject[e].prompt_text = ""
	hsobject[e].mode = 1
	hsobject[e].delay = 1
	hsobject[e].fade_speed = 1
	status[e] = "init"
	hs_time[e] = 0
	played[e] = 0
	doonce[e] = 0
	current_level[e] = GetEntityBaseAlpha(e)	
	SetEntityTransparency(e,1)
	StartTimer(e)
end
 
function hideshow_main(e)
	hsobject[e] = g_Entity[e]
	if status[e] == "init" then
		if hsobject[e].mode == 3 then current_level[e] = 100 end
		if hsobject[e].mode == 4 then current_level[e] = 0 end		
		if hsobject[e].mode == 1 or hsobject[e].mode == 3 then
			Show(e)
			CollisionOn(e)
		end
		if hsobject[e].mode == 2 or hsobject[e].mode == 4 then
			Hide(e)
			CollisionOff(e)
		end
		status[e] = "end"
	end
	
	if g_Entity[e]['activated'] == 0 then played[e] = 0	end	
		
	if g_Entity[e]['activated'] == 1 then
		if hs_time[e] == 0 then
			hs_time[e] = GetTimer(e) + (hsobject[e].delay * 1000)
		end
		
		if hsobject[e].mode == 1 then	--Hide--	
			if GetTimer(e) >= hs_time[e] then
				GravityOff(e)
				CollisionOff(e)
				Hide(e)				
				if doonce[e] == 0 then
					PromptDuration(hsobject[e].prompt_text,2000)
					doonce[e] = 1
				end
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
				g_Entity[e]['activated'] = 0
			end
		end
		if hsobject[e].mode == 2 then	--Show-- 				
			if GetTimer(e) >= hs_time[e] then
				GravityOn(e)
				CollisionOn(e)
				Show(e)
				if doonce[e] == 0 then
					PromptDuration(hsobject[e].prompt_text,2000)
					doonce[e] = 1
				end	
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end				
				g_Entity[e]['activated'] = 0
			end
		end
		if hsobject[e].mode == 3 then	--Fade Hide--	
			if GetTimer(e) >= hs_time[e] then
				GravityOff(e)
				CollisionOff(e)			
				if current_level[e] > 0 then				
					SetEntityBaseAlpha(e,current_level[e])
					current_level[e] = current_level[e] - hsobject[e].fade_speed					
				end
				if current_level[e] <= 0 then
					current_level[e] = 0
					SetEntityBaseAlpha(e,current_level[e])					
					Hide(e)
				end
				if doonce[e] == 0 then
					PromptDuration(hsobject[e].prompt_text,2000)
					doonce[e] = 1
				end
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
				g_Entity[e]['activated'] = 0
			end
		end		
		if hsobject[e].mode == 4 then	--Fade Show--			
			if GetTimer(e) >= hs_time[e] then
				CollisionOn(e)
				GravityOn(e)			
				Show(e)
				if current_level[e] < 100 then
					SetEntityBaseAlpha(e,current_level[e])
					current_level[e] = current_level[e] + hsobject[e].fade_speed					
				end
				if current_level[e] >= 100 then
					current_level[e] = 100
					SetEntityBaseAlpha(e,current_level[e])
				end				
				if doonce[e] == 0 then
					PromptDuration(hsobject[e].prompt_text,2000)
					doonce[e] = 1
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