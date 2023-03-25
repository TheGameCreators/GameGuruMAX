-- DESCRIPTION: When collected can be cast as a Root effect to restrain the target for a period.
-- Root Spell v14
-- DESCRIPTION: [PROMPT_TEXT$="E to Collect, T or RMB to target"]
-- DESCRIPTION: [USEAGE_TEXT$="You cast a Root spell"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyMana"]
-- DESCRIPTION: [MANA_COST=10(1,100)]
-- DESCRIPTION: [CAST_DAMAGE=25(1,100)]
-- DESCRIPTION: [CAST_RADIUS=5(1,100))]
-- DESCRIPTION: [PARTICLE1_NAME$="SpellParticle1"]
-- DESCRIPTION: [PARTICLE2_NAME$="SpellParticle2"]
-- DESCRIPTION: <Sound0> when effect successful
-- DESCRIPTION: <Sound1> when effect unsuccessful

g_root_spell = {}

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
local curhealth = {}
local casttarget = {}
local doonce = {}
local root_time = {}
local current_time = {}
local entaffected = {}

function root_spell_properties(e, prompt_text, useage_text, pickup_range, user_global_affected, mana_cost, cast_damage, cast_radius, particle1_name, particle2_name)
	g_root_spell[e].prompt_text = prompt_text
	g_root_spell[e].useage_text = useage_text
	g_root_spell[e].pickup_range = pickup_range
	g_root_spell[e].user_global_affected = user_global_affected
	g_root_spell[e].mana_cost = mana_cost
	g_root_spell[e].cast_damage = cast_damage
	g_root_spell[e].cast_radius = cast_radius
	g_root_spell[e].particle1_name = lower(particle1_name)
	g_root_spell[e].particle2_name = lower(particle2_name)
end

function root_spell_init(e)
	g_root_spell[e] = {}
	g_root_spell[e].prompt_text = "E to Collect"
	g_root_spell[e].useage_text = "Direct Damage Inflicted"
	g_root_spell[e].pickup_range = 90
	g_root_spell[e].user_global_affected = "MyMana"
	g_root_spell[e].mana_cost = 10
	g_root_spell[e].cast_damage = 25
	g_root_spell[e].cast_radius = 5
	g_root_spell[e].particle1_name = ""
	g_root_spell[e].particle2_name = ""
	g_root_spell[e].particle1_number = 0
	g_root_spell[e].particle2_number = 0
	g_root_spell[e].cast_timeout = 0	
	status[e] = "init"
	tAllegiance[e] = 0
	tEnt[e] = 0
	tName[e] = ""
	tHealth[e] = 0
	tTarget[e] = 0
	cradius[e] = 0
	curhealth[e] = 0
	casttarget[e] = 0
	doonce[e] = 0
	root_time[e] = 8000
	current_time[e] = 0
end

function root_spell_main(e)
	-- get particles for spell effects
	if g_root_spell[e].particle1_number == 0 or nil then
		for n = 1, g_EntityElementMax do
			if n ~= nil and g_Entity[n] ~= nil then
				if lower(GetEntityName(n)) == g_root_spell[e].particle1_name then
					g_root_spell[e].particle1_number = n
					SetPosition(n,g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
					Hide(n)
					break
				end
			end
		end
	end
	if g_root_spell[e].particle2_number == 0 or nil then
		for m = 1, g_EntityElementMax do
			if m ~= nil and g_Entity[m] ~= nil then
				if lower(GetEntityName(m)) == g_root_spell[e].particle2_name then
					g_root_spell[e].particle2_number = m
					SetPosition(m,g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
					Hide(m)
					break
				end
			end
		end
	end
	-- handle states
	if status[e] == "init" then
		cradius[e] = 180.0 - g_root_spell[e].cast_radius
		status[e] = "collect_spell"		
	end
	if status[e] == "collect_spell" then
		PlayerDist = GetPlayerDistance(e)
		local LookingAt = GetPlrLookingAtEx(e,1)
		if LookingAt == 1 and PlayerDist < g_root_spell[e].pickup_range then
			if GetEntityCollectable(e) == 1 then
				if GetEntityCollected(e) == 0 then
					PromptDuration(g_root_spell[e].prompt_text,1000)
					if g_KeyPressE == 1 then
						SetEntityCollected(e,1,-1)
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
		if g_root_spell[e].cast_timeout > 0 then
			if Timer() > g_root_spell[e].cast_timeout + 2100 then
				g_root_spell[e].cast_timeout = 0
				-- hide the spell effect particles again
				if g_root_spell[e].particle1_number > 0 or nil then Hide(g_root_spell[e].particle1_number) end
				if g_root_spell[e].particle2_number > 0 or nil then Hide(g_root_spell[e].particle2_number) end
				casttarget[e] = 0
			else
				-- scale spell to see it radiate outward
				local tscaleradius = 5.0 + ((Timer()-g_root_spell[e].cast_timeout)/cradius[e])
				if g_root_spell[e].particle1_number > 0 then Scale(g_root_spell[e].particle1_number,tscaleradius) end
				if g_root_spell[e].particle2_number > 0 then Scale(g_root_spell[e].particle2_number,tscaleradius) end
				-- apply effect as radius increases from targeted point of origin
				-- do the magic				
				tusedvalue = 0				
			end
			if current_time[e] < root_time[e] then
				if doonce[e] == 0 then 
					SetEntityHealth(tTarget[e],g_Entity[tTarget[e]]['health']-g_root_spell[e].cast_damage/10)								-- Minor health damage of targeted entity					
					ModulateSpeed(tTarget[e],0)
					doonce[e] = 1
				end
				current_time[e] = current_time[e] + 10
			end
			if current_time[e] >= root_time[e] then				
				ModulateSpeed(tTarget[e],1)
				current_time[e] = 0
				g_root_spell[e].cast_timeout = 0
				if g_root_spell[e].particle1_number > 0 or nil then Hide(g_root_spell[e].particle1_number) end
				if g_root_spell[e].particle2_number > 0 or nil then Hide(g_root_spell[e].particle2_number) end
				SetEntityUsed(e,0)
				doonce[e] = 0
			end
		end	
		
		if tusedvalue > 0 and tTarget[e] ~= 0 then
			casttarget[e] = 1
			-- attempt effect
			local mymana = 0 if _G["g_UserGlobal['"..g_root_spell[e].user_global_affected.."']"] ~= nil then mymana = _G["g_UserGlobal['"..g_root_spell[e].user_global_affected.."']"] end
			if mymana >= g_root_spell[e].mana_cost then
				-- enough mana, deduct from player
				mymana = mymana - g_root_spell[e].mana_cost
				-- setup and show the spell effect particles
				if g_root_spell[e].particle1_number > 0 or nil then
					ResetPosition(g_root_spell[e].particle1_number,g_Entity[tTarget[e]]['x'], g_Entity[tTarget[e]]['y'], g_Entity[tTarget[e]]['z'])
					Show(g_root_spell[e].particle1_number)
				end
				if g_root_spell[e].particle2_number > 0 or nil then
					ResetPosition(g_root_spell[e].particle2_number,g_Entity[tTarget[e]]['x'], g_Entity[tTarget[e]]['y'], g_Entity[tTarget[e]]['z'])
					Show(g_root_spell[e].particle2_number)
				end	
				-- prompt we did it
				PromptDuration(g_root_spell[e].useage_text,2000)
				g_root_spell[e].cast_timeout = Timer()
				PlaySound(e,0)				
			else
				-- not successful
				PromptDuration("Not enough mana",2000)
				SetEntityUsed(e,0)
				PlaySound(e,1)
			end			
			_G["g_UserGlobal['"..g_root_spell[e].user_global_affected.."']"] = mymana
		end		
	end
end
