-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Scuba Version 14
-- DESCRIPTION: Attached object pick up gives extended diving capability. Auto activates when underwater. Set Always active ON
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [PROMPT_TEXT$="E to pickup"]
-- DESCRIPTION: [DIVE_TIME=1(1,15)] in minutes
-- DESCRIPTION: [@COMPASS_DISPLAY=1(1=On, 2=Off)] On or Off
-- DESCRIPTION: [IMAGEFILE$="imagebank\\scuba\\mask.png"]
-- DESCRIPTION: [@SCUBA_MASK=1(1=On, 2=Off)]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline)]

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local scuba				= {}
local pickup_range		= {}
local prompt_text		= {}
local dive_time			= {}
local compass_display	= {}
local mask_image		= {}
local scuba_mask		= {}
local prompt_display 	= {}
local item_highlight 	= {}

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
	
function scuba_properties(e, pickup_range, prompt_text, dive_time, compass_display, mask_image, scuba_mask, prompt_display, item_highlight)
	scuba[e].pickup_range = pickup_range
	scuba[e].prompt_text = prompt_text	
	scuba[e].dive_time = dive_time
	scuba[e].compass_display = compass_display	
	scuba[e].mask_image = imagefile or mask_image
	scuba[e].scuba_mask = scuba_mask
	scuba[e].prompt_display = prompt_display
	scuba[e].item_highlight = item_highlight
end
	
function scuba_init(e)
	scuba[e] = {}
	have_scubagear = 0
	scuba[e].pickup_range = 80
	scuba[e].prompt_text = "E to pickup"
	scuba[e].dive_time = 1	
	scuba[e].compass_display = 1
	scuba[e].mask_image = "imagebank\\scuba\\mask.png"
	scuba[e].scuba_mask = 1
	scuba[e].prompt_display = 1
	scuba[e].item_highlight = 0	
	scuba[e].dive_secs = (scuba[e].dive_time*1000)*60
	
	scubagear[e] = CreateSprite(LoadImage(scuba[e].mask_image))
	SetSpriteSize(scubagear[e],100,100)
	SetSpritePosition(scubagear[e],200,200)
	oxbar = LoadImage("imagebank\\scuba\\oxygen-bar.png")
	oxbarsprite[e] = CreateSprite(oxbar)
	SetSpriteSize(oxbarsprite[e],100,100)
	SetSpritePosition(oxbarsprite[e],200,200)
	oxLevel[e] = 0
	alreadyhaveair[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0	
	status[e] = "init"	
	init_compass()	
end
 
function scuba_main(e)
	
	PlayerDist = GetPlayerDistance(e)
	
	if have_scubagear == 0 then
		if PlayerDist < scuba[e].pickup_range and have_scubagear == 0 then
			--pinpoint select object--
			module_misclib.pinpoint(e,scuba[e].pickup_range,scuba[e].item_highlight)
			tEnt[e] = g_tEnt
			--end pinpoint select object--	
			if PlayerDist < scuba[e].pickup_range and tEnt[e] ~= 0 and have_scubagear == 0 then
				if scuba[e].prompt_display == 1 then PromptLocal(e,scuba[e].prompt_text) end
				if scuba[e].prompt_display == 2 then Prompt(scuba[e].prompt_text) end
				if g_KeyPressE == 1 then
					have_scubagear = 1
					PlaySound(e,0)				
					Hide(e)
					CollisionOff(e)
					ActivateIfUsed(e)
					scuba[e].dive_secs = (scuba[e].dive_time*1000)*60
				end
			end	
		end
	end		
	
	if GetGamePlayerControlInWaterState()== 2 or GetGamePlayerControlInWaterState()== 3 and have_scubagear == 1 then
		if status[e] == "init" then			
			alreadyhaveair[e] = GetGamePlayerControlDrownTimestamp()-Timer()
			SetGamePlayerControlDrownTimestamp(Timer()+scuba[e].dive_secs+alreadyhaveair[e])			
			status[e] = "endinit"
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
		end		
	end
	if GetGamePlayerControlInWaterState()== 0 then
		status[e] = "init"
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
