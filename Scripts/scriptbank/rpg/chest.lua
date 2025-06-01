-- Chest v7
-- DESCRIPTION: When player is within [USE_RANGE=100], show
-- DESCRIPTION: [USE_PROMPT$="Press E to open"] when use key is pressed,
-- DESCRIPTION: will display [@@CHEST_HUD_SCREEN$=""(0=hudscreenlist)] Eg: HUD Screen 6],
-- DESCRIPTION: using [CHEST_CONTAINER$="chestunique"].
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline)]
-- DESCRIPTION: <Sound0> for opening chest.
-- DESCRIPTION: <Sound1> for closing chest.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local chest = {}
local use_range = {}
local use_prompt = {}
local chest_hud_screen = {}
local chest_container = {}
local prompt_display = {}
local item_highlight = {}

local status = {}
local doonce = {}
local tEnt = {}
local selectobj = {}

function chest_properties(e, use_range, use_prompt, chest_hud_screen, chest_container, prompt_display, item_highlight)
	chest[e].use_range = use_range
	chest[e].use_prompt = use_prompt
	chest[e].chest_hud_screen = chest_hud_screen
	chest[e].chest_container = chest_container
	chest[e].prompt_display = prompt_display or 1
	chest[e].item_highlight = item_highlight
end

function chest_init(e)
	chest[e] = {}
	chest[e].use_range = 100
	chest[e].use_prompt = "Press E to open"
	chest[e].chest_hud_screen = "HUD Screen 6"
	chest[e].chest_container = "chestunique"
	chest[e].prompt_display = 1
	chest[e].item_highlight = 0	
	
	status[e] = "closed"
	selectobj[e] = 0
	g_tEnt = 0
	tEnt[e] = 0
end

function chest_main(e)
	if GetCurrentScreen() == -1 and status[e] == "closed" then
		-- in the game
		PlayerDist = GetPlayerDistance(e)
		if PlayerDist < chest[e].use_range then
			--pinpoint select object--
			module_misclib.pinpoint(e,chest[e].use_range,chest[e].item_highlight)
			tEnt[e] = g_tEnt
			--end pinpoint select object--
		end
		if PlayerDist < chest[e].use_range and tEnt[e] ~= 0 then
			if chest[e].prompt_display == 1 then PromptLocal(e,chest[e].use_prompt) end
			if chest[e].prompt_display == 2 then Prompt(chest[e].use_prompt) end
			if g_KeyPressE == 1 then
				SetAnimationName(e,"open")
				PlayAnimation(e)
				PlaySound(e,0)
				if chest[e].chest_container == "chestunique" then
					g_UserGlobalContainer = "chestuniquetolevelase"..tostring(e)
				else
					g_UserGlobalContainer = chest[e].chest_container
				end
				ScreenToggle(chest[e].chest_hud_screen)
				status[e] = "opened"				
			end
		end
	else
		-- in chest screen
	end
	if GetCurrentScreen() == -1 and status[e] == "opened" then
		SetAnimationName(e,"close")
		PlayAnimation(e)
		PlaySound(e,1)
		status[e] = "closed"
	end		
end
