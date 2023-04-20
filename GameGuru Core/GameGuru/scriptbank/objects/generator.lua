-- Generator v2
-- DESCRIPTION: This object will be treated as a switch object for activating other objects or game elements. Set Always Active = On
-- DESCRIPTION: [PROMPT_TEXT$ = "to use"], [USE_RANGE=80(1,100)] [@STATUS=1(1=Empty, 2=Full)], [FUEL_REQUIRED=20(1,100)]
-- DESCRIPTION: Play <Sound0> when starting.
-- DESCRIPTION: Play <Sound1> running loop.

g_fuel = {}
local generator = {}
local prompt_text = {}
local use_range = {}
local status = {}
local fuel_required = {}
local fuel_level = {}
local running = {}
local timer = {}
local played = {}

function generator_properties(e, prompt_text, use_range, status, fuel_required)
	generator[e] = g_Entity[e]
	generator[e].prompt_text = prompt_text
	generator[e].use_range = use_range
	generator[e].status = status
	generator[e].fuel_required = fuel_required
end 

function generator_init(e)
	generator[e] = g_Entity[e]
	generator[e].prompt_text = "to use"	
	generator[e].use_range = 80
	generator[e].status = 1
	generator[e].fuel_required = 20
	fuel_level[e] = 0	
	running[e] = 0
	timer[e] = 0
	g_fuel = g_fuel
	played = 0	
end

function generator_main(e)
	generator[e] = g_Entity[e]	
	local PlayerDist = GetPlayerDistance(e)
		
	if PlayerDist < generator[e].use_range and g_PlayerHealth > 0 then
		local LookingAt = GetPlrLookingAtEx(e,1)		
		if generator[e].status == 1 then  --Empty		
			if LookingAt == 1 then				
				if GetGamePlayerStateXBOX() == 1 then
					if running[e] == 0 then PromptLocalForVR(e,"Y Button " ..generator[e].prompt_text,3) end
				else
					if GetHeadTracker() == 1 then
						if running[e] == 0 then PromptLocalForVR(e,"Trigger " ..generator[e].prompt_text,3) end
					else
						if running[e] == 0 then PromptLocalForVR(e,"E " ..generator[e].prompt_text,3) end
					end					
				end
				if g_KeyPressE == 1 then
					if g_fuel > 0 then						
						fuel_level[e] = (fuel_level[e] + g_fuel)
						g_fuel = 0						
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
						Prompt("Fuel Needed.. " ..generator[e].fuel_required-fuel_level[e].. " units required")
						generator[e].status = 1
					end
				end
				if g_KeyPressE == 0 then played = 0 end					
			end			
		end		
		if generator[e].status == 2 then --Full
			if LookingAt == 1 then				
				if GetGamePlayerStateXBOX() == 1 then
					if running[e] == 0 then PromptLocalForVR(e,"Y Button " ..generator[e].prompt_text,3) end
				else
					if GetHeadTracker() == 1 then
						if running[e] == 0 then PromptLocalForVR(e,"Trigger " ..generator[e].prompt_text,3) end
					else
						if running[e] == 0 then PromptLocalForVR(e,"E " ..generator[e].prompt_text,3) end
						if running[e] == 1 then PromptLocalForVR(e,"Q to Stop",3) end
					end					
				end
			end
			if g_KeyPressE == 1 and running[e] == 0 then
				Prompt("Generator Running")
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
				Prompt("Generator Stopped")
				StopSound(e,1)
				SetAnimationName(e,"off")
				PlayAnimation(e)
				PerformLogicConnections(e)
			end		
		end
	end	
end
