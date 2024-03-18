-- DESCRIPTION: When collected can be cast as an Area Damage effect, damaging anything within an area surrounding the player.
-- Area Damage Spell v20
-- DESCRIPTION: [PROMPT_TEXT$="E to collect Area Damage Spell"]
-- DESCRIPTION: [USEAGE_TEXT$="Area Damage Inflicted"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyMana"]
-- DESCRIPTION: [MANA_COST=10(1,100)]
-- DESCRIPTION: [CAST_DAMAGE=500(1,100)]
-- DESCRIPTION: [CAST_RADIUS=50(1,100))]
-- DESCRIPTION: [PLAYER_LEVEL=0(0,100))] player level to be able use this spell
-- DESCRIPTION: [PARTICLE1_NAME$="SpellParticle1"]
-- DESCRIPTION: [PARTICLE2_NAME$="SpellParticle2"]
-- DESCRIPTION: <Sound0> when cast effect successful
-- DESCRIPTION: <Sound1> when cast effect unsuccessful

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
local P = require "scriptbank\\physlib"
g_tEnt = {}

local lower = string.lower
local area_damage_spell 	= {}
local prompt_text 			= {}
local useage_text 			= {}
local pickup_range 			= {}
local user_global_affected 	= {}
local mana_cost 			= {}
local cast_damage 			= {}
local cast_radius 			= {}
local player_level 			= {}
local particle1_name 		= {}
local particle2_name 		= {}

local cast_timeout 		= {}
local tAllegiance 		= {}
local tEnt 				= {}
local selectobj 		= {}
local status 			= {}
local cradius 			= {}
local entaffected 		= {}
local tlevelrequired 	= {}
local tplayerlevel 		= {}
local played			= {}

function area_damage_spell_properties(e, prompt_text, useage_text, pickup_range, user_global_affected, mana_cost, cast_damage, cast_radius, player_level, particle1_name, particle2_name)
	area_damage_spell[e].prompt_text = prompt_text
	area_damage_spell[e].useage_text = useage_text
	area_damage_spell[e].pickup_range = pickup_range
	area_damage_spell[e].user_global_affected = user_global_affected
	area_damage_spell[e].mana_cost = mana_cost
	area_damage_spell[e].cast_damage = cast_damage
	area_damage_spell[e].cast_radius = cast_radius
	area_damage_spell[e].player_level = player_level
	area_damage_spell[e].particle1_name = lower(particle1_name)
	area_damage_spell[e].particle2_name = lower(particle2_name)
end

function area_damage_spell_init(e)
	area_damage_spell[e] = {}
	area_damage_spell[e].prompt_text = "E to Collect"
	area_damage_spell[e].useage_text = "Area Damage Inflicted"
	area_damage_spell[e].pickup_range = 80
	area_damage_spell[e].user_global_affected = "MyMana"
	area_damage_spell[e].mana_cost = 10
	area_damage_spell[e].cast_damage = 500
	area_damage_spell[e].cast_radius = 90
	area_damage_spell[e].player_level = 0	
	area_damage_spell[e].particle1_name = "SpellParticle1"
	area_damage_spell[e].particle2_name = "SpellParticle2"
	area_damage_spell[e].particle1_number = 0
	area_damage_spell[e].particle2_number = 0
	area_damage_spell[e].cast_timeout = 0	
	status[e] = "init"
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
	cradius[e] = 0
	tplayerlevel[e] = 0
	tlevelrequired[e] = 0
	played[e] = 0
end

function area_damage_spell_main(e)

	-- get particles for spell effects
	if area_damage_spell[e].particle1_number == 0 and area_damage_spell[e].particle1_name ~= "" then
		for n = 1, g_EntityElementMax do
			if n ~= nil and g_Entity[n] ~= nil then
				if lower(GetEntityName(n)) == area_damage_spell[e].particle1_name then
					area_damage_spell[e].particle1_number = n
					SetPosition(n,g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
					Hide(n)
					break
				end
			end
		end
	end
	if area_damage_spell[e].particle2_number == 0 and area_damage_spell[e].particle2_name ~= "" then
		for m = 1, g_EntityElementMax do
			if m ~= nil and g_Entity[m] ~= nil then
				if lower(GetEntityName(m)) == area_damage_spell[e].particle2_name then
					area_damage_spell[e].particle2_number = m
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
		tlevelrequired[e] = area_damage_spell[e].player_level
		cradius[e] = 100.0 - area_damage_spell[e].cast_radius
		status[e] = "collect_spell"		
	end
	if status[e] == "collect_spell" then
		local PlayerDist = GetPlayerDistance(e)
		if PlayerDist < area_damage_spell[e].pickup_range then
			--pinpoint select object--
			module_misclib.pinpoint(e,area_damage_spell[e].pickup_range,300)
			tEnt[e] = g_tEnt
			--end pinpoint select object--	
		end	
		if PlayerDist < area_damage_spell[e].pickup_range and tEnt[e] ~= 0 then
			if GetEntityCollectable(e) == 1 then
				if GetEntityCollected(e) == 0 then
					PromptDuration(area_damage_spell[e].prompt_text,1000)
					if g_KeyPressE == 1 then
						SetEntityCollected(e,1)
						status[e] = "have_spell"
					end
				end
			end
		end
	end		

	local tusedvalue = GetEntityUsed(e)		
	if area_damage_spell[e].cast_timeout > 0 then
		if Timer() > area_damage_spell[e].cast_timeout + 2100 then
			area_damage_spell[e].cast_timeout = 0
			-- hide the spell effect particles again
			if area_damage_spell[e].particle1_number > 0 or nil then Hide(area_damage_spell[e].particle1_number) end
			if area_damage_spell[e].particle2_number > 0 or nil then Hide(area_damage_spell[e].particle2_number) end
		else
			-- scale spell to see it radiate outward
			local tscaleradius = 5.0 + ((Timer()-area_damage_spell[e].cast_timeout)/cradius[e])
			if area_damage_spell[e].particle1_number > 0 then Scale(area_damage_spell[e].particle1_number,tscaleradius) end
			if area_damage_spell[e].particle2_number > 0 then Scale(area_damage_spell[e].particle2_number,tscaleradius) end
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
											SetEntityHealth(ee,g_Entity[ee]['health']-area_damage_spell[e].cast_damage)
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
		SetEntityUsed(e,0)
		played[e] = 0
	end

	-- area damage based on player position
	local ttargetanything = 0
	if status[e] == "have_spell" then
		ttargetanything = 1
	end		
	
	if g_MouseClick == 1 and ttargetanything == 1 then SetEntityUsed(e,1) end
	
	if tusedvalue > 0 and ttargetanything == 1 then
		-- check player level		
		if _G["g_UserGlobal['".."MyPlayerLevel".."']"] ~= nil then tplayerlevel[e] = _G["g_UserGlobal['".."MyPlayerLevel".."']"] end
		if tplayerlevel[e] < tlevelrequired[e] then		
			PromptDuration("You need to be level "..tlevelrequired[e].." to cast this spell",1000)
			SetEntityUsed(e,0)
		end	
		if tplayerlevel[e] >= tlevelrequired[e] then
			-- attempt effect
			local mymana = 0 if _G["g_UserGlobal['"..area_damage_spell[e].user_global_affected.."']"] ~= nil then mymana = _G["g_UserGlobal['"..area_damage_spell[e].user_global_affected.."']"] end
			if mymana >= area_damage_spell[e].mana_cost then
				-- enough mana, deduct from player
				mymana = mymana - area_damage_spell[e].mana_cost
				-- setup and show the spell effect particles
				if area_damage_spell[e].particle1_number > 0 or nil then
					ResetPosition(area_damage_spell[e].particle1_number,g_PlayerPosX,g_PlayerPosY+10,g_PlayerPosZ)
					Show(area_damage_spell[e].particle1_number)
				end
				if area_damage_spell[e].particle2_number > 0 or nil then					
					ResetPosition(area_damage_spell[e].particle2_number,g_PlayerPosX,g_PlayerPosY+10,g_PlayerPosZ)					
					Show(area_damage_spell[e].particle2_number)
				end
				-- prepare spell cast
				for ee = 1, g_EntityElementMax, 1 do
					entaffected[ee] = 0
				end
				-- prompt we did it
				PromptDuration(area_damage_spell[e].useage_text,2000)
				area_damage_spell[e].cast_timeout = Timer()
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
			else
				-- not successful
				PromptDuration("Not enough mana",2000)
				SetEntityUsed(e,0)
				if played[e] == 0 then
					PlaySound(e,1)
					played[e] = 1
				end
			end
			_G["g_UserGlobal['"..area_damage_spell[e].user_global_affected.."']"] = mymana
		end
	end
end
