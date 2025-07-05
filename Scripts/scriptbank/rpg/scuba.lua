-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Scuba Version 15
-- DESCRIPTION: Attached object pick up gives extended diving capability. Auto activates when underwater. Set Always active ON
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [PROMPT_TEXT$="E to pickup"]
-- DESCRIPTION: [DIVE_TIME=1(1,15)] in minutes
-- DESCRIPTION: [@COMPASS_DISPLAY=1(1=On, 2=Off)] On or Off
-- DESCRIPTION: [@SCUBA_MASK=1(1=On, 2=Off)]
-- DESCRIPTION: [SCUBA_MASK_IMAGEFILE$="imagebank\\scuba\\mask.png"]
-- DESCRIPTION: [OXYGEN_BAR_IMAGEFILE$="imagebank\\scuba\\oxygen-bar.png"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]
-- DESCRIPTION: <Sound0> pickup sound
-- DESCRIPTION: <Sound1> scuba sound loop

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local scuba				= {}
local pickup_range		= {}
local prompt_text		= {}
local dive_time			= {}
local compass_display	= {}
local mask_image		= {}
local oxygen_image	= {}
local scuba_mask		= {}
local prompt_display 	= {}
local item_highlight 	= {}
local highlight_icon 	= {}

local dive_secs			= {}
local last_gun			= {}
local scubagear			= {}
local wearing			= {}
local oxLevel			= {}	
local oxbar				= {}	
local oxbarsprite		= {}
local status			= {}
local alreadyhaveair	= {}
local gunstatus 		= {}
local compass 			= {}
local selectobj 		= {}
local tEnt 				= {}
local status 			= {}
local hl_icon 			= {}
local hl_imgwidth 		= {}
local hl_imgheight		= {}
	
function scuba_properties(e, pickup_range, prompt_text, dive_time, compass_display, scuba_mask, scuba_mask_imagefile, oxygen_bar_imagefile, prompt_display, item_highlight, highlight_icon_imagefile)
	scuba[e].pickup_range = pickup_range
	scuba[e].prompt_text = prompt_text	
	scuba[e].dive_time = dive_time
	scuba[e].compass_display = compass_display	
	scuba[e].scuba_mask = scuba_mask
	scuba[e].mask_image = scuba_mask_imagefile
	scuba[e].oxygen_image = oxygen_bar_imagefile
	scuba[e].prompt_display = prompt_display
	scuba[e].item_highlight = item_highlight
	scuba[e].highlight_icon = highlight_icon_imagefile	
end
	
function scuba_init(e)
	scuba[e] = {}
	have_scubagear = 0
	scuba[e].pickup_range = 80
	scuba[e].prompt_text = "E to pickup"
	scuba[e].dive_time = 1	
	scuba[e].compass_display = 1
	scuba[e].scuba_mask = 1
	scuba[e].mask_image = "imagebank\\scuba\\mask.png"
	scuba[e].oxygen_image = "imagebank\\icons\\pickup.png"	
	scuba[e].prompt_display = 1
	scuba[e].item_highlight = 0	
	scuba[e].highlight_icon = "imagebank\\icons\\pickup.png"	
	scuba[e].dive_secs = (scuba[e].dive_time*1000)*60
	
	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0
	oxLevel[e] = 0
	alreadyhaveair[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0	
	status[e] = "init"	
	init_compass()	
end
 
function scuba_main(e)

	if status[e] == "init" then
		if scuba[e].item_highlight == 3 and scuba[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(scuba[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(scuba[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(scuba[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		if scuba[e].mask_image ~= "" then
			scubagear[e] = CreateSprite(LoadImage(scuba[e].mask_image))
			SetSpriteSize(scubagear[e],100,100)
			SetSpritePosition(scubagear[e],200,200)
		end
		if scuba[e].oxygen_image ~= "" then			
			oxbarsprite[e] = CreateSprite(LoadImage(scuba[e].oxygen_image))
			SetSpriteSize(oxbarsprite[e],100,100)
			SetSpritePosition(oxbarsprite[e],200,200)
		end	
		status[e] = "endinit"
	end
	
	PlayerDist = GetPlayerDistance(e)
	
	if have_scubagear == 0 then
		if PlayerDist < scuba[e].pickup_range and have_scubagear == 0 then
			--pinpoint select object--
			module_misclib.pinpoint(e,scuba[e].pickup_range,scuba[e].item_highlight,hl_icon[e])
			tEnt[e] = g_tEnt
			--end pinpoint select object--	
			if PlayerDist < scuba[e].pickup_range and tEnt[e] == e and have_scubagear == 0 then
				if scuba[e].prompt_display == 1 then PromptLocal(e,scuba[e].prompt_text) end
				if scuba[e].prompt_display == 2 then Prompt(scuba[e].prompt_text) end
				if g_KeyPressE == 1 then
					have_scubagear = 1
					PlaySound(e,0)				
					Hide(e)
					CollisionOff(e)
					ActivateIfUsed(e)
					scuba[e].dive_secs = (scuba[e].dive_time*1000)*60
					status[e] = "init_ox"
				end
			end	
		end
	end		
	
	if GetGamePlayerControlInWaterState()== 2 or GetGamePlayerControlInWaterState()== 3 and have_scubagear == 1 then
		if status[e] == "init_ox" then	
			alreadyhaveair[e] = GetGamePlayerControlDrownTimestamp()-Timer()
			SetGamePlayerControlDrownTimestamp(Timer()+scuba[e].dive_secs+alreadyhaveair[e])			
			status[e] = "end_ox"
		end
		if have_scubagear == 1 then			
			if scuba[e].scuba_mask == 1 then PasteSpritePosition(scubagear[e],0,0) end
			if scuba[e].compass_display == 1 then show_compass() end
			oxLevel[e] = GetGamePlayerControlDrownTimestamp()-Timer()
			if GetDesktopWidth() > 1024 then 
				SetSpriteSize(oxbarsprite[e],(oxLevel[e]/600)/50,1)			
				SetSpriteOffset(oxbarsprite[e],((oxLevel[e]/600)/50)/2,0)
			else
				SetSpriteSize(oxbarsprite[e],(oxLevel[e]/800)/50,1)			
				SetSpriteOffset(oxbarsprite[e],((oxLevel[e]/800)/50)/2,0)
			end
			SetSpriteDepth(oxbarsprite[e],0)			
			PasteSpritePosition(oxbarsprite[e],50,95)
			if GetGamePlayerControlInWaterState()== 2 and oxLevel[e] > 8000 then SetSpriteColor(oxbarsprite[e],0,225,255,150) end	
			if GetGamePlayerControlInWaterState()== 2 and oxLevel[e] < 8000 then SetSpriteColor(oxbarsprite[e],255,0,0,150) end				
			if scuba[e].scuba_mask == 1 then TextCenterOnXColor(50,94.5,2,"Oxygen Supply Level",0,225,255) end
			if scuba[e].dive_secs >= 1 then
				scuba[e].dive_secs = (scuba[e].dive_secs-12)				
			end
			LoopSound(e,1)
		end
	end
	if GetGamePlayerControlInWaterState()== 0 then
		status[e] = "init_ox"
		SetGamePlayerControlUnderwater(0)
		SetGamePlayerControlInWaterState(0)
		SetUnderwaterOff()
		StopSound(e,1)
		if have_scubagear == 1 and scuba[e].dive_secs <= 0 then
			PromptDuration("Scuba gear discarded",2000)
			have_scubagear = 0
			Destroy(e)
		end
	end	
end

function init_compass()    
	for i=1, 360, 1 do
		if i == 1 then compass[i] = "N" 
		elseif i == 46 then compass[i] = "NE"
		elseif i == 91 then compass[i] = "E"
		elseif i == 136 then compass[i] = "SE"
		elseif i == 181 then compass[i] = "S"
		elseif i == 226 then compass[i] = "SW"
		elseif i == 271 then compass[i] = "W"  
		elseif i == 316 then compass[i] = "NW"      
		elseif math.fmod(i,10) == 1 then
			compass[i] = "|" 
		else
			compass[i] = "."  
		end
	end    
end 

function show_compass()
	local start=0
	local compass_str=""
	local bearing=0
	bearing  = math.floor(math.fmod(g_PlayerAngY, 360))	
	if bearing < 0 then bearing=bearing+360 end	
 	if bearing < 45 then
		start = 315 + bearing
	else
		start=bearing-45
	end
	local arraypos=start    
	for i=1,91, 1 do
		if arraypos==360 or arraypos<=0 then
			arraypos=1 
		else  
			arraypos=arraypos+1
		end
		compass_str=compass_str..compass[arraypos]
	end	
	TextCenterOnXColor(50,1,2,"Navigation Compass",100,255,100)
	TextCenterOnXColor(50,3,2,compass_str,100,255,100)
	TextCenterOnXColor(50,4,2,"^",255,255,255)
	TextCenterOnXColor(50,5,2,bearing,100,255,100)
end
