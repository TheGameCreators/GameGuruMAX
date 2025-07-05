-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Radio v4: by Necrym59
-- DESCRIPTION: Will play sound/music from the radio selected channel.
-- DESCRIPTION: [USE_TEXT$="E to Use, Q to turn off"]
-- DESCRIPTION: [USE_RANGE=90(1,200)]
-- DESCRIPTION: [ALREADY_ON!=0]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\hand.png"]
-- DESCRIPTION: <Sound0> channel 0
-- DESCRIPTION: <Sound1> channel 1
-- DESCRIPTION: <Sound2> channel 2
-- DESCRIPTION: <Sound3> tuning sound

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local radio 			= {}
local use_text 			= {}
local use_range 		= {}
local use_text 			= {}
local already_on 		= {}
local prompt_display 	= {}
local item_highlight 	= {}
local highlight_icon 	= {}

local using_radio		= {}
local radio_channel		= {}
local svolume 			= {}
local doonce			= {}
local shutoff			= {}
local wait				= {}
local tEnt 				= {}
local status			= {}
local hl_icon			= {}
local hl_imgwidth		= {}
local hl_imgheight		= {}
	
function radio_properties(e, use_text, use_range, already_on, prompt_display, item_highlight, highlight_icon_imagefile)
	radio[e].use_text = use_text
	radio[e].use_range = use_range
	radio[e].already_on = already_on
	radio[e].prompt_display = prompt_display
	radio[e].item_highlight = item_highlight
	radio[e].highlight_icon = highlight_icon_imagefile	
end 	
	
function radio_init(e)
	radio[e] = {}
	radio[e].use_text = "E to Use"
	radio[e].use_range = 90
	radio[e].already_on = 0
	radio[e].prompt_display = 1
	radio[e].item_highlight = 0
	radio[e].highlight_icon = "imagebank\\icons\\hand.png"
	
	status[e] = "init"
	tEnt[e] = 0
	g_tEnt = 0	
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0
	using_radio[e] = 0
	radio_channel[e] = 0
	doonce[e] = 0
	shutoff[e] = 0
	svolume[e] = 0
	wait[e] = math.huge	
	SetEntityAlwaysActive(e,1)
end
 
function radio_main(e)

	if status[e] == "init" then
		if radio[e].item_highlight == 3 and radio[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(radio[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(radio[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(radio[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "endinit"
	end

	local PlayerDist = GetPlayerDistance(e)
	svolume[e] = (1000-GetPlayerDistance(e))/10
	SetSoundVolume(svolume[e])
	if svolume[e] <= 0 then svolume[e] = 0 end
	
	if g_Entity[e]['health'] > 0 then
		if radio[e].already_on == 1 and shutoff[e] == 0 then
			radio_channel[e] = 0
			LoopSound(e,radio_channel[e])
			SetSoundVolume(svolume[e])
			radio[e].already_on = 0
		end
		if PlayerDist < radio[e].use_range then
			--pinpoint select object--
			module_misclib.pinpoint(e,radio[e].use_range,radio[e].item_highlight,hl_icon[e])
			tEnt[e] = g_tEnt
			--end pinpoint select object--			
			if PlayerDist < radio[e].use_range and tEnt[e] == e and doonce[e] == 0 and GetEntityVisibility(e) == 1 then	
				if radio[e].prompt_display == 1 then TextCenterOnX(50,54,1,radio[e].use_text) end
				if radio[e].prompt_display == 2 then Prompt(radio[e].use_text) end			
				if g_KeyPressE == 1 and shutoff[e] == 0 then
					if shutoff[e] == 0 then
						if radio_channel[e] >= 0 and radio_channel[e] < 3 then
							StopSound(e,radio_channel[e])
							PlaySound(e,3)
							radio_channel[e] = radio_channel[e] + 1
							LoopSound(e,radio_channel[e])
						end
						if radio_channel[e] == 3 and g_KeyPressE == 1 then
							StopSound(e,radio_channel[e])
							PlaySound(e,3)
							radio_channel[e] = 0
							LoopSound(e,radio_channel[e])
						end
						wait[e] = g_Time + 1000
						doonce[e] = 1
					end	
				end
				if g_KeyPressQ == 1 then
					StopSound(e,0)
					StopSound(e,1)
					StopSound(e,2)
					StopSound(e,3)
					shutoff[e] = 1
					wait[e] = g_Time + 1000				
				end
			end
		end
		if g_Time > wait[e] and shutoff[e] == 0 then		
			doonce[e] = 0
			shutoff[e] = 0
		end
		if g_Time > wait[e] and shutoff[e] == 1 then		
			doonce[e] = 0
			shutoff[e] = 0
		end
	end
	if g_Entity[e]['health'] <= 0 then
		StopSound(e,0)
		StopSound(e,1)
		StopSound(e,2)
		StopSound(e,3)		
	end	
end
