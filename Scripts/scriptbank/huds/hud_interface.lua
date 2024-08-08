-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Hud_Interface v6 by Necrym59
-- DESCRIPTION: Will this will enable to use a hud screen as an interface for the attached object. 
-- DESCRIPTION: Ensure your Hud Button names match the ones you put in here and are set to "return id to lua".
-- DESCRIPTION: [USE_RANGE=80(1,100)]
-- DESCRIPTION: [USE_TEXT$="E to Use"]
-- DESCRIPTION: [HUD_SCREEN$="HUD Screen 9"]
-- DESCRIPTION: [HUD_BUTTON1$="Activate Links"]
-- DESCRIPTION: [@HUD_ACTION1=1(1=Activate Links,2=Play Audio,3=Destroy Object,4=Activate IfUsed,5=Exit Hud)]
-- DESCRIPTION: [HUD_ACTION1_TEXT$="Links Activated"]
-- DESCRIPTION: [HUD_BUTTON2$="Play Audio"]
-- DESCRIPTION: [@HUD_ACTION2=2(1=Activate Links,2=Play Audio,3=Destroy Object,4=Activate IfUsed,5=Exit Hud)]
-- DESCRIPTION: [HUD_ACTION2_TEXT$="Playing Audio"]
-- DESCRIPTION: [HUD_BUTTON3$="Destroy"]
-- DESCRIPTION: [@HUD_ACTION3=3(1=Activate Links,2=Play Audio,3=Destroy Object,4=Activate IfUsed,5=Exit Hud)]
-- DESCRIPTION: [HUD_ACTION3_TEXT$="Object Destroyed"]
-- DESCRIPTION: [HUD_BUTTON4$="Activate IfUsed"]
-- DESCRIPTION: [@HUD_ACTION4=4(1=Activate Links,2=Play Audio,3=Destroy Object,4=Activate IfUsed,5=Exit Hud)]
-- DESCRIPTION: [HUD_ACTION4_TEXT$="Activating"]
-- DESCRIPTION: [HUD_BUTTON5$="Exit"]
-- DESCRIPTION: [@HUD_ACTION5=5(1=Activate Links,2=Play Audio,3=Destroy Object,4=Activate IfUsed,5=Exit Hud)]
-- DESCRIPTION: [HUD_ACTION5_TEXT$="Exiting"]
-- DESCRIPTION: <Sound0>  Interface activation
-- DESCRIPTION: <Sound1>  Audiofile sound

local hud_interface 	= {}
local use_range 		= {}
local use_text 			= {}
local hud_screen 		= {}
local hud_button1 		= {}
local hud_action1 		= {}
local hud_action1_text 	= {}
local hud_button2 		= {}
local hud_action2 		= {}
local hud_action2_text 	= {}
local hud_button3 		= {}
local hud_action3 		= {}
local hud_action3_text 	= {}
local hud_button4 		= {}
local hud_action4 		= {}
local hud_action4_text 	= {}
local hud_button5 		= {}
local hud_action5 		= {}
local hud_action5_text 	= {}

local status			= {}
local pressed			= {}
local actioned			= {}
local playonce			= {}

function hud_interface_properties(e, use_range, use_text, hud_screen, hud_button1, hud_action1, hud_action1_text, hud_button2, hud_action2, hud_action2_text, hud_button3, hud_action3, hud_action3_text, hud_button4, hud_action4, hud_action4_text, hud_button5, hud_action5, hud_action5_text)
	hud_interface[e].use_range = use_range
	hud_interface[e].use_text = use_text
	hud_interface[e].hud_screen = hud_screen
	hud_interface[e].hud_button1 = hud_button1
	hud_interface[e].hud_action1 = hud_action1
	hud_interface[e].hud_action1_text = hud_action1_text
	hud_interface[e].hud_button2 = hud_button2
	hud_interface[e].hud_action2 = hud_action2
	hud_interface[e].hud_action2_text = hud_action2_text	
	hud_interface[e].hud_button3 = hud_button3
	hud_interface[e].hud_action3 = hud_action3
	hud_interface[e].hud_action3_text = hud_action3_text	
	hud_interface[e].hud_button4 = hud_button4
	hud_interface[e].hud_action4 = hud_action4
	hud_interface[e].hud_action4_text = hud_action4_text
	hud_interface[e].hud_button5 = hud_button5
	hud_interface[e].hud_action5 = hud_action5
	hud_interface[e].hud_action5_text = hud_action5_text	
end

function hud_interface_init(e)
	hud_interface[e] = {}
	hud_interface[e].use_range = 80
	hud_interface[e].use_text = "E to use"
	hud_interface[e].hud_screen = "HUD Screen 9"
	hud_interface[e].hud_button1 = "Activate Links"
	hud_interface[e].hud_action1 = 1
	hud_interface[e].hud_action1_text = "Activate Links"
	hud_interface[e].hud_button2 = "Play"
	hud_interface[e].hud_action2 = 2
	hud_interface[e].hud_action2_text = "Playing Audio"	
	hud_interface[e].hud_button3 = "Destroy"
	hud_interface[e].hud_action3 = 3
	hud_interface[e].hud_action3_text = "Object Destroyed"
	hud_interface[e].hud_button4 = "Activate IfUsed"
	hud_interface[e].hud_action4 = 4
	hud_interface[e].hud_action4_text = "Activate IfUsed"	
	hud_interface[e].hud_button5 = "Exit"
	hud_interface[e].hud_action5 = 5
	hud_interface[e].hud_action5_text = "Exiting"		
	
	status[e] = "init"
	pressed[e] = 0
	actioned[e] = 0
	playonce[e] = 0
end 

function hud_interface_main(e)
	
	if status[e] == "init" then
		pressed[e] = 0
		playonce[e] = 0
		actioned[e] = 0
		status[e] = "start"
	end

	if status[e] == "start" then
		local PlayerDist = GetPlayerDistance(e)
		if PlayerDist < hud_interface[e].use_range then
			local LookingAt = GetPlrLookingAtEx(e,1)
			if LookingAt == 1 then
				PromptLocal(e,hud_interface[e].use_text)
				if g_KeyPressE == 1 and pressed[e] == 0 then
					if playonce[e] == 0 then
						PlaySound(e,0)
						playonce[e] = 1
					end
					pressed[e] = 1
					PromptLocal(e,"")
					-- OPEN HUD
					ScreenToggle(hud_interface[e].hud_screen)
					status[e] = "interface"
				end
			end	
		end		
	end			
	
	if status[e] == "interface" then
		local buttonElementID = DisplayCurrentScreen()
		if buttonElementID >= 0 then
			local buttonElementName = GetScreenElementName(1+buttonElementID)
			if string.len(buttonElementName) > 0 then				
				if buttonElementName == hud_interface[e].hud_button1 then
					PromptLocal(e,hud_interface[e].hud_action1_text)
					if hud_interface[e].hud_action1 == 1 then actioned[e] = 1 end
					if hud_interface[e].hud_action1 == 2 then actioned[e] = 2 end
					if hud_interface[e].hud_action1 == 3 then actioned[e] = 3 end
					if hud_interface[e].hud_action1 == 4 then actioned[e] = 4 end
					if hud_interface[e].hud_action1 == 5 then actioned[e] = 5 end 
					status[e] = "action"
				end				
				if buttonElementName == hud_interface[e].hud_button2 then
					PromptLocal(e,hud_interface[e].hud_action2_text)
					if hud_interface[e].hud_action2 == 1 then actioned[e] = 1 end
					if hud_interface[e].hud_action2 == 2 then actioned[e] = 2 end
					if hud_interface[e].hud_action2 == 3 then actioned[e] = 3 end
					if hud_interface[e].hud_action2 == 4 then actioned[e] = 4 end
					if hud_interface[e].hud_action2 == 5 then actioned[e] = 5 end 
					status[e] = "action"
				end
				if buttonElementName == hud_interface[e].hud_button3 then
					PromptLocal(e,hud_interface[e].hud_action3_text)
					if hud_interface[e].hud_action3 == 1 then actioned[e] = 1 end
					if hud_interface[e].hud_action3 == 2 then actioned[e] = 2 end
					if hud_interface[e].hud_action3 == 3 then actioned[e] = 3 end
					if hud_interface[e].hud_action3 == 4 then actioned[e] = 4 end
					if hud_interface[e].hud_action3 == 5 then actioned[e] = 5 end 
					status[e] = "action"
				end				
				if buttonElementName == hud_interface[e].hud_button4 then
					PromptLocal(e,hud_interface[e].hud_action4_text)
					if hud_interface[e].hud_action4 == 1 then actioned[e] = 1 end
					if hud_interface[e].hud_action4 == 2 then actioned[e] = 2 end
					if hud_interface[e].hud_action4 == 3 then actioned[e] = 3 end
					if hud_interface[e].hud_action4 == 4 then actioned[e] = 4 end
					if hud_interface[e].hud_action4 == 5 then actioned[e] = 5 end 
					status[e] = "action"
				end
				if buttonElementName == hud_interface[e].hud_button5 then
					PromptLocal(e,hud_interface[e].hud_action5_text)
					if hud_interface[e].hud_action5 == 1 then actioned[e] = 1 end
					if hud_interface[e].hud_action5 == 2 then actioned[e] = 2 end
					if hud_interface[e].hud_action5 == 3 then actioned[e] = 3 end
					if hud_interface[e].hud_action5 == 4 then actioned[e] = 4 end
					if hud_interface[e].hud_action5 == 5 then actioned[e] = 5 end 
					status[e] = "action"
				end
			end			
		end	
	end
	if status[e] == "action" then
		if actioned[e] == 1 then 
			PerformLogicConnections(e)
			actioned[e] = 0
			status[e] = "interface"
		end	
		if actioned[e] == 2 then 
			PlaySound(e,1)
			actioned[e] = 0
			status[e] = "interface"
		end
		if actioned[e] == 3 then
			ScreenToggle("")
			Hide(e)
			CollisionOff(e)
			Destroy(e)
			actioned[e] = 0
			status[e] = "interface"
		end		
		if actioned[e] == 4 then 
			ActivateIfUsed(e)
			actioned[e] = 0
			status[e] = "interface"			
		end		
		if actioned[e] == 5 then 
			-- CLOSE HUD
			ScreenToggle("")
			status[e] = "init"
		end
	end
end