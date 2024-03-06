-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Helmet v19   by Necrym59
-- DESCRIPTION: The applied object will give the player a Helmet Hud? Set Always active ON.
-- DESCRIPTION: [PICKUP_TEXT$="E to Pickup/Wear"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [USEAGE_TEXT$="Hold B + Wheel to zoom, N=Nightvision ON/OFF, P=Remove/Wear Helmet"]
-- DESCRIPTION: [@HELMET_MODE=1(1=Pickup/Drop, 2=Pickup/Retain, 3=Always On)]
-- DESCRIPTION: [#MIN_ZOOM=-10(-30,1)]
-- DESCRIPTION: [MAX_ZOOM=30(1,50)]
-- DESCRIPTION: [ZOOM_SPEED=1(1,10)]
-- DESCRIPTION: [READOUT_X=50(1,100)]
-- DESCRIPTION: [READOUT_Y=10(1,100)]
-- DESCRIPTION: [@COMPASS=2(1=On, 2=Off)]
-- DESCRIPTION: [@COMPASS_POSITION=2(1=Top, 2=Bottom)]
-- DESCRIPTION: [IMAGEFILE$="imagebank\\misc\\testimages\\helmethud1.png"] for the Helmet overlay image  
-- DESCRIPTION: <Sound0> for pickup
-- DESCRIPTION: <Sound1> for wearing/removing
-- DESCRIPTION: <Sound2> for NightVison On/Off

local U = require "scriptbank\\utillib"

g_helmet = {}
local helmet = {}
local pickup_text = {}
local pickup_range = {}
local useage_text = {}
local start_wheel = {}
local mod = {}
local zoom_speed = {}
local helmet_mode = {}
local min_zoom = {}
local max_zoom = {}
local screen_image = {}
local readout_x = {}
local readout_y = {}
local compass = {}
local compass_position = {}

local compass_pos = {}
local last_gun = {}
local gunstatus = {}
local nvswitch = {}
local hmswitch = {}
local have_helmet = {}
local keypause1 = {}
local keypause2 = {}
local helmetsp = {}
local status = {}
local default_AmbienceRed = {}
local default_AmbienceBlue = {}
local default_AmbienceGreen = {}
local default_AmbienceIntensity = {}
local default_FogRed = {}
local default_FogGreen = {}
local default_FogBlue = {}
local default_FogNearest = {}
local default_FogDistance = {}
local current_fov = {}

function helmet_properties(e, pickup_text, pickup_range, useage_text, helmet_mode, min_zoom, max_zoom, zoom_speed, readout_x, readout_y, compass, compass_position, screen_image)
	g_helmet[e] = g_Entity[e]
	g_helmet[e]['pickup_text'] = pickup_text
	g_helmet[e]['pickup_range'] = pickup_range
	g_helmet[e]['useage_text'] = useage_text
	g_helmet[e]['helmet_mode'] = helmet_mode
	g_helmet[e]['min_zoom'] = min_zoom
	g_helmet[e]['max_zoom'] = max_zoom
	g_helmet[e]['zoom_speed'] = zoom_speed
	g_helmet[e]['readout_x'] = readout_x
	g_helmet[e]['readout_y'] = readout_y
	g_helmet[e]['compass'] = compass or 2
	g_helmet[e]['compass_position'] = compass_position
	g_helmet[e]['screen_image'] = imagefile or screen_image
end -- End properties

function helmet_init(e)
	g_helmet[e] = {}
	g_helmet[e]['pickup_text'] = "E to Pickup/Wear"
	g_helmet[e]['pickup_range'] = 80
	g_helmet[e]['useage_text'] = "Hold B + Wheel to zoom, N=Nightvision ON/OFF,  P=Remove/Wear Helmet"
	g_helmet[e]['helmet_mode'] = 1
	g_helmet[e]['min_zoom'] = -10
	g_helmet[e]['max_zoom'] = 30
	g_helmet[e]['zoom_speed'] = 1
	g_helmet[e]['readout_x'] = 50
	g_helmet[e]['readout_y'] = 10
	g_helmet[e]['compass'] = 2
	g_helmet[e]['compass_position'] = 1
	g_helmet[e]['screen_image'] = "imagebank\\misc\\testimages\\helmethud1.png"	
	
	have_helmet[e] = 0
	start_wheel = 0
	mod = 0
	fov = 0
	current_fov[e] = 0
	last_gun = g_PlayerGunName
	gunstatus[e] = 0
	status[e] = "init"
	init_compass()
	nvswitch[e] = 0
	hmswitch[e] = 0
	keypause1[e] = math.huge
	keypause2[e] = math.huge	
	default_AmbienceRed = GetAmbienceRed()
	default_AmbienceBlue = GetAmbienceBlue()
	default_AmbienceGreen = GetAmbienceGreen()
	default_AmbienceIntensity = GetAmbienceIntensity()
	default_FogRed = GetFogRed()
	default_FogGreen = GetFogGreen()
	default_FogBlue = GetFogBlue()
end

function helmet_main(e)

	if status[e] == "init" then
		helmetsp[e] = CreateSprite(LoadImage(g_helmet[e]['screen_image']))
		SetSpriteSize(helmetsp[e],100,100)
		SetSpriteDepth(helmetsp[e],100)
		SetSpritePosition(helmetsp[e],1000,1000)
		if g_helmet[e]['helmet_mode'] == 1 then have_helmet[e] = 0 end
		if g_helmet[e]['helmet_mode'] == 3 then have_helmet[e] = 1 end
		compass_pos = g_helmet[e]['compass_position']
		mod = g_PlayerFOV
		fov = g_PlayerFOV
		have_helmet[e] = 0
		keypause1[e] = g_Time + 1000
		keypause2[e] = g_Time + 1000		
		status[e] = "endinit"
	end
	
	PlayerDist = GetPlayerDistance(e)
	if fov == nil then fov = g_PlayerFOV end	
	
	if g_helmet[e]['compass'] == 1 then
		if have_helmet[e] == 1 then
			show_compass()
		end
	end

	if have_helmet[e] == 0 then	
		if g_helmet[e]['helmet_mode'] == 1 or g_helmet[e]['helmet_mode'] == 2 then
			if PlayerDist < g_helmet[e]['pickup_range'] and g_PlayerHealth > 0 and have_helmet[e] == 0 then
				PromptLocalForVR(e,g_helmet[e]['pickup_text'])				
				if g_KeyPressE == 1 then
					have_helmet[e] = 1
					PlaySound(e,0)
					Hide(e)
					SetPosition(e,g_PlayerPosX,g_PlayerPosY+1000,g_PlayerPosZ)
					CollisionOff(e)
					ActivateIfUsed(e)
				end
			end
		end
	end

	if have_helmet[e] == 1 then	
		SetPosition(e,g_PlayerPosX,g_PlayerPosY+1000,g_PlayerPosZ)
		if hmswitch[e] == 0 then 
			PasteSpritePosition(helmetsp[e],0,0)
			TextCenterOnXColor(50,95,2,g_helmet[e]['useage_text'],100,255,100)
		end	
		if g_Scancode == 48 then --Hold B Key to use
			if g_PlayerGunID > 0 then
				SetPlayerWeapons(0)
				gunstatus[e] = 1
			end
			if g_MouseWheel < 0 then
				mod = mod - g_helmet[e]['zoom_speed']
			elseif g_MouseWheel > 0 then
				mod = mod + g_helmet[e]['zoom_speed']
			end
			if mod < g_helmet[e]['min_zoom'] then
				mod = g_helmet[e]['min_zoom']
			elseif mod > g_helmet[e]['max_zoom'] then
				mod = g_helmet[e]['max_zoom']
			end
			SetPlayerFOV(fov-mod)
			current_fov[e] = (fov-mod)
			TextCenterOnX(g_helmet[e]['readout_x'],g_helmet[e]['readout_y'],3,"Magnification Factor: " ..mod)
		else
			start_wheel = g_MouseWheel
			mod = 0
			if mod > 0 then SetPlayerFOV(fov) end
			if mod < 0 then	SetPlayerFOV(fov) end
			if gunstatus[e] == 1 then
				ChangePlayerWeapon(last_gun)
				SetPlayerWeapons(1)
				gunstatus[e] = 0
			end
		end

		if g_Time > keypause1[e] and nvswitch[e] == 0 then
			if GetInKey() == "n" or GetInKey() == "N" and nvswitch[e] == 0 then	
				PlaySound(e,2)
				SetAmbienceRed(0)
				SetAmbienceBlue(0)
				SetAmbienceGreen(180)
				SetAmbienceIntensity(120)
				SetFogRed(0)
				SetFogGreen(255)
				SetFogBlue(0)
				keypause1[e] = g_Time + 1000
				nvswitch[e] = 1
			end
		end	
		if g_Time > keypause1[e] and nvswitch[e] == 1 then
			if GetInKey() == "n" or GetInKey() == "N" and nvswitch[e] == 1 then
				PlaySound(e,2)
				SetAmbienceRed(default_AmbienceRed)
				SetAmbienceBlue(default_AmbienceBlue)
				SetAmbienceGreen(default_AmbienceGreen)
				SetAmbienceIntensity(default_AmbienceIntensity)
				SetFogRed(default_FogRed)
				SetFogGreen(default_FogGreen)
				SetFogBlue(default_FogBlue)
				keypause1[e] = g_Time + 1000
				nvswitch[e] = 0
			end
		end	
		if g_helmet[e]['helmet_mode'] == 1 then
			if GetInKey() == "p" or GetInKey() == "P" then
				local ox,oy,oz = U.Rotate3D( 0, 60, 0, math.rad( g_PlayerAngX ), math.rad( g_PlayerAngY ), math.rad( g_PlayerAngZ ) )
				local forwardposx, forwardposy, forwardposz = g_PlayerPosX + ox, g_PlayerPosY + oy, g_PlayerPosZ + oz
				SetPosition(e,forwardposx, forwardposy, forwardposz)
				SetRotation(e,0, -90, 0)
				Show(e)
				SetAmbienceRed(default_AmbienceRed)
				SetAmbienceBlue(default_AmbienceBlue)
				SetAmbienceGreen(default_AmbienceGreen)
				SetAmbienceIntensity(default_AmbienceIntensity)
				SetFogRed(default_FogRed)
				SetFogGreen(default_FogGreen)
				SetFogBlue(default_FogBlue)
				PlaySound(e,1)
				nvswitch[e] = 0
				keypause1[e] = g_Time + 1000
				have_helmet[e] = 0
				SetPlayerFOV(fov)
			end
		end
		if g_helmet[e]['helmet_mode'] == 2 then --reuseable
			if g_Time > keypause2[e] and hmswitch[e] == 0 then
				if GetInKey() == "p" or GetInKey() == "P" and hmswitch[e] == 0 then
					SetPosition(e,g_PlayerPosX,g_PlayerPosY+1000,g_PlayerPosZ)
					Hide(e)
					SetAmbienceRed(default_AmbienceRed)
					SetAmbienceBlue(default_AmbienceBlue)
					SetAmbienceGreen(default_AmbienceGreen)
					SetAmbienceIntensity(default_AmbienceIntensity)
					SetFogRed(default_FogRed)
					SetFogGreen(default_FogGreen)
					SetFogBlue(default_FogBlue)
					PlaySound(e,1)
					nvswitch[e] = 0
					keypause2[e] = g_Time + 1000
					hmswitch[e] = 1
					SetPlayerFOV(fov)
				end
			end	
			if g_Time > keypause2[e] and hmswitch[e] == 1 then
				if GetInKey() == "p" or GetInKey() == "P" and hmswitch[e] == 1 then
					SetPosition(e,g_PlayerPosX,g_PlayerPosY+1000,g_PlayerPosZ)
					Hide(e)
					SetAmbienceRed(default_AmbienceRed)
					SetAmbienceBlue(default_AmbienceBlue)
					SetAmbienceGreen(default_AmbienceGreen)
					SetAmbienceIntensity(default_AmbienceIntensity)
					SetFogRed(default_FogRed)
					SetFogGreen(default_FogGreen)
					SetFogBlue(default_FogBlue)
					PlaySound(e,1)
					nvswitch[e] = 0
					keypause2[e] = g_Time + 1000
					hmswitch[e] = 0
					SetPlayerFOV(current_fov[e])
				end
			end
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
	if compass_pos == 1 then
		TextCenterOnXColor(50,1,2,"Navigation Compass",255,255,255)
		TextCenterOnXColor(50,3,2,compass_str,100,255,100)
		TextCenterOnXColor(50,4,2,"^",255,255,255)
		TextCenterOnXColor(50,5,8,bearing,255,255,255)
	end
	if compass_pos== 2 then
		TextCenterOnXColor(50,88,2,"Navigation Compass",255,255,255)
		TextCenterOnXColor(50,90,2,compass_str,100,255,100)
		TextCenterOnXColor(50,91,2,"^",255,255,255)
		TextCenterOnXColor(50,92,8,bearing,255,255,255)
	end
end
