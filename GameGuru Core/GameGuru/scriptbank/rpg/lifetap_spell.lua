-- DESCRIPTION: When collected can cast Lifetap effect to take health from the target and give to the player.
-- Lifetap Spell v17
-- DESCRIPTION: [PROMPT_TEXT$="E to Collect, T or RMB to target"]
-- DESCRIPTION: [USEAGE_TEXT$="You cast Lifetap and gained some health"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyMana"]
-- DESCRIPTION: [MANA_COST=10(1,100)]
-- DESCRIPTION: [CAST_DAMAGE=25(1,100)]
-- DESCRIPTION: [CAST_RADIUS=5(1,100))]
-- DESCRIPTION: [PARTICLE1_NAME$="SpellParticle1"]
-- DESCRIPTION: [PARTICLE2_NAME$="SpellParticle2"]
-- DESCRIPTION: <Sound0> when effect successful
-- DESCRIPTION: <Sound1> when effect unsuccessful

local lifetap_spell = {}

local U = require "scriptbank\\utillib"
local P = require "scriptbank\\physlib"
local lower = string.lower

local cast_timeout = {}
local tAllegiance = {}
local tEnt = {}
local tName = {}
local tHealth = {}
local tTarget = {}
local status = {}
local cradius = {}
local casttarget = {}
local entaffected = {}

function lifetap_spell_properties(e, prompt_text, useage_text, pickup_range, user_global_affected, mana_cost, cast_damage, cast_radius, particle1_name, particle2_name)
	lifetap_spell[e] = g_Entity[e]
	lifetap_spell[e].prompt_text = prompt_text
	lifetap_spell[e].useage_text = useage_text
	lifetap_spell[e].pickup_range = pickup_range
	lifetap_spell[e].user_global_affected = user_global_affected
	lifetap_spell[e].mana_cost = mana_cost
	lifetap_spell[e].cast_damage = cast_damage
	lifetap_spell[e].cast_radius = cast_radius
	lifetap_spell[e].particle1_name = lower(particle1_name)
	lifetap_spell[e].particle2_name = lower(particle2_name)
end

function lifetap_spell_init(e)
	lifetap_spell[e] = g_Entity[e]
	lifetap_spell[e].prompt_text = "E to Collect"
	lifetap_spell[e].useage_text = "Direct Damage Inflicted"
	lifetap_spell[e].pickup_range = 90
	lifetap_spell[e].user_global_affected = "MyMana"
	lifetap_spell[e].mana_cost = 10
	lifetap_spell[e].cast_damage = 25
	lifetap_spell[e].cast_radius = 5
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
	cradius[e] = 0
	casttarget[e] = 0
end

function lifetap_spell_main(e)
	lifetap_spell[e] = g_Entity[e]
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
		cradius[e] = 180.0 - lifetap_spell[e].cast_radius
		status[e] = "collect_spell"		
	end
	if status[e] == "collect_spell" then
		PlayerDist = GetPlayerDistance(e)
		local LookingAt = GetPlrLookingAtEx(e,1)
		if LookingAt == 1 and PlayerDist < lifetap_spell[e].pickup_range then
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
			Text(50,50,3,"+")															-- temp targeting crosshair
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
				if tTarget[e] > 0 then
					if g_Entity[tTarget[e]]['health'] > 0 then
						for ee = 1, g_EntityElementMax, 1 do
							if e ~= ee then
								if g_Entity[ee] ~= nil then
									if g_Entity[ee]['active'] > 0 then
										if g_Entity[ee]['health'] > 0 then
											local thisallegiance = GetEntityAllegiance(ee)
											if thisallegiance == 0 then
												local thowclosex = g_Entity[ ee ]['x'] - g_Entity[tTarget[e]]['x']
												local thowclosey = g_Entity[ ee ]['y'] - g_Entity[tTarget[e]]['y']
												local thowclosez = g_Entity[ ee ]['z'] - g_Entity[tTarget[e]]['z']
												local thowclosedd = math.sqrt(math.abs(thowclosex*thowclosex)+math.abs(thowclosey*thowclosey)+math.abs(thowclosez*thowclosez))
												if thowclosedd < tscaleradius*2.0 then
													if entaffected[ee] == 0 then
														entaffected[ee] = 1
														-- Take health from targeted entity
														if ee == tTarget[e] then
															local thealthtotake = lifetap_spell[e].cast_damage
															if thealthtotake > g_Entity[ee]['health'] then thealthtotake = g_Entity[ee]['health'] end
															g_Entity[ee]['health'] = g_Entity[ee]['health'] - thealthtotake
															SetEntityHealth(ee,g_Entity[ee]['health'])
															local tnewplayerhealth = g_PlayerHealth + thealthtotake
															if tnewplayerhealth > g_PlayerStartStrength then tnewplayerhealth = g_PlayerStartStrength end
															SetPlayerHealth(tnewplayerhealth)																			
														else
															-- Most nearby entities are unaffected
															SetEntityHealth(ee,g_Entity[ee]['health']-0)																						
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
					if g_Entity[tTarget[e]]['health'] <= 0 then	tTarget[e] = 0 end
				end
				-- cannot use while in effect
				tusedvalue = 0
			end		
			SetEntityUsed(e,0)
		end
		
		if tusedvalue > 0 and tTarget[e] ~= 0 then
			casttarget[e] = 1
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
				PlaySound(e,1)
			end
			_G["g_UserGlobal['"..lifetap_spell[e].user_global_affected.."']"] = mymana
		end
	end
end
