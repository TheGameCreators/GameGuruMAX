-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Helmet v25   by Necrym59
-- DESCRIPTION: The applied object will give the player a Helmet Hud? Set Always active ON.
-- DESCRIPTION: [PICKUP_TEXT$="E to Pickup/Wear"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [USAGE_TEXT$="Hold B + Wheel to zoom, N=Nightvision ON/OFF, H=Remove/Wear Helmet"]
-- DESCRIPTION: [@HELMET_MODE=1(1=Pickup/Drop, 2=Pickup/Retain, 3=Always On)]
-- DESCRIPTION: [#MIN_ZOOM=-10(-30,1)]
-- DESCRIPTION: [MAX_ZOOM=30(1,50)]
-- DESCRIPTION: [ZOOM_SPEED=1(1,10)]
-- DESCRIPTION: [ZOOM_READOUT_X=50(1,100)]
-- DESCRIPTION: [ZOOM_READOUT_Y=10(1,100)]
-- DESCRIPTION: [@COMPASS=2(1=On, 2=Off)]
-- DESCRIPTION: [@COMPASS_POSITION=2(1=Top, 2=Bottom)]
-- DESCRIPTION: [IMAGEFILE$="imagebank\\misc\\testimages\\helmethud1.png"] for the Helmet overlay image
-- DESCRIPTION: [NIGHTVISION_COLOR!=0] If set on will change nightvision rgb values set.
-- DESCRIPTION: [NIGHTVISION_AMBIENCE_R=0(0,255)]
-- DESCRIPTION: [NIGHTVISION_AMBIENCE_G=180(0,255)]
-- DESCRIPTION: [NIGHTVISION_AMBIENCE_B=0(0,255)]
-- DESCRIPTION: [NIGHTVISION_INTENSITY=120(0,255)]
-- DESCRIPTION: [NIGHTVISION_FOG_R=0(0,255)]
-- DESCRIPTION: [NIGHTVISION_FOG_G=255(0,255)]
-- DESCRIPTION: [NIGHTVISION_FOG_B=0(0,255)]
-- DESCRIPTION: [NIGHTVISION_FOG_NEAREST=0(0,100)]
-- DESCRIPTION: [NIGHTVISION_FOG_DISTANCE=50(0,100)]
-- DESCRIPTION: [TOXIC_PROTECTION!=0] If set on will protect against toxic or radiation zones.
-- DESCRIPTION: <Sound0> for pickup/wearing/removing
-- DESCRIPTION: <Sound1> loop for while wearing
-- DESCRIPTION: <Sound2> for NightVison On/Off

local U = require "scriptbank\\utillib"
g_have_helmet = {}
g_toxiczone = {}
g_gasmask_on = {}
g_radsuit_on = {}

local helmet = {}
local pickup_text = {}
local pickup_range = {}
local usage_text = {}
local helmet_mode = {}
local zoom_speed = {}
local zoom_readout_x = {}
local zoom_readout_y = {}
local min_zoom = {}
local max_zoom = {}
local compass = {}
local compass_position = {}
local screen_image = {}
local nightvision_color = {}
local nightvision_ambience_r = {}
local nightvision_ambience_g = {}
local nightvision_ambience_b = {}
local nightvision_intensity = {}
local nightvision_fog_r = {}
local nightvision_fog_g = {}
local nightvision_fog_b = {}
local nightvision_fog_nearest = {}
local nightvision_fog_distance = {}
local toxic_protection = {}

local compass_pos = {}
local last_gun = {}
local gunstatus = {}
local nvswitch = {}
local hmswitch = {}
local have_helmet = {}
local doloop = {}
local keypause1 = {}
local keypause2 = {}
local helmetsp = {}
local status = {}
local start_wheel = {}
local mod = {}
local currenthealth = {}
local default_AmbienceRed = {}
local default_AmbienceBlue = {}
local default_AmbienceGreen = {}
local default_AmbienceIntensity = {}
local default_FogRed = {}
local default_FogGreen = {}
local default_FogBlue = {}
local default_FogNearest = {}
local default_FogDistance = {}
local nv_ambience_r = {}
local nv_ambience_g = {}
local nv_ambience_b = {}
local nv_intensity = {}
local nv_fog_r = {}
local nv_fog_g = {}
local nv_fog_b = {}
local nv_fog_near = {}
local nv_fog_dist = {}
local current_fov = {}

function helmet_properties(e, pickup_text, pickup_range, usage_text, helmet_mode, min_zoom, max_zoom, zoom_speed, zoom_readout_x, zoom_readout_y, compass, compass_position, screen_image, nightvision_color, nightvision_ambience_r, nightvision_ambience_g, nightvision_ambience_b, nightvision_intensity, nightvision_fog_r, nightvision_fog_g, nightvision_fog_b, nightvision_fog_nearest, nightvision_fog_distance, toxic_protection)
	helmet[e].pickup_text = pickup_text
	helmet[e].pickup_range = pickup_range
	helmet[e].usage_text = usage_text
	helmet[e].helmet_mode = helmet_mode
	helmet[e].min_zoom = min_zoom
	helmet[e].max_zoom = max_zoom
	helmet[e].zoom_speed = zoom_speed
	helmet[e].zoom_readout_x = zoom_readout_x
	helmet[e].zoom_readout_y = zoom_readout_y
	helmet[e].compass = compass or 2
	helmet[e].compass_position = compass_position
	helmet[e].screen_image = imagefile or screen_image
	helmet[e].nightvision_color = nightvision_color or 1
	helmet[e].nightvision_ambience_r = nightvision_ambience_r
	helmet[e].nightvision_ambience_g = nightvision_ambience_g
	helmet[e].nightvision_ambience_b = nightvision_ambience_b
	helmet[e].nightvision_intensity = nightvision_intensity
	helmet[e].nightvision_fog_r = nightvision_fog_r
	helmet[e].nightvision_fog_g = nightvision_fog_g
	helmet[e].nightvision_fog_b = nightvision_fog_b
	helmet[e].nightvision_fog_nearest = nightvision_fog_nearest
	helmet[e].nightvision_fog_distance = nightvision_fog_distance
	helmet[e].toxic_protection = toxic_protection or 0
end

function helmet_init(e)
	helmet[e] = {}
	helmet[e].pickup_text = "E to Pickup/Wear"
	helmet[e].pickup_range = 80
	helmet[e].usage_text = "Hold B + Wheel to zoom, N=Nightvision ON/OFF, H=Remove/Wear Helmet"
	helmet[e].helmet_mode = 1
	helmet[e].min_zoom = -10
	helmet[e].max_zoom = 30
	helmet[e].zoom_speed = 1
	helmet[e].zoom_readout_x = 50
	helmet[e].zoom_readout_y = 10
	helmet[e].compass = 2
	helmet[e].compass_position = 1
	helmet[e].screen_image = "imagebank\\misc\\testimages\\helmethud1.png"
	helmet[e].nightvision_color = nightvision_color or 1
	helmet[e].nightvision_ambience_r = 0
	helmet[e].nightvision_ambience_g = 180
	helmet[e].nightvision_ambience_b = 0
	helmet[e].nightvision_intensity = 120
	helmet[e].nightvision_fog_r = 0
	helmet[e].nightvision_fog_g = 255
	helmet[e].nightvision_fog_b = 0
	helmet[e].nightvision_fog_nearest = 0
	helmet[e].nightvision_fog_distance = 50	
	helmet[e].toxic_protection = 0

	have_helmet[e] = 0
	g_have_helmet = 0
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
	doloop[e] = 0
	g_gasmask_on = 0
	g_radsuit_on = 0	
	currenthealth[e] = 0
	keypause1[e] = math.huge
	keypause2[e] = math.huge
	default_AmbienceRed = GetAmbienceRed()
	default_AmbienceBlue = GetAmbienceBlue()
	default_AmbienceGreen = GetAmbienceGreen()
	default_AmbienceIntensity = GetAmbienceIntensity()
	default_FogRed = GetFogRed()
	default_FogGreen = GetFogGreen()
	default_FogBlue = GetFogBlue()
	default_FogNearest = GetFogNearest()
	default_FogDistance = GetFogDistance()
end

function helmet_main(e)

	if status[e] == "init" then
		helmetsp[e] = CreateSprite(LoadImage(helmet[e].screen_image))
		SetSpriteSize(helmetsp[e],100,100)
		SetSpriteDepth(helmetsp[e],100)
		SetSpritePosition(helmetsp[e],1000,1000)
		compass_pos = helmet[e].compass_position
		mod = g_PlayerFOV
		fov = g_PlayerFOV
		current_fov[e] = fov
		currenthealth[e] = g_PlayerHealth
		nv_ambience_r[e] = helmet[e].nightvision_ambience_r
		nv_ambience_g[e] = helmet[e].nightvision_ambience_g
		nv_ambience_b[e] = helmet[e].nightvision_ambience_b
		nv_intensity[e] = helmet[e].nightvision_intensity
		nv_fog_r[e] = helmet[e].nightvision_fog_r
		nv_fog_g[e] = helmet[e].nightvision_fog_g
		nv_fog_b[e] = helmet[e].nightvision_fog_b
		nv_fog_near[e] = helmet[e].nightvision_fog_nearest*10000
		nv_fog_dist[e] = helmet[e].nightvision_fog_distance*10000
		have_helmet[e] = 0
		if helmet[e].helmet_mode == 1 then have_helmet[e] = 0 end
		if helmet[e].helmet_mode == 3 then have_helmet[e] = 1 end
		keypause1[e] = g_Time + 1000
		keypause2[e] = g_Time + 1000
		status[e] = "endinit"
	end

	PlayerDist = GetPlayerDistance(e)
	if fov == nil then fov = g_PlayerFOV end
	
	if have_helmet[e] == 0 then
		g_have_helmet = have_helmet[e]
		if helmet[e].helmet_mode == 1 or helmet[e].helmet_mode == 2 then
			if PlayerDist < helmet[e].pickup_range and g_PlayerHealth > 0 and have_helmet[e] == 0 then
				PromptLocal(e,helmet[e].pickup_text)
				if g_KeyPressE == 1 then
					have_helmet[e] = 1
					if GetEntityCollectable(e) == 1 then SetEntityCollected(e,1) end
					PlaySound(e,0)
					LoopNon3DSound(e,1)
					Hide(e)
					CollisionOff(e)
					SetPosition(e,g_PlayerPosX,g_PlayerPosY+500,g_PlayerPosZ)
					ActivateIfUsed(e)
				end
			end
		end
	end
	if have_helmet[e] == 1 then
		g_have_helmet = have_helmet[e]
		if hmswitch[e] == 0 then
			if helmet[e].toxic_protection == 1 then
				g_gasmask_on = 1
				g_radsuit_on = 1
			end	
			ResetPosition(e,g_PlayerPosX,g_PlayerPosY+500,g_PlayerPosZ)
			PasteSpritePosition(helmetsp[e],0,0)
			TextCenterOnXColor(50,95,2,helmet[e].usage_text,100,255,100)
			if doloop[e] == 0 then
				LoopNon3DSound(e,1)
				doloop[e] = 1
			end
			if helmet[e].compass == 1 then
				if have_helmet[e] == 1 then
					show_compass()
				end
			end
		end
		if hmswitch[e] == 1 then
			if helmet[e].toxic_protection == 1 then
				g_gasmask_on = 0
				g_radsuit_on = 0
			end
			ResetPosition(e,g_PlayerPosX,g_PlayerPosY+500,g_PlayerPosZ)
			if doloop[e] == 1 then
				StopSound(e,1)
				doloop[e] = 0
			end
		end

		if g_Scancode == 48 then --Hold B Key to use
			if g_PlayerGunID > 0 then
				SetPlayerWeapons(0)
				gunstatus[e] = 1
			end
			if g_MouseWheel < 0 then
				mod = mod - helmet[e].zoom_speed
			elseif g_MouseWheel > 0 then
				mod = mod + helmet[e].zoom_speed
			end
			if mod < helmet[e].min_zoom then
				mod = helmet[e].min_zoom
			elseif mod > helmet[e].max_zoom then
				mod = helmet[e].max_zoom
			end
			SetPlayerFOV(fov-mod)
			current_fov[e] = (fov-mod)
			TextCenterOnX(helmet[e].zoom_readout_x,helmet[e].zoom_readout_y,3,"Magnification Factor: " ..mod)
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
				if helmet[e].nightvision_color == 1 then
					SetAmbienceRed(nv_ambience_r[e])
					SetAmbienceGreen(nv_ambience_g[e])
					SetAmbienceBlue(nv_ambience_b[e])
					SetAmbienceIntensity(nv_intensity[e])
					SetFogRed(nv_fog_r[e])
					SetFogGreen(nv_fog_g[e])
					SetFogBlue(nv_fog_b[e])
					SetFogNearest(nv_fog_near[e])
					SetFogDistance(nv_fog_dist[e])
				end	
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
				SetFogNearest(default_FogNearest)
				SetFogDistance(default_FogDistance)
				keypause1[e] = g_Time + 1000
				nvswitch[e] = 0
			end
		end
		if helmet[e].helmet_mode == 1 then
			if GetInKey() == "h" or GetInKey() == "H" then
				local ox,oy,oz = U.Rotate3D( 0, 60, 0, math.rad( g_PlayerAngX ), math.rad( g_PlayerAngY ), math.rad( g_PlayerAngZ ) )
				local forwardposx, forwardposy, forwardposz = g_PlayerPosX + ox, g_PlayerPosY + oy, g_PlayerPosZ + oz
				ResetPosition(e,forwardposx, forwardposy, forwardposz)
				SetRotation(e,0, -90, 0)
				Show(e)
				SetAmbienceRed(default_AmbienceRed)
				SetAmbienceBlue(default_AmbienceBlue)
				SetAmbienceGreen(default_AmbienceGreen)
				SetAmbienceIntensity(default_AmbienceIntensity)
				SetFogRed(default_FogRed)
				SetFogGreen(default_FogGreen)
				SetFogBlue(default_FogBlue)
				SetFogNearest(default_FogNearest)
				SetFogDistance(default_FogDistance)				
				PlaySound(e,0)
				StopSound(e,1)
				nvswitch[e] = 0
				keypause1[e] = g_Time + 1000
				have_helmet[e] = 0
				g_have_helmet = have_helmet[e]
				if helmet[e].toxic_protection == 1 then
					g_gasmask_on = 1
					g_radsuit_on = 1
					if g_toxiczone == 'gas' then SetPlayerHealth(currenthealth[e]) end
					if g_toxiczone == 'radiation' then SetPlayerHealth(currenthealth[e]) end
				end
				SetPlayerFOV(fov)
			end
		end
		if helmet[e].helmet_mode == 2 then --reuseable
			ResetPosition(e,g_PlayerPosX,g_PlayerPosY+500,g_PlayerPosZ)
			if g_Time > keypause2[e] and hmswitch[e] == 0 then
				if GetInKey() == "h" or GetInKey() == "H" and hmswitch[e] == 0 then
					Hide(e)
					SetAmbienceRed(default_AmbienceRed)
					SetAmbienceBlue(default_AmbienceBlue)
					SetAmbienceGreen(default_AmbienceGreen)
					SetAmbienceIntensity(default_AmbienceIntensity)
					SetFogRed(default_FogRed)
					SetFogGreen(default_FogGreen)
					SetFogBlue(default_FogBlue)
					SetFogNearest(default_FogNearest)
					SetFogDistance(default_FogDistance)					
					PlaySound(e,0)
					nvswitch[e] = 0
					keypause2[e] = g_Time + 1000
					hmswitch[e] = 1
					StopSound(e,1)
					if helmet[e].toxic_protection == 1 then
						g_gasmask_on = 1
						g_radsuit_on = 1
						if g_toxiczone == 'gas' then SetPlayerHealth(currenthealth[e]) end
						if g_toxiczone == 'radiation' then SetPlayerHealth(currenthealth[e]) end
					end				
					SetPlayerFOV(fov)
				end
			end
			if g_Time > keypause2[e] and hmswitch[e] == 1 then
				if GetInKey() == "h" or GetInKey() == "H" and hmswitch[e] == 1 then
					Hide(e)
					SetAmbienceRed(default_AmbienceRed)
					SetAmbienceBlue(default_AmbienceBlue)
					SetAmbienceGreen(default_AmbienceGreen)
					SetAmbienceIntensity(default_AmbienceIntensity)
					SetFogRed(default_FogRed)
					SetFogGreen(default_FogGreen)
					SetFogBlue(default_FogBlue)
					SetFogNearest(default_FogNearest)
					SetFogDistance(default_FogDistance)						
					PlaySound(e,0)
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
