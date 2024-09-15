-- Generator v11
-- DESCRIPTION: This object will be treated as a switch object for activating other objects or game elements. Set Always Active = On
-- DESCRIPTION: [PROMPT_TEXT$="E to use"]
-- DESCRIPTION: [USE_RANGE=80(1,100)]
-- DESCRIPTION: [@STATUS=1(1=Empty, 2=Full)]
-- DESCRIPTION: [FUEL_REQUIRED=20(1,100)]
-- DESCRIPTION: [FAIL_TEXT$="More fuel needed"]
-- DESCRIPTION: [RUNNING_TEXT$="Generator Running, Q to stop"]
-- DESCRIPTION: [STOPPED_TEXT$="Generator Stopped"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: Play <Sound0> when starting.
-- DESCRIPTION: Play <Sound1> running loop.

g_fuel = {}
g_fuelamount = {}
g_fuelcollected = {}

local generator = {}
local prompt_text = {}
local use_range = {}
local status = {}
local fuel_required = {}
local fail_text = {}
local running_text = {}
local stopped_text = {}
local prompt_display = {}

local fuel_level = {}
local running = {}
local timer = {}
local played = {}
local tusedvalue = {}
local use_item_now = {}

function generator_properties(e, prompt_text, use_range, status, fuel_required, fail_text, running_text, stopped_text, prompt_display)
	generator[e].prompt_text = prompt_text
	generator[e].use_range = use_range
	generator[e].status = status
	generator[e].fuel_required = fuel_required
	generator[e].fail_text = fail_text
	generator[e].running_text = running_text
	generator[e].stopped_text = stopped_text	
	generator[e].prompt_display = prompt_display	
end 

function generator_init(e)
	generator[e] = {}
	generator[e].prompt_text = "to use"	
	generator[e].use_range = 80
	generator[e].status = 1
	generator[e].fuel_required = 20
	generator[e].fail_text = "More fuel needed"
	generator[e].running_text = "Generator Running"
	generator[e].stopped_text = "Generator Stopped"
	generator[e].prompt_display = 1
	
	fuel_level[e] = 0	
	running[e] = 0
	timer[e] = 0
	g_fuel = 0
	g_fuelamount = g_fuelamount
	played = 0
	tusedvalue[e] = 0
	use_item_now[e] = 0	
end

function generator_main(e)

	local PlayerDist = GetPlayerDistance(e)
		
	if PlayerDist < generator[e].use_range and g_PlayerHealth > 0 then
		local LookingAt = GetPlrLookingAtEx(e,1)		
		if generator[e].status == 1 then  --Empty		
			if LookingAt == 1 then
				if generator[e].prompt_display == 1 then PromptLocal(e,generator[e].prompt_text) end
				if generator[e].prompt_display == 2 then Prompt(generator[e].prompt_text) end	
				if g_KeyPressE == 1 then
					if g_fuel > 0 then
						tusedvalue[e] = g_fuel
						fuel_level[e] = (fuel_level[e] + g_fuelamount)
						g_fuelamount = 0
						g_fuelcollected = 0
					end
					if fuel_level[e] >= generator[e].fuel_required then
						PlaySound(e,0)						
						generator[e].status = 2
					else						
						if played == 0 then
							PlaySound(e,0)
							played = 1
						end
						running[e] = 0
						if generator[e].prompt_display == 1 then PromptLocal(e,generator[e].fail_text) end
						if generator[e].prompt_display == 2 then Prompt(ggenerator[e].fail_text) end							
						generator[e].status = 1
					end
				end
				if g_KeyPressE == 0 then played = 0 end					
			end			
		end		
		if generator[e].status == 2 then --Full
			if LookingAt == 1 then
				if running[e] == 0 then
					if generator[e].prompt_display == 1 then PromptLocal(e,generator[e].prompt_text) end
					if generator[e].prompt_display == 2 then Prompt(generator[e].prompt_text) end	
				end
				if running[e] == 1 then
					if generator[e].prompt_display == 1 then PromptLocal(e,generator[e].running_text) end
					if generator[e].prompt_display == 2 then Prompt(generator[e].running_text) end	
				end				
			end
			if g_KeyPressE == 1 and running[e] == 0 then
				SetActivatedWithMP(e,201)
				running[e] = 1
				LoopSound(e,1)
				SetAnimationName(e,"on")
				PlayAnimation(e)
				PerformLogicConnections(e)				
			end			
			if g_KeyPressQ == 1 and running[e] == 1 then 
				SetActivatedWithMP(e,101)
				running[e] = 0
				if generator[e].prompt_display == 1 then PromptLocal(e,generator[e].stopped_text) end
				if generator[e].prompt_display == 2 then Prompt(generator[e].stopped_text) end
				StopSound(e,1)
				SetAnimationName(e,"off")
				PlayAnimation(e)
				PerformLogicConnections(e)
			end			
		end
		if tusedvalue[e] > 0 then
			-- if this is a resource, it will deplete qty and set used to zero
			SetEntityUsed(e,tusedvalue[e]*-1)
			use_item_now[e] = 1			
		end
		if use_item_now[e] == 1 then
			Destroy(g_fuel) -- can only destroy resources that are qty zero
		end
	end	
end
