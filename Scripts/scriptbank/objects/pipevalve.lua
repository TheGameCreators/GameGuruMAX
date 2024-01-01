-- Pipevalve v2
-- DESCRIPTION: This object will be treated as a switch object for activating other objects or game elements. Set Always Active = On
-- DESCRIPTION: [PROMPT_TEXT$ = "to use"], [USE_RANGE=80(1,150)]
-- DESCRIPTION: Play <Sound0> when attaching.
-- DESCRIPTION: Play <Sound1> when turning
-- DESCRIPTION: Play <Sound2> when opened

g_valve = {}
local pipevalve = {}
local prompt_text = {}
local use_range = {}
local status = {}
local valve_required = {}
local valve_level = {}
local activated = {}
local pressed = {}
local Clock = os.clock

function pipevalve_properties(e, prompt_text, use_range, status, valve_required)
	pipevalve[e] = g_Entity[e]
	pipevalve[e].prompt_text = prompt_text
	pipevalve[e].use_range = use_range
	pipevalve[e].status = 1
	pipevalve[e].valve_required = 1
end 

function pipevalve_init(e)
	pipevalve[e] = g_Entity[e]
	pipevalve[e].prompt_text = "to use"	
	pipevalve[e].use_range = 80
	pipevalve[e].status = 1
	pipevalve[e].valve_required = 1
	valve_level[e] = 0	
	activated[e] = 0
	g_valve = g_valve
	pressed = 0
	StartTime = Clock()	+ 2000
end

function pipevalve_main(e)
	pipevalve[e] = g_Entity[e]	
	local PlayerDist = GetPlayerDistance(e)
	
	if PlayerDist < pipevalve[e].use_range and g_PlayerHealth > 0 then
		local LookingAt = GetPlrLookingAtEx(e,1)
		if pipevalve[e].status == 1 then  --Inactive			
			if LookingAt == 1 and pipevalve[e].status == 1 then					
				if g_valve > 0 then						
					valve_level[e] = (valve_level[e] + g_valve)
					g_valve = 0						
				end
				if valve_level[e] == pipevalve[e].valve_required then					
					StopAnimation(e)											
					SetAnimationName(e,"off")
					SetAnimationSpeed(e,10)					
					PlayAnimation(e)
					PlaySound(e,0)
					pipevalve[e].status = 2					
				else
					activated[e] = 0
					PromptLocal(e,"Valve wheel required",3)
					SetAnimationName(e,"idle")
					PlayAnimation(e)
					pipevalve[e].status = 1
				end
				if g_KeyPressE == 0 then pressed = 0 end
			end			
		end
		if pipevalve[e].status == 2 then --Active			
			if LookingAt == 1 and pipevalve[e].status == 2 and activated[e] == 1 then PromptLocal(e,"Valve Opened",3) end
			if LookingAt == 1 and pipevalve[e].status == 2 and activated[e] == 0 then				
				if GetGamePlayerStateXBOX() == 1 then
					PromptLocal(e,"Y Button " ..pipevalve[e].prompt_text,3)
				else
					if GetHeadTracker() == 1 then
						PromptLocal(e,"Trigger " ..pipevalve[e].prompt_text,3)
					else
						PromptLocal(e,"E " ..pipevalve[e].prompt_text,3)
					end					
				end
			end
			if g_KeyPressE == 1 and activated[e] == 0 then		
				SetActivatedWithMP(e,201)
				activated[e] = 1
				StopSound(e,0)
				PlaySound(e,1)
				StopAnimation(e)
				SetAnimationSpeed(e,1)
				SetAnimationName(e,"on")
				PlayAnimation(e)				
				PerformLogicConnections(e)
				PlaySound(e,2)
			end
		end	
	end	
end
