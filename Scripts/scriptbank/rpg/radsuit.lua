-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Rad suit v10   by Necrym59
-- DESCRIPTION: The applied object will give the player radiation protection. Set Always active ON.
-- DESCRIPTION: [PICKUP_TEXT$="E to Pickup"]
-- DESCRIPTION: [PICKUP_RANGE=80(1-200)]
-- DESCRIPTION: [USEAGE_TEXT$="K to wear, Q to remove"]
-- DESCRIPTION: [IMAGEFILE$="imagebank\\misc\\testimages\\radsuit.png"] for screen overlay
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: <Sound0> Put-on Remove Sound
-- DESCRIPTION: <Sound1> Breathing Sound

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

g_radsuit = {}
g_toxiczone = {}
g_ppequipment = {}
local radsuit 			= {}
local pickup_text 		= {}
local useage_text 		= {}
local screen_image 		= {}
local prompt_display	= {}

local status 			= {}
local radmasksp			= {}
local rsswitch 			= {}
local have_radsuit 		= {}
local currenthealth 	= {}
local played			= {}
local selectobj 		= {}
local tEnt 				= {}

function radsuit_properties(e, pickup_text, pickup_range, useage_text, screen_image, prompt_display)
	radsuit[e] = g_Entity[e]
	radsuit[e].pickup_text = pickup_text
	radsuit[e].pickup_range = pickup_range
	radsuit[e].useage_text = useage_text
	radsuit[e].screen_image = imagefile or screen_image
	radsuit[e].prompt_display = prompt_display
end

function radsuit_init(e)
	radsuit[e] = g_Entity[e]
	radsuit[e].pickup_text = "E to Pickup"
	radsuit[e].pickup_range = 80
	radsuit[e].useage_text = "K to wear, Q to remove"
	radsuit[e].screen_image ="imagebank\\misc\\testimages\\radsuit.png"
	radsuit[e].prompt_display = 1

	status[e] = "init"
	currenthealth[e] = 0
	have_radsuit = 0
	g_ppequipment = 0	--Gasmask = 1, Radsuit = 2
	played[e] = 0
	rsswitch[e] = 0
	selectobj[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
end

function radsuit_main(e)
	radsuit[e] = g_Entity[e]
	if status[e] == "init" then
		radmasksp = CreateSprite(LoadImage(radsuit[e].screen_image))
		SetSpriteSize(radmasksp,100,100)
		SetSpritePosition(radmasksp,200,200)
		currenthealth[e] = g_PlayerHealth
		status = "endinit"
	end
	PlayerDist = GetPlayerDistance(e)

	if have_radsuit == 0 then
		if PlayerDist < radsuit[e].pickup_range and g_PlayerHealth > 0 then
			--pinpoint select object--
			module_misclib.pinpoint(e,radsuit[e].pickup_range,300)
			tEnt[e] = g_tEnt
			--end pinpoint select object--
			if PlayerDist < radsuit[e].pickup_range and tEnt[e] ~= 0 then
				if radsuit[e].prompt_display == 1 then PromptLocal(e,radsuit[e].pickup_text) end
				if radsuit[e].prompt_display == 2 then Prompt(radsuit[e].pickup_text) end
				if g_KeyPressE == 1 then
					if GetEntityCollectable(tEnt[e]) == 0 then
						if radsuit[e].prompt_display == 1 then PromptDuration(radsuit[e].useage_text,3000) end
						if radsuit[e].prompt_display == 2 then PromptDuration(radsuit[e].useage_text,3000) end
						have_radsuit = 1
						PlaySound(e,0)
						CollisionOff(e)
						PerformLogicConnections(e)
						ActivateIfUsed(e)
						Hide(e)
					end
					if GetEntityCollectable(tEnt[e]) == 1 then
						if radsuit[e].prompt_display == 1 then PromptDuration(radsuit[e].useage_text,3000) end
						if radsuit[e].prompt_display == 2 then PromptDuration(radsuit[e].useage_text,3000) end
						have_radsuit = 1
						PlaySound(e,0)
						CollisionOff(e)
						PerformLogicConnections(e)
						ActivateIfUsed(e)
						SetEntityCollected(tEnt[e],1)
					end
				end
			end
		end
	end

	if GetEntityCollected(tEnt[e]) == 0 then have_radsuit = 0 end
	if GetEntityCollected(tEnt[e]) == 1 then have_radsuit = 1 end

	if have_radsuit == 1 then
		if rsswitch[e] == 0 then
			if GetInKey() == "k" or GetInKey() == "K" and rsswitch[e] == 0 then
				if played[e] == 0 then
					if radsuit[e].prompt_display == 1 then PromptDuration(radsuit[e].useage_text,3000) end
					if radsuit[e].prompt_display == 2 then PromptDuration(radsuit[e].useage_text,3000) end
					PlaySound(e,0)
					played[e] = 1
				end
				rsswitch[e] = 1
			end
		end
		if rsswitch[e] == 1 then
			if GetInKey() == "q" or GetInKey() == "Q" and rsswitch[e] == 1 then
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
				rsswitch[e] = 0
			end
		end
		if rsswitch[e] == 1 then
			PasteSpritePosition(radmasksp,0,0)
			LoopSound(e,1)
			if g_toxiczone == 'radiation' then SetPlayerHealth(currenthealth[e]) end
			g_ppequipment = 2
			played[e] = 0
		end
		if rsswitch[e] == 0 then
			StopSound(e,1)
			PasteSpritePosition(radmasksp,1000,1000)
			g_ppequipment = 0
			played[e] = 0
			currenthealth[e] = g_PlayerHealth
		end
		if g_PlayerHealth <=0 then have_radsuit = 0 end
	end
end
