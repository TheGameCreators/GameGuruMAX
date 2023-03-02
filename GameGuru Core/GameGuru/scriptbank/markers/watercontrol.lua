-- Water Control v4 - thanks to Necrym59
-- DESCRIPTION: Control water height options when player is in zone or switched by control.
-- DESCRIPTION: Attach to object and set Static Mode to Physics Off, and set Always active ON. 
-- DESCRIPTION: Change [PROMPT_TEXT$="Don't drown"] [@MODE=1(1=Raise, 2=Lower)] [LEVEL=80] [#SPEED=0.02(0,5)] and play <Sound0>

local watercontrol = {}
local prompt_text = {}
local mode = {}
local level = {}
local speed = {}
local current_level = {}
local status = {}

function watercontrol_properties(e, prompt_text, mode, level, speed)
	watercontrol[e] = g_Entity[e]
	watercontrol[e].prompt_text 	= prompt_text
	watercontrol[e].mode			= mode
	watercontrol[e].level 			= level
	watercontrol[e].speed 			= speed
end 

function watercontrol_init(e)
	watercontrol[e] = g_Entity[e]
	watercontrol[e].prompt_text 	= "Don't drown"
	watercontrol[e].mode			= 1
	watercontrol[e].level 			= 80
	current_level[e]				= GetWaterHeight()	
	watercontrol[e].speed			= 1
	watercontrol[e].state 			= 0
	status[e] = "init"
end

function watercontrol_main(e)
	watercontrol[e] = g_Entity[e]
	if status[e] == "init" then
		if watercontrol[e].mode == 1 then watercontrol[e].level = (GetWaterHeight() + watercontrol[e].level) end
		if watercontrol[e].mode == 2 then watercontrol[e].level = (GetWaterHeight() - watercontrol[e].level) end
		status[e] = "end"
	end
	if g_Entity[e]['plrinzone']==1 and g_Entity[e]['active'] == 1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+100 then
		g_Entity[e]['activated'] = 1
	end
	if g_Entity[e]['activated'] == 1 then		
		if watercontrol[e].state == 0 then
			watercontrol[e].state = 1
			Prompt(watercontrol[e].prompt_text)		
			PlaySound(e,0)
		end
		if watercontrol[e].mode == 1 then -- Raise Water Height				
			if current_level[e] <= watercontrol[e].level then				
				current_level[e] = current_level[e] + watercontrol[e].speed				
				SetWaterHeight(current_level[e])
				ShowWater()				
			end			
		end
		if watercontrol[e].mode == 2 then -- Lower Water Height				
			if current_level[e] >= watercontrol[e].level then
				current_level[e] = current_level[e] - watercontrol[e].speed				
				SetWaterHeight(current_level[e])
				ShowWater()				
			end			
		end		
	end
	current_level[e] = GetWaterHeight()
	PerformLogicConnections(e)
end
