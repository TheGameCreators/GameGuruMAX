-- Eat v2 by Necrym59
-- DESCRIPTION: The attached object will give the player a quarter portion of the specified health (QUANTITY) until consumed.
-- DESCRIPTION: Change [PROMPT_TEXT$="Press E to Eat"]
-- DESCRIPTION: [QUANTITY=10(1,100)]
-- DESCRIPTION: [EAT_RANGE=80(1,100)]
-- DESCRIPTION: [@EAT_EFFECT=1(1=Healthy, 2=Poisoned)]
-- DESCRIPTION: [@POISON_EFFECT=1(1=On, 2=Off)]
-- DESCRIPTION: [CONSUMED_TEXT$="Tasted Fine"]
-- DESCRIPTION: <Sound0> for eating
-- DESCRIPTION: <Sound1> if poisoned 

local eat = {}
local prompt_text = {}
local quantity = {}
local eat_range = {}
local eat_effect = {}
local poison_effect = {}
local consumed_text = {}
local eat_last = {}
local eat_cam = {}
local calchealth = {}
local eatamount = {}
local eattime = {}
local poisoned = {}
local status = {}
local played = {}

function eat_properties(e, prompt_text, quantity, eat_range, eat_effect, poison_effect, consumed_text)
	eat[e] = g_Entity[e]
	eat[e].prompt_text = prompt_text
	eat[e].quantity = quantity
	eat[e].eat_range = eat_range
	eat[e].eat_effect = eat_effect
	eat[e].poison_effect = poison_effect
	eat[e].consumed_text = consumed_text	
end

function eat_init(e)
	eat[e] = g_Entity[e]
	eat[e].prompt_text = "Press E to eat"
	eat[e].quantity = 10
	eat[e].eat_range = 50
	eat[e].eat_effect = 1
	eat[e].poison_effect = 1
	eat[e].consumed_text = "Tasted Fine"
	eat[e].eat_last = GetTimer(e) + 2500
	eat[e].eat_cam = GetTimer(e)
	calchealth = eat[e].quantity
	eattime[e] = 0
	poisoned[e] = 0
	played[e] = 0
	status[e] = "init"	
end

function eat_main(e)
	eat[e] = g_Entity[e]
	PlayerDist = GetPlayerDistance(e)
	if status[e] == "init" then
		eatamount[e] = eat[e].quantity/4
		status[e] = "consume"
	end
	if status[e] == "consume" then
		if GetTimer(e) < eat[e].eat_cam then		
			SetCameraPosition(0,GetCameraPositionX(0),GetCameraPositionY(0)-eattime[e],GetCameraPositionZ(0))
			eattime[e] = eattime[e] + 0.5
			if eattime[e] >= 10 then  eattime[e] = 10 end			
		end
		if GetTimer(e) > eat[e].eat_cam then		
			SetCameraPosition(0,GetCameraPositionX(0),GetCameraPositionY(0)-eattime[e],GetCameraPositionZ(0))
			eattime[e] = eattime[e] - 0.5
			if eattime[e] < 0 then  eattime[e] = 0 end
			if poisoned[e] > 0 then	status[e] = "poisoned" end
		end
			
		if PlayerDist < eat[e].eat_range and g_PlayerHealth > 0 and GetTimer(e) > eat[e].eat_last  then

			local LookingAt = GetPlrLookingAtEx(e,1)
			if LookingAt == 1 then
				Prompt(eat[e].prompt_text)
					
				if g_KeyPressE == 1 then
					PlaySound(e,0)
					if eat[e].eat_effect == 1 then						
						PromptDuration(eat[e].consumed_text,3000)
						calchealth = g_PlayerHealth + eatamount[e]
						if calchealth > g_gameloop_StartHealth then
							calchealth = g_gameloop_StartHealth
						end						
					end
					if eat[e].eat_effect == 2 then
						if played[e] == 0 then							
							PlaySound(e,1)
							played[e] = 1
						end
						PromptDuration(eat[e].consumed_text,3000)
						calchealth = g_PlayerHealth - eatamount[e]						
						if calchealth < 0 then
							calchealth = 0
						end
						poisoned[e] = eatamount[e]*50						
					end				
					SetPlayerHealth(calchealth)
					eat[e].eat_last = GetTimer(e) + 2500
					eat[e].eat_cam = GetTimer(e) + 1500
					eat[e].quantity = eat[e].quantity - eatamount[e]
					if eat[e].quantity <= 0 then
						CollisionOff(e)
						Hide(e)						
						Destroy(e)
					end
				end			
				eattime[e] = 0				
			end
		end
	end	
	if status[e] == "poisoned" then		
		if eat[e].poison_effect == 1 then	
			if poisoned[e] > 0 then
				GamePlayerControlSetShakeTrauma(565.0)
				GamePlayerControlSetShakePeriod(960.00)				
				poisoned[e] = poisoned[e]-1			
			end
			if poisoned[e] <= 0 then
				GamePlayerControlSetShakeTrauma(0.0)
				GamePlayerControlSetShakePeriod(0.00)				
			end			
		end
		if g_PlayerHealth > 0 then status[e] = "consume" end
		played[e] = 0
	end
end
