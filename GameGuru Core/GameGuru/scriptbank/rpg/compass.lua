-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Compass v6 by Necrym59
-- DESCRIPTION: Will give the player a Compass? Set always active ON.
-- DESCRIPTION: [PICKUP_RANGE=100(1,200)]
-- DESCRIPTION: [PICKUP_TEXT$="E to pickup"]
-- DESCRIPTION: [@COMPASS_TYPE=1(1=Digital, 2=Magnetic)]
-- DESCRIPTION: [@COMPASS_MODE=1(1=Pickup, 2=Always On)]
-- DESCRIPTION: [POSITION_X=50(1,100)]
-- DESCRIPTION: [POSITION_Y=85(1,100)]
-- DESCRIPTION: [RADIAL_SIZE=10(5,50)]
-- DESCRIPTION: <Sound0> Pickup


g_compasses					= {}
	local compass			= {}
	local pickup_range		= {}
	local pickup_text 		= {}
	local compass_type		= {}
	local position_x 		= {}
	local position_y 		= {}
	local radial_size  		= {}
	
	local compass_posx		= {}
	local compass_posy		= {}
	local have_compass 		= {}
	local compass_active 	= {}
	local status  			= {}
	
function compass_properties(e, pickup_range, pickup_text, compass_type, compass_mode, position_x, position_y, radial_size)
	g_compasses[e] = g_Entity[e]
	g_compasses[e]['pickup_range'] = pickup_range
	g_compasses[e]['pickup_text'] = pickup_text
	g_compasses[e]['compass_type'] = compass_type
	g_compasses[e]['compass_mode'] = compass_mode
	g_compasses[e]['position_x'] = position_x
	g_compasses[e]['position_y'] = position_y
	g_compasses[e]['radial_size'] = radial_size
end

function compass_init(e)
	g_compasses[e] = g_Entity[e]
	g_compasses[e]['pickup_range'] = 100
	g_compasses[e]['pickup_text'] = "[E] to pickup"
	g_compasses[e]['compass_type'] = 1
	g_compasses[e]['compass_mode'] = 1	
	g_compasses[e]['position_x'] = 50
	g_compasses[e]['position_y'] = 3
	g_compasses[e]['radial_size'] =	10
	compass_posx 	= 0
	compass_posy 	= 0
	have_compass 	= 0
	compass_active	= 0	
	status = 'init'
end

function compass_main(e)
	g_compasses[e] = g_Entity[e]
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
		if g_compasses[e]['compass_type'] == 1 then
			defn_Compass()
			compass_posx = g_compasses[e]['position_x']
			compass_posy = g_compasses[e]['position_y']
		end	
		if g_compasses[e]['compass_type'] == 2 then
			CompassBase = CreateSprite(LoadImage("imagebank\\misc\\testimages\\compass\\compass_base.png"))
			CompassSpin = CreateSprite(LoadImage("imagebank\\misc\\testimages\\compass\\compass_spin.png"))
			CompassTop = CreateSprite(LoadImage("imagebank\\misc\\testimages\\compass\\compass_top.png"))
			SetSpriteDepth(CompassBase,100)
			SetSpritePosition(CompassBase,1000,1000)
			SizeSpriteCentred(CompassBase,g_compasses[e]['radial_size'],g_compasses[e]['radial_size'] * g_base_ratio)
			SetSpriteDepth(CompassSpin,99)
			SetSpritePosition(CompassSpin,1000,1000)
			SizeSpriteCentred(CompassSpin,g_compasses[e]['radial_size']-2,(g_compasses[e]['radial_size']-2) * g_base_ratio)
			SetSpriteDepth(CompassTop,98)
			SetSpritePosition(CompassTop,1000,1000)
			SizeSpriteCentred(CompassTop,g_compasses[e]['radial_size'],g_compasses[e]['radial_size'] * g_base_ratio)				
		end
		if g_compasses[e]['compass_mode'] == 1 then have_compass = 0 end
		if g_compasses[e]['compass_mode'] == 2 then
			have_compass = 1
			compass_active = 1
		end
		status = 'endinit'
	end	
	local PlayerDist = GetPlayerDistance(e)	

	if PlayerDist < g_compasses[e]['pickup_range'] and g_PlayerHealth > 0 and have_compass == 0 then
		local LookingAt = GetPlrLookingAtEx(e,1)
		if LookingAt == 1 then
			Prompt(g_compasses[e]['pickup_text'])
			if g_KeyPressE == 1 then				
				PromptDuration("[O] to use",1000)
				PlaySound(e,0)
				Hide(e)
				CollisionOff(e)
				ActivateIfUsed(e)
				have_compass = 1
			end
		end
	end
	
	if have_compass == 1 then
		if GetInKey() == "o" and compass_active==0 then 
			PromptDuration("[Q] to disable",1000)
			compass_active=1
		elseif GetInKey() =="q" and compass_active==1 then  
			compass_active=0
		end
		if compass_active==1 and g_compasses[e]['compass_type'] == 1 then show_Compass() end 
		if compass_active==1 and g_compasses[e]['compass_type'] == 2 then
			PasteSpritePosition(CompassBase,g_compasses[e]['position_x'],g_compasses[e]['position_y'])
			PasteSpritePosition(CompassSpin,g_compasses[e]['position_x'],g_compasses[e]['position_y'])
			SetSpriteAngle(CompassSpin,-g_PlayerAngY)
			PasteSpritePosition(CompassTop,g_compasses[e]['position_x'],g_compasses[e]['position_y'])
		end
		if compass_active==0 and g_compasses[e]['compass_type'] == 2 then
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