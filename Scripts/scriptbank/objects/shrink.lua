-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Shrink v4 - by Necrym59
-- DESCRIPTION: Will make an object shrink from range or a trigger zone. Always Active=ON. Physics=ON, Gravity=OFF, IsImobile=ON
-- DESCRIPTION: [PROMPT_TEXT$="What the hell"]
-- DESCRIPTION: [ACTIVATION_RANGE=100(1,1000)]
-- DESCRIPTION: [@SHRINK_MODE=1(1=XYZ All, 2=X Only, 3=Y Only, 4=Z Only)]
-- DESCRIPTION: [SHRINK_SPEED#=0.5(0.0,10.0)]
-- DESCRIPTION: [SHRINK_SCALE#=1.0(1.0,100.0)]
-- DESCRIPTION: <Sound0> shrink loop


local shrink 			= {}
local prompt_text 		= {}
local activation_range 	= {}
local shrink_mode 		= {}
local shrink_speed		= {}
local shrink_scale 		= {}
local status 			= {}
local shrinksize 		= {}

function shrink_properties(e, prompt_text, activation_range, shrink_mode, shrink_speed, shrink_scale)
	shrink[e].prompt_text = prompt_text
	shrink[e].activation_range = activation_range
	shrink[e].shrink_mode = shrink_mode
	shrink[e].shrink_speed = shrink_speed
	shrink[e].shrink_scale = shrink_scale
end

function shrink_init(e)
	shrink[e] = {}
	shrink[e].prompt_text = "What the hell"
	shrink[e].activation_range = 0
	shrink[e].shrink_mode = 1
	shrink[e].shrink_speed = 0.5
	shrink[e].shrink_scale = 100
	shrinksize[e] = 0
	status[e] = "init"
end

function shrink_main(e)

	if status[e] == "init" then
		status[e] = "start"
	end
	if shrink[e].activation_range > 0 then
		if GetPlayerDistance(e) < shrink[e].activation_range then
			g_Entity[e]['activated'] = 1
		end
	end

	if g_Entity[e]['activated'] == 1 then
		if status[e] == "start" then
			PromptDuration(shrink[e].prompt_text,3000)
			Show(e)
			status[e] = "scaleing"
		end

		if status[e] == "scaleing" then
			LoopSound(e,0)
			shrink[e].activation_range = (shrink[e].activation_range + shrinksize[e])
			if shrink[e].shrink_mode == 1 then
				local xs, ys, zs = GetObjectScales(g_Entity[e]['obj'])
				if shrinksize[e] < shrink[e].shrink_scale then
					CollisionOff(e)
					ScaleObject(g_Entity[e]['obj'],(xs*100)-shrinksize[e]/100,(ys*100)-shrinksize[e]/100,(zs*100)-shrinksize[e]/100)
					shrinksize[e] = shrinksize[e] + shrink[e].shrink_speed/10
					CollisionOn(e)
				end
				if shrinksize[e] >= shrink[e].shrink_scale then
					shrinksize[e] = shrink[e].shrink_scale
					status[e] = "end"
				end
			end
			if shrink[e].shrink_mode == 2 then
				local xs, ys, zs = GetObjectScales(g_Entity[e]['obj'])
				local tscale = xs*100
				if shrinksize[e] < shrink[e].shrink_scale then
					CollisionOff(e)
					ScaleObject(g_Entity[e]['obj'],(xs*100)-shrinksize[e],ys*100,zs*100)
					shrinksize[e] = shrinksize[e] + shrink[e].shrink_speed/10
					CollisionOn(e)
				end
				if shrinksize[e] >= shrink[e].shrink_scale or shrinksize[e] >= tscale then
					shrinksize[e] = shrink[e].shrink_scale
					status[e] = "end"
				end
			end
			if shrink[e].shrink_mode == 3 then
				local xs, ys, zs = GetObjectScales(g_Entity[e]['obj'])
				local tscale = ys*100
				if shrinksize[e] < shrink[e].shrink_scale then
					CollisionOff(e)
					ScaleObject(g_Entity[e]['obj'],xs*100,(ys*100)-shrinksize[e],zs*100)
					shrinksize[e] = shrinksize[e] + shrink[e].shrink_speed/10
					CollisionOn(e)
				end
				if shrinksize[e] >= shrink[e].shrink_scale or shrinksize[e] >= tscale then
					shrinksize[e] = shrink[e].shrink_scale
					status[e] = "end"
				end
			end
			if shrink[e].shrink_mode == 4 then
				local xs, ys, zs = GetObjectScales(g_Entity[e]['obj'])
				local tscale = zs*100
				if shrinksize[e] < shrink[e].shrink_scale then
					CollisionOff(e)
					ScaleObject(g_Entity[e]['obj'],xs*100,ys*100,(zs*100)-shrinksize[e])
					shrinksize[e] = shrinksize[e] + shrink[e].shrink_speed/10
					CollisionOn(e)
				end
				if shrinksize[e] >= shrink[e].shrink_scale or shrinksize[e] >= tscale then
					shrinksize[e] = shrink[e].shrink_scale
					status[e] = "end"
				end
			end
		end
		if status[e] == "end" then
			StopSound(e,0)
		end
	end
end