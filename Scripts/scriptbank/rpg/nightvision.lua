-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Nightvision v18 by Necrym59
-- DESCRIPTION: The applied object will give the player Nightvision Goggles?
-- DESCRIPTION: [PICKUP_TEXT$="E to Pickup"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [USEAGE_TEXT$="N=Nightvision ON/OFF, Hold B + Wheel to zoom"]
-- DESCRIPTION: [#MIN_ZOOM=-10(-20,1)]
-- DESCRIPTION: [#MAX_ZOOM=30(1,30)]
-- DESCRIPTION: [ZOOM_SPEED=1(1,10)]
-- DESCRIPTION: [ZOOM_READOUT_X=50(1,100)]
-- DESCRIPTION: [ZOOM_READOUT_Y=10(1,100)]
-- DESCRIPTION: [@COMPASS=1(1=On, 2=Off)]
-- DESCRIPTION: [@COMPASS_POSITION=2(1=Top, 2=Bottom)]
-- DESCRIPTION: [GOGGLES_IMAGEFILE$="imagebank\\misc\\testimages\\nvgoggles.png"]
-- DESCRIPTION: [#EXPOSURE=2.95(1.0,5.0)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]
-- DESCRIPTION: <Sound0> for pickup
-- DESCRIPTION: <Sound1> for wearing/removing

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local nightvision 		= {}
local pickup_text 		= {}
local pickup_range		= {}
local useage_text		= {}
local start_wheel		= {}
local mod				= {}
local zoom_speed		= {}
local min_zoom			= {}
local max_zoom			= {}
local zoom_readout_x	= {}
local zoom_readout_y	= {}
local screen_image		= {}
local exposure			= {}
local item_highlight	= {}
local highlight_icon	= {}

local compass = {}
local compass_position = {}
local status = {}
local tEnt = {}
local compass_pos = {}
local oldfov = {}
local played ={}
local keypause = {}
local nvswitch = {}
local nvsprite = {}
local selectobj	= {}
local have_nvgoggles = {}
local default_AmbienceRed = {}
local default_AmbienceBlue = {}
local default_AmbienceGreen = {}
local default_AmbienceIntensity = {}
local default_FogRed = {}
local default_FogGreen = {}
local default_FogBlue = {}
local default_FogNearest = {}
local default_FogDistance = {}
local default_Exposure = {}
local hl_icon = {}
local hl_imgwidth = {}
local hl_imgheight = {}

function nightvision_properties(e, pickup_text, pickup_range, useage_text, min_zoom, max_zoom, zoom_speed, zoom_readout_x, zoom_readout_y, compass, compass_position, screen_image, exposure, item_highlight, highlight_icon_imagefile)
	nightvision[e].pickup_text = pickup_text
	nightvision[e].pickup_range = pickup_range	
	nightvision[e].useage_text = useage_text
	nightvision[e].min_zoom = min_zoom
	nightvision[e].max_zoom = max_zoom
	nightvision[e].zoom_speed = zoom_speed
	nightvision[e].zoom_readout_x = zoom_readout_x
	nightvision[e].zoom_readout_y = zoom_readout_y
	nightvision[e].compass = compass
	nightvision[e].compass_position = compass_position
	nightvision[e].screen_image = goggles_imagefile or screen_image
	nightvision[e].exposure = exposure
	nightvision[e].item_highlight = item_highlight
	nightvision[e].highlight_icon = highlight_icon_imagefile
end 

function nightvision_init(e)
	nightvision[e] = {}
	nightvision[e].pickup_text = ""
	nightvision[e].pickup_range = 80	
	nightvision[e].useage_text = ""
	nightvision[e].min_zoom = -10
	nightvision[e].max_zoom = 30
	nightvision[e].zoom_speed = 1
	nightvision[e].zoom_readout_x = 50
	nightvision[e].zoom_readout_y = 10
	nightvision[e].compass = 1
	nightvision[e].compass_position = 1
	nightvision[e].screen_image = "imagebank\\misc\\testimages\\nvgoggles.png"
	nightvision[e].exposure = 2.95
	nightvision[e].item_highlight = 0
	nightvision[e].highlight_icon = "imagebank\\icons\\pickup.png"
	
	start_wheel = 0
	mod = 0
	fov = 0
	status = "init"
	have_nvgoggles[e] = 0	
	defn_Compass()
	nvswitch[e] = 0
	nvsprite[e] = 0
	played[e] = 0
	tEnt[e] = 0
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0
	selectobj[e] = 0	
	keypause[e] = math.huge
	default_AmbienceRed[e] = GetAmbienceRed()
	default_AmbienceBlue[e] = GetAmbienceBlue()
	default_AmbienceGreen[e] = GetAmbienceGreen()
	default_AmbienceIntensity[e] = GetAmbienceIntensity()
	default_FogRed[e] = GetFogRed()
	default_FogGreen[e] = GetFogGreen()
	default_FogBlue[e] = GetFogBlue()
	default_FogNearest[e] = GetFogNearest()
	default_FogDistance[e] = GetFogDistance()
	default_Exposure[e] = GetExposure() --0.95 --
end

function nightvision_main(e)

	if status == "init" then
		if nightvision[e].item_highlight == 3 and nightvision[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(nightvision[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(nightvision[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(nightvision[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end	
		if nightvision[e].screen_image ~= "" then
			nvsprite[e] = CreateSprite(LoadImage(nightvision[e].screen_image))
			SetSpriteSize(nvsprite[e],100,100)
			SetSpriteDepth(nvsprite[e],100)
			SetSpritePosition(nvsprite[e],500,500)
		end			
		compass_pos = nightvision[e].compass_position
		mod = g_PlayerFOV
		fov = g_PlayerFOV
		keypause[e] = g_Time + 1000
		status = "endinit"
	end
	PlayerDist = GetPlayerDistance(e)
	if fov == nil then fov = g_PlayerFOV end
	
	if have_nvgoggles[e] == 0 then
		if PlayerDist < nightvision[e].pickup_range then
			--pinpoint select object--
			module_misclib.pinpoint(e,nightvision[e].pickup_range,nightvision[e].item_highlight,hl_icon[e])
			tEnt[e] = g_tEnt
			--end pinpoint select object--
		end
	
		if PlayerDist < nightvision[e].pickup_range and tEnt[e] == e and GetEntityVisibility(e) == 1 then
			PromptLocal(e,nightvision[e].pickup_text)
			if g_KeyPressE == 1 then
				PromptDuration(nightvision[e].useage_text,2000)
				PlaySound(e,0)
				have_nvgoggles[e] = 1
				Hide(e)
				CollisionOff(e)
				ActivateIfUsed(e)
			end
		end
	end
	
	if have_nvgoggles[e] == 1 then
		if g_Time > keypause[e] and nvswitch[e] == 0 then
			if GetInKey() == "n" or GetInKey() == "N" and nvswitch[e] == 0 then
				SetAmbienceRed(0)
				SetAmbienceBlue(0)
				SetAmbienceGreen(180)
				SetAmbienceIntensity(120)
				SetExposure(nightvision[e].exposure)
				SetFogRed(0)
				SetFogGreen(255)
				SetFogBlue(0)
				SetFogNearest(1000)
				SetFogDistance(10000)
				if played[e] == 0 and nvswitch[e] == 0 then
					PlaySound(e,1)
					played[e] = 1
				end
				keypause[e] = g_Time + 1000
				nvswitch[e] = 1
			end
		end
		if nvswitch[e] == 1 then
			PasteSpritePosition(nvsprite[e],0,0)
			TextCenterOnXColor(50,99,2,nightvision[e].useage_text,100,255,100)
			if nightvision[e].compass == 1 then
				show_Compass()
			end
		end
		if g_Scancode == 48 and nvswitch[e] == 1 then --Hold B Key to use
			if g_MouseWheel < 0 then
				mod = mod - nightvision[e].zoom_speed
			elseif g_MouseWheel > 0 then
				mod = mod + nightvision[e].zoom_speed
			end
			if mod < nightvision[e].min_zoom then
				mod = nightvision[e].min_zoom
			elseif mod > nightvision[e].max_zoom then
				mod = nightvision[e].max_zoom
			end
			SetPlayerFOV(fov-mod)
			TextCenterOnX(nightvision[e].zoom_readout_x,nightvision[e].zoom_readout_y,3,"Magnification Factor: " ..mod)			
		else
			start_wheel = g_MouseWheel
			mod = 0
			SetPlayerFOV(fov)
		end
		if g_Scancode == 0 then
			played[e] = 0
		end
		if g_Time > keypause[e] and nvswitch[e] == 1 then
			if GetInKey() == "n" or GetInKey() == "N" and nvswitch[e] == 1 then
				SetAmbienceRed(default_AmbienceRed[e])
				SetAmbienceBlue(default_AmbienceBlue[e])
				SetAmbienceGreen(default_AmbienceGreen[e])
				SetAmbienceIntensity(default_AmbienceIntensity[e])
				SetFogRed(default_FogRed[e])
				SetFogGreen(default_FogGreen[e])
				SetFogBlue(default_FogBlue[e])
				SetFogNearest(default_FogNearest[e])
				SetFogDistance(default_FogDistance[e])
				SetExposure(default_Exposure[e])
				if played[e] == 0 and nvswitch[e] == 1 then
					PlaySound(e,1)
					played[e] = 1
				end
				keypause[e] = g_Time + 1000
				nvswitch[e] = 0
			end
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

	if compass_pos == 1 then
		TextCenterOnXColor(50,1,2,"Navigation Compass",255,255,255)
		TextCenterOnXColor(50,3,2,compass_str,100,255,100)
		TextCenterOnXColor(50,4,2,"^",255,255,255)
		TextCenterOnXColor(50,5,2,bearing,255,255,255)
	end
	if compass_pos== 2 then
		TextCenterOnXColor(50,88,2,"Navigation Compass",255,255,255)
		TextCenterOnXColor(50,90,2,compass_str,100,255,100)
		TextCenterOnXColor(50,91,2,"^",255,255,255)
		TextCenterOnXColor(50,92,2,bearing,255,255,255)
	end
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
