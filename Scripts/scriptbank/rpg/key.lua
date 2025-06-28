-- Key v26 - Necrym59 and Lee
-- DESCRIPTION: This object is treated as a key object for unlocking doors.
-- DESCRIPTION: [PICKUP_TEXT$="Collect Key"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Ranged, 2=Accurate)]
-- DESCRIPTION: [COLLECTED_TEXT$="Key collected"]
-- DESCRIPTION: [@LOGIC_TRIGGER=1(1=None, 2=On Pickup)]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]
-- DESCRIPTION: Play the audio <Sound0> when the object is picked up by the player.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"

g_tEnt = {}
local key = {}
local pickup_text = {}
local pickup_range = {}
local pickup_style = {}
local collected_text = {}
local logic_trigger = {}
local prompt_display = {}
local item_highlight = {}
local highlight_icon = {}

local selectobj = {}
local status = {}
local tEnt = {}
local hl_icon = {}
local hl_imgwidth = {}
local hl_imgheight = {}


function key_properties(e,pickup_text, pickup_range, pickup_style, collected_text, logic_trigger, prompt_display, item_highlight, highlight_icon_imagefile)
	key[e].pickup_text = pickup_text
	key[e].pickup_range = pickup_range
	key[e].pickup_style = pickup_style
	key[e].collected_text = collected_text
	key[e].logic_trigger = logic_trigger
	key[e].prompt_display = prompt_display
	key[e].item_highlight = item_highlight
	key[e].highlight_icon = highlight_icon_imagefile
end 

function key_init(e)
	key[e] = {}
	key[e].pickup_text = "Collect Key"
	key[e].pickup_range = 80
	key[e].pickup_style = 1
	key[e].collected_text = "Key collected"
	key[e].logic_trigger = 1
	key[e].prompt_display = 1
	key[e].item_highlight = 0
	key[e].highlight_icon = "imagebank\\icons\\pickup.png"
	key[e].associatekeyatstart = 1	

	status[e] = "init"
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0	
end

function key_main(e)
	if status[e] == "init" then
		if key[e].item_highlight == 3 and key[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(key[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(key[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(key[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "endinit"
	end
	
	local PlayerDist = GetPlayerDistance(e)	
	if key[e].associatekeyatstart == 1 then
		PerformLogicConnectionsAsKey(e)
		key[e].associatekeyatstart = 0
	end
	local performthecollection = 0
	if key[e].pickup_style == 1 then
		if PlayerDist < key[e].pickup_range then
			if GetEntityCollectable(e) == 1 then
				if GetEntityCollected(e) == 0 then
					performthecollection = 1
				end
			end
			if GetEntityCollectable(e) == 0 then
				performthecollection = 1				
			end
		end
	end
	if key[e].pickup_style == 2 and PlayerDist < key[e].pickup_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,key[e].pickup_range,key[e].item_highlight,hl_icon[e])
		tEnt[e] = g_tEnt
		--end pinpoint select object--
		
		if PlayerDist < key[e].pickup_range and tEnt[e] == e then	
			if GetEntityCollected(tEnt[e]) == 0 then
				if GetGamePlayerStateXBOX() == 1 then
					if key[e].prompt_display == 1 then PromptLocal(e,"Y button to " ..key[e].pickup_text) end
					if key[e].prompt_display == 2 then Prompt("Y button to " ..key[e].pickup_text) end
				else
					if GetHeadTracker() == 1 then
						PromptLocalForVR(e,"Trigger to " ..key[e].pickup_text,3)
					else
						if key[e].prompt_display == 1 then PromptLocal(e,"E to " ..key[e].pickup_text) end
						if key[e].prompt_display == 2 then Prompt("E to " ..key[e].pickup_text) end
					end
				end
				if g_KeyPressE == 1 then
					performthecollection = 1
				end
			end
		end
	end
	if performthecollection == 1 then
		if GetHeadTracker() == 1 then
			PromptLocalForVR(e,key[e].collected_text,3)
		else
			Prompt(key[e].collected_text)
		end
		PlaySound(e,0)
		SetEntityCollected(e,1)
		if key[e].logic_trigger == 2 then PerformLogicConnections(e) end
		if GetEntityCollectable(e) == 0 then
			Destroy(e)
		end
		performthecollection = 0
	end	
end