-- Pipevalve v4 by Necrym59 
-- DESCRIPTION: This object will be treated as a switch object for activating other linked objects and/or IfUsed game element. Set Always Active = On
-- DESCRIPTION: [PROMPT_TEXT$ = "E to use"]
-- DESCRIPTION: [USE_RANGE=80(1,150)]
-- DESCRIPTION: [UNUSED_TEXT$ = "Valve wheel required"]
-- DESCRIPTION: [USED_TEXT$ = "Valve opened"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: <Sound0> when attaching.
-- DESCRIPTION: <Sound1> when turning
-- DESCRIPTION: <Sound2> when opened

g_valve = {}
local pipevalve = {}
local prompt_text = {}
local use_range = {}

local status = {}
local valve_required = {}
local valve_level = {}
local activated = {}
local tusedvalue = {}
local use_item_now = {}
local pressed = {}

function pipevalve_properties(e, prompt_text, use_range, status, valve_required, unused_text, used_text, prompt_display)
	pipevalve[e].prompt_text = prompt_text
	pipevalve[e].use_range = use_range
	pipevalve[e].status = 1
	pipevalve[e].valve_required = 1		
	pipevalve[e].unused_text = unused_text		
	pipevalve[e].used_text = used_text	
	pipevalve[e].prompt_display = prompt_display
end 

function pipevalve_init(e)
	pipevalve[e] = {}
	pipevalve[e].prompt_text = "E to use"	
	pipevalve[e].use_range = 80
	pipevalve[e].status = 1
	pipevalve[e].valve_required = 1	
	pipevalve[e].unused_text = "Valve wheel required"		
	pipevalve[e].used_text = "Valve Opened"	
	pipevalve[e].prompt_display = 1	
	valve_level[e] = 0	
	activated[e] = 0
	tusedvalue[e] = 0
	use_item_now[e] = 0
	g_valve = g_valve
	pressed = 0
end

function pipevalve_main(e)

	local PlayerDist = GetPlayerDistance(e)
	
	if PlayerDist < pipevalve[e].use_range and g_PlayerHealth > 0 then
		local LookingAt = GetPlrLookingAtEx(e,1)
		if pipevalve[e].status == 1 then  --Inactive			
			if LookingAt == 1 and pipevalve[e].status == 1 then
				if g_valve > 0 then
					tusedvalue[e] = g_valve
					valve_level[e] = (valve_level[e] + 1)											
				end
				if valve_level[e] == pipevalve[e].valve_required then					
					StopAnimation(e)
					SetAnimationSpeed(e,1)					
					SetAnimationName(e,"off")
					PlayAnimation(e)
					PlaySound(e,0)
					pipevalve[e].status = 2					
				end
				if valve_level[e] ~= pipevalve[e].valve_required then	
					activated[e] = 0
					PromptLocal(e,"Valve wheel required",3)
					if pipevalve[e].prompt_display == 1 then PromptLocal(e,pipevalve[e].unused_text) end
					if pipevalve[e].prompt_display == 2 then Prompt(pipevalve[e].unused_text) end
					SetAnimationName(e,"idle")
					PlayAnimation(e)
					pipevalve[e].status = 1
				end
				if g_KeyPressE == 0 then pressed = 0 end
			end			
		end
		if pipevalve[e].status == 2 then --Active
			if LookingAt == 1 and pipevalve[e].status == 2 and activated[e] == 1 then
				if pipevalve[e].prompt_display == 1 then PromptLocal(e,pipevalve[e].used_text) end
				if pipevalve[e].prompt_display == 2 then Prompt(pipevalve[e].used_text) end			
			end
			if LookingAt == 1 and pipevalve[e].status == 2 and activated[e] == 0 then				
				if pipevalve[e].prompt_display == 1 then PromptLocal(e,pipevalve[e].prompt_text) end
				if pipevalve[e].prompt_display == 2 then Prompt(pipevalve[e].prompt_text) end
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
				ActivateIfUsed(e)
				PlaySound(e,2)				
				if tusedvalue[e] > 0 then
					-- if this is a resource, it will deplete qty and set used to zero
					SetEntityUsed(e,tusedvalue[e]*-1)
					use_item_now[e] = 1
				end
				if use_item_now[e] == 1 then
					Destroy(g_valve) -- can only destroy resources that are qty zero
					g_valve = 0
				end
			end
		end
	end
end
