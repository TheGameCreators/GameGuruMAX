-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Compass v7 by Necrym59
-- DESCRIPTION: Will give the player a Compass? Set always active ON.
-- DESCRIPTION: [PICKUP_RANGE=100(1,200)]
-- DESCRIPTION: [PICKUP_TEXT$="E to pickup"]
-- DESCRIPTION: [@COMPASS_TYPE=1(1=Digital, 2=Magnetic)]
-- DESCRIPTION: [@COMPASS_MODE=1(1=Pickup, 2=Always On)]
-- DESCRIPTION: [POSITION_X=50(1,100)]
-- DESCRIPTION: [POSITION_Y=85(1,100)]
-- DESCRIPTION: [RADIAL_SIZE=10(5,50)]
-- DESCRIPTION: [BASE_IMAGEFILE$="imagebank\\misc\\testimages\\compass\\compass_base.png"]
-- DESCRIPTION: [SPIN_IMAGEFILE$="imagebank\\misc\\testimages\\compass\\compass_spin.png"]
-- DESCRIPTION: [TOP_IMAGEFILE$="imagebank\\misc\\testimages\\compass\\compass_top.png"]
-- DESCRIPTION: <Sound0> Pickup


local xcompass			= {}
local compass			= {}
local pickup_range		= {}
local pickup_text 		= {}
local compass_type		= {}
local position_x 		= {}
local position_y 		= {}
local radial_size  		= {}
local base_image  		= {}
local spin_image  		= {}
local top_image  		= {}

local compass_posx		= {}
local compass_posy		= {}
local have_compass 		= {}
local compass_active 	= {}
local status  			= {}
	
function compass_properties(e, pickup_range, pickup_text, compass_type, compass_mode, position_x, position_y, radial_size, base_image, spin_image, top_image)
	xcompass[e] = g_Entity[e]
	xcompass[e].pickup_range = pickup_range
	xcompass[e].pickup_text = pickup_text
	xcompass[e].compass_type = compass_type
	xcompass[e].compass_mode = compass_mode
	xcompass[e].position_x = position_x
	xcompass[e].position_y = position_y
	xcompass[e].radial_size = radial_size
	xcompass[e].base_image = base_image or base_imagefile
	xcompass[e].spin_image = spin_image or spin_imagefile
	xcompass[e].top_image = top_image or top_imagefile
end

function compass_init(e)
	xcompass[e] = {}
	xcompass[e].pickup_range = 100
	xcompass[e].pickup_text = "[E] to pickup"
	xcompass[e].compass_type = 1
	xcompass[e].compass_mode = 1	
	xcompass[e].position_x = 50
	xcompass[e].position_y = 3
	xcompass[e].radial_size =	10
	xcompass[e].base_image = ""
	xcompass[e].spin_image = ""
	xcompass[e].top_image = ""
	compass_posx 	= 0
	compass_posy 	= 0
	have_compass 	= 0
	compass_active	= 0	
	status = 'init'
end

function compass_main(e)
	xcompass[e] = g_Entity[e]
	if status == 'init' then
		g_base_ratio = 16 / 9
		g_aspect_ratio = ( GetDeviceWidth() / GetDeviceHeight() )
		g_aspectXcorrect = 1
		g_aspectYcorrect = 1
		if g_aspect_ratio < g_base_ratio then
			g_aspectYcorrect = g_aspect_ratio / g_base_ratio
		elseif
			g_aspect_ratio > g_base_ratio then
			g_aspectXcorrect = g_base_ratio / g_aspect_ratio 
		end
		if xcompass[e].compass_type == 1 then
			defn_Compass()
			compass_posx = xcompass[e].position_x
			compass_posy = xcompass[e].position_y
		end	
		if xcompass[e].compass_type == 2 then
			CompassBase = CreateSprite(LoadImage(xcompass[e].base_image))
			CompassSpin = CreateSprite(LoadImage(xcompass[e].spin_image))
			CompassTop = CreateSprite(LoadImage(xcompass[e].top_image))
			SetSpriteDepth(CompassBase,100)
			SetSpritePosition(CompassBase,1000,1000)
			SizeSpriteCentred(CompassBase,xcompass[e].radial_size,xcompass[e].radial_size * g_base_ratio)
			SetSpriteDepth(CompassSpin,99)
			SetSpritePosition(CompassSpin,1000,1000)
			SizeSpriteCentred(CompassSpin,xcompass[e].radial_size-2,(xcompass[e].radial_size-2) * g_base_ratio)
			SetSpriteDepth(CompassTop,98)
			SetSpritePosition(CompassTop,1000,1000)
			SizeSpriteCentred(CompassTop,xcompass[e].radial_size,xcompass[e].radial_size * g_base_ratio)				
		end
		if xcompass[e].compass_mode == 1 then have_compass = 0 end
		if xcompass[e].compass_mode == 2 then
			have_compass = 1
			compass_active = 1
			CollisionOff(e)
			Hide(e)
		end
		status = 'endinit'
	end	
	
	local PlayerDist = GetPlayerDistance(e)	
	if PlayerDist < xcompass[e].pickup_range and have_compass == 0 then
		local LookingAt = GetPlrLookingAtEx(e,1)
		if LookingAt == 1 then
			PromptLocal(e,xcompass[e].pickup_text)
			if g_KeyPressE == 1 then				
				PromptDuration("[>] to enable",1000)
				PlaySound(e,0)
				Hide(e)
				CollisionOff(e)
				ActivateIfUsed(e)
				have_compass = 1
			end
		end
	end
	
	if have_compass == 1 then
		if GetInKey() == "." or GetInKey() == ">" and compass_active == 0 then 
			PromptDuration("[<] to disable",1000)
			compass_active=1
		end	
		if GetInKey() =="," or GetInKey() =="<" and compass_active == 1 then 
			if xcompass[e].compass_mode == 1 then compass_active=0 end
			if xcompass[e].compass_mode == 2 then compass_active=1 end
		end
		if compass_active==1 and xcompass[e].compass_type == 1 then show_Compass() end 
		if compass_active==1 and xcompass[e].compass_type == 2 then
			PasteSpritePosition(CompassBase,xcompass[e].position_x,xcompass[e].position_y)
			PasteSpritePosition(CompassSpin,xcompass[e].position_x,xcompass[e].position_y)
			SetSpriteAngle(CompassSpin,-g_PlayerAngY)
			PasteSpritePosition(CompassTop,xcompass[e].position_x,xcompass[e].position_y)
		end
		if compass_active==0 and xcompass[e].compass_type == 2 then
			PasteSpritePosition(CompassBase,1000,1000)
			PasteSpritePosition(CompassSpin,1000,1000)
			PasteSpritePosition(CompassTop,1000,1000)			
		end
	end
end

function show_Compass()	
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
	TextCenterOnXColor(compass_posx,compass_posy,2,"Navigation Compass",255,255,255)
	TextCenterOnXColor(compass_posx,compass_posy+2,2,compass_str,100,255,100)
	TextCenterOnXColor(compass_posx,compass_posy+3,2,"^",255,255,255)	
	TextCenterOnXColor(compass_posx,compass_posy+3,8,bearing,255,255,255)	
end

function defn_Compass()  
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

function SizeSpriteCentred(spr,x,y,offx,offy)
	local xs, ys = x * g_aspectXcorrect, y * g_aspectYcorrect
	offx = offx or xs / 2
	offy = offy or ys / 2
	SetSpriteSize(spr,xs,ys)
	SetSpriteOffset(spr,offx,offy )
end