--Shake v4 adapted by Necrym59 from an original script by AmenMoses
-- DESCRIPTION: Creates camera shake. Attache to an object and link to a trigger zone or switch.
-- DESCRIPTION: Select Shake [@STYLE=6(1=Tremble, 2=Tremor, 3=Earthquake, 4=Explosion, 5=Drunkard, 6=Manual)]
-- DESCRIPTION: Change the [PROMPT_TEXT$=""]
-- DESCRIPTION: [TRAUMA#=0(0,100)]
-- DESCRIPTION: [PERIOD#=0(1,2000)]
-- DESCRIPTION: [FADE#=0(0,10)]
-- DESCRIPTION: [BACKGROUND_TRAUMA#=0(0,100)]
-- DESCRIPTION: [BACKGROUND_PERIOD#=0(1,2000)]
-- DESCRIPTION: [@MODE=1(1=Once Only, 2=Continuous)]
-- DESCRIPTION: Play <Sound0> when activated

local shake = {}
local style = {}
local prompt_text = {}
local trauma = {}
local period = {}
local fade = {}
local background_trauma = {}
local background_period = {}
local mode = {}
local shaken = {}
local doonce = {}
local shakeoff = {}
local delay = {}

function shake_properties(e, style, prompt_text, trauma, period, fade, background_trauma, background_period, mode)
	shake[e] = g_Entity[e]
	shake[e].style = style
	shake[e].prompt_text = prompt_text
	shake[e].trauma = trauma
	shake[e].period = period
	shake[e].fade = fade
	shake[e].background_trauma = background_trauma
	shake[e].background_period = background_period
	shake[e].mode = mode
end

function shake_init(e)
	shake[e] = g_Entity[e]
	shake[e].style = 6
	shake[e].prompt_text = ""
	shake[e].trauma = 0
	shake[e].period = 0
	shake[e].fade = 0
	shake[e].background_trauma = 0
	shake[e].background_period = 0
	shake[e].mode = 1
	g_Entity[e]['activated'] = 0
	shaken[e] = 0
	doonce[e] = 0
	shakeoff[e] = 0
	delay[e] = math.huge
end

function shake_main(e)
	shake[e] = g_Entity[e]
	
	if GamePlayerControlSetShakeTrauma == nil then return end -- early return if not compatible gameplayercontrol available
	
	if g_Entity[e]['activated'] == 1 then
	
		if doonce[e] == 0 then
			PlaySound(e,0)
			doonce[e] = 1
		end
		if shake[e].style == 1 and shaken[e] == 0 then
			Prompt(shake[e].prompt_text)
			GamePlayerControlAddShakeTrauma(25.0)
			GamePlayerControlAddShakePeriod(47.0)
			GamePlayerControlAddShakeFade  (0.14)
			GamePlayerControlSetShakeTrauma(0)
			GamePlayerControlSetShakePeriod(0)
			shaken[e] = 1
		end
		if shake[e].style == 2 and shaken[e] == 0 then
			Prompt(shake[e].prompt_text)
			GamePlayerControlAddShakeTrauma(30.90)
			GamePlayerControlAddShakePeriod(83.0)
			GamePlayerControlAddShakeFade  (0.17)
			GamePlayerControlSetShakeTrauma(1.4)
			GamePlayerControlSetShakePeriod(190.0)
			shaken[e] = 1
		end
		if shake[e].style == 3 and shaken[e] == 0 then
			Prompt(shake[e].prompt_text)
			GamePlayerControlAddShakeTrauma(40.90)
			GamePlayerControlAddShakePeriod(113.0)
			GamePlayerControlAddShakeFade  (0.1)
			GamePlayerControlSetShakeTrauma(3.4)
			GamePlayerControlSetShakePeriod(180.0)
			shaken[e] = 1
		end
		if shake[e].style == 4 and shaken[e] == 0 then
			Prompt(shake[e].prompt_text)
			GamePlayerControlAddShakeTrauma(65.0)
			GamePlayerControlAddShakePeriod(30.0)
			GamePlayerControlAddShakeFade  (2)
			GamePlayerControlSetShakeTrauma(0)
			GamePlayerControlSetShakePeriod(0)
			shaken[e] = 1
		end
		if shake[e].style == 5 and shaken[e] == 0 then
			Prompt(shake[e].prompt_text)
			GamePlayerControlAddShakeTrauma(0)
			GamePlayerControlAddShakePeriod(0)
			GamePlayerControlAddShakeFade  (0)
			GamePlayerControlSetShakeTrauma(65.0)
			GamePlayerControlSetShakePeriod(960.0)
			shaken[e] = 1
			delay[e] = g_Time + 7000
		end
		if shake[e].style == 6 and shaken[e] == 0 then	---- Manual style Settings
			Prompt(shake[e].prompt_text)
			GamePlayerControlAddShakeTrauma(shake[e].trauma)
			GamePlayerControlAddShakePeriod(shake[e].period)
			GamePlayerControlAddShakeFade  (shake[e].fade)
			GamePlayerControlSetShakeTrauma(shake[e].background_trauma)
			GamePlayerControlSetShakePeriod(shake[e].background_period)
			shaken[e] = 1
		end
		if g_Time > delay[e] and shake[e].mode == 1 then
			if shakeoff[e] == 0 then
				GamePlayerControlSetShakeTrauma(0.0)
				GamePlayerControlSetShakePeriod(0.0)
				shakeoff[e] = 1				
			end
		end	
		if shake[e].mode == 2 then GamePlayerControlAddShakeFade(0.0) end
		if shaken[e] == 1 then
			SetActivated(e,0)
			shaken[e] = 0
		end	
	end
end
 

	
	


