-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Scuba Version 5
-- DESCRIPTION: Attached object pick up gives extended diving capability. Auto activates when underwater. Set Always active ON
-- DESCRIPTION: Change [PROMPT_TEXT$="E to pickup"]
-- DESCRIPTION: Minutes of [DIVE_TIME=1(1,15)]
-- DESCRIPTION: [@COMPASS_DISPLAY=1(1=On, 2=Off)] On or Off
-- DESCRIPTION: Scuba mask [IMAGEFILE$="imagebank\\misc\\testimages\\mask.png"]

g_scuba = {}
local scubagear = {}
local prompt_text = {}
local dive_time = {}
local compass_display = {}
local mask_image = {}
local dive_secs = {}
local last_gun = {}
local wearing = {}
local oxLevel = {}	
local oxbar = {}	
local oxbarsprite = {}
local status = {}
local gunstatus = {}
local compass = {}
	
function scuba_properties(e, prompt_text, dive_time, compass_display, mask_image, dive_secs)
	g_scuba[e] = g_Entity[e]
	g_scuba[e]['prompt_text'] 		= prompt_text
	g_scuba[e]['dive_time'] 		= dive_time
	g_scuba[e]['compass_display'] 	= compass_display	
	g_scuba[e]['mask_image'] 		= imagefile
	g_scuba[e]['dive_secs'] 		= (g_scuba[e]['dive_time']*60)
end
	
function scuba_init(e)
	g_scuba[e] = g_Entity[e]
	have_scubagear = 0
	g_scuba[e]['prompt_text'] = "E to pickup"
	g_scuba[e]['dive_time'] = 1	
	g_scuba[e]['compass_display'] = 1
	g_scuba[e]['mask_image'] = "imagebank\\misc\\testimages\\mask.png"
	g_scuba[e]['dive_secs'] = (g_scuba[e]['dive_time']*60)
	scubagear = CreateSprite(LoadImage(g_scuba[e]['mask_image']))
	SetSpriteSize(scubagear,100,100)
	SetSpritePosition(scubagear,200,200)
	oxbar = LoadImage("imagebank\\misc\\testimages\\oxygen-bar.png")
	oxbarsprite = CreateSprite(oxbar)
	SetSpriteSize(oxbarsprite,100,100)
	SetSpritePosition(oxbarsprite,200,200)
	oxLevel[e] = 0
	status[e] = "init"	
	init_compass()
end
 
function scuba_main(e)
	g_scuba[e] = g_Entity[e]
	PlayerDist = GetPlayerDistance(e)
	if have_scubagear == 0 then
		if PlayerDist < 80 and g_PlayerHealth > 0 and have_scubagear == 0 then
			Prompt(g_scuba[e]['prompt_text'])
			if GetInKey() == "e" or GetInKey() == "E" then
				have_scubagear = 1
				PlaySound(e,0)				
				Destroy(e)
				ActivateIfUsed(e)
				g_scuba[e]['dive_secs'] = g_scuba[e]['dive_secs']*1000					
			end
		end
	end		
	
	if GetGamePlayerControlInWaterState()== 2 or GetGamePlayerControlInWaterState()== 3 then
		if status[e] == "init" then			
			local alreadyhaveair = GetGamePlayerControlDrownTimestamp()-Timer()
			SetGamePlayerControlDrownTimestamp(Timer()+g_scuba[e]['dive_secs']+alreadyhaveair)
			status[e] = "endinit"
		end
		if have_scubagear == 1 then
			if g_scuba[e]['compass_display'] == 1 then show_compass() end
			PasteSpritePosition(scubagear,0,0)
			oxLevel[e] = GetGamePlayerControlDrownTimestamp()-Timer()		
			SetSpriteSize(oxbarsprite,(oxLevel[e]/500)/50,1)			
			SetSpriteOffset(oxbarsprite,((oxLevel[e]/500)/50)/2,0)
			SetSpriteDepth(oxbarsprite,0)			
			if GetGamePlayerControlInWaterState()== 2 and g_scuba[e]['dive_secs'] >= 1 then SetSpriteColor(oxbarsprite,0,225,255,150) end	
			if GetGamePlayerControlInWaterState()== 2 and g_scuba[e]['dive_secs'] <= 0 then SetSpriteColor(oxbarsprite,255,0,0,150)	end
			PasteSpritePosition(oxbarsprite,50,95)			
			local timeleftmin = math.ceil(oxLevel[e]/1000/60)			
			TextCenterOnXColor(50,94,2,"Oxygen Level",0,225,255)
			if g_scuba[e]['dive_secs'] >= 1 then				
				g_scuba[e]['dive_secs'] = (g_scuba[e]['dive_secs']-8)			
			end
		end			
	end
	if GetGamePlayerControlInWaterState()== 0 then
		status[e] = "init"
		if have_scubagear == 1 and g_scuba[e]['dive_secs'] <= 0 then
			PromptDuration("Scuba gear discarded",2000)
			have_scubagear = 0
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
