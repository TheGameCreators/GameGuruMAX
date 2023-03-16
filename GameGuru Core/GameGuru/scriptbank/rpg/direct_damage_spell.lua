-- DESCRIPTION: This object can be collected and will Direct Damage a targeted enemy entity.
-- Direct Damage Spell v6
-- DESCRIPTION: [PROMPT_TEXT$="E to Collect"]
-- DESCRIPTION: [USEAGE_TEXT$="You cast Direct Damage spell"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyMana"]
-- DESCRIPTION: [MANA_COST=10(1,100)]
-- DESCRIPTION: [CAST_DAMAGE=500(1,100)]
-- DESCRIPTION: [IMAGEFILE$="imagebank\\spell_icons\\direct_damage_spell_icon.png"]
-- DESCRIPTION: [PARTICLE1_NAME$="SpellParticle1"]
-- DESCRIPTION: [PARTICLE2_NAME$="SpellParticle2"]
-- DESCRIPTION: <Sound0> when effect successful
-- DESCRIPTION: <Sound1> when effect unsuccessful

local U = require "scriptbank\\utillib"
local P = require "scriptbank\\physlib"

local lower = string.lower

local direct_damage_spell = {}
local prompt_text = {}
local useage_text = {}
local pickup_range = {}
local user_global_affected = {}
local mana_cost = {}
local cast_damage = {}
local icon_image = {}
local particle1_name = {}
local particle1_number = {}
local particle2_name = {}
local particle2_number = {}

local cast_timeout = {}
local tTarget = {}
local tAllegiance = {}
local tEnt = {}
local tName = {}
local status = {}

function direct_damage_spell_properties(e, prompt_text, useage_text, pickup_range, user_global_affected, mana_cost, cast_damage, imagefile, particle1_name, particle1_number, particle2_name, particle2_number)
	direct_damage_spell[e] = g_Entity[e]
	direct_damage_spell[e].prompt_text = prompt_text
	direct_damage_spell[e].useage_text = useage_text
	direct_damage_spell[e].pickup_range = pickup_range
	direct_damage_spell[e].user_global_affected = user_global_affected
	direct_damage_spell[e].mana_cost = 10
	direct_damage_spell[e].cast_damage = cast_damage
	direct_damage_spell[e].icon_image = imagefile
	direct_damage_spell[e].particle1_name = lower(particle1_name)
	direct_damage_spell[e].particle1_number = 0
	direct_damage_spell[e].particle2_name = lower(particle2_name)
	direct_damage_spell[e].particle2_number = 0
end

function direct_damage_spell_init(e)
	direct_damage_spell[e] = g_Entity[e]
	direct_damage_spell[e].prompt_text = "E to Collect"
	direct_damage_spell[e].useage_text = "Area Damage Inflicted"
	direct_damage_spell[e].pickup_range = 80
	direct_damage_spell[e].user_global_affected = "MyMana"
	direct_damage_spell[e].mana_cost = 10
	direct_damage_spell[e].cast_damage = 100
	direct_damage_spell[e].icon_image = ""
	direct_damage_spell[e].particle1_name = ""
	direct_damage_spell[e].particle1_number = 0
	direct_damage_spell[e].particle2_name = ""
	direct_damage_spell[e].particle2_number = 0
	direct_damage_spell[e].cast_timeout = 0
	status[e] = "init"
	tTarget[e] = 0
	tAllegiance[e] = 0
	tEnt[e] = 0
	tName[e] = ""
end

function direct_damage_spell_main(e)
	direct_damage_spell[e] = g_Entity[e]	
	
	if status[e] == "init" then
	-- get particles for spell effects
		if direct_damage_spell[e].particle1_number == 0 or nil then
			for n = 1, g_EntityElementMax do
				if n ~= nil and g_Entity[n] ~= nil then
					if lower(GetEntityName(n)) == direct_damage_spell[e].particle1_name then
						direct_damage_spell[e].particle1_number = n
						SetPosition(n,g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
						Hide(n)
						break
					end
				end
			end
		end
		if direct_damage_spell[e].particle2_number == 0 or nil then
			for m = 1, g_EntityElementMax do
				if m ~= nil and g_Entity[m] ~= nil then
					if lower(GetEntityName(m)) == direct_damage_spell[e].particle2_name then
						direct_damage_spell[e].particle2_number = m
						SetPosition(m,g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
						Hide(m)
						break
					end
				end
			end			
		end
		-------------------------------
		status[e] = "collect_spell"
	end
	
	if status[e] == "collect_spell" then
		PlayerDist = GetPlayerDistance(e)
		local LookingAt = GetPlrLookingAtEx(e,1)
		if LookingAt == 1 and PlayerDist < direct_damage_spell[e].pickup_range then
			if GetEntityCollectable(e) == 1 then
				if GetEntityCollected(e) == 0 then
					PromptDuration(direct_damage_spell[e].prompt_text,1000)
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
		if g_MouseClick == 2 then
			Text(50,50,3,"+")								-- temp targeting crosshair
			local t = U.ObjectPlayerLookingAt(2000,0)		-- get object # in view
			tEnt[e] = P.ObjectToEntity(t)					-- get entity # of object
			tName[e] = GetEntityName(tEnt[e])				-- get entity name
			tAllegiance[e] = GetEntityAllegiance(tEnt[e]) 	-- get the allegiance value for this entity (0-enemy, 1-ally, 2-neutral)
			if tAllegiance[e] == 0 and tEnt[e] ~= nil then	-- if allegiance = enemy then give option to target
				if g_InKey == "t" or g_InKey == "T" then 	-- Using T Key to confirm target (using T or t at moment)
					tTarget[e] = tEnt[e]					-- entity set as target								
				end
			end	
			Text(50,20,3,"No:")		 													---> Temporary Text (to be deleted?)
			Text(52,20,3,tEnt[e])														---> Temporary Text (to be deleted?)
			Text(50,22,3,"Name:")														---> Temporary Text (to be deleted?)
			if tName[e] ~= 0 then Text(52,22,3,tName[e]) end							---> Temporary Text (to be deleted?)
			Text(50,24,3,"Type:")														---> Temporary Text (to be deleted?)
			if tEnt[e] ~= nil then
				if tAllegiance[e] == 0 then TextColor(52,24,3,"Enemy",255,0,0) end		---> Temporary Text (to be deleted?)
				if tAllegiance[e] == 1 then TextColor(52,24,3,"Ally",0,255,0) end		---> Temporary Text (to be deleted?)
				if tAllegiance[e] == 2 then TextColor(52,24,3,"Neutral",0,0,255) end	---> Temporary Text (to be deleted?)
			end
			if tTarget[e] > 0 then Text(50,26,3,"Targeted " ..tTarget[e]) end			---> Temporary Text (to be deleted?)
		end		
		------------------------------
	end
		
	local tusedvalue = GetEntityUsed(e)
	if direct_damage_spell[e].cast_timeout > 0 then
		if Timer() > direct_damage_spell[e].cast_timeout + 2100 then
			direct_damage_spell[e].cast_timeout = 0
		else
			tusedvalue = 0
		end
		SetEntityUsed(e,0)
	end

	if tusedvalue > 0 and tTarget[e] ~= 0 then
		-- attempt effect		
		local mymana = 0 if _G["g_UserGlobal['"..direct_damage_spell[e].user_global_affected.."']"] ~= nil then mymana = _G["g_UserGlobal['"..direct_damage_spell[e].user_global_affected.."']"] end
		if mymana >= direct_damage_spell[e].mana_cost then
			-- enough mana, deduct from player
			mymana = mymana - direct_damage_spell[e].mana_cost
			-- setup and show the spell effect particles at target position	and do magic
			if direct_damage_spell[e].particle1_number > 0 or nil then
				ResetPosition(direct_damage_spell[e].particle1_number,g_Entity[tTarget[e]]['x'], g_Entity[tTarget[e]]['y'], g_Entity[tTarget[e]]['z'])
				Show(direct_damage_spell[e].particle1_number)
			end
			if direct_damage_spell[e].particle2_number > 0 or nil then			
				ResetPosition(direct_damage_spell[e].particle2_number,g_Entity[tTarget[e]]['x'], g_Entity[tTarget[e]]['y'], g_Entity[tTarget[e]]['z'])			
				Show(direct_damage_spell[e].particle2_number)
			end
			SetEntityHealth(tTarget[e],g_Entity[tTarget[e]]['health']-direct_damage_spell[e].cast_damage)				
			-- prompt we did it
			PromptDuration(direct_damage_spell[e].useage_text,2000)
			direct_damage_spell[e].cast_timeout = Timer()
			PlaySound(e,0)
		else
			-- not successful
			PromptDuration("Not enough mana",2000)
			SetEntityUsed(e,0)
			PlaySound(e,1)
		end
		--hide the spell effect particles again
		if direct_damage_spell[e].particle1_number > 0 or nil then Hide(direct_damage_spell[e].particle1_number) end
		if direct_damage_spell[e].particle2_number > 0 or nil then Hide(direct_damage_spell[e].particle2_number) end
		_G["g_UserGlobal['"..direct_damage_spell[e].user_global_affected.."']"] = mymana
	end
end