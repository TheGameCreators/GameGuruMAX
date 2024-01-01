-- Countdown v8 by Necrym59
-- DESCRIPTION: Countdown timer to count down to an end action.
-- DESCRIPTION: Attach to an object. Set Always Active ON. Trigger from a zone or switch.
-- DESCRIPTION: [#MAXIMUM_TIME=1.00(0.25,60.00)]
-- DESCRIPTION: [@TIME_DISPLAY=1(1=On, 2=Off)]
-- DESCRIPTION: [DISPLAY_TEXT$="Time Left:"]
-- DESCRIPTION: [DISPLAY_X=10]
-- DESCRIPTION: [DISPLAY_Y=10]
-- DESCRIPTION: [DISPLAY_SIZE=3(1,5)]
-- DESCRIPTION: [@END_ACTION=1(1=Terminate Player, 2= Hurt Player, 3=Activate Entities, 4=Lose Game, 5=Win Game)]
-- DESCRIPTION: [USER_GLOBAL_MODIFIER$="MyGlobalModifier"]
-- DESCRIPTION: <Sound0> for end sound

local countdown 	= {}
local maximum_time 	= {}
local time_display 	= {}
local display_text 	= {}
local display_x 	= {}
local display_y 	= {}
local display_size 	= {}
local end_action 	= {}
local user_global_modifier	= {}
local secondsleft 	= {}
local minutesleft 	= {}
local timeleftsec	= {}
local startcount 	= {}
local maxcount 		= {}
local played		= {}
local status		= {}
local wait			= {}
local currentvalue	= {}
local doonce		= {}

function countdown_properties(e,maximum_time, time_display, display_text, display_x, display_y, display_size, end_action, user_global_modifier)
	countdown[e] = g_Entity[e]
	countdown[e].maximum_time = maximum_time
	countdown[e].time_display = time_display
	countdown[e].display_text = display_text	
	countdown[e].display_x = display_x
	countdown[e].display_y = display_y
	countdown[e].display_size = display_size
	countdown[e].end_action = end_action
	countdown[e].user_global_modifier = user_global_modifier	
end

function countdown_init(e)
	countdown[e] = {}
	countdown[e].maximum_time = 10
	countdown[e].time_display = 1
	countdown[e].display_text = "Time Left:"
	countdown[e].display_x = 10
	countdown[e].display_y = 10
	countdown[e].display_size = 3
	countdown[e].end_action = 1
	countdown[e].user_global_modifier = ""
	
	startcount[e] = 0
	maxcount[e] = 0
	secondsleft[e] = 0
	minutesleft[e] = 0
	timeleftsec[e] = 0
	played[e] = 0
	doonce[e] = 0
	currentvalue[e] = 0	
	wait[e] = math.huge	
	status[e] = "init"
end
 
function countdown_main(e)
	if status[e] == "init" then
		startcount[e] = 0
		maxcount[e] = (countdown[e].maximum_time * 1000) * 60
		status[e] = "endinit"
	end
	
	if g_Entity[e]['activated'] == 1 then	
		if startcount[e] == 0 then
			StartTimer(e)
			startcount[e] = 1
		end
		if startcount[e] == 1 then
			if countdown[e].user_global_modifier > "" then
				if _G["g_UserGlobal['"..countdown[e].user_global_modifier.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..countdown[e].user_global_modifier.."']"] end
				if currentvalue[e] > 0 then
					maxcount[e] = (maxcount[e] + currentvalue[e] * 1000)
					StartTimer(e)
					startcount[e] = 1
					_G["g_UserGlobal['"..countdown[e].user_global_modifier.."']"] = 0					
				end	
			end
			secondsleft[e] = math.floor((maxcount[e]/1000)-GetTimer(e)/1000)
			minutesleft[e] = math.floor(((maxcount[e]/1000)/60)-((GetTimer(e)/1000)/60))
			timeleftsec[e] = secondsleft[e] - (minutesleft[e]*60)			
			if countdown[e].time_display == 1 then
				if countdown[e].display_x < 50 or countdown[e].display_x > 50 then
					if minutesleft[e] > 0 and secondsleft[e] > 9 then Text(countdown[e].display_x, countdown[e].display_y, countdown[e].display_size, countdown[e].display_text.. " " ..minutesleft[e].. " : " ..timeleftsec[e]) end
					if minutesleft[e] == 0 and secondsleft[e] > 9 then Text(countdown[e].display_x, countdown[e].display_y, countdown[e].display_size,countdown[e].display_text.. " 0 : " ..timeleftsec[e]) end
					if minutesleft[e] > 0 and secondsleft[e] < 10 then Text(countdown[e].display_x, countdown[e].display_y, countdown[e].display_size, countdown[e].display_text.. " " ..minutesleft[e].. " : 0" ..timeleftsec[e]) end
					if minutesleft[e] == 0 and secondsleft[e] < 10 then Text(countdown[e].display_x, countdown[e].display_y, countdown[e].display_size,countdown[e].display_text.. " 0 : 0" ..timeleftsec[e]) end
					
				end
				if countdown[e].display_x == 50 then
					if minutesleft[e] > 0 and secondsleft[e] > 9  then TextCenterOnX(countdown[e].display_x, countdown[e].display_y, countdown[e].display_size, countdown[e].display_text.. " " ..minutesleft[e].. " : " ..timeleftsec[e]) end
					if minutesleft[e] == 0 and secondsleft[e] > 9 then TextCenterOnX(countdown[e].display_x, countdown[e].display_y, countdown[e].display_size,countdown[e].display_text.. " 0 : " ..timeleftsec[e]) end
					if minutesleft[e] > 0 and secondsleft[e] < 10 then TextCenterOnX(countdown[e].display_x, countdown[e].display_y, countdown[e].display_size, countdown[e].display_text.. " " ..minutesleft[e].. " : 0" ..timeleftsec[e]) end
					if minutesleft[e] == 0 and secondsleft[e] < 10 then TextCenterOnX(countdown[e].display_x, countdown[e].display_y, countdown[e].display_size,countdown[e].display_text.. " 0 : 0" ..timeleftsec[e]) end
				end
			end
			if countdown[e].time_display == 2 then end
		end
		if secondsleft[e] == 0 then
			if played[e] == 0 then
				PlaySound(e,0)
				played[e] = 1
			end
			if countdown[e].end_action == 1 then
				HurtPlayer(e,g_PlayerHealth)
			end
			if countdown[e].end_action == 2 then
				HurtPlayer(e,g_PlayerHealth/3)
			end
			if countdown[e].end_action == 3 then
				if doonce[e] == 0 then
					ActivateIfUsed(e)
					PerformLogicConnections(e)
					doonce[e] = 1
				end
			end
			if countdown[e].end_action == 4 then
				LoseGame()
			end
			if countdown[e].end_action == 5 then
				WinGame()
			end
		end		
	end
	if g_Entity[e]['activated'] == 0 then
		status[e] = "init"
	end
end

function level_countdown_exit(e)
end

