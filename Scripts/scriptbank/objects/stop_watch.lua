-- Stop Watch v3 by Necrym59
-- DESCRIPTION: A time counter using a user global for display that when stopped can process an end action.
-- DESCRIPTION: Attach to an object. Set Always Active ON. Trigger Start/Stop from a zone or switch.
-- DESCRIPTION: [DISPLAY_USER_GLOBAL$=""] eg: MyTime
-- DESCRIPTION: [@END_ACTION=1(1=None, 2=Terminate Player, 3=Hurt Player, 4=Activate Entities, 5=Lose Game, 6=Win Game, 7=Display Hud Screen)]
-- DESCRIPTION: [@LAUNCH_WARNING=1(1=Off, 2=On)]
-- DESCRIPTION: [END_SCREEN$="HUD Screen #"]
-- DESCRIPTION: [@READOUT=1(1=Mins:Secs:MilliSecs, 2=Mins:Secs:Tenths, 3=Hrs:Mins:Secs:MilliSecs, 4=Hrs:Min:Sec:Tenths:MilliSecs)]
-- DESCRIPTION: <Sound0> for commence sound
-- DESCRIPTION: <Sound1> for end sound

local stop_watch 			= {}
local display_user_global	= {}
local end_action 			= {}
local launch_warning		= {}

local currenttm = {}
local millisec	= {}
local tenths	= {}
local seconds 	= {}
local minutes 	= {}
local hours		= {}

local startcount 	= {}
local played		= {}
local status		= {}
local wait			= {}
local launch_stage	= {}
local launch_count	= {}
local state			= {}
local doonce		= {}

function stop_watch_properties(e, display_user_global, end_action, launch_warning, end_screen, readout)
	stop_watch[e].display_user_global = display_user_global
	stop_watch[e].end_action = end_action
	stop_watch[e].launch_warning = launch_warning
	stop_watch[e].end_screen = end_screen
	stop_watch[e].readout = readout
end

function stop_watch_init(e)
	stop_watch[e] = {}
	stop_watch[e].display_user_global = ""
	stop_watch[e].end_action = 1
	stop_watch[e].launch_warning = 1
	stop_watch[e].end_screen = ""
	stop_watch[e].readout = 1
	
	startcount[e] = 0
	currenttm[e] = 0
	millisec[e] = 0
	tenths[e] = 0	
	seconds[e] = 0
	minutes[e] = 0
	hours[e] = 0
	played[e] = 0
	doonce[e] = 0
	launch_stage[e] = 0
	launch_count[e] = 600	
	wait[e] = math.huge	
	status[e] = "init"
	state[e] = ""
end
 
function stop_watch_main(e)
	if status[e] == "init" then
		_G["g_UserGlobal['"..stop_watch[e].display_user_global.."']"] = nil
		launch_stage[e] = 0
		if stop_watch[e].launch_warning == 1 then launch_stage[e] = 1 end
		startcount[e] = 0
		status[e] = "endinit"
		state[e] = "stopped"
	end

	if g_Entity[e]['activated'] == 1 then
		if state[e] == "stopped" then
			if stop_watch[e].launch_warning == 2 and launch_stage[e] == 0 then
				if launch_stage[e] == 0 then 
					if launch_count[e] > 400  then TextCenterOnX(50,50,5,"READY") end
					if launch_count[e] > 200 and launch_count[e] <= 400 then TextCenterOnX(50,50,5,"SET") end
					if launch_count[e] <= 200  then TextCenterOnX(50,50,5,"GO") end
					if launch_count[e] <= 100 then
						launch_stage[e] = 1
						if played[e] == 0 then
							PlaySound(e,0)
							played[e] = 1
						end
					end
					launch_count[e] = launch_count[e] - 1
				end
			end		
			if startcount[e] == 0 and launch_stage[e] == 1 then
				played[e] = 0
				StartTimer(e)
				startcount[e] = 1
				state[e] = "running"
			end
		end	
		if state[e] == "running" then
			if startcount[e] == 1 then
				currenttm[e] = GetTimer(e)/1000
				millisec[e] = millisec[e] + 1
				seconds[e] = currenttm[e]
				if millisec[e] > 99 then millisec[e] = 0 end
				if millisec[e] >= 99 then
					tenths[e] = tenths[e] + 1
					if tenths[e] >= 10 then tenths[e] = 0 end	
				end
				if seconds[e] >= 60 then
					minutes[e] = minutes[e] + 1
					seconds[e] = 0
					StartTimer(e)
				end
				if minutes[e] >= 60 then
					minutes[e] = 0
					hours[e] = hours[e] + 1					
				end
				if hours[e] == 25 then
					hours[e] = 0
				end
				if stop_watch[e].readout == 1 then 
					_G["g_UserGlobal['"..stop_watch[e].display_user_global.."']"] = string.format("%02d", minutes[e]).. ":" ..string.format("%02d", seconds[e]).. ":" ..string.format("%02d", millisec[e])
				end
				if stop_watch[e].readout == 2 then 
					_G["g_UserGlobal['"..stop_watch[e].display_user_global.."']"] = string.format("%02d", minutes[e]).. ":" ..string.format("%02d", seconds[e]).. ":" ..string.format("%02d", tenths[e])
				end
				if stop_watch[e].readout == 3 then 
					_G["g_UserGlobal['"..stop_watch[e].display_user_global.."']"] = string.format("%02d",hours[e]).. ":" ..string.format("%02d", minutes[e]).. ":" ..string.format("%02d", seconds[e]).. ":" ..string.format("%02d", millisec[e])
				end	
				if stop_watch[e].readout == 4 then 
					_G["g_UserGlobal['"..stop_watch[e].display_user_global.."']"] = string.format("%02d",hours[e]).. ":" ..string.format("%02d", minutes[e]).. ":" ..string.format("%02d", seconds[e]).. ":"  ..string.format("%02d", tenths[e]).. ":" ..string.format("%02d", millisec[e])
				end					
			end
		end		
	end
	if g_Entity[e]['activated'] == 0 and state[e] == "running" then
		if stop_watch[e].end_action == 1 then					
		end	
		if stop_watch[e].end_action == 2 then
			HurtPlayer(e,g_PlayerHealth)
		end
		if stop_watch[e].end_action == 3 then
			HurtPlayer(e,g_PlayerHealth/3)
		end
		if stop_watch[e].end_action == 4 then
			if doonce[e] == 0 then
				ActivateIfUsed(e)
				PerformLogicConnections(e)
				doonce[e] = 1
			end
		end
		if stop_watch[e].end_action == 5 then
			LoseGame()
		end
		if stop_watch[e].end_action == 6 then
			WinGame()
		end
		if stop_watch[e].end_action == 7 then
			ScreenToggle(stop_watch[e].end_screen)
		end
		state[e] = "stopped"
		SwitchScript(e,"no_behavior_selected.lua")
	end
end

function stop_watch_exit(e)
end

