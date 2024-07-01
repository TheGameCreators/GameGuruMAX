-- Document v18 - thanks to Necrym59 and Lee
-- DESCRIPTION: Change the [PICKUP_TEXT$="E to look at document"].
-- DESCRIPTION: View position [SCREEN_X=50(0,100)] and [SCREEN_Y=50(0,100)]
-- DESCRIPTION: Set the [SPRITE_SIZE=15(1,100)] percentage.
-- DESCRIPTION: The document [IMAGEFILE$="imagebank\\documents\\default_doc.png"]
-- DESCRIPTION: Set the [PICKUP_RANGE=80(1,300)] 
-- DESCRIPTION: Change the [USAGE_TEXT$="MB to +-Zoom, MMW to scroll, Q to return document"]
-- DESCRIPTION: Set [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: Set to [LOCK_SCREEN!=0] when viewing.
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline)]
-- DESCRIPTION: <Sound0> when picking up document.
-- DESCRIPTION: <Sound1> reading/narration.
-- DESCRIPTION: <Sound2> when returning document.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local document 			= {}
local pickup_text 		= {}
local pickup_range 		= {}
local usage_text 		= {}	
local screen_x 			= {}
local screen_y 			= {}
local sprite_size 		= {}
local imagefile 		= {}
local prompt_display 	= {}
local lock_screen 		= {}

local image 			= {}
local imgAspectRatio 	= {}
local imgWidth 			= {}
local imgHeight 		= {}
local showing 			= {}
local pressed 			= {}
local played			= {}
local doonce			= {}
local sprite 			= {}
local adjusty			= {}
local startwidth		= {}
local tEnt 				= {}
local selectobj			= {}
local status 			= {}
		
function document_properties(e, pickup_text, screen_x, screen_y, sprite_size, imagefile, pickup_range, usage_text, prompt_display, lock_screen, item_highlight)
	document[e].pickup_text = pickup_text
	document[e].screen_x = screen_x					
	document[e].screen_y = screen_y					
	document[e].sprite_size = sprite_size			
	document[e].imagefile = imagefile
	if pickup_range == nil then pickup_range = 80 end
	document[e].pickup_range = pickup_range
	if usage_text == nil then usage_text = "" end
	document[e].usage_text = usage_text	
	if prompt_display == nil then prompt_display = 1 end
	document[e].prompt_display = prompt_display
	if lock_screen == nil then lock_screen = 0 end
	document[e].lock_screen = lock_screen
	if item_highlight == nil then item_highlight = 0 end
	document[e].item_highlight = item_highlight
end 
	
function document_init_name(e)
	document[e] = {}	
	document[e].pickup_text = "E to look at document"
	document[e].pickup_range = 80
	document[e].usage_text = "MB to +-Zoom, MMW to scroll, Q to return document"
	document[e].screen_x = 0	
	document[e].screen_y = 0
	document[e].sprite_size = 1
	document[e].imagefile = "imagebank\\documents\\default_doc.png"
	document[e].prompt_display = 1
	document[e].lock_screen = 0
	document[e].item_highlight = 0
	
	showing[e] = 0
	pressed[e] = 0
	sprite[e] = 0
	imgAspectRatio[e] = 0	
	imgWidth[e] = 0	
	imgHeight[e] = 0
	selectobj[e] = 0
	startwidth[e] = 0
	adjusty[e] = 0
	tEnt[e] = 0	
	g_tEnt = 0	
	played[e] = 0
	doonce[e] = 0
	status[e] ="init"
end

function document_main(e)
	
	local PlayerDist = GetPlayerDistance(e)
	
	if status[e] == "init" then	
		SetCameraOverride(0)
		sprite[e] = CreateSprite(LoadImage(document[e].imagefile))
		imgAspectRatio[e] = GetImageHeight(LoadImage(document[e].imagefile)) / GetImageWidth(LoadImage(document[e].imagefile))
		imgWidth[e] = document[e].sprite_size
		imgHeight[e] = imgWidth[e] * imgAspectRatio[e]
		SetSpriteSize(sprite[e],imgWidth[e],imgHeight[e])
		SetSpritePosition(sprite[e],500,500)
		SetSpriteDepth(sprite[e],100)
		SetSpritePriority(sprite[e],0)
		SetSpriteOffset(sprite[e],imgWidth[e]/2.0,imgHeight[e]/2.0)
		startwidth[e] = imgWidth[e]
		status[e] = "endinit"
	end	
	
	local LookingAt = GetPlrLookingAtEx(e,1)
	if PlayerDist < document[e].pickup_range and GetEntityVisibility(e) == 1 and LookingAt == 1 then
		--pinpoint select object--
		module_misclib.pinpoint(e,document[e].pickup_range,document[e].item_highlight)
		tEnt[e] = g_tEnt
		--end pinpoint select object--
	end	
	
	if PlayerDist < document[e].pickup_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then
		if showing[e] == 0 and tEnt[e] ~= 0 then
			if document[e].prompt_display ==1 then	PromptLocal(e,document[e].pickup_text) end
			if document[e].prompt_display ==2 then	Prompt(document[e].pickup_text) end
		end
		if g_KeyPressE == 1 then
			if showing[e] == 0 and pressed[e] == 0 then
				pressed[e] = 1
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
				Hide(e)
				showing[e] = 1
				if document[e].lock_screen == 1 then
					SetCameraOverride(3)
					FreezePlayer()
				end
				if played[e] == 1 then
					PlaySound(e,1)
					played[e] = 2
				end
				PerformLogicConnections(e)
			end
		end		
	end
	if showing[e] == 1 then
		if g_MouseClick == 1 then
			document[e].sprite_size = document[e].sprite_size + 0.1
			if document[e].sprite_size >= 50 then document[e].sprite_size = 50 end
		end
		if g_MouseClick == 2 then
			document[e].sprite_size = document[e].sprite_size - 0.1
			if document[e].sprite_size <= startwidth[e] then document[e].sprite_size = startwidth[e] end
		end
		if g_MouseWheel < 0 then	
			adjusty[e] = adjusty[e] + 1
		elseif g_MouseWheel > 0 then
			adjusty[e] = adjusty[e] - 1
		end
		imgWidth[e] = document[e].sprite_size
		imgHeight[e] = imgWidth[e] * imgAspectRatio[e]
		SetSpriteSize(sprite[e],imgWidth[e],imgHeight[e])
		SetSpriteOffset(sprite[e],imgWidth[e]/2.0,imgHeight[e]/2.0)
		SetSpritePosition(sprite[e],document[e].screen_x,document[e].screen_y + adjusty[e])
		PasteSpritePosition(sprite[e],document[e].screen_x,document[e].screen_y + adjusty[e])
		PromptLocal(e,"")
		Prompt("")
		TextCenterOnX(document[e].screen_x,(document[e].screen_y + imgHeight[e]/2+2) + adjusty[e],1,document[e].usage_text)		
	end

	local legacyhide = 0
	if document[e].usage_text == "" and PlayerDist > document[e].pickup_range then legacyhide = 1 end
	if g_KeyPressQ == 1 or legacyhide == 1 then
		pressed[e] = 0
		document[e].sprite_size = startwidth[e]
		adjusty[e] = 0
		if played[e] == 2 then
			PlaySound(e,2)
			played[e] = 0
		end
		if document[e].lock_screen == 1 then
			SetCameraOverride(0)
			UnFreezePlayer()
		end
		Show(e)
		SetSpritePosition(sprite[e],500,500)
		PasteSpritePosition(sprite[e],500,500)		
		showing[e] = 0
		tEnt[e] = 0
	end
end
