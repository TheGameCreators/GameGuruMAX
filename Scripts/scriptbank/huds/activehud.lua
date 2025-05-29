-- ActiveHud v4   by Necrym59
-- DESCRIPTION: Makes the In-Game Hud or a User Hud display based on a certain condition being met.
-- DESCRIPTION: Attach to an object and set AlwaysActive = ON
-- DESCRIPTION: [@SHOW_CONDITION=1(1=Home Key, 2=Health Damage, 3=Shooting, 4=Gun Zoom, 5=Scroll-Lock Toggle, 6=User Global+User Hud)]
-- DESCRIPTION: [SHOW_TIME=8(1,30)] Seconds
-- DESCRIPTION: [@@USER_GLOBAL$=""(0=globallist)] eg; MyGlobal
-- DESCRIPTION: [@@USER_HUD_SCREEN$=""(0=hudscreenlist)] eg; HUD Screen 9

g_liveHudScreen			= {}

local activehud			= {}
local show_condition	= {}
local show_time			= {}
local user_global		= {}
local user_hud_screen	= {}

local wait				= {}
local doonce			= {}
local curhealth			= {}
local toggle			= {}
local keypause			= {}
local status			= {}
local currentvalue		= {}
local initialvalue		= {}
	
function activehud_properties(e, show_condition, show_time, user_global, user_hud_screen)
	activehud[e] = g_Entity[e]
	activehud[e].show_condition = show_condition
	activehud[e].show_time = show_time
	activehud[e].user_global = user_global	
	activehud[e].user_hud_screen = user_hud_screen
end 

function activehud_init(e)
	activehud[e] = {}
	activehud[e].show_condition = 1
	activehud[e].show_time = 3
	activehud[e].user_global = ""
	activehud[e].user_hud_screen = ""	
	
	wait[e] = math.huge
	doonce[e] = 0
	toggle[e] = 0
	keypause[e] = math.huge
	currentvalue[e] = 0
	initialvalue[e] = 0
	g_liveHudScreen = 0
	status[e] = "init"
end

function activehud_main(e)
	activehud[e] = g_Entity[e]
	if status[e] == "init" then
		curhealth[e] = g_PlayerHealth
		keypause[e] = g_Time +1000
		HideHuds()
		if activehud[e].show_condition == 5 then
			ShowHuds()
			toggle[e] = 1
		end
		if activehud[e].show_condition == 6 then
			ShowHuds()
		end		
		status[e] = "endinit"
	end
	
	if activehud[e].show_condition == 1 then
		if g_Scancode == 199 then
			ShowHuds()
			wait[e] = g_Time + (activehud[e].show_time*1000)
		end
	end
	
	if activehud[e].show_condition == 2 then
		if g_PlayerHealth < curhealth[e] and doonce[e] == 0 then
			ShowHuds()
			curhealth[e] = g_PlayerHealth
			wait[e] = g_Time + (activehud[e].show_time*1000)
			doonce[e] = 1
		end
	end
	
	if activehud[e].show_condition == 3 then
		if GetGamePlayerStateFiringMode() == 1 and doonce[e] == 0 then
			ShowHuds()
			wait[e] = g_Time + (activehud[e].show_time*1000)
			doonce[e] = 1
		end
	end
	
	if activehud[e].show_condition == 4 then
		if GetGamePlayerStateGunZoomMode() > 0 and doonce[e] == 0 then
			ShowHuds()
			wait[e] = g_Time + (activehud[e].show_time*1000)
			doonce[e] = 1
		end
	end
	
	if activehud[e].show_condition == 5 then
		if g_Scancode == 70 and toggle[e] == 0 and g_Time > keypause[e] then
			ShowHuds()
			keypause[e] = g_Time + 1000
			toggle[e] = 1
		end
		if g_Scancode == 70 and toggle[e] == 1 and g_Time > keypause[e] then
			HideHuds()
			keypause[e] = g_Time + 1000
			toggle[e] = 0
		end
	end
	
	if activehud[e].show_condition == 6 and doonce[e] == 0 then
		if _G["g_UserGlobal['"..activehud[e].user_global.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..activehud[e].user_global.."']"] end
		if currentvalue[e] > initialvalue[e] then
			ScreenToggle(activehud[e].user_hud_screen)			
			wait[e] = g_Time + (activehud[e].show_time*1000)
			initialvalue[e] = currentvalue[e]
			g_liveHudScreen = 1
			doonce[e] = 1
		end
	end			

	if g_Time > wait[e] then
		ScreenToggle("")
		g_liveHudScreen = 0
		HideHuds()
		if activehud[e].show_condition == 6 then ShowHuds() end
		doonce[e] = 0
	end
end
