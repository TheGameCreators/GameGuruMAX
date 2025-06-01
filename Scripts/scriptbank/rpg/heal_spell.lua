-- DESCRIPTION: When collected can be cast to heal the player.
-- Heal Spell v24
-- DESCRIPTION: [PROMPT_TEXT$="E to collect Healing Spell"]
-- DESCRIPTION: [USEAGE_TEXT$="You gain some health"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@@USER_GLOBAL_AFFECTED$=""(0=globallist)] eg: MyMana
-- DESCRIPTION: [MANA_COST=10(1,100)]
-- DESCRIPTION: [CAST_VALUE=500(1,100)]
-- DESCRIPTION: [CAST_RADIUS=50(1,100))]
-- DESCRIPTION: [PLAYER_LEVEL=0(0,100))] player level to be able use this spell
-- DESCRIPTION: [PARTICLE1_NAME$=""] eg: SpellParticle1
-- DESCRIPTION: [PARTICLE2_NAME$=""] eg: SpellParticle2
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline)]
-- DESCRIPTION: <Sound0> when effect successful
-- DESCRIPTION: <Sound1> when effect unsuccessful

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
local P = require "scriptbank\\physlib"
g_tEnt = {}

local lower = string.lower
local heal_spell 				= {}
local prompt_text 				= {}
local useage_text 				= {}
local pickup_range 				= {}
local user_global_affected 		= {}
local mana_cost 				= {}
local cast_value 				= {}
local cast_radius 				= {}
local player_level 				= {}
local particle1_name 			= {}
local particle2_name 			= {}
local item_highlight 			= {}

local cast_timeout 		= {}
local tAllegiance 		= {}
local tEnt 				= {}
local tName 			= {}
local sEnt 				= {}
local selectobj 		= {}
local status			= {}
local cradius 			= {}
local doonce 			= {}
local played			= {}
local tlevelrequired	= {}
local tplayerlevel		= {}
local healcount 		= {}
local entaffected 		= {}

function heal_spell_properties(e, prompt_text, useage_text, pickup_range, user_global_affected, mana_cost, cast_value, cast_radius, player_level, particle1_name, particle2_name, item_highlight)
	heal_spell[e] = g_Entity[e]
	heal_spell[e].prompt_text = prompt_text
	heal_spell[e].useage_text = useage_text
	heal_spell[e].pickup_range = pickup_range
	heal_spell[e].user_global_affected = user_global_affected
	heal_spell[e].mana_cost = mana_cost
	heal_spell[e].cast_value = cast_value
	heal_spell[e].cast_radius = cast_radius
	heal_spell[e].player_level = player_level
	heal_spell[e].particle1_name = lower(particle1_name)
	heal_spell[e].particle2_name = lower(particle2_name)
	heal_spell[e].item_highlight = item_highlight
end

function heal_spell_init(e)
	heal_spell[e] = g_Entity[e]
	heal_spell[e].prompt_text = "E to Collect"
	heal_spell[e].useage_text = "You gain some health"
	heal_spell[e].pickup_range = 80
	heal_spell[e].user_global_affected = "MyMana"
	heal_spell[e].mana_cost = 10
	heal_spell[e].cast_value = 500
	heal_spell[e].cast_radius = 90
	heal_spell[e].player_level = 0
	heal_spell[e].particle1_name = ""
	heal_spell[e].particle2_name = ""
	heal_spell[e].particle1_number = 0
	heal_spell[e].particle2_number = 0
	heal_spell[e].item_highlight = 0
	heal_spell[e].cast_timeout = 0	
	status[e] = "init"
	tAllegiance[e] = 0
	tEnt[e] = 0
	tName[e] = ""
	sEnt[e] = 0
	g_tEnt = 0	
	selectobj[e] = 0
	cradius[e] = 0
	doonce[e] = 0
	played[e] = 0
	tplayerlevel[e] = 0
	tlevelrequired[e] = 0
	healcount[e] = 1
end

function heal_spell_main(e)
	heal_spell[e] = g_Entity[e]
	-- get particles for spell effects
	if heal_spell[e].particle1_number == 0 and heal_spell[e].particle1_name ~= "" then
		for n = 1, g_EntityElementMax do
			if n ~= nil and g_Entity[n] ~= nil then
				if lower(GetEntityName(n)) == heal_spell[e].particle1_name then
					heal_spell[e].particle1_number = n
					SetPosition(n,g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
					Hide(n)
					break
				end
			end
		end
	end
	if heal_spell[e].particle2_number == 0 and heal_spell[e].particle2_name ~= "" then
		for m = 1, g_EntityElementMax do
			if m ~= nil and g_Entity[m] ~= nil then
				if lower(GetEntityName(m)) == heal_spell[e].particle2_name then
					heal_spell[e].particle2_number = m
					SetPosition(m,g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
					Hide(m)
					break
				end
			end
		end
	end
	-- handle states
	if status[e] == "init" then
		tplayerlevel[e] = 0
		tlevelrequired[e] = heal_spell[e].player_level
		cradius[e] = 180.0 - heal_spell[e].cast_radius
		status[e] = "collect_spell"		
	end
	if status[e] == "collect_spell" then
		local PlayerDist = GetPlayerDistance(e)
		if PlayerDist < heal_spell[e].pickup_range then
			--pinpoint select object--
			module_misclib.pinpoint(e,heal_spell[e].pickup_range,heal_spell[e].item_highlight)
			sEnt[e] = g_tEnt
			--end pinpoint select object--
		end	
		if PlayerDist < heal_spell[e].pickup_range and sEnt[e] ~= 0 then
			if GetEntityCollectable(e) == 1 then
				if GetEntityCollected(e) == 0 then
					PromptDuration(heal_spell[e].prompt_text,1000)
					if g_KeyPressE == 1 then
						SetEntityCollected(e,1)
						status[e] = "have_spell"
					end
				end
			end
		end
	end	

	local tusedvalue = GetEntityUsed(e)
	
	if heal_spell[e].cast_timeout > 0 then
		if Timer() > heal_spell[e].cast_timeout + 2100 then
			heal_spell[e].cast_timeout = 0
			-- hide the spell effect particles again
			if heal_spell[e].particle1_number > 0 then Hide(heal_spell[e].particle1_number) end
			if heal_spell[e].particle2_number > 0 then Hide(heal_spell[e].particle2_number) end
		else
			-- scale spell to see it radiate outward
			local tscaleradius = 5.0 + ((Timer()-heal_spell[e].cast_timeout)/cradius[e])
			if heal_spell[e].particle1_number > 0 then Scale(heal_spell[e].particle1_number,tscaleradius) end
			if heal_spell[e].particle2_number > 0 then Scale(heal_spell[e].particle2_number,tscaleradius) end
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
											SetEntityHealth(ee,g_Entity[ee]['health'] + heal_spell[e].cast_value/3)						-- Heal nearby entities in range a small amount													
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
			if g_PlayerHealth < g_PlayerStartStrength then SetPlayerHealth(g_PlayerHealth + heal_spell[e].cast_value)	end				-- Heal player if doesnt exceeed start health
			if g_PlayerHealth > g_PlayerStartStrength then SetPlayerHealth(g_PlayerStartStrength) end	
			doonce[e] = 1
		end	
		SetEntityUsed(e,0)
		played[e] = 0		
	end

	-- heal area based on player position
	local ttargetanything = 0
	if status[e] == "have_spell" then
		ttargetanything = 1
	end	

	if g_MouseClick == 1 and ttargetanything == 1 then SetEntityUsed(e,1) end
	
	if tusedvalue > 0 and ttargetanything == 1 then
		doonce[e] = 0
		-- check player level		
		if _G["g_UserGlobal['".."MyPlayerLevel".."']"] ~= nil then tplayerlevel[e] = _G["g_UserGlobal['".."MyPlayerLevel".."']"] end
		if tplayerlevel[e] < tlevelrequired[e] then		
			PromptDuration("You need to be level "..tlevelrequired[e].." to cast this spell",1000)
			SetEntityUsed(e,0)
		end	
		if tplayerlevel[e] >= tlevelrequired[e] then
		-- attempt effect
			local mymana = 0 if _G["g_UserGlobal['"..heal_spell[e].user_global_affected.."']"] ~= nil then mymana = _G["g_UserGlobal['"..heal_spell[e].user_global_affected.."']"] end
			if mymana >= heal_spell[e].mana_cost then
				-- enough mana, deduct from player
				mymana = mymana - heal_spell[e].mana_cost
				-- setup and show the spell effect particles
				if heal_spell[e].particle1_number > 0 then
					ResetPosition(heal_spell[e].particle1_number,g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
					Show(heal_spell[e].particle1_number)
				end
				if heal_spell[e].particle2_number > 0 then
					ResetPosition(heal_spell[e].particle2_number,g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)				
					Show(heal_spell[e].particle2_number)
				end
				-- prepare spell cast
				for ee = 1, g_EntityElementMax, 1 do
					entaffected[ee] = 0
				end
				-- prompt we did it
				PromptDuration(heal_spell[e].useage_text,2000)
				heal_spell[e].cast_timeout = Timer()
				PlaySound(e,0)
			else
				-- not successful
				PromptDuration("Not enough mana",2000)
				SetEntityUsed(e,0)
				if played[e] == 0 then
					PlaySound(e,1)
					played[e] = 1
				end
			end
			_G["g_UserGlobal['"..heal_spell[e].user_global_affected.."']"] = mymana
		end
	end
end
