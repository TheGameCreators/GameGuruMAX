-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Named Sounder v4
-- DESCRIPTION: Will play or loop a sound when a Named object is activated.
-- DESCRIPTION: Attach to any object set Always ON. Single use only.
-- DESCRIPTION: Set the [OBJECT_NAME$=""] to find and any
-- DESCRIPTION: [ACTIVATION_TEXT$=""] to show at activation.
-- DESCRIPTION: Set the [@SOUND_STYLE=1(1=Play, 2=Loop)] to play or loop
-- DESCRIPTION: Set the [SOUND_VOLUME=100[1,100] adjust this sounds volume
-- DESCRIPTION: <Sound0>

local lower = string.lower

local nsobject 			= {}
local object_name		= {}
local activation_text 	= {}
local sound_style 		= {}
local sound_volume		= {}
local object_no			= {}

local status		= {}
local played		= {}
	
function named_sounder_properties(e, object_name, activation_text, sound_style, sound_volume)
	nsobject[e].object_name = lower(object_name)
	nsobject[e].activation_text = activation_text
	nsobject[e].sound_style = sound_style or 1
	nsobject[e].sound_volume = sound_volume
end
 
function named_sounder_init(e)
	nsobject[e] = {}
	nsobject[e].object_name = ""	
	nsobject[e].activation_text = ""
	nsobject[e].sound_style = 1
	nsobject[e].sound_volume = 100
	nsobject[e].object_no = 0

	status[e] = "init"
	played[e] = 0
end
 
function named_sounder_main(e)
	if status[e] == "init" then
		for n = 1, g_EntityElementMax do
			if n ~= nil and g_Entity[n] ~= nil then
				if lower(GetEntityName(n)) == nsobject[e].object_name then
					nsobject[e].object_no = n
					break
				end
			end
		end
		status[e] = "process"
	end

	if status[e] == "process" then		
		if g_Entity[nsobject[e].object_no]['activated'] == 0 then played[e] = 0 end	
		if g_Entity[nsobject[e].object_no]['activated'] == 1 then			
			if played[e] == 0 then
				PromptDuration(nsobject[e].activation_text,1000)
				if nsobject[e].sound_style == 1	then PlaySound(e,0) end
				if nsobject[e].sound_style == 2	then LoopSound(e,0) end
				SetSound(e,0)
				SetSoundVolume(nsobject[e].sound_volume)				
				PositionSound(e,0,g_Entity[nsobject[e].object_no]['x'],g_Entity[nsobject[e].object_no]['y'],g_Entity[nsobject[e].object_no]['z'])
				played[e] = 1
			end			
		
			if played[e] == 1 and nsobject[e].sound_style == 1 then
				SwitchScript(e,"no_behavior_selected.lua")
			end
			if played[e] == 1 and nsobject[e].sound_style == 2 then
				if g_Entity[nsobject[e].object_no]['health'] <= 0 then
					StopSound(e,0)
					SwitchScript(e,"no_behavior_selected.lua")
				end	
			end	
		end	
	end	
end 
 
function named_sounder_exit(e) 
end