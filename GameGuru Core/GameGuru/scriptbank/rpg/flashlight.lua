-- Flashlight v8: by Necrym59
-- DESCRIPTION: Will give the player a Flashlight. Set Always Active ON.
-- DESCRIPTION: [PICKUP_TEXT$="E to pickup"]
-- DESCRIPTION: [PICKUP_RANGE=100(1,200)]
-- DESCRIPTION: [USEAGE_TEXT$="F to use flashlight"]
-- DESCRIPTION: [FLASHLIGHT_RANGE=350(1,1000)]
-- DESCRIPTION: [FLASHLIGHT_RADIUS=40(1,80)]
-- DESCRIPTION: [FLASHLIGHT_R=255(0,255)]
-- DESCRIPTION: [FLASHLIGHT_G=255(0,255)]
-- DESCRIPTION: [FLASHLIGHT_B=255(0,255)]
-- DESCRIPTION: [!FLASHLIGHT_SHADOWS=1]
-- DESCRIPTION: [BATTERY_LEVEL=100(0,100)]
-- DESCRIPTION: [#BATTERY_DRAIN=0.01(0.00,5)]
-- DESCRIPTION: [#BATTERY_RECHARGE=0.01(0.00,1)]
-- DESCRIPTION: [@BATTERY_INDICATOR=1(1=Bottom, 2=Top, 3=None)]
-- DESCRIPTION: [@LIGHT_ACTIVATION=1(1=Off, 2=On Pickup)]
-- DESCRIPTION: [@PICKUP_TRIGGER=1(1=Off, 2=On)]

g_energy = {}

local flashlight 		= {}
local pickup_text 		= {}
local pickup_range 		= {}
local useage_text 		= {}
local flashlight_range	= {}
local flashlight_radius	= {}
local flashlight_r		= {}
local flashlight_g		= {}
local flashlight_b		= {}
local flashlight_shadows= {}
local battery_level	 	= {}	
local battery_drain 	= {}
local battery_recharge 	= {}
local level_indicator	= {}
local light_activation	= {}

local have_flashlight 	= {}
local drain_level		= {}
local status 			= {}
local doonce			= {}
	
function flashlight_properties(e, pickup_text, pickup_range, useage_text, flashlight_range, flashlight_radius, flashlight_r, flashlight_g, flashlight_b, flashlight_shadows, battery_level, battery_drain, battery_recharge, battery_indicator, light_activation, pickup_trigger)
	flashlight[e] = g_Entity[e]
	flashlight[e].pickup_text = pickup_text
	flashlight[e].pickup_range = pickup_range
	flashlight[e].useage_text = useage_text
	flashlight[e].flashlight_range = flashlight_range
	flashlight[e].flashlight_radius = flashlight_radius
	flashlight[e].flashlight_r = flashlight_r
	flashlight[e].flashlight_g = flashlight_g
	flashlight[e].flashlight_b = flashlight_b
	flashlight[e].flashlight_shadows = flashlight_shadows
	flashlight[e].battery_level = battery_level
	flashlight[e].battery_drain = battery_drain
	flashlight[e].battery_recharge = battery_recharge
	flashlight[e].battery_indicator = battery_indicator
	flashlight[e].light_activation = light_activation
	flashlight[e].pickup_trigger = pickup_trigger
end 	
	
function flashlight_init(e)
	flashlight[e] = g_Entity[e]	
	flashlight[e].pickup_text = "E to pickup"
	flashlight[e].pickup_range = 100
	flashlight[e].useage_text = "F to use"
	flashlight[e].flashlight_range = 350
	flashlight[e].flashlight_radius = 40
	flashlight[e].flashlight_r = 255
	flashlight[e].flashlight_g = 255
	flashlight[e].flashlight_b = 255
	flashlight[e].flashlight_shadows = 1
	flashlight[e].battery_level = 100
	flashlight[e].battery_drain = 0.01
	flashlight[e].battery_recharge = 0.01
	flashlight[e].battery_indicator = 1
	flashlight[e].light_activation = 1
	flashlight[e].pickup_trigger = 1
	have_flashlight[e] = 0
	drain_level[e] = 0
	doonce[e] = 0
	status[e] = 'init'
	SetFlashLightKeyEnabled(0)
	g_energy = 0
end
 
function flashlight_main(e)
	flashlight[e] = g_Entity[e]
	local PlayerDist = GetPlayerDistance(e)
	
	if status[e] == 'init' then
		SetGamePlayerStateFlashlightRange(flashlight[e].flashlight_range)
		SetGamePlayerStateFlashlightRadius(flashlight[e].flashlight_radius)
		SetGamePlayerStateFlashlightColorR(flashlight[e].flashlight_r)
		SetGamePlayerStateFlashlightColorG(flashlight[e].flashlight_g)
		SetGamePlayerStateFlashlightColorB(flashlight[e].flashlight_b)
		SetGamePlayerStateFlashlightCastShadow(flashlight[e].flashlight_shadows)	
		if flashlight[e].battery_level > 100 then flashlight[e].battery_level = 100 end
		have_flashlight[e] = 0
		status[e] ='end'
	end
	
	if have_flashlight[e] == 0 then
		if PlayerDist < flashlight[e].pickup_range and g_PlayerHealth > 0 then
			local LookingAt = GetPlrLookingAtEx(e,1)	
			if LookingAt == 1 then
				PromptLocal(e, flashlight[e].pickup_text)
				if g_KeyPressE == 1 then								
					PromptDuration(flashlight[e].useage_text,2000)
					PlaySound(e,0)
					Hide(e)
					CollisionOff(e)
					ActivateIfUsed(e)
					have_flashlight[e] = 1
					if flashlight[e].pickup_trigger == 2 then
						SetActivatedWithMP(e,201)
						PerformLogicConnections(e)
					end
				end
			end
		end
	end
	
	if have_flashlight[e] == 1 then
		SetFlashLightKeyEnabled(1)
		SetGamePlayerStatePlrKeyForceKeystate(0)
		if GetInKey() == "f" or GetInKey() == "F" then	
			if (GetGamePlayerStateFlashlightControl() >= 0.89) then status[e] = 'ON' end						
			if (GetGamePlayerStateFlashlightControl() <= 0.89) then status[e] = 'OFF' end
		end	
		if flashlight[e].light_activation == 2 and doonce[e] == 0 then
			SetGamePlayerStatePlrKeyForceKeystate(33)
			status[e] = 'ON'			
			doonce[e] = 1
		end
	end	
	if status[e] == 'ON' then		
		if flashlight[e].battery_level > 0 then
			flashlight[e].battery_level = flashlight[e].battery_level - (flashlight[e].battery_drain/10)
			if flashlight[e].battery_indicator == 1 then TextCenterOnXColor(50,95,3,"BATTERY LEVEL: " ..math.floor(flashlight[e].battery_level).. "%",100,255,100) end
			if flashlight[e].battery_indicator == 2 then TextCenterOnXColor(50,5,3,"BATTERY LEVEL: " ..math.floor(flashlight[e].battery_level).. "%",100,255,100) end
		end
		if flashlight[e].battery_level > 1 and flashlight[e].battery_level < 2 then
			SetFlashLight(math.random(8))
			SetGamePlayerStateFlashlightRange(math.random(flashlight[e].flashlight_range-10,flashlight[e].flashlight_range))
			SetGamePlayerStateFlashlightRadius(math.random(flashlight[e].flashlight_radius-10,flashlight[e].flashlight_radius))
		end				
		if flashlight[e].battery_level < 1 then SetGamePlayerStateFlashlightControl(0.0) end	
	end
	if status[e] == 'OFF' then
		flashlight[e].battery_level = flashlight[e].battery_level + (flashlight[e].battery_recharge/20)
	end	
	if g_energy > 0 then 
		flashlight[e].battery_level = flashlight[e].battery_level + g_energy
		if flashlight[e].battery_level > 100 then flashlight[e].battery_level = 100 end
		g_energy = 0
	end	
end