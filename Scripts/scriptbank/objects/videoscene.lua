-- VideoScene v4 by Necrym59
-- DESCRIPTION: Triggers a video clip from a trigger zone or switch.
-- DESCRIPTION: Apply to an object and link to a trigger zone or switch.
-- DESCRIPTION: [@ALLOW_SKIP=1(1=Yes, 2=No)]
-- DESCRIPTION: [@AMBIENT_TRACK=1(1=Stop, 2=Play)]
-- DESCRIPTION: [@TRIGGER_TYPE=1(1=Single Use, 2=Multiple Use)]
-- DESCRIPTION: <Video Slot>

local videoscene 			= {}
local allow_skip 			= {}
local ambient_track			= {}
local trigger_type		 	= {}

local status 				= {}

function videoscene_properties(e, allow_skip, ambient_track, trigger_type)
	videoscene[e] = g_Entity[e]
	videoscene[e].allow_skip = allow_skip
	videoscene[e].ambient_track = ambient_track
	videoscene[e].trigger_type = trigger_type
end

function videoscene_init(e)
	videoscene[e] = {}
	videoscene[e].allow_skip = 1
	videoscene[e].ambient_track = 1	
	videoscene[e].trigger_type = 1
	status[e] = "start"
end

function videoscene_main(e)

	if g_Entity[e]['activated'] == 1 then
	
		if status[e] == "start" then
			status[e] = "play"
			HideHuds()
			if radar_hideallsprites ~= nil then radar_hideallsprites() end
			if videoscene[e].ambient_track == 1 then StopAmbientMusicTrack() end
		end
		
		if status[e] == "play" then
			if videoscene[e].allow_skip == 1 then PromptVideo(e,1) end
			if videoscene[e].allow_skip == 2 then PromptVideoNoSkip(e,1) end
			status[e] = "end"
		end
		
		if status[e] == "end" then
			if radar_showallsprites ~= nil then radar_showallsprites() end
			ShowHuds()
			if videoscene[e].trigger_type == 1 then 
				status[e] = "ended"
				SetActivated(e,0)
				SwitchScript(e,"no_behavior_selected.lua")
			end
			if videoscene[e].trigger_type == 2 then 
				status[e] = "start"
				SetActivated(e,0)
			end			
		end
	end
end
