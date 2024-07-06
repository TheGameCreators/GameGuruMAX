-- DESCRIPTION: When collected can be cast Fireball damage on the target.
-- Fireball Spell v23
-- DESCRIPTION: [PROMPT_TEXT$="E to collect Fireball Spell, T or RMB to target"]
-- DESCRIPTION: [USEAGE_TEXT$="Fireball damage inflicted"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyMana"]
-- DESCRIPTION: [MANA_COST=10(1,100)]
-- DESCRIPTION: [CAST_DAMAGE=500(1,100)]
-- DESCRIPTION: [CAST_RADIUS=5(1,100))]
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
local fireball_spell 		= {}
local prompt_text			= {}
local useage_text			= {}
local pickup_range			= {}
local user_global_affected	= {}
local mana_cost				= {}
local cast_damage			= {}
local cast_radius			= {}
local player_level			= {}
local particle1_name 		= {}
local particle2_name 		= {}
local item_highlight 		= {}

local cast_timeout		= {}
local tAllegiance		= {}
local tEnt				= {}
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

function fireball_spell_properties(e, prompt_text, useage_text, pickup_range, user_global_affected, mana_cost, cast_damage, cast_radius, player_level, particle1_name, particle2_name, item_highlight)
	fireball_spell[e].prompt_text = prompt_text
	fireball_spell[e].useage_text = useage_text
	fireball_spell[e].pickup_range = pickup_range
	fireball_spell[e].user_global_affected = user_global_affected
	fireball_spell[e].mana_cost = mana_cost
	fireball_spell[e].cast_damage = cast_damage
	fireball_spell[e].cast_radius = cast_radius
	fireball_spell[e].player_level = player_level
	fireball_spell[e].particle1_name = lower(particle1_name)
	fireball_spell[e].particle2_name = lower(particle2_name)
	fireball_spell[e].item_highlight = item_highlight	
end

function fireball_spell_init(e)
	fireball_spell[e] = {}
	fireball_spell[e].prompt_text = "E to Collect"
	fireball_spell[e].useage_text = "Fireball damage inflicted"
	fireball_spell[e].pickup_range = 90
	fireball_spell[e].user_global_affected = "MyMana"
	fireball_spell[e].mana_cost = 10
	fireball_spell[e].cast_damage = 1
	fireball_spell[e].cast_radius = 5
	fireball_spell[e].player_level = 0
	fireball_spell[e].particle1_name = ""
	fireball_spell[e].particle2_name = ""
	fireball_spell[e].particle1_number = 0
	fireball_spell[e].particle2_number = 0
	fireball_spell[e].item_highlight = 0
	fireball_spell[e].cast_timeout = 0	
	status[e] = "init"
	tAllegiance[e] = 0
	tEnt[e] = 0
	tName[e] = ""
	tHealth[e] = 0
	tTarget[e] = 0
	sEnt[e] = 0
	g_tEnt = 0	
	played[e] = 0
	selectobj[e] = 0
	tplayerlevel[e] = 0
	tlevelrequired[e] = 0
	cradius[e] = 0
	casttarget[e] = 0
end

function fireball_spell_main(e)

	-- get particles for spell effects
	if fireball_spell[e].particle1_number == 0 or fireball_spell[e].particle1_number == nil and fireball_spell[e].particle1_name ~= "" then
		for n = 1, g_EntityElementMax do
			if n ~= nil and g_Entity[n] ~= nil then
				if lower(GetEntityName(n)) == fireball_spell[e].particle1_name then
					fireball_spell[e].particle1_number = n
					SetPosition(n,g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
					Hide(n)
					break
				end
			end
		end
	end
	if fireball_spell[e].particle2_number == 0 or fireball_spell[e].particle2_number == nil and fireball_spell[e].particle2_name ~= "" then
		for m = 1, g_EntityElementMax do
			if m ~= nil and g_Entity[m] ~= nil then
				if lower(GetEntityName(m)) == fireball_spell[e].particle2_name then
					fireball_spell[e].particle2_number = m
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
		tlevelrequired[e] = fireball_spell[e].player_level
		cradius[e] = 180 - fireball_spell[e].cast_radius
		status[e] = "collect_spell"		
	end
	if status[e] == "collect_spell" then
		local PlayerDist = GetPlayerDistance(e)
		if PlayerDist < fireball_spell[e].pickup_range then
			--pinpoint select object--
			module_misclib.pinpoint(e,fireball_spell[e].pickup_range,fireball_spell[e].item_highlight)
			sEnt[e] = g_tEnt
			--end pinpoint select object--	
		end	
		if PlayerDist < fireball_spell[e].pickup_range and sEnt[e] ~= 0 then
			if GetEntityCollectable(e) == 1 then
				if GetEntityCollected(e) == 0 then
					PromptDuration(fireball_spell[e].prompt_text,1000)
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
				SetEntityOutline(tEnt[e],1)				
			end
			if tTarget[e] > 0 then
				TextCenterOnX(50,20,3,"TARGETED")
				if tAllegiance[e] == 0 then TextCenterOnX(50,22,3,tName[e]) end
			end			
		end

		local tusedvalue = GetEntityUsed(e)
		if g_MouseClick == 1 and tTarget[e] ~= 0 then SetEntityUsed(e,1) end
	
		if fireball_spell[e].cast_timeout > 0 then			
			if Timer() > fireball_spell[e].cast_timeout + 2100 then
				fireball_spell[e].cast_timeout = 0
				-- hide the spell effect particles again
				if fireball_spell[e].particle1_number > 0 then Hide(fireball_spell[e].particle1_number) end
				if fireball_spell[e].particle2_number > 0 then Hide(fireball_spell[e].particle2_number) end
				casttarget[e] = 0
			else
				-- scale spell to see it radiate outward
				if fireball_spell[e].particle1_number > 0 then
					if g_Entity[fireball_spell[e].particle1_number]['y'] > g_Entity[tTarget[e]]['y'] then 
						ResetPosition(fireball_spell[e].particle1_number,g_Entity[tTarget[e]]['x'], g_Entity[fireball_spell[e].particle1_number]['y']-5, g_Entity[tTarget[e]]['z'])
						Show(fireball_spell[e].particle1_number)
					end
				end
				if fireball_spell[e].particle2_number > 0 then
					if g_Entity[fireball_spell[e].particle2_number]['y'] > g_Entity[tTarget[e]]['y'] then 
						ResetPosition(fireball_spell[e].particle2_number,g_Entity[tTarget[e]]['x'], g_Entity[fireball_spell[e].particle2_number]['y']-3, g_Entity[tTarget[e]]['z'])
						Show(fireball_spell[e].particle2_number)
					end
				end
				local tscaleradius = 10.0 + ((Timer()-fireball_spell[e].cast_timeout)/cradius[e])
				if fireball_spell[e].particle1_number > 0 then Scale(fireball_spell[e].particle1_number,tscaleradius) end				
				if fireball_spell[e].particle2_number > 0 then Scale(fireball_spell[e].particle2_number,tscaleradius) end
				-- apply effect as radius increases from targeted point of origin
				-- do the magic
				if g_Entity[fireball_spell[e].particle1_number]['y'] <= g_Entity[tTarget[e]]['y']+5 or g_Entity[fireball_spell[e].particle2_number]['y'] <= g_Entity[tTarget[e]]['y']+5 then
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
														SetEntityHealth(ee,g_Entity[ee]['health']-fireball_spell[e].cast_damage/3)																		-- Give smaller damage to nearby entities
														if g_Entity[tTarget[e]]['health'] > 0 then SetEntityHealth(tTarget[e],g_Entity[tTarget[e]]['health']-fireball_spell[e].cast_damage) end			-- Give damage to targeted entity
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
				local mymana = 0 if _G["g_UserGlobal['"..fireball_spell[e].user_global_affected.."']"] ~= nil then mymana = _G["g_UserGlobal['"..fireball_spell[e].user_global_affected.."']"] end
				if mymana >= fireball_spell[e].mana_cost then
					-- enough mana, deduct from player
					mymana = mymana - fireball_spell[e].mana_cost
					-- setup and show the spell effect particles
					if fireball_spell[e].particle1_number > 0 then
						ResetPosition(fireball_spell[e].particle1_number,g_Entity[tTarget[e]]['x'], g_Entity[tTarget[e]]['y']+200, g_Entity[tTarget[e]]['z'])
						Show(fireball_spell[e].particle1_number)
					end
					if fireball_spell[e].particle2_number > 0 then
						ResetPosition(fireball_spell[e].particle2_number,g_Entity[tTarget[e]]['x']+ math.random(1,10), g_Entity[tTarget[e]]['y']+200, g_Entity[tTarget[e]]['z']+ math.random(1,10))
						Show(fireball_spell[e].particle2_number)
					end			
					-- prepare spell cast
					for ee = 1, g_EntityElementMax, 1 do
						entaffected[ee] = 0
					end
					-- prompt we did it
					PromptDuration(fireball_spell[e].useage_text,2000)
					fireball_spell[e].cast_timeout = Timer()
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
				_G["g_UserGlobal['"..fireball_spell[e].user_global_affected.."']"] = mymana
			end
		end
	end
end
