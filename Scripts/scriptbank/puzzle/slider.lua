-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Slider Script v13
-- DESCRIPTION: Slides an object in 90 degree directions, ideal for crawler or grid style games.
-- DESCRIPTION: Can be activated by player or zone/switch
-- DESCRIPTION: Set Physics=ON, IsImobile=ON, AlwaysActive=ON.
-- DESCRIPTION: Set the [PROMPT$="Press E to Activate"],[RANGE=80],
-- DESCRIPTION: [@SLIDE_MODE=1(1=Manual,2=Switch/Zone)]
-- DESCRIPTION: [@SLIDE_TYPE=1(1=Open/Close, 2=Lock Open, 3=Continuous)],
-- DESCRIPTION: [@SLIDE_DIRECTION=1(1=Left, 2=Right, 3=Up, 4=Down, 5=Forward, 6=Backward)]
-- DESCRIPTION: [SLIDE_CLOSE_DELAY=1(1,100)], [SLIDE_AMOUNT=200], [SLIDE_SPEED=1(1,20)]
-- DESCRIPTION: [DAMAGE_AMOUNT=0(0,1000)] to receive if in range and slider speed is 5 or faster
-- DESCRIPTION: <Sound0> for start move sound.
-- DESCRIPTION: <Sound1> loop for moving sound.
-- DESCRIPTION: <Sound2> for end stop sound.

local slider = {}
local prompt = {}
local range = {}
local slide_mode = {}	
local slide_type = {}
local slide_close_delay = {}
local slide_amount = {}
local slide_speed  = {}
local slide_directon = {}
local damage_amount = {}
local state = {}
local moved = {}
local played = {}
local open_time = {}
local damage_time = {}
local doonce = {}

function slider_properties(e, prompt, range, slide_mode, slide_type, slide_direction, slide_close_delay, slide_amount, slide_speed, damage_amount)
	slider[e] = g_Entity[e]
	slider[e].prompt = prompt
	slider[e].range = range
	slider[e].slide_mode = slide_mode
	slider[e].slide_type = slide_type
	slider[e].slide_direction = slide_direction	
	slider[e].slide_close_delay = slide_close_delay
	slider[e].slide_amount = slide_amount
	slider[e].slide_speed = slide_speed/5
	slider[e].damage_amount = damage_amount
end

function slider_init(e)
	slider[e] = g_Entity[e]
	slider[e].prompt = "Press E to Activate"
	slider[e].range = 80
	slider[e].slide_mode = 1	
	slider[e].slide_type = 1
	slider[e].slide_direction = 1	
	slider[e].slide_close_delay = 1
	slider[e].slide_amount = 400
	slider[e].slide_speed = 3
	slider[e].damage_amount = 0
	state[e] = "opening"
	moved[e] = 0
	played[e] = 0
	doonce[e] = 0
	open_time[e] = 0
	damage_time[e] = math.huge	
end

function slider_main(e)
	slider[e] = g_Entity[e]

	if GetPlayerDistance(e) < slider[e].range and slider[e].slide_mode == 1 then
		Prompt(slider[e].prompt)
		if g_KeyPressE == 1 then
			state[e] = "opening"
			if played[e] == 0 then
				PlaySound(e,0)
				played[e] = 1
			end
			SetActivated(e,1)
			played[e] = 0
		end
	end
	
	if state[e] == "open" then 
		if slider[e].slide_type == 1 then
			if slider[e].slide_close_delay > 0 then
				if GetTimer(e) > open_time[e] then state[e] = "closing" end
			end
		end
		if slider[e].slide_type == 2 then
			state[e] = "locked"
			SetActivated(e,0)
		end
		if slider[e].slide_type == 3 then
			if slider[e].slide_close_delay > 0 then
				if GetTimer(e) > open_time[e] then state[e] = "closing" end
			end	
		end
	end
	if state[e] == "closed" and slider[e].slide_type == 3 then
		if slider[e].slide_close_delay > 0 then
			if GetTimer(e) > open_time[e] then state[e] = "opening" end
		end	
	end
		
	if g_Entity[e].activated == 1 then
		if doonce[e] == 0 then
			damage_time[e] = g_Time + 100
			doonce[e] = 1
		end
		if state[e] == "opening" then
			if played[e] == 0 then
				LoopSound(e,1)
				played[e] = 1
			end			
			if moved[e] < slider[e].slide_amount then
				GravityOff(e)
				CollisionOff(e)
				if slider[e].slide_direction == 1 then
					SetPosition(e,g_Entity[e]['x'] + slider[e].slide_speed,g_Entity[e]['y'],g_Entity[e]['z'])
					ResetPosition(e,g_Entity[e]['x'] + slider[e].slide_speed,g_Entity[e]['y'],g_Entity[e]['z'])
				elseif slider[e].slide_direction == 2 then
					SetPosition(e,g_Entity[e]['x'] - slider[e].slide_speed,g_Entity[e]['y'],g_Entity[e]['z'])
					ResetPosition(e,g_Entity[e]['x'] - slider[e].slide_speed,g_Entity[e]['y'],g_Entity[e]['z'])
				elseif slider[e].slide_direction == 3 then
					SetPosition(e,g_Entity[e]['x'],g_Entity[e]['y']+slider[e].slide_speed,g_Entity[e]['z'])
					ResetPosition(e,g_Entity[e]['x'],g_Entity[e]['y']+slider[e].slide_speed,g_Entity[e]['z'])
				elseif slider[e].slide_direction == 4 then
					SetPosition(e,g_Entity[e]['x'],g_Entity[e]['y']-slider[e].slide_speed,g_Entity[e]['z'])
					ResetPosition(e,g_Entity[e]['x'],g_Entity[e]['y']-slider[e].slide_speed,g_Entity[e]['z'])
				elseif slider[e].slide_direction == 5 then
					SetPosition(e,g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z']+slider[e].slide_speed)
					ResetPosition(e,g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z']+slider[e].slide_speed)
				elseif slider[e].slide_direction == 6 then
					SetPosition(e,g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z']-slider[e].slide_speed)
					ResetPosition(e,g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z']-slider[e].slide_speed)
				end
				CollisionOn(e)
				GravityOn(e)
				moved[e] = moved[e] + slider[e].slide_speed
				if moved[e] >= slider[e].slide_amount then moved[e] = slider[e].slide_amount end
			else
				state[e] = "open"
				StopSound(e,1)
				PlaySound(e,2)
				open_time[e] = GetTimer(e) + (slider[e].slide_close_delay * 1000)
			end
		end	

		if state[e] == "closing" then
			if played[e] == 1 then
				LoopSound(e,1)
				played[e] = 0
			end
			if moved[e] > 0 then
				GravityOff(e)
				CollisionOff(e)
				if slider[e].slide_direction == 1 then
					SetPosition(e,g_Entity[e]['x'] - slider[e].slide_speed,g_Entity[e]['y'],g_Entity[e]['z'])
					ResetPosition(e,g_Entity[e]['x'] - slider[e].slide_speed,g_Entity[e]['y'],g_Entity[e]['z'])
				elseif slider[e].slide_direction == 2 then
					SetPosition(e,g_Entity[e]['x'] + slider[e].slide_speed,g_Entity[e]['y'],g_Entity[e]['z'])
					ResetPosition(e,g_Entity[e]['x'] + slider[e].slide_speed,g_Entity[e]['y'],g_Entity[e]['z'])
				elseif slider[e].slide_direction == 3 then
					SetPosition(e,g_Entity[e]['x'],g_Entity[e]['y']-slider[e].slide_speed,g_Entity[e]['z'])
					ResetPosition(e,g_Entity[e]['x'],g_Entity[e]['y']-slider[e].slide_speed,g_Entity[e]['z'])
				elseif slider[e].slide_direction == 4 then
					SetPosition(e,g_Entity[e]['x'],g_Entity[e]['y']+slider[e].slide_speed,g_Entity[e]['z'])
					ResetPosition(e,g_Entity[e]['x'],g_Entity[e]['y']+slider[e].slide_speed,g_Entity[e]['z'])
				elseif slider[e].slide_direction == 5 then
					SetPosition(e,g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z']-slider[e].slide_speed)
					ResetPosition(e,g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z']-slider[e].slide_speed)
				elseif slider[e].slide_direction == 6 then
					SetPosition(e,g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z']+slider[e].slide_speed)
					ResetPosition(e,g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z']+slider[e].slide_speed)
				end
				CollisionOn(e)
				GravityOn(e)
				moved[e] = moved[e] - slider[e].slide_speed
				if moved[e] <= 0 then moved[e] = 0 end
			else
				state[e] = "closed"
				StopSound(e,1)
				PlaySound(e,2)
				open_time[e] = GetTimer(e) + (slider[e].slide_close_delay * 1000)
			end			
		end
		if GetPlayerDistance(e) < slider[e].range and slider[e].slide_speed >= 5 then
			if g_Time > damage_time[e] then HurtPlayer(-1,slider[e].damage_amount) end
			Prompt("")
			damage_time[e] = g_Time + 100
		end
	end
end