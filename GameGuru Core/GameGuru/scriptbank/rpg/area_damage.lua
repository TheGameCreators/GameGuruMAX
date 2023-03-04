-- DESCRIPTION: The object can be collected and used as an area damage effect to kill anything within an area.
-- AreaDamage v1
-- DESCRIPTION: [PROMPT_TEXT$="E to Collect"]
-- DESCRIPTION: [USEAGE_TEXT$="Area Damage Inflicted"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyMana"]
-- DESCRIPTION: [MANA_COST=10(1,100)]
-- DESCRIPTION: <Sound0> when effect successful
-- DESCRIPTION: <Sound1> when effect unsuccessful

local area_damage = {}

function area_damage_properties(e, prompt_text, useage_text, pickup_range, user_global_affected, mana_cost)
	area_damage[e] = g_Entity[e]	
	area_damage[e].prompt_text = prompt_text
	area_damage[e].useage_text = useage_text
	area_damage[e].pickup_range = pickup_range
	area_damage[e].user_global_affected = user_global_affected
	area_damage[e].mana_cost = 10
end

function area_damage_init(e)
	area_damage[e] = g_Entity[e]
	area_damage[e].prompt_text = "E to Collect"
	area_damage[e].useage_text = "Area Damage Inflicted"
	area_damage[e].pickup_range = 80
	area_damage[e].user_global_affected = "MyMana"
	area_damage[e].mana_cost = 10
	area_damage[e].cast_timeout = 0
end

function area_damage_main(e)
	area_damage[e] = g_Entity[e]
	PlayerDist = GetPlayerDistance(e)	
	local LookingAt = GetPlrLookingAtEx(e,1)
	if LookingAt == 1 and PlayerDist < area_damage[e].pickup_range then
		if GetEntityCollectable(e) == 1 then			
			if GetEntityCollected(e) == 0 then
				PromptDuration(area_damage[e].prompt_text,1000)
				if g_KeyPressE == 1 then
					SetEntityCollected(e,1,-1)
				end
			end
		end
	end
	local tusedvalue = GetEntityUsed(e)
	if area_damage[e].cast_timeout > 0 then
		if Timer() > area_damage[e].cast_timeout + 2100 then
			area_damage[e].cast_timeout = 0
		else
			tusedvalue = 0
		end
		SetEntityUsed(e,0)
	end
	if tusedvalue > 0 then
		-- attempt effect
		local mymana = 0 if _G["g_UserGlobal['"..area_damage[e].user_global_affected.."']"] ~= nil then mymana = _G["g_UserGlobal['"..area_damage[e].user_global_affected.."']"] end
		if mymana >= area_damage[e].mana_cost then
			-- enough mana, deduct from player
			mymana = mymana - area_damage[e].mana_cost
			-- do the magic
			for ee = 1, g_EntityElementMax, 1 do
				if e ~= ee then
					if g_Entity[ee] ~= nil then
						if g_Entity[ee]['active'] > 0 then
							if g_Entity[ee]['health'] > 0 then
								local thisallegiance = GetEntityAllegiance(ee)
								if thisallegiance == 0 then
									local thowclosex = g_Entity[ ee ]['x'] - g_PlayerPosX
									local thowclosey = g_Entity[ ee ]['y'] - g_PlayerPosY
									local thowclosez = g_Entity[ ee ]['z'] - g_PlayerPosZ
									local thowclosedd = math.sqrt(math.abs(thowclosex*thowclosex)+math.abs(thowclosey*thowclosey)+math.abs(thowclosez*thowclosez))
									if thowclosedd < 500 then
										SetEntityHealth ( ee, 0 )
									end
								end
							end
						end
					end
				end
			end			
			-- prompt we did it
			PromptDuration(area_damage[e].useage_text,2000)
			area_damage[e].cast_timeout = Timer()
			PlaySound(e,0)
		else
			-- not successful
			PromptDuration("Not enough mana",2000)
			SetEntityUsed(e,0)
			PlaySound(e,1)
		end
		_G["g_UserGlobal['"..area_damage[e].user_global_affected.."']"] = mymana
	end		
end
