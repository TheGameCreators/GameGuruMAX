-- DESCRIPTION: When collected can be cast as an Poison effect to damage the target.
-- Poison Spell v25
-- DESCRIPTION: [PROMPT_TEXT$="E to collect Poison Spell, T or RMB to target"]
-- DESCRIPTION: [USEAGE_TEXT$="You cast Poison spell"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@@USER_GLOBAL_AFFECTED$=""(0=globallist)] eg: MyMana
-- DESCRIPTION: [MANA_COST=10(1,100)]
-- DESCRIPTION: [CAST_DAMAGE=25(1,100)]
-- DESCRIPTION: [CAST_RADIUS=5(1,100))]
-- DESCRIPTION: [PLAYER_LEVEL=0(0,100))] player level to be able use this spell
-- DESCRIPTION: [PARTICLE1_NAME$=""] eg: SpellParticle1
-- DESCRIPTION: [PARTICLE2_NAME$=""] eg: SpellParticle2
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]
-- DESCRIPTION: <Sound0> pickup sound
-- DESCRIPTION: <Sound1> when cast effect successful
-- DESCRIPTION: <Sound2> when cast effect unsuccessful

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
local P = require "scriptbank\\physlib"
g_tEnt = {}

local lower = string.lower
local poison_spell 				= {}
local prompt_text				= {}
local useage_text				= {}
local pickup_range				= {}
local user_global_affected		= {}
local mana_cost					= {}
local cast_damage				= {}
local cast_radius				= {}
local player_level				= {}
local particle1_name			= {}
local particle2_name			= {}
local prompt_display			= {}
local item_highlight 			= {}
local highlight_icon 			= {}

local cast_timeout 		= {}
local tAllegiance 		= {}
local tEnt				= {}
local tName				= {}
local tHealth			= {}
local tTarget			= {}
local sEnt				= {}
local selectobj			= {}
local status			= {}
local cradius			= {}
local casttarget		= {}
local poison_time		= {}
local current_time		= {}
local tlevelrequired	= {}
local tplayerlevel		= {}
local played			= {}
local entaffected		= {}
local hl_icon 			= {}
local hl_imgwidth 		= {}
local hl_imgheight 		= {}

function poison_spell_properties(e, prompt_text, useage_text, pickup_range, user_global_affected, mana_cost, cast_damage, cast_radius, player_level, particle1_name, particle2_name, prompt_display, item_highlight, highlight_icon_imagefile)
	poison_spell[e].prompt_text = prompt_text
	poison_spell[e].useage_text = useage_text
	poison_spell[e].pickup_range = pickup_range
	poison_spell[e].user_global_affected = user_global_affected
	poison_spell[e].mana_cost = mana_cost
	poison_spell[e].cast_damage = cast_damage
	poison_spell[e].cast_radius = cast_radius
	poison_spell[e].player_level = player_level
	poison_spell[e].particle1_name = lower(particle1_name)
	poison_spell[e].particle2_name = lower(particle2_name)
	poison_spell[e].prompt_display = prompt_display	
	poison_spell[e].item_highlight = item_highlight
	poison_spell[e].highlight_icon = highlight_icon_imagefile	
end

function poison_spell_init(e)
	poison_spell[e] = {}
	poison_spell[e].prompt_text = "E to Collect"
	poison_spell[e].useage_text = "You cast Poison spell"
	poison_spell[e].pickup_range = 90
	poison_spell[e].user_global_affected = "MyMana"
	poison_spell[e].mana_cost = 10
	poison_spell[e].cast_damage = 25
	poison_spell[e].cast_radius = 5
	poison_spell[e].player_level = 0
	poison_spell[e].particle1_name = ""
	poison_spell[e].particle2_name = ""
	poison_spell[e].particle1_number = 0
	poison_spell[e].particle2_number = 0
	poison_spell[e].prompt_display = 1
	poison_spell[e].item_highlight = 0
	poison_spell[e].highlight_icon = "imagebank\\icons\\pickup.png"
	
	poison_spell[e].cast_timeout = 0	
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
	played[e] = 0
	tplayerlevel[e] = 0
	tlevelrequired[e] = 0
	casttarget[e] = 0
	poison_time[e] = 500
	current_time[e] = 0
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0	
end

function poison_spell_main(e)

	-- handle states
	if status[e] == "init" then
		if poison_spell[e].item_highlight == 3 and poison_spell[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(poison_spell[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(poison_spell[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(poison_spell[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		-- get particles for spell effects
		if poison_spell[e].particle1_number == 0 and poison_spell[e].particle1_name ~= "" then
			for n = 1, g_EntityElementMax do
				if n ~= nil and g_Entity[n] ~= nil then
					if lower(GetEntityName(n)) == poison_spell[e].particle1_name then
						poison_spell[e].particle1_number = n
						SetPosition(n,g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
						Hide(n)
						break
					end
				end
			end
		end
		if poison_spell[e].particle2_number == 0 and poison_spell[e].particle2_name ~= "" then
			for m = 1, g_EntityElementMax do
				if m ~= nil and g_Entity[m] ~= nil then
					if lower(GetEntityName(m)) == poison_spell[e].particle2_name then
						poison_spell[e].particle2_number = m
						SetPosition(m,g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
						Hide(m)
						break
					end
				end
			end
		end	
		tplayerlevel[e] = 0
		tlevelrequired[e] = poison_spell[e].player_level
		cradius[e] = 100.0 - poison_spell[e].cast_radius
		status[e] = "collect_spell"		
	end
	if status[e] == "collect_spell" then
		local PlayerDist = GetPlayerDistance(e)
		if PlayerDist < poison_spell[e].pickup_range then
			--pinpoint select object--
			module_misclib.pinpoint(e,poison_spell[e].pickup_range,poison_spell[e].item_highlight,hl_icon[e])
			sEnt[e] = g_tEnt
			--end pinpoint select object--
		end	
		if PlayerDist < poison_spell[e].pickup_range and sEnt[e] == e then
			if GetEntityCollectable(e) == 1 then
				if GetEntityCollected(e) == 0 then
					if poison_spell[e].prompt_display == 1 then PromptLocal(e,poison_spell[e].prompt_text) end
					if poison_spell[e].prompt_display == 2 then Prompt(poison_spell[e].prompt_text) end
					if g_KeyPressE == 1 then
						SetEntityCollected(e,1)
						PlayNon3DSound(e,0)
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
		
		if poison_spell[e].cast_timeout > 0 then
			if Timer() > poison_spell[e].cast_timeout + 2100 then
				poison_spell[e].cast_timeout = 0
				-- hide the spell effect particles again
				if poison_spell[e].particle1_number > 0 then Hide(poison_spell[e].particle1_number) end
				if poison_spell[e].particle2_number > 0 then Hide(poison_spell[e].particle2_number) end
				casttarget[e] = 0
			else
				-- scale spell to see it radiate outward
				local tscaleradius = 5.0 + ((Timer()-poison_spell[e].cast_timeout)/cradius[e])
				if poison_spell[e].particle1_number > 0 then Scale(poison_spell[e].particle1_number,tscaleradius) end
				if poison_spell[e].particle2_number > 0 then Scale(poison_spell[e].particle2_number,tscaleradius) end
				-- apply effect as radius increases from targeted point of origin
				-- do the magic
				if tTarget[e] ~= 0 or nil and g_Entity[tTarget[e]]['health'] > 0 then
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
														SetEntityHealth(ee,g_Entity[ee]['health']-poison_spell[e].cast_damage/3)	-- Give smaller damage to nearby entities
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
				-- cannot use while in effects				
				tusedvalue = 0
			end			
			if current_time[e] < poison_time[e] and tTarget[e] ~= 0 or nil then
				if g_Entity[tTarget[e]]['health'] > 0 then SetEntityHealth(tTarget[e],g_Entity[tTarget[e]]['health']-poison_spell[e].cast_damage/poison_time[e]) end	-- Take health from targeted entity over time
				if g_Entity[tTarget[e]]['health'] <= 0 then	tTarget[e] = 0 end
				current_time[e] = current_time[e] + 1				
			end
			if current_time[e] >= poison_time[e] then 
				SetEntityUsed(e,0)
				played[e] = 0	
				current_time[e] = 0
			end			
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
				local mymana = 0 if _G["g_UserGlobal['"..poison_spell[e].user_global_affected.."']"] ~= nil then mymana = _G["g_UserGlobal['"..poison_spell[e].user_global_affected.."']"] end
				if mymana >= poison_spell[e].mana_cost then
					-- enough mana, deduct from player
					mymana = mymana - poison_spell[e].mana_cost
					-- setup and show the spell effect particles
					if poison_spell[e].particle1_number > 0 then
						ResetPosition(poison_spell[e].particle1_number,g_Entity[tTarget[e]]['x'], g_Entity[tTarget[e]]['y'], g_Entity[tTarget[e]]['z'])
						Show(poison_spell[e].particle1_number)
					end
					if poison_spell[e].particle2_number > 0 then
						ResetPosition(poison_spell[e].particle2_number,g_Entity[tTarget[e]]['x'], g_Entity[tTarget[e]]['y'], g_Entity[tTarget[e]]['z'])
						Show(poison_spell[e].particle2_number)
					end			
					-- prepare spell cast
					for ee = 1, g_EntityElementMax, 1 do
						entaffected[ee] = 0
					end
					-- prompt we did it
					PromptDuration(poison_spell[e].useage_text,2000)
					poison_spell[e].cast_timeout = Timer()
					PlayNon3DSound(e,1)				
				else
					-- not successful
					PromptDuration("Not enough mana",2000)
					SetEntityUsed(e,0)
					casttarget[e] = 0
					if played[e] == 0 then
						PlayNon3DSound(e,2)
						played[e] = 1
					end
				end			
				_G["g_UserGlobal['"..poison_spell[e].user_global_affected.."']"] = mymana
			end			
		end		
	end
end
