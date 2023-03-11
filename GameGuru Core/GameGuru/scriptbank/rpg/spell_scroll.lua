-- DESCRIPTION: The object can be collected and used as a spell effect to activate all characters within an area.
-- Spell Scroll v4
-- DESCRIPTION: [PROMPT_TEXT$="E to Collect"]
-- DESCRIPTION: [USEAGE_TEXT$="Spell Cast"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyMana"]
-- DESCRIPTION: [MANA_COST=10(1,100)]
-- DESCRIPTION: <Sound0> when spell successful
-- DESCRIPTION: <Sound1> when spell unsuccessful

local spell_scroll = {}

function spell_scroll_properties(e, prompt_text, useage_text, pickup_range, user_global_affected, mana_cost)
	spell_scroll[e].prompt_text = prompt_text
	spell_scroll[e].useage_text = useage_text
	spell_scroll[e].pickup_range = pickup_range
	spell_scroll[e].user_global_affected = user_global_affected
	spell_scroll[e].mana_cost = 10
end

function spell_scroll_init(e)
	spell_scroll[e] = {}
	spell_scroll[e].prompt_text = "E to Collect"
	spell_scroll[e].useage_text = "Spell cast"
	spell_scroll[e].pickup_range = 80
	spell_scroll[e].user_global_affected = "MyMana"
	spell_scroll[e].mana_cost = 10
	spell_scroll[e].cast_timeout = 0
end

function spell_scroll_main(e)
	PlayerDist = GetPlayerDistance(e)	
	local LookingAt = GetPlrLookingAtEx(e,1)
	if LookingAt == 1 and PlayerDist < spell_scroll[e].pickup_range then
		if GetEntityCollectable(e) == 1 then			
			if GetEntityCollected(e) == 0 then
				PromptDuration(spell_scroll[e].prompt_text,1000)
				if g_KeyPressE == 1 then
					SetEntityCollected(e,1,-1)
				end
			end
		end
	end
	local tusedvalue = GetEntityUsed(e)
	if spell_scroll[e].cast_timeout > 0 then
		if Timer() > spell_scroll[e].cast_timeout + 2100 then
			spell_scroll[e].cast_timeout = 0
		else
			tusedvalue = 0
		end
		SetEntityUsed(e,0)
	end
	if tusedvalue > 0 then
		-- attempt effect
		local mymana = 0 if _G["g_UserGlobal['"..spell_scroll[e].user_global_affected.."']"] ~= nil then mymana = _G["g_UserGlobal['"..spell_scroll[e].user_global_affected.."']"] end
		if mymana >= spell_scroll[e].mana_cost then
			-- enough mana, deduct from player
			mymana = mymana - spell_scroll[e].mana_cost
			-- do the magic
			for ee = 1, g_EntityElementMax, 1 do
				if e ~= ee then
					if g_Entity[ee] ~= nil then
						if g_Entity[ee]['active'] > 0 then
							if g_Entity[ee]['health'] > 0 then
								local thisallegiance = GetEntityAllegiance(ee)
								if thisallegiance >= 0 then
									local thowclosex = g_Entity[ ee ]['x'] - g_PlayerPosX
									local thowclosey = g_Entity[ ee ]['y'] - g_PlayerPosY
									local thowclosez = g_Entity[ ee ]['z'] - g_PlayerPosZ
									local thowclosedd = math.sqrt(math.abs(thowclosex*thowclosex)+math.abs(thowclosey*thowclosey)+math.abs(thowclosez*thowclosez))
									if thowclosedd < 500 then
										SetActivated(ee,1)
										PerformLogicConnections(ee)
										RotateToPlayer(ee)
									end
								end
							end
						end
					end
				end
			end			
			-- prompt we did it
			PromptDuration(spell_scroll[e].useage_text,2000)
			spell_scroll[e].cast_timeout = Timer()
			PlaySound(e,0)
		else
			-- not successful
			PromptDuration("Not enough mana",2000)
			SetEntityUsed(e,0)
			PlaySound(e,1)
		end
		_G["g_UserGlobal['"..spell_scroll[e].user_global_affected.."']"] = mymana
	end		
end
