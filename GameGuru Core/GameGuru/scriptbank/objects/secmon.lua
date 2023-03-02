-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Security Cam Monitor v26  by Necrym59 - special thanks to OldFlak
-- DESCRIPTION: Will give the player access to a Security Camera Monitor? Always active ON
-- DESCRIPTION: [ACTIVATION_TEXT$="Press E to use camera monitor"]
-- DESCRIPTION: [USEAGE_TEXT$="Press Q to exit, MouseWheel to Zoom"]
-- DESCRIPTION: [USEAGE_RANGE=50(1,100)]
-- DESCRIPTION: [@CAMERA_HUD=1(1=In-built, 2=Custom Image, 3=Game Hud, 4=Variable Hud, 5=None)]
-- DESCRIPTION: [HUD_RESTORE!=1]
-- DESCRIPTION: [HUD_VARIABLE_NAME$="g_MYGAME_Hud"]
-- DESCRIPTION: [HUD_VARIABLE_VALUE=0]
-- DESCRIPTION: [IMAGEFILE$="imagebank\\misc\\testimages\\camerahud01.png"]
-- DESCRIPTION: [@CAMERA_FEED_ANGLE=1(1=Forward, 2=15 Degrees, 3=45 Degrees, 4=90 Degrees, 5=Backward)]
-- DESCRIPTION: [ANGLE_CYCLE_KEY!=0] allows R to cycle
-- DESCRIPTION: [#CAMERA_FEED_Y=20(-100,100)]
-- DESCRIPTION: [CAMERA_NAME$="Security Camera"]
-- DESCRIPTION: [CAMERA_TARGET=0(0,100)]
-- DESCRIPTION: Play <Sound0> when turned on/off.

local U = require "scriptbank\\utillib"
local V = require "scriptbank\\vectlib"

local lower = string.lower
local upper = string.upper

local secmons = {}
local hud_image = "imagebank\\misc\\testimages\\camerahud01.png"

function secmon_properties(e, activation_text, useage_text, useage_range, camera_hud, hud_restore, hud_variable_name, hud_variable_value, imagefile, camera_feed_angle, angle_cycle_key, camera_feed_y, camera_name, camera_target, camera_number)
	local secmon = secmons[e]
	secmon.activation_text 		= activation_text
	secmon.useage_text     		= useage_text
	secmon.useage_range 		= useage_range
	secmon.camera_hud 			= camera_hud
	secmon.hud_restore 			= hud_restore
	secmon.hud_variable_name 	= hud_variable_name
	secmon.hud_variable_value 	= hud_variable_value
	secmon.camera_feed_angle 	= camera_feed_angle
	secmon.angle_cycle_key 		= angle_cycle_key
	secmon.camera_feed_y 		= camera_feed_y
	secmon.camera_name 			= lower( camera_name )
	secmon.camera_target 		= camera_target
	secmon.camera_number 		= camera_number or 0

	if imagefile ~= hud_image then secmon.hud_image = imagefile	end
end

function secmon_init(e)
	secmons[e] =
	  { activation_text		= "Press E to use camera monitor",
		useage_text			= "Press Q to exit, MouseWheel to Zoom",
		useage_range		= 0,
		camera_hud			= 1,
		hud_restore 		= 1,
		hud_variable_name 	= "g_MYGAME_Hud",
		hud_variable_value 	= 0,
		hud_image 			= hud_image,
		camera_feed_angle 	= 1,
		angle_cycle_key		= 0,
		camera_feed_y		= -1,
		camera_name			= "Security Camera",
		camera_target 		= 0,
		camera_number 		= 0,
		cam_feed 			= 0,
		gunstatus 			= 0,
		status				= "init",
		wait 				= math.huge,
		feedswitch			= 0,
		seccamxpos			= 0,
		seccamypos			= 0,
		seccamzpos			= 0,
		seccamyang			= 0,
		current_hvv			= 0,
		mod 				= 0,
		fov 				= g_PlayerFOV,
		doonce              = true
	  }
	StartTimer(e)
end

function secmon_main(e)
	local secmon = secmons[e]
	if secmon == nil then return end

	if secmon.status == "init" then
		secmon.fov = g_PlayerFOV
		secmon.current_hvv = _G[secmon.hud_variable_name]
		if secmon.camera_hud == 2 then
			secmon.overlaysp = CreateSprite( LoadImage(secmon.hud_image))
			SetSpriteSize(secmon.overlaysp,100,100)
			SetSpritePosition(secmon.overlaysp,500,500)
		end
		if secmon.camera_number == 0 then
			for k, v in pairs(g_Entity) do
				if v ~= nil then
					if lower( GetEntityName(k) ) == secmon.camera_name then
						secmon.camera_number = k
						local x,y,z,Ax,Ay,Az = GetEntityPosAng(k)
						secmon.seccamxpos = x
						secmon.seccamypos = y
						secmon.seccamzpos = z
						secmon.seccamyang = Ay
						break
					end
				end
			end
		end
		secmon.status = "monitor"

	elseif secmon.status == "monitor" then
		if U.PlayerCloserThan(e,secmon.useage_range) and g_PlayerHealth > 0 and g_PlayerThirdPerson == 0 then
			local LookingAt = GetPlrLookingAtEx(e,1)
			if LookingAt == 1 then
				Prompt( secmon.activation_text )
				if g_KeyPressE == 1 then
					if secmon.camera_number == 0 then Prompt("No camera connected") end
					if secmon.camera_number > 0 then
						PlaySound( e, 0 )
						secmon.wait = g_Time + 1000
						if secmon.camera_hud ~= 3 then HideHuds() end
						secmon.status = "camfeed"
					end
				end
			end
		end

	elseif secmon.status == "camfeed" then
		
		if secmon.camera_hud == 4 then
			if secmon.hud_variable_value > 0 and secmon.hud_variable_name > "" then _G[secmon.hud_variable_name] = secmon.hud_variable_value end
		end
		secmon.last_gun = g_PlayerGunName
		if g_PlayerGunID > 0 then
			CurrentlyHeldWeaponID = GetPlayerWeaponID()
			SetPlayerWeapons(0)
			secmon.gunstatus = 1
		end
		if secmon.cam_feed == 0 then
			secmon.cam_feed = 1
			FreezePlayer()
			SetCameraOverride(3)
			StartTimer(e)
			SetCameraPosition(0,secmon.seccamxpos,secmon.seccamypos,secmon.seccamzpos)						
		end
		if secmon.camera_target == 0 then
			local x,y,z,Ax,Ay,Az = GetEntityPosAng(secmon.camera_number)
			if secmon.camera_feed_angle == 1 then
				Ax = 0
			elseif
			   secmon.camera_feed_angle == 2 then
				Ax = Ax + 15
			elseif
			   secmon.camera_feed_angle == 3 then
				Ax = Ax + 45
			elseif
			   secmon.camera_feed_angle == 4 then
				Ax = Ax + 90
			elseif
			   secmon.camera_feed_angle == 5 then
				Ax = 0
				Ay = Ay + 180
			end
			secmon.seccamyang = 360 + Ay
			SetCameraPosition(0,x,y + secmon.camera_feed_y,z)
			SetCameraAngle(0,Ax,secmon.seccamyang, 0 )
			Prompt( secmon.useage_text )
			if secmon.angle_cycle_key == 1 then
				if g_KeyPressR == 1 and secmon.feedswitch == 0 then
					secmon.wait = g_Time + 500
					secmon.camera_feed_angle = secmon.camera_feed_angle + 1
					if secmon.camera_feed_angle == 6 then secmon.camera_feed_angle = 1 end
					PlaySound(e,0)
					secmon.feedswitch = 1
				end
				if g_Time > secmon.wait then secmon.feedswitch = 0 end
			end
		end
		if secmon.camera_target > 0 then
			PointCamera(0, GetEntityPositionX(secmon.camera_target), GetEntityPositionY(secmon.camera_target), GetEntityPositionZ(secmon.camera_target))
			Prompt("Observing Target  - " .. secmon.useage_text)
		end
		-- Show in-built or image overlay on camera -----------------------------------------
		if secmon.camera_hud == 1 then
			Text(5,5,3,"[")
			if secmon.camera_target == 0 then
				Text(5,5,3,"[    SCAN MODE")
			elseif
			   secmon.camera_target > 0 then
				Text(5,5,3,"[    LOCKED MODE")
			end
			Text(5,95,3,"[    5600K   F4.8")
			TextCenterOnX(50,5,3,"[  " ..upper(secmon.camera_name).. "  ]")
			TextCenterOnX(50,50,3,"[ - ]")
			Text(95,5,3,"]")
			Text(95,95,3,"]")
		end
		if secmon.camera_hud == 2 then
			PasteSpritePosition(secmon.overlaysp,0,0)
		end
		-------------------------------------------------------------------------------------
		if GetInKey() == "q" or GetInKey() == "Q" then
			secmon.status = "init"
			if secmon.camera_hud == 4 then _G[secmon.hud_variable_name] = secmon.current_hvv end
			if secmon.hud_restore == 1 then ShowHuds() end
			SetCameraPosition(0,g_PlayerPosX,g_PlayerPosY+35,g_PlayerPosZ)
			SetCameraAngle(0,g_PlayerAngX,g_PlayerAngY,g_PlayerAngZ)
			secmon.mod = 0
			SetPlayerFOV(secmon.fov)
			UnFreezePlayer()
			SetCameraOverride(0)
			PlaySound(e,0)
			secmon.cam_feed = 0
			if secmon.gunstatus == 1 then
				ChangePlayerWeapon(secmon.last_gun)
				SetPlayerWeapons(1)
				ChangePlayerWeaponID(CurrentlyHeldWeaponID)
				secmon.gunstatus = 0
			end
		end
		
		if g_MouseWheel < 0 then secmon.mod = secmon.mod - 4 end		
		if g_MouseWheel > 0 then secmon.mod = secmon.mod + 4 end
		if secmon.mod <= 0 then secmon.mod = 0 end
		if secmon.mod > 60 then secmon.mod = 60 end
		SetPlayerFOV(secmon.fov-secmon.mod)
		
		if g_Time > secmon.wait then
			if secmon.doonce then
				SetActivatedWithMP(e,201)
				PerformLogicConnections(e)
				secmon.doonce = false
			end
		end
	else
		secmon.status = "init"
		secmon.cam_feed = 0
	end
end
