-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Grow v3 - by Necrym59 
-- DESCRIPTION: Will make an object grow from range or a trigger zone. Always Active=ON. Physics=ON, Gravity=OFF, IsImobile=ON
-- DESCRIPTION: [PROMPT_TEXT$="What the hell"], [ACTIVATION_RANGE=0(0,1000)], [@GROW_MODE=1(1=XYZ All, 2=X Only, 3=Y Only, 4=Z Only)],[GROW_SPEED#=0.10(0.0,1.0)],[GROW_SCALE#=5.0(0.0,100.0)]
-- DESCRIPTION: <Sound0> grow loop


	local grow 				= {}
	local prompt_text 		= {}
	local activation_range 	= {}	
	local grow_mode 		= {}
	local grow_speed		= {}
	local grow_scale 		= {}
	local status 			= {}
	local growsize 			= {}

function grow_properties(e, prompt_text, activation_range, grow_mode, grow_speed, grow_scale)
	grow[e] = g_Entity[e]
	grow[e].prompt_text = prompt_text
	grow[e].activation_range = activation_range
	grow[e].grow_mode = grow_mode
	grow[e].grow_speed = grow_speed
	grow[e].grow_scale = grow_scale
end

function grow_init(e)
	grow[e] = g_Entity[e]
	grow[e].prompt_text = "What the hell"	
	grow[e].activation_range = 0
	grow[e].grow_mode = 1
	grow[e].grow_speed = 0.5
	grow[e].grow_scale = 100
	growsize[e] = 0
	status[e] = "init"
end

function grow_main(e)
	grow[e] = g_Entity[e]
	
	if status[e] == "init" then		
		status[e] = "start"
	end
	if grow[e].activation_range > 0 then 
		if GetPlayerDistance(e) < grow[e].activation_range then
			g_Entity[e]['activated'] = 1			
		end
	end	
	
	if g_Entity[e]['activated'] == 1 then				
		if status[e] == "start" then
			PromptDuration(grow[e].prompt_text,3000)		
			Show(e)			
			status[e] = "scaleing"
		end
		
		if status[e] == "scaleing" then
			LoopSound(e,0)
			grow[e].activation_range = (grow[e].activation_range + growsize[e])
			if grow[e].grow_mode == 1 then				
				if growsize[e] < grow[e].grow_scale then
					local xs, ys, zs = GetObjectScales(g_Entity[e]['obj'])
					CollisionOff(e)					
					ScaleObject(g_Entity[e]['obj'],(xs*100)+growsize[e],(ys*100)+growsize[e],(zs*100)+growsize[e])
					growsize[e] = growsize[e] + grow[e].grow_speed
					CollisionOn(e)					
				end			
			end	
			if grow[e].grow_mode == 2 then
				local xs, ys, zs = GetObjectScales(g_Entity[e]['obj'])	
				if growsize[e] < grow[e].grow_scale then				
					CollisionOff(e)
					ScaleObject(g_Entity[e]['obj'],(xs*100)+growsize[e],ys*100,zs*100)
					growsize[e] = growsize[e] + grow[e].grow_speed
					CollisionOn(e)
				end
			end	
			if grow[e].grow_mode == 3 then
				local xs, ys, zs = GetObjectScales(g_Entity[e]['obj'])	
				if growsize[e] < grow[e].grow_scale then				
					CollisionOff(e)
					ScaleObject(g_Entity[e]['obj'],xs*100,(ys*100)+growsize[e],zs*100)
					growsize[e] = growsize[e] + grow[e].grow_speed
					CollisionOn(e)
				end
			end	
			if grow[e].grow_mode == 4 then
				local xs, ys, zs = GetObjectScales(g_Entity[e]['obj'])	
				if growsize[e] < grow[e].grow_scale then				
					CollisionOff(e)
					ScaleObject(g_Entity[e]['obj'],xs*100,ys*100,(zs*100)+growsize[e])
					growsize[e] = growsize[e] + grow[e].grow_speed
					CollisionOn(e)
				end				
			end
			if growsize[e] >= grow[e].grow_scale then status[e] = "end" end			
		end
		if status[e] == "end" then
			StopSound(e,0)
		end
	end
end