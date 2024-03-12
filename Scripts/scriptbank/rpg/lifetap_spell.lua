-- DESCRIPTION: When collected can cast Lifetap effect to take health from the target and give to the player.
-- Lifetap Spell v20
-- DESCRIPTION: [PROMPT_TEXT$="E to collect Lifetap Spell, T or RMB to target"]
-- DESCRIPTION: [USEAGE_TEXT$="You cast Lifetap and gained some health"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyMana"]
-- DESCRIPTION: [MANA_COST=10(1,100)]
-- DESCRIPTION: [CAST_DAMAGE=25(1,100)]
-- DESCRIPTION: [CAST_RADIUS=5(1,100))]
-- DESCRIPTION: [PLAYER_LEVEL=0(0,100))] player level to be able use this spell
-- DESCRIPTION: [PARTICLE1_NAME$="SpellParticle1"]
-- DESCRIPTION: [PARTICLE2_NAME$="SpellParticle2"]
-- DESCRIPTION: <Sound0> when effect successful
-- DESCRIPTION: <Sound1> when effect unsuccessful

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
local P = require "scriptbank\\physlib"
g_tEnt = {}

local lower = string.lower
local lifetap_spell 		= {}
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
local tName				= {}
local tHealth			= {}
local tTarget			= {}
local sEnt				= {}
local selectobj			= {}
local status			= {}
local cradius			= {}
local casttarget		= {}
local tlevelrequired	= {}
local tplayerlevel		= {}
local played			= {}
local entaffected		= {}

function lifetap_spell_properties(e, prompt_text, useage_text, pickup_range, user_global_affected, mana_cost, cast_damage, cast_radius, player_level, particle1_name, particle2_name)
	lifetap_spell[e].prompt_text = prompt_text
	lifetap_spell[e].useage_text = useage_text
	lifetap_spell[e].pickup_range = pickup_range
	lifetap_spell[e].user_global_affected = user_global_affected
	lifetap_spell[e].mana_cost = mana_cost
	lifetap_spell[e].cast_damage = cast_damage
	lifetap_spell[e].cast_radius = cast_radius
	lifetap_spell[e].player_level = player_level
	lifetap_spell[e].particle1_name = lower(particle1_name)
	lifetap_spell[e].particle2_name = lower(particle2_name)
end

function lifetap_spell_init(e)
	lifetap_spell[e] = {}
	lifetap_spell[e].prompt_text = "E to Collect"
	lifetap_spell[e].useage_text = "Direct Damage Inflicted"
	lifetap_spell[e].pickup_range = 90
	lifetap_spell[e].user_global_affected = "MyMana"
	lifetap_spell[e].mana_cost = 10
	lifetap_spell[e].cast_damage = 25
	lifetap_spell[e].cast_radius = 5
	lifetap_spell[e].player_level = 0
	lifetap_spell[e].particle1_name = ""
	lifetap_spell[e].particle2_name = ""
	lifetap_spell[e].particle1_number = 0
	lifetap_spell[e].particle2_number = 0
	lifetap_spell[e].cast_timeout = 0	
	status[e] = "init"
	tAllegiance[e] = 0
	tEnt[e] = 0
	tName[e] = ""
	tHealth[e] = 0
	tTarget[e] = 0
	sEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
	cradius[e] = 0
	casttarget[e] = 0
	played[e] = 0
	tplayerlevel[e] = 0
	tlevelrequired[e] = 0
end

function lifetap_spell_main(e)

	-- get particles for spell effects
	if lifetap_spell[e].particle1_number == 0 or nil then
		for n = 1, g_EntityElementMax do
			if n ~= nil and g_Entity[n] ~= nil then
				if lower(GetEntityName(n)) == lifetap_spell[e].particle1_name then
					lifetap_spell[e].particle1_number = n
					SetPosition(n,g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
					Hide(n)
					break
				end
			end
		end
	end
	if lifetap_spell[e].particle2_number == 0 or nil then
		for m = 1, g_EntityElementMax do
			if m ~= nil and g_Entity[m] ~= nil then
				if lower(GetEntityName(m)) == lifetap_spell[e].particle2_name then
					lifetap_spell[e].particle2_number = m
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
		tlevelrequired[e] = lifetap_spell[e].player_level
		cradius[e] = 180.0 - lifetap_spell[e].cast_radius
		status[e] = "collect_spell"		
	end
	if status[e] == "collect_spell" then
		local PlayerDist = GetPlayerDistance(e)
		if PlayerDist < lifetap_spell[e].pickup_range then
			--pinpoint select object--
			module_misclib.pinpoint(e,lifetap_spell[e].pickup_range,300)
			sEnt[e] = g_tEnt
			--end pinpoint select object--	
		end	
		if PlayerDist < lifetap_spell[e].pickup_range and sEnt[e] ~= 0 then
			if GetEntityCollectable(e) == 1 then
				if GetEntityCollected(e) == 0 then
					PromptDuration(lifetap_spell[e].prompt_text,1000)
					if g_KeyPressE == 1 then
						SetEntityCollected(e,1)
						status[e] = "have_spell"
					end
				end
			end
		end
	end	
	
	if status[e] == "have_spell" then	
		--- Select Entity to target ---		
		if g_InKey == "t" or g_InKey == "T" or g_MouseClick == 2 and casttarget[e] == 0 then
			TextCenterOnXColor(50-0.01,50,3,"+",255,255,255)							-- temp targeting crosshair
			local t = U.ObjectPlayerLookingAt(2000,0)									-- get object # in view
			tEnt[e] = P.ObjectToEntity(t)												-- get entity # of object
			tName[e] = GetEntityName(tEnt[e])											-- get entity name			
			if tEnt[e] ~= nil then tHealth[e] = g_Entity[tEnt[e]]['health'] end			-- get entity health
			tAllegiance[e] = GetEntityAllegiance(tEnt[e]) 								-- get the allegiance value for this entity (0-enemy, 1-ally, 2-neutral)
			if tAllegiance[e] == 0 and tEnt[e] ~= nil then								-- if allegiance = enemy then give option to target
				tTarget[e] = tEnt[e]													-- entity set as target								
			end
			if tTarget[e] > 0 then
				TextCenterOnX(50,20,3,"TARGETED")
				if tAllegiance[e] == 0 then TextCenterOnX(50,22,3,tName[e]) end
			end	
		end	

		local tusedvalue = GetEntityUsed(e)
		if g_MouseClick == 1 and tTarget[e] ~= 0 then SetEntityUsed(e,1) end
		
		if lifetap_spell[e].cast_timeout > 0 then
			if Timer() > lifetap_spell[e].cast_timeout + 2100 then
				lifetap_spell[e].cast_timeout = 0
				-- hide the spell effect particles again
				if lifetap_spell[e].particle1_number > 0 or nil then Hide(lifetap_spell[e].particle1_number) end
				if lifetap_spell[e].particle2_number > 0 or nil then Hide(lifetap_spell[e].particle2_number) end
				casttarget[e] = 0
			else
				-- scale spell to see it radiate outward
				local tscaleradius = 5.0 + ((Timer()-lifetap_spell[e].cast_timeout)/cradius[e])
				if lifetap_spell[e].particle1_number > 0 then Scale(lifetap_spell[e].particle1_number,tscaleradius) end
				if lifetap_spell[e].particle2_number > 0 then Scale(lifetap_spell[e].particle2_number,tscaleradius) end
				-- apply effect as radius increases from targeted point of origin
				-- do the magic
				for ee = 1, g_EntityElementMax, 1 do
					if e ~= ee then
						if g_Entity[ee] ~= nil then
							if g_Entity[ee]['active'] > 0 then
								if g_Entity[ee]['health'] > 0 then
									local thisallegiance = GetEntityAllegiance(ee)
									if thisallegiance == 0 then
										if g_Entity[tTarget[e]]['health'] > 0 then
											local thowclosex = g_Entity[ ee ]['x'] - g_Entity[tTarget[e]]['x']
											local thowclosey = g_Entity[ ee ]['y'] - g_Entity[tTarget[e]]['y']
											local thowclosez = g_Entity[ ee ]['z'] - g_Entity[tTarget[e]]['z']
											local thowclosedd = math.sqrt(math.abs(thowclosex*thowclosex)+math.abs(thowclosey*thowclosey)+math.abs(thowclosez*thowclosez))
											if thowclosedd < tscaleradius*2.0 then
												if entaffected[ee] == 0 then
													entaffected[ee] = 1											
													SetEntityHealth(ee,g_Entity[ee]['health']-0) 																							-- Set negative proximity damage to nearby entities
													if g_Entity[tTarget[e]]['health'] > 0 then SetEntityHealth(tTarget[e],g_Entity[tTarget[e]]['health']-lifetap_spell[e].cast_damage) end	-- Take health from targeted entity
													SetPlayerHealth(g_PlayerHealth + lifetap_spell[e].cast_damage)																			-- Give health to player
													if g_PlayerHealth > g_PlayerStartStrength then g_PlayerHealth = g_PlayerStartStrength end												-- Check player health doesnt exceeed start health (Not sure if this is still standard)
													if g_Entity[tTarget[e]]['health'] <= 0 then	tTarget[e] = 0 end
												end
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
		
		if tusedvalue > 0 and tTarget[e] ~= 0 then
			casttarget[e] = 1
			-- check player level		
			if _G["g_UserGlobal['".."MyPlayerLevel".."']"] ~= nil then tplayerlevel[e] = _G["g_UserGlobal['".."MyPlayerLevel".."']"] end
			if tplayerlevel[e] < tlevelrequired[e] then		
				PromptDuration("You need to be level "..tlevelrequired[e].." to cast this spell",1000)
				SetEntityUsed(e,0)
			end	
			if tplayerlevel[e] >= tlevelrequired[e] then
				-- attempt effect
				local mymana = 0 if _G["g_UserGlobal['"..lifetap_spell[e].user_global_affected.."']"] ~= nil then mymana = _G["g_UserGlobal['"..lifetap_spell[e].user_global_affected.."']"] end
				if mymana >= lifetap_spell[e].mana_cost then
					-- enough mana, deduct from player
					mymana = mymana - lifetap_spell[e].mana_cost
					-- setup and show the spell effect particles
					if lifetap_spell[e].particle1_number > 0 or nil then
						ResetPosition(lifetap_spell[e].particle1_number,g_Entity[tTarget[e]]['x'], g_Entity[tTarget[e]]['y'], g_Entity[tTarget[e]]['z'])
						Show(lifetap_spell[e].particle1_number)
					end
					if lifetap_spell[e].particle2_number > 0 or nil then
						ResetPosition(lifetap_spell[e].particle2_number,g_Entity[tTarget[e]]['x'], g_Entity[tTarget[e]]['y'], g_Entity[tTarget[e]]['z'])
						Show(lifetap_spell[e].particle2_number)
					end			
					-- prepare spell cast
					for ee = 1, g_EntityElementMax, 1 do
						entaffected[ee] = 0
					end
					-- prompt we did it
					PromptDuration(lifetap_spell[e].useage_text,2000)
					lifetap_spell[e].cast_timeout = Timer()
					PlaySound(e,0)
				else
					-- not successful
					PromptDuration("Not enough mana",2000)
					SetEntityUsed(e,0)
					casttarget[e] = 0
					if played[e] == 0 then
						PlaySound(e,1)
						played[e] = 1
					end
				end
				_G["g_UserGlobal['"..lifetap_spell[e].user_global_affected.."']"] = mymana
			end
		end
	end
end
