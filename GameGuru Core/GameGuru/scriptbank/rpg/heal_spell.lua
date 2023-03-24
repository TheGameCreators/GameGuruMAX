-- DESCRIPTION: When collected can be cast to heal the player.
-- Heal Spell v13
-- DESCRIPTION: [PROMPT_TEXT$="E to Collect"]
-- DESCRIPTION: [USEAGE_TEXT$="You gain some health"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyMana"]
-- DESCRIPTION: [MANA_COST=10(1,100)]
-- DESCRIPTION: [CAST_VALUE=500(1,100)]
-- DESCRIPTION: [CAST_RADIUS=50(1,100))]
-- DESCRIPTION: [PARTICLE1_NAME$="SpellParticle1"]
-- DESCRIPTION: [PARTICLE2_NAME$="SpellParticle2"]
-- DESCRIPTION: <Sound0> when effect successful
-- DESCRIPTION: <Sound1> when effect unsuccessful

g_heal_spell = {}

local U = require "scriptbank\\utillib"
local P = require "scriptbank\\physlib"
local lower = string.lower

local cast_timeout = {}
local tAllegiance = {}
local tEnt = {}
local tName = {}
local status = {}
local cradius = {}
local doonce = {}
local healcount = {}
local entaffected = {}

function heal_spell_properties(e, prompt_text, useage_text, pickup_range, user_global_affected, mana_cost, cast_value, cast_radius, particle1_name, particle2_name)
	g_heal_spell[e].prompt_text = prompt_text
	g_heal_spell[e].useage_text = useage_text
	g_heal_spell[e].pickup_range = pickup_range
	g_heal_spell[e].user_global_affected = user_global_affected
	g_heal_spell[e].mana_cost = 10
	g_heal_spell[e].cast_value = cast_value
	g_heal_spell[e].cast_radius = cast_radius
	g_heal_spell[e].particle1_name = lower(particle1_name)
	g_heal_spell[e].particle2_name = lower(particle2_name)
end

function heal_spell_init(e)
	g_heal_spell[e] = {}
	g_heal_spell[e].prompt_text = "E to Collect"
	g_heal_spell[e].useage_text = "Area Damage Inflicted"
	g_heal_spell[e].pickup_range = 80
	g_heal_spell[e].user_global_affected = "MyMana"
	g_heal_spell[e].mana_cost = 10
	g_heal_spell[e].cast_value = 500
	g_heal_spell[e].cast_radius = 90
	g_heal_spell[e].particle1_name = "SpellParticle1"
	g_heal_spell[e].particle2_name = "SpellParticle2"
	g_heal_spell[e].particle1_number = 0
	g_heal_spell[e].particle2_number = 0
	g_heal_spell[e].cast_timeout = 0	
	status[e] = "init"
	tAllegiance[e] = 0
	tEnt[e] = 0
	tName[e] = ""
	cradius[e] = 0
	doonce[e] = 0
	healcount[e] = 1
end

function heal_spell_main(e)
	-- get particles for spell effects
	if g_heal_spell[e].particle1_number == 0 or nil then
		for n = 1, g_EntityElementMax do
			if n ~= nil and g_Entity[n] ~= nil then
				if lower(GetEntityName(n)) == g_heal_spell[e].particle1_name then
					g_heal_spell[e].particle1_number = n
					SetPosition(n,g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
					Hide(n)
					break
				end
			end
		end
	end
	if g_heal_spell[e].particle2_number == 0 or nil then
		for m = 1, g_EntityElementMax do
			if m ~= nil and g_Entity[m] ~= nil then
				if lower(GetEntityName(m)) == g_heal_spell[e].particle2_name then
					g_heal_spell[e].particle2_number = m
					SetPosition(m,g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
					Hide(m)
					break
				end
			end
		end
	end
	-- handle states
	if status[e] == "init" then
		cradius[e] = 180.0 - g_heal_spell[e].cast_radius
		status[e] = "collect_spell"		
	end
	if status[e] == "collect_spell" then
		PlayerDist = GetPlayerDistance(e)
		local LookingAt = GetPlrLookingAtEx(e,1)
		if LookingAt == 1 and PlayerDist < g_heal_spell[e].pickup_range then
			if GetEntityCollectable(e) == 1 then
				if GetEntityCollected(e) == 0 then
					PromptDuration(g_heal_spell[e].prompt_text,1000)
					if g_KeyPressE == 1 then
						SetEntityCollected(e,1,-1)
						status[e] = "have_spell"
					end
				end
			end
		end
	end	

	local tusedvalue = GetEntityUsed(e)
	if g_heal_spell[e].cast_timeout > 0 then
		if Timer() > g_heal_spell[e].cast_timeout + 2100 then
			g_heal_spell[e].cast_timeout = 0
			-- hide the spell effect particles again
			if g_heal_spell[e].particle1_number > 0 or nil then Hide(g_heal_spell[e].particle1_number) end
			if g_heal_spell[e].particle1_number > 0 or nil then Hide(g_heal_spell[e].particle2_number) end
		else
			-- scale spell to see it radiate outward
			local tscaleradius = 5.0 + ((Timer()-g_heal_spell[e].cast_timeout)/cradius[e])
			if g_heal_spell[e].particle1_number > 0 then Scale(g_heal_spell[e].particle1_number,tscaleradius) end
			if g_heal_spell[e].particle1_number > 0 then Scale(g_heal_spell[e].particle2_number,tscaleradius) end
			-- apply effect as radius increases
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
									if thowclosedd < tscaleradius*2.0 then
										if entaffected[ee] == 0 then
											entaffected[ee] = 1
											SetEntityHealth(ee,g_Entity[ee]['health'] + g_heal_spell[e].cast_value/3)						-- Heal nearby entities in range a small amount													
										end
									end
								end
							end
						end
					end
				end
			end
			-- cannot use while in effect
			tusedvalue = 0					
		end
		if doonce[e]== 0 then
			if g_PlayerHealth < g_PlayerStartStrength then SetPlayerHealth(g_PlayerHealth + g_heal_spell[e].cast_value)	end				-- Heal player if doesnt exceeed start health
			if g_PlayerHealth > g_PlayerStartStrength then SetPlayerHealth(g_PlayerStartStrength) end	
			doonce[e] = 1
		end	
		SetEntityUsed(e,0)		
	end

	-- area damage based on player position
	local ttargetanything = 0
	if status[e] == "have_spell" then
		ttargetanything = 1
	end	
	if tusedvalue > 0 and ttargetanything == 1 then
		doonce[e] = 0
		-- attempt effect
		local mymana = 0 if _G["g_UserGlobal['"..g_heal_spell[e].user_global_affected.."']"] ~= nil then mymana = _G["g_UserGlobal['"..g_heal_spell[e].user_global_affected.."']"] end
		if mymana >= g_heal_spell[e].mana_cost then
			-- enough mana, deduct from player
			mymana = mymana - g_heal_spell[e].mana_cost
			-- setup and show the spell effect particles
			if g_heal_spell[e].particle1_number > 0 or nil then
				ResetPosition(g_heal_spell[e].particle1_number,g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
				Show(g_heal_spell[e].particle1_number)
			end
			if g_heal_spell[e].particle2_number > 0 or nil then
				ResetPosition(g_heal_spell[e].particle2_number,g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)				
				Show(g_heal_spell[e].particle2_number)
			end
			-- prepare spell cast
			for ee = 1, g_EntityElementMax, 1 do
				entaffected[ee] = 0
			end
			-- prompt we did it
			PromptDuration(g_heal_spell[e].useage_text,2000)
			g_heal_spell[e].cast_timeout = Timer()
			PlaySound(e,0)
		else
			-- not successful
			PromptDuration("Not enough mana",2000)
			SetEntityUsed(e,0)
			PlaySound(e,1)
		end
		_G["g_UserGlobal['"..g_heal_spell[e].user_global_affected.."']"] = mymana
	end
end
