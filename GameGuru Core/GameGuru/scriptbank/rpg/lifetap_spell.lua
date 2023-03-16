-- DESCRIPTION: This object can be collected and will Lifetap a targeted enemy of health and give it to you.
-- Lifetap Spell v6
-- DESCRIPTION: [PROMPT_TEXT$="E to Collect"]
-- DESCRIPTION: [USEAGE_TEXT$="You cast a Lifetap spell"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyMana"]
-- DESCRIPTION: [MANA_COST=10(1,100)]
-- DESCRIPTION: [CAST_DAMAGE=30(1,100)]
-- DESCRIPTION: [IMAGEFILE$="imagebank\\spell_icons\\lifetap_spell_icon.png"]
-- DESCRIPTION: [PARTICLE1_NAME$="SpellParticle1"]
-- DESCRIPTION: [PARTICLE2_NAME$="SpellParticle2"]
-- DESCRIPTION: <Sound0> when effect successful
-- DESCRIPTION: <Sound1> when effect unsuccessful

local U = require "scriptbank\\utillib"
local P = require "scriptbank\\physlib"

local lower = string.lower

local lifetap_spell = {}
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

function lifetap_spell_properties(e, prompt_text, useage_text, pickup_range, user_global_affected, mana_cost, cast_damage, imagefile, particle1_name, particle1_number, particle2_name, particle2_number)
	lifetap_spell[e] = g_Entity[e]
	lifetap_spell[e].prompt_text = prompt_text
	lifetap_spell[e].useage_text = useage_text
	lifetap_spell[e].pickup_range = pickup_range
	lifetap_spell[e].user_global_affected = user_global_affected
	lifetap_spell[e].mana_cost = 10
	lifetap_spell[e].cast_damage = cast_damage
	lifetap_spell[e].icon_image = imagefile
	lifetap_spell[e].particle1_name = lower(particle1_name)
	lifetap_spell[e].particle1_number = 0
	lifetap_spell[e].particle2_name = lower(particle2_name)
	lifetap_spell[e].particle2_number = 0
end

function lifetap_spell_init(e)
	lifetap_spell[e] = g_Entity[e]
	lifetap_spell[e].prompt_text = "E to Collect"
	lifetap_spell[e].useage_text = "You cast a Lifetap spell"
	lifetap_spell[e].pickup_range = 80
	lifetap_spell[e].user_global_affected = "MyMana"
	lifetap_spell[e].mana_cost = 10
	lifetap_spell[e].cast_damage = 30
	lifetap_spell[e].icon_image = ""
	lifetap_spell[e].particle1_name = ""
	lifetap_spell[e].particle1_number = 0
	lifetap_spell[e].particle2_name = ""
	lifetap_spell[e].particle2_number = 0
	lifetap_spell[e].cast_timeout = 0
	status[e] = "init"
	tTarget[e] = 0
	tAllegiance[e] = 0
	tEnt[e] = 0
	tName[e] = ""
end

function lifetap_spell_main(e)
	lifetap_spell[e] = g_Entity[e]	
	
	if status[e] == "init" then
	-- get particles for spell effects
		if lifetap_spell[e].particle1_number == 0 or nil then
			for n = 1, g_EntityElementMax do
				if n ~= nil and g_Entity[n] ~= nil then
					if lower(GetEntityName(n)) == lifetap_spell[e].particle1_name then
						lifetap_spell[e].particle1_number = m
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
		-------------------------------
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
	if lifetap_spell[e].cast_timeout > 0 then
		if Timer() > lifetap_spell[e].cast_timeout + 2100 then
			lifetap_spell[e].cast_timeout = 0
		else
			tusedvalue = 0
		end
		SetEntityUsed(e,0)
	end

	if tusedvalue > 0 and tTarget[e] ~= 0 then
		-- attempt effect		
		local mymana = 0 if _G["g_UserGlobal['"..lifetap_spell[e].user_global_affected.."']"] ~= nil then mymana = _G["g_UserGlobal['"..lifetap_spell[e].user_global_affected.."']"] end
		if mymana >= lifetap_spell[e].mana_cost then
			-- enough mana, deduct from player
			mymana = mymana - lifetap_spell[e].mana_cost
			-- setup and show the spell effect particles at target position	and do magic		
			if lifetap_spell[e].particle1_number > 0 or nil then
				ResetPosition(lifetap_spell[e].particle1_number,g_Entity[tTarget[e]]['x'], g_Entity[tTarget[e]]['y'], g_Entity[tTarget[e]]['z'])
				Show(lifetap_spell[e].particle1_number)
			end
			if lifetap_spell[e].particle2_number > 0 or nil then
				ResetPosition(lifetap_spell[e].particle2_number,g_Entity[tTarget[e]]['x'], g_Entity[tTarget[e]]['y'], g_Entity[tTarget[e]]['z'])
				Show(lifetap_spell[e].particle2_number)
			end	 
			if g_Entity[tTarget[e]]['health'] > 0 then
				SetEntityHealth(tTarget[e],g_Entity[tTarget[e]]['health']-lifetap_spell[e].cast_damage)
				SetPlayerHealth(g_PlayerHealth + lifetap_spell[e].cast_damage)
				if g_PlayerHealth > g_PlayerStartStrength then g_PlayerHealth = g_PlayerStartStrength end
			end	
			-- prompt we did it
			PromptDuration(lifetap_spell[e].useage_text,2000)
			lifetap_spell[e].cast_timeout = Timer()
			PlaySound(e,0)
		else
			-- not successful
			PromptDuration("Not enough mana",2000)
			SetEntityUsed(e,0)
			PlaySound(e,1)
		end		
		--hide the spell effect particles again
		if lifetap_spell[e].particle1_number > 0 or nil then Hide(lifetap_spell[e].particle1_number) end
		if lifetap_spell[e].particle2_number > 0 or nil then Hide(lifetap_spell[e].particle2_number) end
		_G["g_UserGlobal['"..lifetap_spell[e].user_global_affected.."']"] = mymana
	end
end