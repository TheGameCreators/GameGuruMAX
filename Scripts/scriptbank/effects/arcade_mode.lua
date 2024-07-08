-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Arcade_Mode Script v5 by Necrym59
-- DESCRIPTION: Global behavior to create arcade mode gameplay? Attach to an object. Set AlwaysActive=ON.
-- DESCRIPTION: [#Z_MOVE_SPEED=5(0,200)] the players forward auto-movement speed
-- DESCRIPTION: [#X_MOVE_RANGE=100(0,500)] the players lateral movement range
-- DESCRIPTION: [HORIZONTAL_VIEW_LIMIT=45(1,180)] the players horizontal view angle range limit
-- DESCRIPTION: [VERTICAL_VIEW_LIMIT=45(1,90)] the players vertical view angle range limit
-- DESCRIPTION: [ALLOW_RUNNING!=0]
-- DESCRIPTION: [ALLOW_JUMPING!=0]
-- DESCRIPTION: [ALLOW_REVERSE!=0]
-- DESCRIPTION: [HIDE_WEAPON!=0]
-- DESCRIPTION: [HIDE_WEAPONFIRE!=0]
-- DESCRIPTION: [USE_CROSSHAIR!=0]
-- DESCRIPTION: [CROSSHAIR_IMAGEFILE$="imagebank\\crosshairs\\crosshair.dds"]

local arcade				= {}
local z_move_speed			= {}
local x_move_range			= {}
local horizontal_view_limit	= {}
local vertical_view_limit	= {}
local allow_running			= {}
local allow_jumping			= {}
local allow_reverse			= {}
local hide_weapon			= {}
local hide_weaponfire		= {}
local use_crosshair			= {}
local crosshair				= {}
local y_direction			= {}

local status 		= {}
local startposx		= {}
local startposy		= {}
local startposz		= {}
local minxmovement	= {}
local maxxmovement	= {}
local am_crosshair	= {}
local am_imgwidth1	= {}
local am_imgheight1	= {}

function arcade_mode_properties(e, z_move_speed, x_move_range, horizontal_view_limit, vertical_view_limit, allow_running, allow_jumping, allow_reverse, hide_weapon, hide_weaponfire, use_crosshair, crosshair_imagefile)
	arcade[e].z_move_speed = z_move_speed	
	arcade[e].x_move_range = x_move_range	
	arcade[e].horizontal_view_limit = horizontal_view_limit
	arcade[e].vertical_view_limit = vertical_view_limit
	arcade[e].allow_running	= allow_running or 0
	arcade[e].allow_jumping	= allow_jumping or 0
	arcade[e].allow_reverse = allow_reverse or 0
	arcade[e].hide_weapon = hide_weapon or 0
	arcade[e].hide_weaponfire = hide_weaponfire or 0
	arcade[e].use_crosshair = use_crosshair or 0
	arcade[e].crosshair = crosshair_imagefile
	arcade[e].y_direction = 0	
end

function arcade_mode_init(e)
	arcade[e] = {}	
	arcade[e].z_move_speed = 1
	arcade[e].x_move_range = 100	
	arcade[e].horizontal_view_limit = 45
	arcade[e].vertical_view_limit = 45
	arcade[e].allow_running	= 0
	arcade[e].allow_jumping	= 0
	arcade[e].allow_reverse = 0	
	arcade[e].hide_weapon = 0
	arcade[e].hide_weaponfire = 0
	arcade[e].use_crosshair = 0	
	arcade[e].crosshair = ""
	arcade[e].y_direction = 0	
	status[e] = "init"
end

function arcade_mode_main(e)
	if status[e] == "init" then		
		if arcade[e].use_crosshair == 1 then 
			if arcade[e].crosshair > "" then
				am_crosshair = CreateSprite(LoadImage(arcade[e].crosshair))
				am_imgwidth1 = GetImageWidth(LoadImage(arcade[e].crosshair))
				am_imgheight1 = GetImageHeight(LoadImage(arcade[e].crosshair))
				SetSpriteSize(am_crosshair,-1,-1)
				SetSpriteDepth(am_crosshair,100)
				SetSpritePosition(am_crosshair,50,50)
				SetSpriteOffset(am_crosshair,am_imgwidth1/2.0, am_imgheight1/2.0)
				SetSpriteColor(sp_crosshair,255,255,255,200)			
			end
		end
		startposx[e] = g_PlayerPosX
		startposy[e] = g_PlayerPosY
		startposz[e] = g_PlayerPosZ
		minxmovement[e] = startposx[e] - arcade[e].x_move_range
		maxxmovement[e] = startposx[e] + arcade[e].x_move_range
		SetGamePlayerControlFinalCameraAngley(arcade[e].y_direction)
		SetFlashLightKeyEnabled(0)
		if arcade[e].hide_weapon == 1 then
			SetWeaponArmsVisible(1,0,0)
			SetGamePlayerStateRightMouseHold(2)
		end
		if arcade[e].hide_weaponfire == 1 then
			if arcade[e].hide_weapon == 0 then SetWeaponArmsVisible(0,1,1) end
			if arcade[e].hide_weapon == 1 then SetWeaponArmsVisible(1,1,1) end
			SetGamePlayerStateRightMouseHold(2)
		end
		status[e] = "endinit"		
	end
	
	SetGamePlayerControlWobble(0)
	if arcade[e].hide_weapon == 1 then SetGamePlayerStateRightMouseHold(2) end
	if arcade[e].allow_running == 0 then SetGamePlayerControlCanRun(0) end
	if arcade[e].allow_jumping == 0 then SetGamePlayerControlJumpMode(1) end
	if arcade[e].allow_reverse == 0	then
		if g_PlrKeyS == 1 then
			SetFreezePosition(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ+0.2)
			TransportToFreezePositionOnly()	
		end
	end
	--Set Lateral View Limit
	if GetGamePlayerControlFinalCameraAngley()>arcade[e].horizontal_view_limit+arcade[e].y_direction then SetGamePlayerControlFinalCameraAngley(arcade[e].horizontal_view_limit+arcade[e].y_direction) end
	if GetGamePlayerControlFinalCameraAngley()<-arcade[e].horizontal_view_limit+arcade[e].y_direction then SetGamePlayerControlFinalCameraAngley(-arcade[e].horizontal_view_limit+arcade[e].y_direction) end
	--Set Vertical View Limit
	if (GetGamePlayerStateCamAngleX()<-arcade[e].vertical_view_limit) then SetGamePlayerStateCamAngleX(-arcade[e].vertical_view_limit) end
	if (GetGamePlayerStateCamAngleX()>arcade[e].vertical_view_limit) then SetGamePlayerStateCamAngleX(arcade[e].vertical_view_limit) end		
	
	if arcade[e].z_move_speed > 0 then
		ForcePlayer(arcade[e].y_direction,arcade[e].z_move_speed/100)		
	end
	if arcade[e].z_move_speed <= 0 then
		SetFreezePosition(g_PlayerPosX,startposy[e],startposz[e])
		TransportToFreezePositionOnly()
	end	
	if arcade[e].x_move_range <= 0 and arcade[e].z_move_speed <= 0 then
		SetFreezePosition(startposx[e],startposy[e],startposz[e])
		TransportToFreezePositionOnly()
	end
	if arcade[e].x_move_range <= 0 and arcade[e].z_move_speed > 0 then
		SetFreezePosition(startposx[e],g_PlayerPosY,g_PlayerPosZ)
		TransportToFreezePositionOnly()
	end
	if arcade[e].x_move_range > 0 then	
		if g_PlayerPosX >= maxxmovement[e] then
			SetFreezePosition(maxxmovement[e]-0.01,g_PlayerPosY,g_PlayerPosZ)
			TransportToFreezePositionOnly()
		end
		if g_PlayerPosX <= minxmovement[e] then
			SetFreezePosition(minxmovement[e]+0.01,g_PlayerPosY,g_PlayerPosZ)
			TransportToFreezePositionOnly()
		end
	end
end
	
function arcade_mode_exit(e)
end