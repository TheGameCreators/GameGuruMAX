-- Spell Scroll v2
-- DESCRIPTION: The object will give the player a Spell Scroll if collected.
-- DESCRIPTION: Spell Scroll details are entered in the collectable settings.
-- DESCRIPTION: [PROMPT_IF_COLLECTABLE$="E to collect"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Automatic, 2=Manual)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MySpellScrolls"]
-- DESCRIPTION: <Sound0> for collection sound.

local spell_scroll = {}
local addquantity = {}
local currentvalue = {}

function spell_scroll_properties(e, prompt_if_collectable, pickup_range, pickup_style, user_global_affected)
	spell_scroll[e] = g_Entity[e]
	spell_scroll[e].prompt_if_collectable = prompt_if_collectable
	spell_scroll[e].pickup_range = pickup_range
	spell_scroll[e].pickup_style = pickup_style
	spell_scroll[e].user_global_affected = user_global_affected
end

function spell_scroll_init(e)
	spell_scroll[e] = g_Entity[e]
	spell_scroll[e].prompt_if_collectable = "E to collect"
	spell_scroll[e].pickup_range = 80
	spell_scroll[e].pickup_style = 1
	spell_scroll[e].user_global_affected = "MySpellScrolls"
	if GetEntityCollectable(e) == 1 then SetEntityCollectable (e,1) end
	addquantity[e] = 0
	currentvalue = 0
end

function spell_scroll_main(e)
	spell_scroll[e] = g_Entity[e]
	PlayerDist = GetPlayerDistance(e)
	if GetEntityCollectable(e) == 0 and PlayerDist < spell_scroll[e].pickup_range then Prompt("Crafting Formula must have IsCollectable set") end
	if GetEntityCollectable(e) == 1 then
		if spell_scroll[e].pickup_style == 1 then
			if PlayerDist < spell_scroll[e].pickup_range then			
				PlaySound(e,0)
				addquantity[e] = 1
				-- add entry to Spell Scroll Inventory
				-- including data from collectable panel (eg: Direct Damage: 120, Mana Cost: 20, Level Required: 3)
				-- Destroy(e) from world ??
				Hide(e)
				CollisionOff(e)
				SetEntityCollected(e,1)				
			end
		end
		if spell_scroll[e].pickup_style == 2 then
			local LookingAt = GetPlrLookingAtEx(e,1)
			if LookingAt == 1 and PlayerDist < spell_scroll[e].pickup_range then
				if GetEntityCollected(e) == 0 then
					Prompt(spell_scroll[e].prompt_if_collectable)
					if g_KeyPressE == 1 then
						PlaySound(e,0)
						addquantity[e] = 1
						-- add entry to Spell Scroll Inventory
						-- including data from collectable panel (eg: Direct Damage: 120, Mana Cost: 20, Level Required: 3)
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
		PromptDuration("Spell Scroll added to the Spell Scroll Inventory",3000)
		ResetPosition(e,g_PlayerX,g_PlayerY,g_PlayerZ)		
	end
	
	if addquantity[e] == 1 then
		if spell_scroll[e].user_global_affected > "" then 
			if _G["g_UserGlobal['"..spell_scroll[e].user_global_affected.."']"] ~= nil then currentvalue = _G["g_UserGlobal['"..spell_scroll[e].user_global_affected.."']"] end
			_G["g_UserGlobal['"..spell_scroll[e].user_global_affected.."']"] = currentvalue + 1
		end
	end
end
