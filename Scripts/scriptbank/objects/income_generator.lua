-- Income Generator v7 by Necrym59 with thanks to oosayeroo
-- DESCRIPTION: The object will give the player a generated income value over time to a User Global.
-- DESCRIPTION: [COLLECTION_RANGE=80(1,100)]
-- DESCRIPTION: [@COLLECTION_STYLE=1(1=Ranged, 2=Manual, 3=Automatic)]
-- DESCRIPTION: [COLLECTION_PROMPT$="E to Collect"]
-- DESCRIPTION: [COLLECTED_TEXT$="Income Collected"]
-- DESCRIPTION: [GENERATING_TEXT$="Accumulating Income"]
-- DESCRIPTION: [INCOME=10(0,500)]
-- DESCRIPTION: [UPKEEP=10(0,500)]
-- DESCRIPTION: [ELAPSE_TIME=10(1,100)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$=""] Your user global eg: 'MyMoney'
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline)]
-- DESCRIPTION: [ACTIVE_AT_START!=1] if unchecked use a switch or other trigger to activate this object
-- DESCRIPTION: <Sound0> for collection sound.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local ic_generator			= {}
local collection_range		= {}
local collection_style		= {}
local collection_prompt		= {}
local collected_text		= {}
local generating_text		= {}
local income				= {}
local upkeep				= {}
local collection_style		= {}
local elapse_time			= {}
local user_global_affected	= {}
local prompt_display 		= {}
local item_highlight		= {}
local active_at_start		= {}

local tEnt				= {}
local selectobj			= {}
local played			= {}
local currentvalue		= {}
local collectiontime	= {}
local status			= {}

function income_generator_properties(e, collection_range, collection_style, collection_prompt, collected_text, generating_text, income, upkeep, elapse_time, user_global_affected, prompt_display, item_highlight, active_at_start)
	ic_generator[e].collection_range = collection_range
	ic_generator[e].collection_style = collection_style	
	ic_generator[e].collection_prompt = collection_prompt
	ic_generator[e].collected_text = collected_text
	ic_generator[e].generating_text = generating_text
	ic_generator[e].income = income
	ic_generator[e].upkeep = upkeep
	ic_generator[e].elapse_time = elapse_time	
	ic_generator[e].user_global_affected = user_global_affected or ""
	ic_generator[e].prompt_display = prompt_display
	ic_generator[e].item_highlight = item_highlight	
	ic_generator[e].active_at_start = active_at_start or 1
end

function income_generator_init(e)
	ic_generator[e] = {}
	ic_generator[e].collection_range = 80
	ic_generator[e].collection_style = 1	
	ic_generator[e].collection_prompt = "E to Collect"
	ic_generator[e].collected_text = "Income Collected"
	ic_generator[e].generating_text = "Accumulating Income"
	ic_generator[e].income = 1
	ic_generator[e].upkeep = 1
	ic_generator[e].elapse_time = 10
	ic_generator[e].user_global_affected = ""
	ic_generator[e].prompt_display = 1
	ic_generator[e].item_highlight = 0
	ic_generator[e].active_at_start = 1	
	
	collectiontime[e] = 0
	currentvalue[e] = 0
	played[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
	status[e] = "init"
end

function income_generator_main(e)

	if status[e] == "init" then	
		if ic_generator[e].ActiveAtStart == 0 then SetActivated(e,0) end
		if ic_generator[e].ActiveAtStart == 1 then SetActivated(e,1) end
		if ic_generator[e].collection_style == 1 or ic_generator[e].collection_style == 2 then
			status[e] = "ready"
		end
		if ic_generator[e].collection_style == 3 then
			collectiontime[e] = GetTimer(e) + ic_generator[e].elapse_time * 1000
			status[e] = "collected"
		end
	end
	
	local PlayerDist = GetPlayerDistance(e)	
	
	if g_Entity[e]['activated'] == 1 then	
		if ic_generator[e].collection_style == 1 and PlayerDist < ic_generator[e].collection_range and GetEntityVisibility(e) == 1 then
			if status[e] == "ready" then
				if ic_generator[e].prompt_display == 1 then PromptLocal(e,ic_generator[e].collected_text) end
				if ic_generator[e].prompt_display == 2 then Prompt(ic_generator[e].collected_text) end			
				PerformLogicConnections(e)
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
				if ic_generator[e].user_global_affected > "" then
					if _G["g_UserGlobal['"..ic_generator[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..ic_generator[e].user_global_affected.."']"] end
					_G["g_UserGlobal['"..ic_generator[e].user_global_affected.."']"] = currentvalue[e] + ic_generator[e].income
					currentvalue[e] = _G["g_UserGlobal['"..ic_generator[e].user_global_affected.."']"]
					if currentvalue[e] - ic_generator[e].upkeep < 0 then
						_G["g_UserGlobal['"..ic_generator[e].user_global_affected.."']"] = 0
					else
						_G["g_UserGlobal['"..ic_generator[e].user_global_affected.."']"] = currentvalue[e] - ic_generator[e].upkeep
					end
				end				
				status[e] = "collected"
				if collectiontime[e] == 0 then
					collectiontime[e] = GetTimer(e) + ic_generator[e].elapse_time * 1000
				end		
			else
				if collectiontime[e] < GetTimer(e) + ic_generator[e].elapse_time * 1000 / 2 then
					if ic_generator[e].prompt_display == 1 then PromptLocal(e,ic_generator[e].generating_text) end
					if ic_generator[e].prompt_display == 2 then Prompt(ic_generator[e].generating_text) end
				end
			end
		end
		
		if ic_generator[e].collection_style == 2 and PlayerDist < ic_generator[e].collection_range then
			--pinpoint select object--
			module_misclib.pinpoint(e,ic_generator[e].collection_range,ic_generator[e].item_highlight)
			tEnt[e] = g_tEnt
			--end pinpoint select object--
			
			if PlayerDist < ic_generator[e].collection_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then
				if status[e] == "ready" then
					if ic_generator[e].prompt_display == 1 then PromptLocal(e,ic_generator[e].collection_prompt) end
					if ic_generator[e].prompt_display == 2 then Prompt(ic_generator[e].collection_prompt) end			
				else
					if ic_generator[e].prompt_display == 1 then PromptLocal(e,ic_generator[e].generating_text) end
					if ic_generator[e].prompt_display == 2 then Prompt(ic_generator[e].generating_text) end
				end
				if g_KeyPressE == 1 then
					if status[e] == "ready" then
						if ic_generator[e].prompt_display == 1 then PromptLocal(e,ic_generator[e].collected_text) end
						if ic_generator[e].prompt_display == 2 then Prompt(ic_generator[e].collected_text) end	
						PerformLogicConnections(e)
						if played[e] == 0 then
							PlaySound(e,0)
							played[e] = 1
						end
						if ic_generator[e].user_global_affected > "" then
							if _G["g_UserGlobal['"..ic_generator[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..ic_generator[e].user_global_affected.."']"] end
							_G["g_UserGlobal['"..ic_generator[e].user_global_affected.."']"] = currentvalue[e] + ic_generator[e].income
							currentvalue[e] = _G["g_UserGlobal['"..ic_generator[e].user_global_affected.."']"]
							if currentvalue[e] - ic_generator[e].upkeep < 0 then
								_G["g_UserGlobal['"..ic_generator[e].user_global_affected.."']"] = 0
							else
								_G["g_UserGlobal['"..ic_generator[e].user_global_affected.."']"] = currentvalue[e] - ic_generator[e].upkeep
							end
						end	
						
						tEnt[e] = 0	
						status[e] = "collected"
						if collectiontime[e] == 0 then
							collectiontime[e] = GetTimer(e) + ic_generator[e].elapse_time * 1000
						end	
					else
						if collectiontime[e] < GetTimer(e) + ic_generator[e].elapse_time * 1000 / 2 then
							if ic_generator[e].prompt_display == 1 then PromptLocal(e,ic_generator[e].generating_text) end
							if ic_generator[e].prompt_display == 2 then Prompt(ic_generator[e].generating_text) end
						end
					end			
				end
			end
		end
		
		if ic_generator[e].collection_style == 3 then
			if status[e] == "ready" then
				if ic_generator[e].prompt_display == 1 then PromptLocal(e,ic_generator[e].collected_text) end
				if ic_generator[e].prompt_display == 2 then Prompt(ic_generator[e].collected_text) end		
				PerformLogicConnections(e)
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
				if ic_generator[e].user_global_affected > "" then
					if _G["g_UserGlobal['"..ic_generator[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..ic_generator[e].user_global_affected.."']"] end
					_G["g_UserGlobal['"..ic_generator[e].user_global_affected.."']"] = currentvalue[e] + ic_generator[e].income
					currentvalue[e] = _G["g_UserGlobal['"..ic_generator[e].user_global_affected.."']"]
					if currentvalue[e] - ic_generator[e].upkeep < 0 then
						_G["g_UserGlobal['"..ic_generator[e].user_global_affected.."']"] = 0
					else
						_G["g_UserGlobal['"..ic_generator[e].user_global_affected.."']"] = currentvalue[e] - ic_generator[e].upkeep
					end
				end	
				
				status[e] = "collected"
				if collectiontime[e] == 0 then
					collectiontime[e] = GetTimer(e) + ic_generator[e].elapse_time * 1000
				end		
			end
			if collectiontime[e] < GetTimer(e) + ic_generator[e].elapse_time * 1000 / 2 then			
				if ic_generator[e].prompt_display == 1 then PromptLocal(e,ic_generator[e].generating_text) end
				if ic_generator[e].prompt_display == 2 then Prompt(ic_generator[e].generating_text) end
			end	
		end
		if status[e] == "collected" then
			if GetTimer(e) >= collectiontime[e] then
				status[e] = "ready"
				collectiontime[e] = 0
				played[e] = 0
			end
		end
	end	
end
