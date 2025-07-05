-- Life v4 by Necrym59
-- DESCRIPTION: The object will give the player an extra life if collected
-- DESCRIPTION: [PROMPT_TEXT$="E to collect"]
-- DESCRIPTION: [PICKUP_RANGE=90(1,200)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Ranged, 2=Accurate)]
-- DESCRIPTION: [COLLECTION_TEXT$="Extra Life collected"]
-- DESCRIPTION: [!ACTIVATE_LOGIC=0] on pickup
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)] Use emmisive color for shape option
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]
-- DESCRIPTION: <Sound0> for collection

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local life 				= {}
local prompt_text 		= {}
local pickup_range 		= {}
local pickup_style 		= {}
local collection_text 	= {}
local activate_logic 	= {}
local prompt_display 	= {}
local item_highlight 	= {}
local highlight_icon	= {}

local status 			= {}
local hl_icon 			= {}
local hl_imgwidth		= {}
local hl_imgheight		= {}
local tEnt 				= {}
local selectobj 		= {}
local curlives			= {}
local doonce			= {}
local wait 				= {}

function life_properties(e, prompt_text, pickup_range, pickup_style, collection_text, activate_logic, prompt_display, item_highlight, highlight_icon_imagefile)
	life[e].prompt_text = prompt_text
	life[e].pickup_range = pickup_range
	life[e].pickup_style = pickup_style
	life[e].collection_text = collection_text
	life[e].activate_logic = 1
	life[e].prompt_display = prompt_display		
	life[e].item_highlight = item_highlight or 0
	life[e].highlight_icon = highlight_icon_imagefile	
end

function life_init(e)
	life[e] = {}
	life[e].prompt_text = "E to collect"
	life[e].pickup_range = 90
	life[e].pickup_style = 1
	life[e].collection_text = "Extra Life collected"
	life[e].activate_logic = 0
	life[e].prompt_display = 1
	life[e].item_highlight = 0
	life[e].highlight_icon = "imagebank\\icons\\pickup.png"
	
	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0	
	g_tEnt = 0
	tEnt[e] = 0
	selectobj[e] = 0
	curlives[e] = 0
	doonce[e] = 0
	wait[e] = math.huge	
end

function life_main(e)	
	
	if status[e] == "init"  then
		if life[e].item_highlight == 3 and life[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(life[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(life[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(life[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end	
		status[e] = "start"
	end
	if status[e] == "start"  then
		PlayerDist = GetPlayerDistance(e)
		if life[e].pickup_style == 1 then
			if PlayerDist < life[e].pickup_range and g_PlayerHealth > 0 and GetEntityVisibility(e) == 1 then
				status[e] = "pickup"
			end
		end
		if life[e].pickup_style == 2 then
			--pinpoint select object--
			module_misclib.pinpoint(e,life[e].pickup_range,life[e].item_highlight,hl_icon[e])
			tEnt[e] = g_tEnt
			--end pinpoint select object--a
			if PlayerDist < life[e].pickup_range and tEnt[e] == e then
				if life[e].prompt_display == 1 then PromptLocal(e,life[e].prompt_text) end
				if life[e].prompt_display == 2 then PromptDuration(life[e].prompt_text,1000) end	
				if g_KeyPressE == 1 then
					status[e] = "pickup"
				end
			end
		end
	end	
	if status[e] == "pickup" then
		if life[e].prompt_display == 1 then PromptLocal(e,life[e].collection_text) end
		if life[e].prompt_display == 2 then PromptDuration(life[e].collection_text,2000) end
		if doonce[e] == 0 then
			PlaySound(e,0)
			if life[e].activate_logic == 1 then
				PerformLogicConnections(e)
				ActivateIfUsed(e)
			end
			curlives[e] = g_PlayerLives + 1
			SetPlayerLives(curlives[e]) -- UPDATE LIVES			
			Hide(e)
			CollisionOff(e)
			doonce[e] = 1
			wait[e] = g_Time + 1000
			status[e] = "cleanup"
		end		
	end
	if status[e] == "cleanup" and g_Time > wait[e] then		
		Destroy(e)
	end
end
