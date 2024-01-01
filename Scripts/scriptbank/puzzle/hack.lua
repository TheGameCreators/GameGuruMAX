-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Hack v3 by Necrym59
-- DESCRIPTION: Will this will enable player to hack and activate a Logic Linked or ActivateIfUsed Entity? 
-- DESCRIPTION: [USE_RANGE=80(1,100)]
-- DESCRIPTION: [USE_TEXT$="E to Hack"]
-- DESCRIPTION: [HACK_TIME=8(1,30)]
-- DESCRIPTION: [HACK_TEXT$="Working..."]
-- DESCRIPTION: [SUCCESS_TEXT$="Success.."]
-- DESCRIPTION: [FAILURE_TEXT$="Failed.."]
-- DESCRIPTION: [FAILURE_COUNT=3(1,20)]
-- DESCRIPTION: [@FAILURE_ALARM=1(1=Off, 2=On)]
-- DESCRIPTION: [ALARM_RESET=5(1,60)] Seconds
-- DESCRIPTION: [NOISE_RANGE=500(0,5000)]
-- DESCRIPTION: [@HACK_TRIGGER=1(1=Off, 2=On)]
-- DESCRIPTION: <Sound0> Hacking sound
-- DESCRIPTION: <Sound1> Success sound
-- DESCRIPTION: <Sound2> Failure sound
-- DESCRIPTION: <Sound3> Alarm sound

local hack 			= {}
local use_range 	= {}
local use_text 		= {}
local hack_time 	= {}
local hack_text 	= {}
local success_text 	= {}
local failure_text 	= {}
local failure_count	= {}
local failure_alarm	= {}
local alarm_reset	= {}	
local noise_range	= {}
local hack_trigger	= {}

local hackbar		= {}
local htime 		= {}
local status		= {}
local hackresult	= {}
local failcount		= {}
local alarm			= {}
local wait			= {}
local doonce		= {}
local playonce		= {}

function hack_properties(e, use_range, use_text, hack_time, hack_text, success_text, failure_text, failure_count, failure_alarm, alarm_reset, noise_range, hack_trigger)
	hack[e] = g_Entity[e]
	hack[e].use_range = use_range	
	hack[e].use_text = use_text
	hack[e].hack_time = hack_time
	hack[e].hack_text = hack_text
	hack[e].success_text = success_text
	hack[e].failure_text = failure_text
	hack[e].failure_count = failure_count
	hack[e].failure_alarm = failure_alarm
	hack[e].alarm_reset = alarm_reset
	hack[e].noise_range = noise_range
	hack[e].hack_trigger = hack_trigger
end

function hack_init(e)
	hack[e] = {}
	hack[e].use_range = 80
	hack[e].use_text = "E to Hack"
	hack[e].hack_time = 8
	hack[e].hack_text = ""
	hack[e].success_text = ""
	hack[e].failure_text = ""
	hack[e].failure_count = 3	
	hack[e].failure_alarm = 1
	hack[e].alarm_reset = 5
	hack[e].noise_range = 500
	hack[e].hack_trigger = 1
	hackbar = CreateSprite(LoadImage("imagebank\\misc\\testimages\\search-bar.png"))
	SetSpriteSize(hackbar,5,-1)
	SetSpriteColor(hackbar,0,100,255,255)
	SetSpritePosition(hackbar,200,200)	
	status[e] = "init"	
	wait[e] = math.huge
	alarm[e] = math.huge
	hackresult[e] = 0
	failcount[e] = 0	
	doonce[e] = 0
	playonce[e] = 0
end 

function hack_main(e)
	hack[e] = g_Entity[e]
	local PlayerDist = GetPlayerDistance(e)
	
	if status[e] == "init" then
		htime[e] = hack[e].hack_time * 5
		doonce[e] = 0
		playonce[e] = 0
		g_hackresult = 0
		status[e] = "hacking"
	end

	if PlayerDist < hack[e].use_range then
		local LookingAt = GetPlrLookingAtEx(e,1)
		if LookingAt == 1 then
			if status[e] == "hacking" then  --Hacking	
				PromptLocalForVR(e,hack[e].use_text)			
				if g_KeyPressE == 1 then
					PromptLocalForVR(e,"")
					if htime[e] > 0 then
						if playonce[e] == 0 then
							LoopSound(e,0)
							playonce[e] = 1
						end
						Prompt(hack[e].hack_text)
						PasteSpritePosition(hackbar,50-(htime[e]/16),95)
						SetSpriteSize(hackbar,htime[e]/8,1)
						htime[e] = htime[e]-0.1
						if htime[e] < 0 then htime[e] = 0 end
					end
					if hack[e].noise_range > 0 then MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,hack[e].noise_range,1,e) end
					hackresult[e] = math.random(1,2)					
					if htime[e] == 0 then
						if hackresult[e] == 1 then -- Success
							SetAnimationName(e,"on")
							PlayAnimation(e)						
							wait[e] = g_Time + 1000
							status[e] = "success"
						end
						if hackresult[e] == 2 then -- Failure
							SetAnimationName(e,"off")
							PlayAnimation(e)						
							wait[e] = g_Time + 1000
							failcount[e] = failcount[e] + 1
							status[e] = "failure"
						end						
					end
				end	
				if g_KeyPressE == 0 then StopSound(e,0) end	
			end
		end
		-----------------------------------------------------------------------
		if status[e] == "success" then  
			PromptDuration(hack[e].success_text,1000)					
			if doonce[e] == 0 then					
				StopSound(e,0)
				PlaySound(e,1)
				doonce[e] = 1
			end
			if g_Time > wait[e] then status[e] = "hacked" end
		end
		if status[e] == "failure" then  
			PromptDuration(hack[e].failure_text,1000)
			if hack[e].failure_alarm == 1 then	
				if doonce[e] == 0 then					
					StopSound(e,0)
					PlaySound(e,2)
					doonce[e] = 1
				end
				if g_Time > wait[e] then status[e] = "init" end
			end	
			if hack[e].failure_alarm == 2 and failcount[e] == hack[e].failure_count then
				if doonce[e] == 0 then					
					StopSound(e,0)
					LoopSound(e,3)	
					doonce[e] = 1
					alarm[e] = g_Time + (hack[e].alarm_reset*1000)
				end
				MakeAISound(g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'],2000,1,e)				
			end			
		end
		if status[e] == "hacked" then  --Finished		
			if hack[e].hack_trigger == 2 then
				ActivateIfUsed(e)
				PerformLogicConnections(e)
				status[e] = "finish"
			end
		end		
	end
	if g_Time > alarm[e] and failcount[e] == hack[e].failure_count then -- Alarm reset
		StopSound(e,3)
		failcount[e] = 0				
		status[e] = "init"
	end
end