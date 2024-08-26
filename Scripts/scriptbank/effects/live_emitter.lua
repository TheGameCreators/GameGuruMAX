-- Live_Emitter v6 by Necrym59
-- DESCRIPTION: Attached object can monitor a Named Objects health and pulse its emmisive texture
-- DESCRIPTION: and run any inbuilt named animation in a response to being hurt.
-- DESCRIPTION: [OBJECT_NAME$=""]
-- DESCRIPTION: [PULSE_STRENGTH=1000(1,10000)]
-- DESCRIPTION: [PULSE_SPEED=100(1,5000)]
-- DESCRIPTION: [PULSE_DECAY=100(1,1000)]
-- DESCRIPTION: [IDLE_ANIMATION$=""] (case sensitive)
-- DESCRIPTION: [PULSE_ANIMATION$=""] (case sensitive)

local lower = string.lower
local lemitter				= {}
local object_name 			= {}
local pulse_strength		= {}
local pulse_speed			= {}
local pulse_decay			= {}
local idle_animation 		= {}
local pulse_animation 		= {}

local object_no				= {}
local start_health 			= {}
local emst					= {}
local emlevel 				= {}
local empulse 				= {}
local status				= {}
local emtimer				= {}
local antimer				= {}
local anionce				= {}

function live_emitter_properties(e, object_name, pulse_strength, pulse_speed, pulse_decay, idle_animation, pulse_animation)
	lemitter[e].object_name = lower(object_name)
	lemitter[e].pulse_strength = pulse_strength
	lemitter[e].pulse_speed = pulse_speed
	lemitter[e].pulse_decay = pulse_decay
	lemitter[e].idle_animation = idle_animation
	lemitter[e].pulse_animation = pulse_animation	
end

function live_emitter_init(e)
	lemitter[e] = {}
	lemitter[e].object_name = ""		
	lemitter[e].pulse_strength = 3000
	lemitter[e].pulse_speed = 50
	lemitter[e].pulse_decay = 100
	lemitter[e].idle_animation = ""
	lemitter[e].pulse_animation = ""
	
	lemitter[e].object_no = 0
	lemitter[e].start_health = 0
	
	emlevel[e] = 0
	empulse[e] = "up"	
	emtimer[e] = math.huge
	antimer[e] = math.huge
	anionce[e] = 0
	status[e] = "init"		
end
 
function live_emitter_main(e)

	if status[e] == "init" then

		if lemitter[e].object_no == 0 or lemitter[e].object_no == nil then
			for m = 1, g_EntityElementMax do
				if m ~= nil and g_Entity[m] ~= nil then
					if lower(GetEntityName(m)) == lower(lemitter[e].object_name) then
						lemitter[e].object_no = m
						lemitter[e].start_health = g_Entity[m]['health']
						emst[e] = GetEntityEmissiveStrength(m)
						SetEntityEmissiveStrength(m,emst[e])
						emlevel[e] = emst[e]
						break
					end
				end			
			end
		end
		if lemitter[e].idle_animation > "" then
			SetAnimationName(lemitter[e].object_no, lemitter[e].idle_animation)
			LoopAnimation(lemitter[e].object_no)
		end	
		status[e] = "live"
	end
	
	if status[e] == "live" then		
		if g_Entity[lemitter[e].object_no]['health'] < lemitter[e].start_health then
			if emlevel[e] < lemitter[e].pulse_strength and empulse[e] == "up" then
				SetEntityEmissiveStrength(lemitter[e].object_no,emlevel[e])
				emlevel[e] = emlevel[e] + lemitter[e].pulse_speed
				if emlevel[e] >= lemitter[e].pulse_strength then
					empulse[e] = "down"
					emtimer[e] = g_Time + lemitter[e].pulse_decay
					if lemitter[e].pulse_animation > "" then
						SetAnimationName(lemitter[e].object_no, lemitter[e].pulse_animation)
						PlayAnimation(lemitter[e].object_no)
						anionce[e] = 1
						antimer[e] = g_Time + lemitter[e].pulse_speed
					end	
				end
			end			
		end
		
		if g_Time > emtimer[e] then
			if emlevel[e] > emst[e] and empulse[e] == "down" then
				SetEntityEmissiveStrength(lemitter[e].object_no,emlevel[e])
				emlevel[e] = emlevel[e] - lemitter[e].pulse_speed
				if emlevel[e] <= emst[e] then					
					SetEntityEmissiveStrength(lemitter[e].object_no,emst[e])
					empulse[e] = "up"
					lemitter[e].start_health = g_Entity[lemitter[e].object_no]['health']
					SetEntityHealth(lemitter[e].object_no,lemitter[e].start_health)
				end
			end
		end
		if g_Time > antimer[e] and anionce[e] == 1 then
			if lemitter[e].idle_animation > "" then
				SetAnimationName(lemitter[e].object_no, lemitter[e].idle_animation)
				LoopAnimation(lemitter[e].object_no)
				anionce[e] = 0
			end
		end
		if g_Entity[lemitter[e].object_no]['health'] <= 0 then
			StopAnimation(lemitter[e].object_no)
			emst[e] = 0
		end	
	end
end

function live_emitter_exit(e)
end