-- Security Cam Monitor v30  by Necrym59
-- DESCRIPTION: Will give the player access to a Security Camera Monitor? Always active ON
-- DESCRIPTION: [@MONITOR_MODE=1(1=Static, 2=Mobile)]
-- DESCRIPTION: [ATTACHED_TO$=""]
-- DESCRIPTION: [ACTIVATION_TEXT$="Press E to use camera monitor"]
-- DESCRIPTION: [USEAGE_TEXT$="Press Q to exit, MouseWheel to Zoom"]
-- DESCRIPTION: [USEAGE_RANGE=50(1,500)]
-- DESCRIPTION: [@CAMERA_HUD=1(1=In-built, 2=Custom Image, 3=Hud Screen, 5=None)]
-- DESCRIPTION: [HUD_SCREEN$="HUD Screen ?"]
-- DESCRIPTION: [IMAGEFILE$="imagebank\\misc\\testimages\\camerahud01.png"]
-- DESCRIPTION: [@CAMERA_FEED_ANGLE=1(1=Forward, 2=15-Degrees Down, 3=45-Degrees Down, 4=90-Degrees Down, 5=90-Degrees Up, 6=Backward)]
-- DESCRIPTION: [ANGLE_CYCLE_KEY!=0] allows R to cycle
-- DESCRIPTION: [#CAMERA_FEED_Y=20(-1000,1000)]
-- DESCRIPTION: [CAMERA_NAME$="Security Camera"]
-- DESCRIPTION: [CAMERA_TARGET_NAME$=""]
-- DESCRIPTION: [#CAMERA_TARGET_Z=0(-1000,1000)]
-- DESCRIPTION: Play <Sound0> when turned on/off.

local U = require "scriptbank\\utillib"
local P = require "scriptbank\\physlib"
local V = require "scriptbank\\vectlib"

local lower = string.lower
local upper = string.upper

local secmons = {}
local hud_image = "imagebank\\misc\\testimages\\camerahud01.png"

function secmon_properties(e, monitor_mode, attached_to, activation_text, useage_text, useage_range, camera_hud, hud_screen, imagefile, camera_feed_angle, angle_cycle_key, camera_feed_y, camera_name, camera_target_name, camera_target_z, camera_target_no, camera_number, attached_to_number)
	local secmon = secmons[e]
	secmon.monitor_mode			= monitor_mode
	secmon.attached_to			= lower(attached_to)
	secmon.activation_text 		= activation_text
	secmon.useage_text     		= useage_text
	secmon.useage_range 		= useage_range
	secmon.camera_hud 			= camera_hud
	secmon.hud_screen			= hud_screen
	secmon.hud_image			= imagefile
	secmon.camera_feed_angle 	= camera_feed_angle
	secmon.angle_cycle_key 		= angle_cycle_key
	secmon.camera_feed_y 		= camera_feed_y
	secmon.camera_name 			= lower(camera_name)
	secmon.camera_target_name 	= lower(camera_target_name)
	secmon.camera_target_z 		= camera_target_z	
	secmon.camera_target_no		= camera_target_no or 0
	secmon.camera_number 		= camera_number or 0
	secmon.attached_to_number	= attached_to_number or 0	
	if imagefile ~= hud_image then secmon.hud_image = imagefile	end
end

function secmon_init(e)
	secmons[e] =
	  { monitor_mode		= 1,
		attached_to			= "",
		activation_text		= "Press E to use camera monitor",
		useage_text			= "Press Q to exit, MouseWheel to Zoom",
		useage_range		= 0,
		camera_hud			= 1,
		hud_screen			= "",
		hud_image 			= hud_image,
		camera_feed_angle 	= 1,
		angle_cycle_key		= 0,
		camera_feed_y		= -1,
		camera_name			= "Security Camera",
		camera_target_name	= "",
		camera_target_no	= 0,
		camera_target_z		= 0,		
		camera_number 		= 0,
		attached_to_number	= 0,		
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
		if secmon.camera_target_no_name ~= "" then
			for n = 1, g_EntityElementMax do			
				if n ~= nil and g_Entity[n] ~= nil then										
					if lower(GetEntityName(n)) == secmon.camera_target_name then
						secmon.camera_target_no = n						
						break
					end			
				end				
			end
		end
		if secmon.attached_to ~="" and secmon.attached_to_number == 0 then
			for m, v in pairs(g_Entity) do
				if v ~= nil then
					if lower( GetEntityName(m) ) == secmon.attached_to then
						secmon.attached_to_number = m
						break
					end
				end
			end
		end
		secmon.status = "monitor"
	end

	if secmon.status == "monitor" then
		--- reposition with with entity
		if secmon.monitor_mode == 2 and secmon.attached_to_number ~= 0 then 
			GravityOff(e)
			CollisionOff(e)
			ResetPosition(e,g_Entity[secmon.attached_to_number]['x'], g_PlayerPosY+40,g_Entity[secmon.attached_to_number]['z'])
			Hide(e)
		end	
		-------------------------------		
		if U.PlayerCloserThan(e,secmon.useage_range) and secmon.monitor_mode == 1 then
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
		if GetPlayerDistance(e) < secmon.useage_range and secmon.monitor_mode == 2 then
			TextCenterOnX(50,5,3,"[Mobile Monitor Detected, Press INS to use]")
			if GetScancode() == 82 or GetScancode() == 210 then				
				if secmon.camera_number == 0 then Prompt("No camera connected") end
				if secmon.camera_number > 0 then
					PlaySound( e, 0 )
					secmon.wait = g_Time + 1000
					if secmon.camera_hud ~= 3 then HideHuds() end
					secmon.status = "camfeed"
				end
			end	
		end			
			
	elseif secmon.status == "camfeed" then
		if secmon.monitor_mode == 2 and secmon.angle_cycle_key == 0 then TextCenterOnX(50,5,3,"[Press DEL to exit]") end
		if secmon.monitor_mode == 2 and secmon.angle_cycle_key == 1 and secmon.camera_target_no == 0 then TextCenterOnX(50,5,3,"[Press DEL to exit, R to cycle camera angle]") end
		if secmon.monitor_mode == 2 and secmon.angle_cycle_key == 1 and secmon.camera_target_no > 0 then TextCenterOnX(50,5,3,"[Press DEL to exit monitor view]") end
		if secmon.camera_hud == 3 then
			ScreenToggle(secmon.hud_screen)	--HUDSCREEN
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
		if secmon.camera_target_no == 0 then			
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
				Ax = Ax - 90	
			elseif
			   secmon.camera_feed_angle == 6 then
				Ax = 0
				Ay = Ay + 180
			end
			secmon.seccamyang = 360 + Ay
			SetCameraPosition(0,x,y + secmon.camera_feed_y,z)
			SetCameraAngle(0,Ax,secmon.seccamyang,0)
			if secmon.angle_cycle_key == 0 and secmon.monitor_mode == 1 then Prompt(secmon.useage_text) end
			if secmon.angle_cycle_key == 1 then
				if secmon.monitor_mode == 1 then Prompt(secmon.useage_text.. ", R to cycle camera angle") end
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
		if secmon.camera_target_no > 0 then
			local dims = P.GetObjectDimensions(g_Entity[secmon.camera_target_no]['obj'])
			local midpoint = dims.h/2
			if secmon.monitor_mode == 1 then PointCamera(0, GetEntityPositionX(secmon.camera_target_no), GetEntityPositionY(secmon.camera_target_no)+dims.h/2, GetEntityPositionZ(secmon.camera_target_no)) end
			if secmon.monitor_mode == 2 then 
				local x,y,z,Ax,Ay,Az = GetEntityPosAng(secmon.camera_number)
				SetCameraPosition(0,x,y + secmon.camera_feed_y,z+secmon.camera_target_z)
				PointCamera(0, GetEntityPositionX(secmon.camera_target_no), GetEntityPositionY(secmon.camera_target_no)+dims.h/2, GetEntityPositionZ(secmon.camera_target_no))
			end
			Prompt("Observing Target  - " .. secmon.useage_text)
		end
		-- Show in-built or image overlay on camera -----------------------------------------
		if secmon.camera_hud == 1 then
			Text(5,5,3,"[")
			if secmon.camera_target_no == 0 then
				Text(5,5,3,"[    SCAN MODE")
			elseif
			   secmon.camera_target_no > 0 then
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
		if secmon.monitor_mode == 1 then
			if GetInKey() == "q" or GetInKey() == "Q" then
				secmon.status = "init"
				ScreenToggle("")
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
		end	
		if secmon.monitor_mode == 2 then
			if GetScancode() == 83 or GetScancode() == 211 then
				secmon.status = "init"
				ScreenToggle("")
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
		end		
		
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
