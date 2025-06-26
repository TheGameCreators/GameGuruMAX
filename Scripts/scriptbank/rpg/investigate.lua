-- Investigate v11
-- DESCRIPTION: Allows to receive information about an object.
-- DESCRIPTION: [PROMPT_TEXT$="E to investigate"]
-- DESCRIPTION: [USE_RANGE=80(1,100)]
-- DESCRIPTION: [@INVESTIGATION_STYLE=1(1=Text, 2=Text+Sound, 3=Image+Sound, 4=Text+Image+Sound)]
-- DESCRIPTION: [INVESTIGATION_TEXT$="Evidence"]
-- DESCRIPTION: [TEXT_POSITION_Y=90(1,100)]
-- DESCRIPTION: [EXIT_TEXT$="Q to exit"]
-- DESCRIPTION: [IMAGEFILE$="imagebank\\misc\\testimages\\investigate_01.png"]
-- DESCRIPTION: [IMAGE_SIZE=8(0,100)]
-- DESCRIPTION: [@FREEZE_SCREEN=2(1=Yes, 2=No)]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]
-- DESCRIPTION: <Sound0> investigate narration sound file

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local investigate 			= {}
local prompt_text			= {}
local use_range				= {}
local investigate_style		= {}
local investigate_text		= {}
local text_position_y		= {}
local exit_text				= {}
local investigation_image	= {}
local image_size			= {}
local freeze_screen			= {}
local prompt_display		= {}
local item_highlight		= {}


local investigatesp	= {}
local status		= {}
local looking		= {}
local imagesize		= {}
local played		= {}
local tEnt			= {}
local selectobj		= {}
local imgAspectRatio= {}
local imgWidth		= {}
local imgHeight		= {}
local hl_icon 		= {}
local hl_imgwidth	= {}
local hl_imgheight	= {}

function investigate_properties(e, prompt_text, use_range, investigation_style, investigation_text, text_position_y, exit_text, investigation_image, image_size, freeze_screen, prompt_display, item_highlight, highlight_icon_imagefile)
	investigate[e].prompt_text = prompt_text
	investigate[e].use_range = use_range
	investigate[e].investigation_style = investigation_style
	investigate[e].investigation_text = investigation_text
	investigate[e].text_position_y = text_position_y
	investigate[e].exit_text = exit_text
	investigate[e].investigation_image = investigation_image or imagefile
	investigate[e].image_size = image_size
	investigate[e].freeze_screen = freeze_screen	
	investigate[e].prompt_display = prompt_display
	investigate[e].item_highlight = item_highlight
	investigate[e].highlight_icon = highlight_icon_imagefile
end

function investigate_init(e)
	investigate[e] = {}
	investigate[e].prompt_text = "E to investigate"
	investigate[e].use_range = 80
	investigate[e].investigation_style = 1
	investigate[e].investigation_text = ""
	investigate[e].text_position_y = 90
	investigate[e].exit_text = exit_text
	investigate[e].investigation_image = ""
	investigate[e].image_size = 8
	investigate[e].freeze_screen = 2	
	investigate[e].prompt_display = 1
	investigate[e].item_highlight = 0
	investigate[e].highlight_icon = "imagebank\\icons\\pickup.png"

	played[e] = 0
	looking[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
	imgAspectRatio[e] = 0
	imgWidth[e] = 0
	imgHeight[e] = 0
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0
	status[e] = "init"
end

function investigate_main(e)

	if status[e] == "init" then
		if investigate[e].item_highlight == 3 and investigate[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(investigate[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(investigate[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(investigate[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		if investigate[e].investigation_style == 3 or investigate[e].investigation_style == 4 then
			if investigate[e].investigation_image ~= "" then
				imgAspectRatio[e] = GetImageHeight(investigate[e].investigation_image) / GetImageWidth(investigate[e].investigation_image)
				imgWidth[e] = investigate[e].image_size
				imgHeight[e] = imgWidth[e] * imgAspectRatio[e]
				investigatesp[e] = CreateSprite(LoadImage(investigate[e].investigation_image))
				SetSpriteSize(investigatesp[e],imgWidth[e],imgHeight[e])
				SetSpriteDepth(investigatesp[e],100)
				SetSpritePosition(investigatesp[e],500,500)
			end
		end
		status[e] = "search"
	end

	if status[e] == "search" then
		local PlayerDist = GetPlayerDistance(e)
		if PlayerDist < investigate[e].use_range then
			--pinpoint select object--
			module_misclib.pinpoint(e,investigate[e].use_range,investigate[e].item_highlight,hl_icon[e])
			tEnt[e] = g_tEnt
			--end pinpoint select object--
		end
		if PlayerDist < investigate[e].use_range and tEnt[e] == e and GetEntityVisibility(e) == 1 then
			if investigate[e].prompt_display == 1 then PromptLocal(e,investigate[e].prompt_text) end
			if investigate[e].prompt_display == 2 then Prompt(investigate[e].prompt_text) end
			if g_KeyPressE == 1 then
				status[e] = "investigate"
				looking[e] = 1
				if investigate[e].freeze_screen == 1 then SetCameraOverride(3) end
			end
		end
	end

	if status[e] == "investigate" then
		PromptLocal(e,"")
		if investigate[e].investigation_style == 1 then
			TextCenterOnX(50,investigate[e].text_position_y,3,investigate[e].investigation_text)
			if GetPlayerDistance(e) > investigate[e].use_range*3 and looking[e] == 1 then SetGamePlayerStatePlrKeyForceKeystate(16) end
			Prompt(investigate[e].exit_text)
		end
		if investigate[e].investigation_style == 2 then
			TextCenterOnX(50,investigate[e].text_position_y,3,investigate[e].investigation_text)
			if played[e] == 0 then
				PlaySound(e,0)
				played[e] = 1
			end
			if GetPlayerDistance(e) > investigate[e].use_range*3 and looking[e] == 1 then SetGamePlayerStatePlrKeyForceKeystate(16) end
			Prompt(investigate[e].exit_text)
		end
		if investigate[e].investigation_style == 3 then
			if investigate[e].investigation_image > "" then
				PasteSpritePosition(investigatesp[e],50-imgWidth[e]/2,50-imgHeight[e]/2)
			end
			if played[e] == 0 then
				PlaySound(e,0)
				played[e] = 1
			end
			if GetPlayerDistance(e) > investigate[e].use_range*3 and looking[e] == 1 then SetGamePlayerStatePlrKeyForceKeystate(16) end
			Prompt(investigate[e].exit_text)
		end
		if investigate[e].investigation_style == 4 then
			if investigate[e].investigation_image > "" then
				PasteSpritePosition(investigatesp[e],50-imgWidth[e]/2,50-imgHeight[e]/2)
			end
			TextCenterOnX(50,investigate[e].text_position_y,3,investigate[e].investigation_text)
			if played[e] == 0 then
				PlaySound(e,0)
				played[e] = 1
			end
			if GetPlayerDistance(e) > investigate[e].use_range*3 and looking[e] == 1 then SetGamePlayerStatePlrKeyForceKeystate(16) end
			Prompt(investigate[e].exit_text)
		end
		if g_KeyPressQ == 1 then
			if investigate[e].investigation_style == 3 or investigate[e].investigation_style == 4 and investigate[e].investigation_image > "" then SetSpritePosition(investigatesp[e],500,500) end
			SetGamePlayerStatePlrKeyForceKeystate(0)
			Prompt("")
			SetCameraOverride(0)
			status[e] = "search"
			played[e] = 0
			looking[e] = 0
		end
	end
end
