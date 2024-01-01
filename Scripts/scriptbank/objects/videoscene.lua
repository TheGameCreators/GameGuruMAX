-- VideoScene v3 by Necrym59
-- DESCRIPTION: Triggers a video clip from a trigger zone or switch.
-- DESCRIPTION: Apply to an object and link to a trigger zone or switch.
-- DESCRIPTION: [@ALLOW_SKIP=1(1=Yes, 2=No)]
-- DESCRIPTION: <Video Slot>

local videoscene 	= {}
local allow_skip 	= {}
local play_mode 	= {}

function videoscene_properties(e, allow_skip)
	videoscene[e] = g_Entity[e]
	videoscene[e].allow_skip = allow_skip
	videoscene[e].play_mode = 0
end

function videoscene_init(e)
	videoscene[e] = {}
	videoscene[e].allow_skip = 2
	videoscene[e].play_mode = 0
end

function videoscene_main(e)

	if g_Entity[e]['activated'] == 1 then
		if videoscene[e].play_mode ~= nil then
			if videoscene[e].play_mode == 0 then
				videoscene[e].play_mode = 1
				HideHuds()
				if radar_hideallsprites ~= nil then radar_hideallsprites() end
			end
			if videoscene[e].play_mode == 1 then
				if videoscene[e].allow_skip == 1 then PromptVideo(e,1) end
				if videoscene[e].allow_skip == 2 then PromptVideoNoSkip(e,1) end
				videoscene[e].play_mode = 2
			end
			if videoscene[e].play_mode == 2 then
				if radar_showallsprites ~= nil then radar_showallsprites() end
				ShowHuds()
				PerformLogicConnections(e)
				videoscene[e].play_mode = 0
				SetActivated(e,0)
				--Destroy(e)	
			end
		end
	end
end
