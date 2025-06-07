-- ActiveHud v7 by Necrym59 and Lee
-- DESCRIPTION: Provides an alternative User Hud display based on a certain show condition being met.
-- DESCRIPTION: Attach to an object and set AlwaysActive = ON
-- DESCRIPTION: [@SHOW_CONDITION=1(1=Always On, 2=Activation Key, 3=Health Damage, 4=Shooting, 5=Gun Zoom, 6=ScrollLock Toggle, 7=User Global)]
-- DESCRIPTION: [SHOW_TIME=4(1,30)] Seconds
-- DESCRIPTION: [@@USER_GLOBAL$=""(0=globallist)] eg; MyGlobal
-- DESCRIPTION: [@@USER_HUD_SCREEN$="In-Game HUD"(0=hudscreenlist)] eg; In-Game HUD 2
-- DESCRIPTION: [ACTIVATION_KEY$="Z"] Bound key to activate

g_liveHudScreen			= {}
local lower = string.lower
local upper = string.upper
local activehud			= {}
local wait				= {}
local waitreset			= {}
local doonce			= {}
local curhealth			= {}
local toggle			= {}
local keypause			= {}
local status			= {}
local currentvalue		= {}
local initialvalue		= {}
	
function activehud_properties(e, show_condition, show_time, user_global, user_hud_screen, activation_key)
	activehud[e].show_condition = show_condition or 1
	activehud[e].show_time = show_time or 4
	activehud[e].user_global = user_global or ""
	activehud[e].user_hud_screen = user_hud_screen or "In-Game HUD"
	activehud[e].activation_key = activation_key or "Z"
end 

function activehud_init(e)
	activehud[e] = {}
	activehud[e].show_condition = 1
	activehud[e].show_time = 4
	activehud[e].user_global = ""
	activehud[e].user_hud_screen = "In-Game HUD"	
	activehud[e].activation_key = "Z"
	wait[e] = math.huge
	waitreset[e] = 0
	doonce[e] = 0
	toggle[e] = 0
	keypause[e] = math.huge
	currentvalue[e] = 0
	initialvalue[e] = 0
	g_liveHudScreen = 0
	status[e] = "init"
end

function activehud_main(e)
	if status[e] == "init" then
		curhealth[e] = g_PlayerHealth
		keypause[e] = g_Time +1000
		toggle[e] = 0
		if activehud[e].show_condition == 6 and activehud[e].user_hud_screen == "In-Game HUD" then
			toggle[e] = 1
		end
		status[e] = "endinit"
	end
	
	if activehud[e].show_condition == 1 then  --Always On - use Specified User Hud Screen
		if doonce[e] == 0 and activehud[e].user_hud_screen > "" then
			if activehud[e].user_hud_screen == "In-Game HUD" then
				-- Showing In-Game HUD
			else
				ScreenToggle(activehud[e].user_hud_screen)
				g_liveHudScreen = 1
			end
			doonce[e] = 1
		end
	end
	
	if activehud[e].show_condition == 2 then  --Activation Key Timed
		if g_InKey == string.lower(activehud[e].activation_key) or g_InKey == tostring(activehud[e].activation_key) and activehud[e].user_hud_screen > "" then
			if activehud[e].user_hud_screen == "In-Game HUD" then
				-- Showing In-Game HUD
			else
				ScreenToggle(activehud[e].user_hud_screen)
				g_liveHudScreen = 1
			end
			wait[e] = g_Time + (activehud[e].show_time*1000)
			waitreset[e] = 1
			DisableBoundHudKeys()
		end
	end
	
	if activehud[e].show_condition == 3 then  --Health Damage
		if g_PlayerHealth < curhealth[e] and doonce[e] == 0 and activehud[e].user_hud_screen > "" then
			if activehud[e].user_hud_screen == "In-Game HUD" then
				-- Showing In-Game HUD
			else
				ScreenToggle(activehud[e].user_hud_screen)
				g_liveHudScreen = 1	
				DisableBoundHudKeys()				
			end
			curhealth[e] = g_PlayerHealth
			wait[e] = g_Time + (activehud[e].show_time*1000)
			waitreset[e] = 1
			doonce[e] = 1
		end
	end
	
	if activehud[e].show_condition == 4 then  --Shooting
		if GetGamePlayerStateFiringMode() == 1 and doonce[e] == 0 and activehud[e].user_hud_screen > "" then
			if activehud[e].user_hud_screen == "In-Game HUD" then
				-- Showing In-Game HUD
			else
				ScreenToggle(activehud[e].user_hud_screen)
				g_liveHudScreen = 1
				DisableBoundHudKeys()
			end
			wait[e] = g_Time + (activehud[e].show_time*1000)
			waitreset[e] = 1
			doonce[e] = 1
		end
	end
	
	if activehud[e].show_condition == 5 then  --Gun Zoom
		if GetGamePlayerStateGunZoomMode() > 0 and doonce[e] == 0 and activehud[e].user_hud_screen > "" then
			if activehud[e].user_hud_screen == "In-Game HUD" then
				-- Showing In-Game HUD
			else
				ScreenToggle(activehud[e].user_hud_screen)
				g_liveHudScreen = 1
				DisableBoundHudKeys()
			end
			wait[e] = g_Time + (activehud[e].show_time*1000)
			waitreset[e] = 1
			doonce[e] = 1
		end
	end

	if activehud[e].show_condition == 6 then  --ScrollLock Key Toggle
		if g_Scancode == 70 and toggle[e] == 0 and g_Time > keypause[e] and activehud[e].user_hud_screen > "" then
			if activehud[e].user_hud_screen == "In-Game HUD" then
				-- Showing In-Game HUD
			else
				ScreenToggle(activehud[e].user_hud_screen)
				g_liveHudScreen = 1
				DisableBoundHudKeys()
			end
			keypause[e] = g_Time + 1000
			toggle[e] = 1
		end
		if g_Scancode == 70 and toggle[e] == 1 and g_Time > keypause[e] and activehud[e].user_hud_screen > "" then
			if activehud[e].user_hud_screen == "In-Game HUD" then
				-- Showing In-Game HUD
			else
				ScreenToggle("")
				g_liveHudScreen = 0
				EnableBoundHudKeys()
			end		
			keypause[e] = g_Time + 1000
			toggle[e] = 0
		end
	end
	
	if activehud[e].show_condition == 7 and doonce[e] == 0 and activehud[e].user_hud_screen > "" then  --User Global
		if _G["g_UserGlobal['"..activehud[e].user_global.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..activehud[e].user_global.."']"] end
		if currentvalue[e] > initialvalue[e] then
			if activehud[e].user_hud_screen == "In-Game HUD" then
				-- Showing In-Game HUD
			else
				ScreenToggle(activehud[e].user_hud_screen)
				g_liveHudScreen = 1
			end		
			wait[e] = g_Time + (activehud[e].show_time*1000)
			waitreset[e] = 1
			initialvalue[e] = currentvalue[e]
			doonce[e] = 1
		end
	end			

	if g_Time > wait[e] and waitreset[e] == 1 then
		ScreenToggle("")
		g_liveHudScreen = 0
		EnableBoundHudKeys()
		doonce[e] = 0
		waitreset[e] = 0
	end
end
