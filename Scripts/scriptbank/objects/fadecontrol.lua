-- Fade Control v7 by Necrym59
-- DESCRIPTION: Fades the transparency of an object when player is in zone or switched by control.
-- DESCRIPTION: Attach to object and set Physics ON/OFF, Always active ON. 
-- DESCRIPTION: Change [PROMPT_TEXT$="What was that"] [@MODE=1(1=Increase Visibility, 2=Decrease Visibility)] [LEVEL=80] [#SPEED=0.02(0,5)] and play <Sound0>

local fadecontrol = {}
local prompt_text = {}
local mode = {}
local speed = {}
local current_level = {}
local status = {}
local doonce = {}

function fadecontrol_properties(e, prompt_text, mode, level, speed)
	fadecontrol[e] = g_Entity[e]
	fadecontrol[e].prompt_text 	= prompt_text
	fadecontrol[e].mode			= mode
	fadecontrol[e].level 		= level
	fadecontrol[e].speed 		= speed
end 

function fadecontrol_init(e)
	fadecontrol[e] = g_Entity[e]
	fadecontrol[e].prompt_text 	= ""
	fadecontrol[e].mode			= 1
	fadecontrol[e].level 		= 80
	fadecontrol[e].speed		= 1
	status[e] = "init"
	doonce = 0
	current_level[e] = GetEntityBaseAlpha(e)
	SetEntityTransparency(e,1)
end

function fadecontrol_main(e)
	fadecontrol[e] = g_Entity[e]
	if status[e] == "init" then		
		status[e] = "end"
	end
	
	if g_Entity[e]['activated'] == 1 then
		if doonce == 0 then			
			Prompt(fadecontrol[e].prompt_text)		
			PlaySound(e,0)
			doonce = 1
		end
		if fadecontrol[e].mode == 1 then -- Increase Visibility
			if current_level[e] < fadecontrol[e].level then				
				SetEntityBaseAlpha(e,current_level[e])
				current_level[e] = current_level[e] + fadecontrol[e].speed
			end	
		end
		if fadecontrol[e].mode == 2 then -- Decrease Visibility	
			if current_level[e] > 0 then				
				SetEntityBaseAlpha(e,current_level[e])
				current_level[e] = current_level[e] - fadecontrol[e].speed
			end	
			if current_level[e] <= fadecontrol[e].level then
				current_level[e] = fadecontrol[e].level
			end	
			if current_level[e] <= 0 then
				CollisionOff(e)
				Destroy(e)
			end
		end		
	end
	PerformLogicConnections(e)
end
