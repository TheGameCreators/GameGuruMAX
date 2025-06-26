-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Rad suit v17   by Necrym59
-- DESCRIPTION: The applied object will give the player radiation protection. Set Always active ON.
-- DESCRIPTION: [PICKUP_TEXT$="E to Pickup"]
-- DESCRIPTION: [PICKUP_RANGE=80(1-200)]
-- DESCRIPTION: [USEAGE_TEXT$="K to wear, Q to remove"]
-- DESCRIPTION: [IMAGEFILE$="imagebank\\misc\\testimages\\radsuit.png"] for screen overlay
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]
-- DESCRIPTION: <Sound0> Put-on Remove Sound
-- DESCRIPTION: <Sound1> Breathing Sound

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

g_toxiczone = {}
g_radsuit_on = {}

local radsuit 			= {}
local pickup_text 		= {}
local useage_text 		= {}
local screen_image 		= {}
local prompt_display	= {}
local item_highlight 	= {}
local highlight_icon	= {}

local status			= {}
local hl_icon			= {}
local hl_imgwidth		= {}
local hl_imgheight		= {}
local radmasksp			= {}
local rsswitch 			= {}
local have_radsuit 		= {}
local currenthealth 	= {}
local played			= {}
local selectobj 		= {}
local tEnt 				= {}

function radsuit_properties(e, pickup_text, pickup_range, useage_text, screen_image, prompt_display, item_highlight, highlight_icon_imagefile)
	radsuit[e].pickup_text = pickup_text
	radsuit[e].pickup_range = pickup_range
	radsuit[e].useage_text = useage_text
	radsuit[e].screen_image = imagefile or screen_image
	radsuit[e].prompt_display = prompt_display
	radsuit[e].item_highlight = item_highlight
	radsuit[e].highlight_icon = highlight_icon_imagefile	
end

function radsuit_init(e)
	radsuit[e] = {}
	radsuit[e].pickup_text = "E to Pickup"
	radsuit[e].pickup_range = 80
	radsuit[e].useage_text = "K to wear, Q to remove"
	radsuit[e].screen_image ="imagebank\\misc\\testimages\\radsuit.png"
	radsuit[e].prompt_display = 1
	radsuit[e].item_highlight = 0 
	radsuit[e].highlight_icon =	"imagebank\\icons\\pickup.png"
	
	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0	
	currenthealth[e] = 0
	have_radsuit = 0
	g_radsuit_on = 0
	played[e] = 0
	rsswitch[e] = 0
	selectobj[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
end

function radsuit_main(e)

	if status[e] == "init" then
		if radsuit[e].item_highlight == 3 and radsuit[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(radsuit[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(radsuit[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(radsuit[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end	
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
			module_misclib.pinpoint(e,radsuit[e].pickup_range,radsuit[e].item_highlight,hl_icon[e])
			tEnt[e] = g_tEnt
			--end pinpoint select object--
			if PlayerDist < radsuit[e].pickup_range and tEnt[e] == e then
				if radsuit[e].prompt_display == 1 then PromptLocal(e,radsuit[e].pickup_text) end
				if radsuit[e].prompt_display == 2 then Prompt(radsuit[e].pickup_text) end
				if g_KeyPressE == 1 then
					if GetEntityCollectable(tEnt[e]) == 0 then
						PromptDuration(radsuit[e].useage_text,3000)
						have_radsuit = 1
						PlaySound(e,0)
						CollisionOff(e)
						PerformLogicConnections(e)
						ActivateIfUsed(e)
						Hide(e)
					end
					if GetEntityCollectable(tEnt[e]) == 1 then
						PromptDuration(radsuit[e].useage_text,3000)
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

	if have_radsuit == 1 then
		SetPosition(e,g_PlayerPosX,g_PlayerPosY+500,g_PlayerPosZ)
		if rsswitch[e] == 0 then
			if GetInKey() == "k" or GetInKey() == "K" and rsswitch[e] == 0 then
				if played[e] == 0 then
					PromptDuration(radsuit[e].useage_text,3000)
					PlaySound(e,0)
					played[e] = 1
				end
				rsswitch[e] = 1
				g_radsuit_on = 1
			end
		end
		if rsswitch[e] == 1 then
			if GetInKey() == "q" or GetInKey() == "Q" and rsswitch[e] == 1 then
				if played[e] == 1 then
					PlaySound(e,0)
					played[e] = 0
				end
				rsswitch[e] = 0
				g_radsuit_on = 0
			end
		end
		if rsswitch[e] == 1 then
			PasteSpritePosition(radmasksp,0,0)
			LoopSound(e,1)
			if g_toxiczone == 'radiation' then SetPlayerHealth(currenthealth[e]) end
		end
		if rsswitch[e] == 0 then
			StopSound(e,1)
			PasteSpritePosition(radmasksp,1000,1000)
			currenthealth[e] = g_PlayerHealth
		end
		if g_PlayerHealth <=0 then have_radsuit = 0 end
	end
end
