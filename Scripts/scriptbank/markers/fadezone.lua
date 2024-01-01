-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Fadezone v7 by Necrym59
-- DESCRIPTION: Fades out to black when player enters the zone. Place under a teleport or spawn point to fade in.
-- DESCRIPTION: Link to a trigger Zone.
-- DESCRIPTION: [PROMPT_TEXT$="In Fade Zone"]
-- DESCRIPTION: [@FADE_TYPE=1(1=Fade Out, 2=Fade In)]
-- DESCRIPTION: [FADE_TIME#=0.5]
-- DESCRIPTION: [IMAGEFILE$="imagebank\\misc\\testimages\\black.png"]
-- DESCRIPTION: [ZONEHEIGHT=100(0,1000)]
-- DESCRIPTION: [!FADE_OUT_TELEPORT=0]
-- DESCRIPTION: [@TELEPORT_DESTINATION=1(1=Local, 2=Level)]
-- DESCRIPTION: Select [@GoToLevelMode=1(1=Use Storyboard Logic,2=Go to Specific Level)] controls whether the next level in the Storyboard, or another level is loaded after object use.
-- DESCRIPTION: <Sound0> In zone effect sound

local fadezone 				= {}
local prompt_text 			= {}
local fade_type				= {}
local fade_time				= {}
local fade_image			= {}
local zoneheight			= {}
local fade_out_teleport		= {}
local teleport_destination	= {}

local fade_sprite 			= {}
local current_level 		= {}
local played				= {}
local status				= {}
local doonce 				= {}

function fadezone_properties(e, prompt_text, fade_type, fade_time, fade_image, zoneheight, fade_out_teleport, teleport_destination)
	fadezone[e] = g_Entity[e]
	fadezone[e].prompt_text = prompt_text
	fadezone[e].fade_type = fade_type
	fadezone[e].fade_time = fade_time
	fadezone[e].fade_image = fade_image or imagefile
	fadezone[e].zoneheight = zoneheight or 100
	fadezone[e].fade_out_teleport = fade_out_teleport
	fadezone[e].teleport_destination = teleport_destination
end

function fadezone_init(e)
	fadezone[e] = g_Entity[e]
	fadezone[e].prompt_text = "In Fade Zone"
	fadezone[e].fade_type = 1
	fadezone[e].fade_time = 1.0
	fadezone[e].zoneheight = 100
	fadezone[e].fade_out_teleport = 0
	fadezone[e].teleport_destination = 1
	current_level[e] = 0
	played[e] = 0
	doonce[e] = 0
	status[e] = "init"
end

function fadezone_main(e)
	fadezone[e] = g_Entity[e]

	if status[e] == "init" then
		fade_sprite = CreateSprite(LoadImage(fadezone[e].fade_image))
		SetSpriteSize(fade_sprite,100,100)
		SetSpritePosition(fade_sprite,0,0)
		SetSpriteColor(fade_sprite, 255, 255, 255, 0)
		if fadezone[e].fade_type == 1 then current_level[e] = 0	end
		if fadezone[e].fade_type == 2 then current_level[e] = 255 end
		status[e] = "endinit"
	end

	if g_Entity[e]['plrinzone'] == 1 and g_PlayerHealth > 0 and g_PlayerPosY < g_Entity[e]['y']+fadezone[e].zoneheight then
		if doonce[e] == 0 then
			Prompt(fadezone[e].prompt_text)
			doonce[e] = 1
		end
		if fadezone[e].fade_type == 1 then
			PasteSpritePosition(fade_sprite,0,0)
			if current_level[e] < 255 then
				SetCameraOverride(3)
				SetSpriteColor(fade_sprite, 255, 255, 255, current_level[e])
				current_level[e] = current_level[e] + fadezone[e].fade_time
			end
			if played[e] == 0 then
				PlaySound(e,0)
				played[e] = 1
			end
			if fadezone[e].fade_out_teleport == 1 then
				if fadezone[e].teleport_destination == 1 then
					if current_level[e] >= 255 then
						SetCameraOverride(0)
						PerformLogicConnections(e)
						TransportToIfUsed(e)
					end
				end
				if fadezone[e].teleport_destination == 2 then
					if current_level[e] >= 255 then
						SetCameraOverride(0)
						PerformLogicConnections(e)
						SetLevelFadeoutEnabled(0)
						JumpToLevelIfUsed(e)
					end
				end
			end
		end
		if fadezone[e].fade_type == 2 then
			PasteSpritePosition(fade_sprite,0,0)
			SetLevelFadeoutEnabled(1)
			if current_level[e] > 0 then
				SetCameraOverride(3)
				SetSpriteColor(fade_sprite, 255, 255, 255, current_level[e])
				current_level[e] = current_level[e] - fadezone[e].fade_time
			end
			if played[e] == 0 then
				PlaySound(e,0)
				played[e] = 1
			end
			if current_level[e] <= 0 then
				SetCameraOverride(0)
			end
		end

		if g_Entity[e]['plrinzone'] == 0 then
			StopSound(e,0)
			played[e] = 0
		end
	end
end

function fadezone_exit(e)
end

