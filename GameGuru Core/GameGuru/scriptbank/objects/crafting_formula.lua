-- Crafting_crafting_formula v2
-- DESCRIPTION: The object will give the player a Crafting Formula if collected.
-- DESCRIPTION: Crafting Formula requirements are entered in the collectable settings.
-- DESCRIPTION: [PROMPT_IF_COLLECTABLE$="E to collect"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Automatic, 2=Manual)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyCraftingFormulas"]
-- DESCRIPTION: <Sound0> for collection sound.

local crafting_formula = {}
local addquantity = {}
local currentvalue = {}

function crafting_formula_properties(e, prompt_if_collectable, pickup_range, pickup_style, user_global_affected)
	crafting_formula[e] = g_Entity[e]
	crafting_formula[e].prompt_if_collectable = prompt_if_collectable
	crafting_formula[e].pickup_range = pickup_range
	crafting_formula[e].pickup_style = pickup_style
	crafting_formula[e].user_global_affected = user_global_affected
end

function crafting_formula_init(e)
	crafting_formula[e] = g_Entity[e]
	crafting_formula[e].prompt_if_collectable = "E to collect"
	crafting_formula[e].pickup_range = 80
	crafting_formula[e].pickup_style = 1
	crafting_formula[e].user_global_affected = "MyCraftingFormulas"
	if GetEntityCollectable(e) == 1 then SetEntityCollectable (e,1) end
	addquantity[e] = 0
	currentvalue = 0
end

function crafting_formula_main(e)
	crafting_formula[e] = g_Entity[e]
	PlayerDist = GetPlayerDistance(e)
	if GetEntityCollectable(e) == 0 and PlayerDist < crafting_formula[e].pickup_range then Prompt("Crafting Formula must have IsCollectable set") end
	if GetEntityCollectable(e) == 1 then
		if crafting_formula[e].pickup_style == 1 then
			if PlayerDist < crafting_formula[e].pickup_range then			
				PlaySound(e,0)
				addquantity[e] = 1
				-- add entry to Crafting Formula Inventory
				-- including data from collectable panel
				-- Destroy(e) from world ??
				Hide(e)
				CollisionOff(e)
				SetEntityCollected(e,1)				
			end
		end
		if crafting_formula[e].pickup_style == 2 then
			local LookingAt = GetPlrLookingAtEx(e,1)
			if LookingAt == 1 and PlayerDist < crafting_formula[e].pickup_range then
				if GetEntityCollected(e) == 0 then
					Prompt(crafting_formula[e].prompt_if_collectable)
					if g_KeyPressE == 1 then
						PlaySound(e,0)
						addquantity[e] = 1
						-- add entry to Crafting Formula Inventory
						-- including data from collectable panel
						-- Destroy(e) from world ??
						Hide(e)
						CollisionOff(e)
						SetEntityCollected(e,1)
					end
				end
			end
		end
	end
	
	if GetEntityCollected(e) == 1 then
		PromptDuration("Crafting Formula added to the Crafting Formula Inventory",3000)
		ResetPosition(e,g_PlayerX,g_PlayerY,g_PlayerZ)		
	end
	
	if addquantity[e] == 1 then
		if crafting_formula[e].user_global_affected > "" then 
			if _G["g_UserGlobal['"..crafting_formula[e].user_global_affected.."']"] ~= nil then currentvalue = _G["g_UserGlobal['"..crafting_formula[e].user_global_affected.."']"] end
			_G["g_UserGlobal['"..crafting_formula[e].user_global_affected.."']"] = currentvalue + 1
		end
	end
end
