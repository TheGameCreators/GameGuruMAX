-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Rockfall v4 - by Necrym59 
-- DESCRIPTION: A Rockfall activated from range or a trigger zone. Always Active=ON. Physics=ON, Gravity=ON, Collision=BOX
-- DESCRIPTION: [PROMPT_TEXT$="Rock Fall"], [ACTIVATION_RANGE=800(1,2000)], [EVENT_DURATION=6(1,10)] secs
-- DESCRIPTION: [HIT_DAMAGE=10(1,500)],[HIT_RADIUS=50(1,500)], [START_HEIGHT=10(0,1000)], [ROCK_SCALE#=1.5(1.0,3.0)]
-- DESCRIPTION: [@GROUND_SHAKE=1(1=Off, 2=On)], [@HIDE_ROCK=1(1=Off, 2=On)]
-- DESCRIPTION: <Sound0> Rockfall loop

	local P = require "scriptbank\\physlib"

	local rockfall 			= {}
	local prompt_text 		= {}
	local activation_range 	= {}
	local event_duration 	= {}
	local hit_damage		= {}
	local hit_radius 		= {}
	local start_height		= {}
	local rock_scale		= {}
	local ground_shake		= {}
	local hide_rock			= {}
	local status 			= {}
	local rock_hit 			= {}
	local height 			= {}
	local rockfall_time 	= {}
	local rocks				= {}
	local startx			= {}
	local starty			= {}
	local startz			= {}
	
function rockfall_properties(e, prompt_text, activation_range, event_duration, hit_damage, hit_radius, start_height, rock_scale, ground_shake, hide_rock)
	rockfall[e] = g_Entity[e]
	rockfall[e].prompt_text = prompt_text
	rockfall[e].activation_range = activation_range
	rockfall[e].event_duration = event_duration
	rockfall[e].hit_damage	= hit_damage
	rockfall[e].hit_radius = hit_radius
	rockfall[e].start_height = start_height
	rockfall[e].rock_scale = rock_scale
	rockfall[e].ground_shake = ground_shake
	rockfall[e].hide_rock = hide_rock
end

function rockfall_init(e)
	rockfall[e] = g_Entity[e]
	rockfall[e].prompt_text = "Rock Fall"	
	rockfall[e].activation_range = 800
	rockfall[e].event_duration = 6
	rockfall[e].hit_damage = 10
	rockfall[e].hit_radius = 50
	rockfall[e].start_height = 300
	rockfall[e].rock_scale = 1.5
	rockfall[e].ground_shake = 1
	rockfall[e].hide_rock = 1
	rockfall_time[e] = 0
	status[e] = "init"			
	rock_hit[e] = 0	
	GravityOff(e)
	CollisionOff(e)
	rocks[e] = 0
	g_Time = 0
end

function rockfall_main(e)
	rockfall[e] = g_Entity[e]

	if status[e] == "init" then
		if rockfall[e].hide_rock == 2 then Hide(e) end
		local xs, ys, zs = GetObjectScales(g_Entity[e]['obj'])
		if xs*100 < 100 and ys*100 < 100 and zs*100 < 100 then
			if rockfall[e].rock_scale > 1.0 then
				local rocksize = math.random(100,105)
				Scale(e,100*rockfall[e].rock_scale)			
			end			
		end		
		SetPosition(e,g_Entity[e]['x'],g_Entity[e]['y'] + rockfall[e].start_height, g_Entity[e]['z'])
		ResetPosition(e,g_Entity[e]['x'],g_Entity[e]['y'] + rockfall[e].start_height, g_Entity[e]['z'])
		status[e] = "start_event"
	end
	
	if GetPlayerDistance(e) < rockfall[e].activation_range then g_Entity[e]['activated'] = 1 end
	
	if g_Entity[e]['activated'] == 1 then	
		if status[e] == "start_event" then					
			PromptDuration(rockfall[e].prompt_text,3000)
			rockfall_time[e] = g_Time + (rockfall[e].event_duration*1000)
			StartTimer(e)			
			Show(e)		
			status[e] = "rockfall"
			GravityOn(e)
			CollisionOn(e)
		end			
		if status[e] == "rockfall" and g_PlayerHealth > 0 then			
			LoopSound(e,0)			
			if GetPlayerDistance(e) < rockfall[e].hit_radius and rock_hit[e] == 0 then
				HurtPlayer(-1,rockfall[e].hit_damage)
				ForcePlayer(0,3)
				rock_hit[e] = 1
			end
			if rockfall[e].ground_shake == 2 then
				if GamePlayerControlSetShakeTrauma ~= nil then
					if g_Time < rockfall_time[e] then
						GamePlayerControlSetShakeTrauma(13.9)
						GamePlayerControlSetShakePeriod(0.2)
					end
					if g_Time > rockfall_time[e] then
						GamePlayerControlSetShakeTrauma(0.0)
						GamePlayerControlSetShakePeriod(0.0)					
					end
				end
			end
		end		
	end		
end
