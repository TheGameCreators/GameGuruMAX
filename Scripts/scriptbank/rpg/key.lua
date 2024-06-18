-- Key v25 - Necrym59 and Lee
-- DESCRIPTION: This object is treated as a key object for unlocking doors.
-- DESCRIPTION: [PICKUP_TEXT$="Collect Key"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Automatic, 2=Manual)]
-- DESCRIPTION: [COLLECTED_TEXT$="Key collected"]
-- DESCRIPTION: [@LOGIC_TRIGGER=1(1=None, 2=On Pickup)]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline)]
-- DESCRIPTION: Play the audio <Sound0> when the object is picked up by the player.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"

g_tEnt = {}
local key = {}
local selectobj = {}
local tEnt = {}

function key_properties(e,pickup_text, pickup_range, pickup_style, collected_text, logic_trigger, prompt_display, item_highlight)
	key[e].pickup_text = pickup_text
	key[e].pickup_range = pickup_range
	key[e].pickup_style = pickup_style
	key[e].collected_text = collected_text
	key[e].logic_trigger = logic_trigger
	key[e].prompt_display = prompt_display
	key[e].item_highlight = item_highlight	
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
	key[e].associatekeyatstart = 1	

	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0	
end

function key_main(e)
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
		module_misclib.pinpoint(e,key[e].pickup_range,key[e].item_highlight)
		tEnt[e] = g_tEnt
		--end pinpoint select object--
		
		if PlayerDist < key[e].pickup_range and tEnt[e] ~= 0 then	
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
