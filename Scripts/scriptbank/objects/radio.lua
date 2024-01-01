-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Radio v2: by Necrym59
-- DESCRIPTION: Will play sound/music from the radio selected channel.
-- DESCRIPTION: [PROMPT_TEXT$="E to Use"]
-- DESCRIPTION: [USE_RANGE=90(1,200)]
-- DESCRIPTION: [ALREADY_ON!=0]

-- DESCRIPTION: <Sound0> channel 0
-- DESCRIPTION: <Sound1> channel 1
-- DESCRIPTION: <Sound2> channel 2
-- DESCRIPTION: <Sound3> tuning sound

local radio 			= {}
local prompt_text 		= {}
local use_range 		= {}
local use_text 			= {}
local already_on 		= {}

local using_radio		= {}
local radio_channel		= {}
local svolume 			= {}
local doonce			= {}
local shutoff			= {}
local wait				= {}
	
function radio_properties(e, prompt_text, use_range, already_on)
	radio[e] = g_Entity[e]
	radio[e].prompt_text = prompt_text
	radio[e].use_range = use_range
	radio[e].already_on = already_on
end 	
	
function radio_init(e)
	radio[e] = {}
	radio[e].prompt_text = "E to Use"
	radio[e].use_range = 90
	radio[e].already_on = 0
	
	using_radio[e] = 0
	radio_channel[e] = 0
	doonce[e] = 0
	shutoff[e] = 0
	svolume[e] = 0
	wait[e] = math.huge	
end
 
function radio_main(e)

	local PlayerDist = GetPlayerDistance(e)
	svolume[e] = (3000-GetPlayerDistance(e))/30
	SetSoundVolume(svolume[e])
	
	if radio[e].already_on == 1 and shutoff[e] == 0 then
		radio_channel[e] = 0
		LoopSound(e,radio_channel[e])
		radio[e].already_on = 0
	end	
		
	if PlayerDist < radio[e].use_range and doonce[e] == 0 then
		local LookingAt = GetPlrLookingAtEx(e,1)	
		if LookingAt == 1 then		
			PromptLocal(e,radio[e].prompt_text)
			if g_KeyPressE == 1 and shutoff[e] == 0 then				
				PromptLocal(e,"Q to turn off")
				if shutoff[e] == 0 then
					if radio_channel[e] >= 0 and radio_channel[e] < 3 then
						StopSound(e,radio_channel[e])
						PlaySound(e,3)
						radio_channel[e] = radio_channel[e] + 1
						LoopSound(e,radio_channel[e])
					end
					if radio_channel[e] == 3 and g_KeyPressE == 1 then
						StopSound(e,radio_channel[e])
						PlaySound(e,3)
						radio_channel[e] = 0
						LoopSound(e,radio_channel[e])
					end
					wait[e] = g_Time + 1000
					doonce[e] = 1
				end	
			end
			if g_KeyPressQ == 1 then
				StopSound(e,0)
				StopSound(e,1)
				StopSound(e,2)
				StopSound(e,3)
				shutoff[e] = 1
				wait[e] = g_Time + 1000				
			end
		end
	end
	if g_Time > wait[e] and shutoff[e] == 0 then		
		doonce[e] = 0
		shutoff[e] = 0
	end
	if g_Time > wait[e] and shutoff[e] == 1 then		
		doonce[e] = 0
		shutoff[e] = 0
	end
end
