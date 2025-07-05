-- Weapon v16 - Necrym and Lee
-- DESCRIPTION: Assign to a weapon object to be collected, and play an optional pickup <Sound0>.
-- DESCRIPTION: [PICKUP_RANGE=75(1,200)]
-- DESCRIPTION: [@PICKUP_STYLE=2(1=Ranged, 2=Accurate)]
-- DESCRIPTION: [!PLAY_PICKUP=0]
-- DESCRIPTION: [!ACTIVATE_LOGIC=0]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)] Use emmisive color for shape option
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local weapon			= {}
local pickup_range		= {}
local pickup_style		= {}
local play_pickup		= {}
local activate_logic	= {}
local item_highlight 	= {}
local highlight_icon	= {}
local weapon_name		= {}

local status 			= {}
local hl_icon	 		= {}
local hl_imgwidth		= {}
local hl_imgheight		= {}

weapon_therecanbeonlyone = 0
weapon_temprompttimer = 0

function weapon_properties(e, pickup_range, pickup_style, play_pickup, activate_logic, item_highlight, highlight_icon_imagefile)
	weapon[e].pickup_range = pickup_range or 75
	weapon[e].pickup_style = pickup_style
	weapon[e].play_pickup = play_pickup or 0
	weapon[e].activate_logic = activate_logic or 0
	weapon[e].item_highlight = item_highlight or 0
	weapon[e].highlight_icon = highlight_icon_imagefile
end

function weapon_init_name(e,name)
	weapon[e] = {}
	weapon[e].pickup_range = 75
	weapon[e].pickup_style = 1
	weapon[e].play_pickup = 0
	weapon[e].activate_logic = 0
	weapon[e].item_highlight = 0
	weapon[e].highlight_icon = "imagebank\\icons\\pickup.png"
	weapon_name[e] = name
	
	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0
end

function weapon_main(e)

	if status[e] == "init" then
		if weapon[e].item_highlight == 3 and weapon[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(weapon[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(weapon[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(weapon[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "endinit"
	end		
	local PlayerDist = GetPlayerDistance(e)
	local LookingAt = GetPlrLookingAtEx(e,1)
	if weapon_therecanbeonlyone==-1 then
		if g_KeyPressE == 0 and g_InKey == "" then weapon_therecanbeonlyone = 0 end
	end
	if weapon[e].pickup_style == 2 then
		if PlayerDist < weapon[e].pickup_range and g_PlayerHealth > 0 and g_PlayerThirdPerson==0 then
			--pinpoint select object--
			module_misclib.pinpoint(e,weapon[e].pickup_range,weapon[e].item_highlight,hl_icon[e])
			--end pinpoint select object--
		end
	end

	if PlayerDist < weapon[e].pickup_range and g_PlayerHealth > 0 and g_PlayerThirdPerson==0 then
		if weapon[e].pickup_style == 1 then
			if LookingAt == 1 and weapon_therecanbeonlyone==0 then 
				weapon_therecanbeonlyone = e
				g_tEnt = e
			end
			if LookingAt == 0 and weapon_therecanbeonlyone==e then
				weapon_therecanbeonlyone = 0
				g_tEnt = 0
			end			
		end
		if weapon[e].pickup_style == 2 and g_tEnt == e then
			if g_tEnt > 0 and weapon_therecanbeonlyone==0 then weapon_therecanbeonlyone = e end
			if g_tEnt == 0 and weapon_therecanbeonlyone==e then weapon_therecanbeonlyone = 0 end
		end
		if g_Entity[e]['haskey'] == 1 then
			-- reused haskey flag for use when auto collect from dead character if player already got the weapon
			weapon_therecanbeonlyone = e
		end
		if weapon_therecanbeonlyone==e and g_tEnt == e then
			--with inventory you can collect as many weapons as you like
			if weapon_temprompttimer > 0 then
				PromptLocal("Cannot collect the " .. weapon_name[e] .. " into preferred slot")
				if Timer() > weapon_temprompttimer + 3000 then weapon_temprompttimer = 0 end
			else
				if g_Entity[e]['haskey'] ~= 1 then
					if g_PlayerGunID > 0 then
						local actiontext = "pick up"
						if GetInventoryExist("inventory:player") == 0 then
							local possibleslotchoices = -1
							for a = 1, 9 do
								if GetWeaponSlot(a) <= 0 and GetWeaponSlotPref(a) == 0 then
									possibleslotchoices = a
									break
								end
							end
							if possibleslotchoices <= 0 then
								actiontext = "permanently replace with"
							end
						end
						if g_PlayerController==0 then
							if GetHeadTracker() == 1 then
								PromptLocal(e,"Right trigger to " .. actiontext .. " the " .. weapon_name[e] )
							else
								PromptLocal(e,"Press E to " .. actiontext .. " the " .. weapon_name[e] )
							end
						else
							if weapon[e].prompt_display == 1 then
								PromptLocal(e,"Press Y Button to " .. actiontext .. " the " .. weapon_name[e] )
							end	
						end
					else
						if g_PlayerController==0 then
							if GetHeadTracker() == 1 then
								PromptLocal(e,"Right trigger to pick up the " .. weapon_name[e] )
							else
								PromptLocal(e,"Press E or LMB to pick up the " .. weapon_name[e] )
							end	
						else
							if weapon[e].prompt_display == 1 then
								PromptLocal(e,"Press Y Button to pick up the " .. weapon_name[e] )
							end	
						end
					end
				end
				if g_KeyPressE == 1 or g_MouseClick == 1 or g_Entity[e]['haskey'] == 1 then
					--Collect into hotkeys inventory
					local collectedweapon = 1
					local addedasweapon = 0
					local preferredslot = -1
					local weaponID = GetEntityWeaponID(e)
					for slot = 1, 9, 1 do
						local thisWeaponPref = GetWeaponSlotPref(slot)
						if thisWeaponPref == weaponID then
							preferredslot = slot-1
							break
						end
					end
					if preferredslot > -1 then
					   local suggestedslot = SetEntityCollected(e,2,preferredslot)
					   if preferredslot == suggestedslot then
							-- must go in preferred slot
							AddPlayerWeaponSuggestSlot(e,1+preferredslot)
							addedasweapon = 1
					   else
							-- or cannot collect (something in slot and not auto-collecting its ammo)
							if g_Entity[e]['haskey'] ~= 1 then
								weapon_temprompttimer = Timer()
								collectedweapon = 0
							else
								AddPlayerWeapon(e)
							end
					   end
					else
						local sslot = -1
						for a = 1, 9 do
							if GetWeaponSlot(a) <= 0 and GetWeaponSlotPref(a) == 0 then
								sslot = a
								break
							end
						end
					    if sslot > 0 then
							-- can add to weapon slots
							SetEntityCollected(e,2,sslot-1)
							AddPlayerWeaponSuggestSlot(e,sslot)
							addedasweapon = 1
					    else
							-- check if have an inventory
							if GetInventoryExist("inventory:player") == 1 then
								-- add to general inventory
								SetEntityCollected(e,1,-1)
							else
								-- no inventory and no slots, can only replace current weapon
								ReplacePlayerWeapon(e)
								SetEntityCollected(e,1)
							end
						end
					end
					if collectedweapon == 1 then
						if g_Entity[e]['haskey'] == 1 then
							if addedasweapon == 1 then
								Prompt(weapon_name[e] .. " collected" )
							else
								Prompt("Ammo collected from " .. weapon_name[e] )
							end
						end
						if weapon[e].play_pickup == 1 then
							PlayNon3DSound(e,0)
						end
						if weapon[e].activate_logic == 1 then
							PerformLogicConnections(e)
							ActivateIfUsed(e)
						end
					end
					weapon_therecanbeonlyone = -1
				end
			end
		end
	else
		if weapon_therecanbeonlyone==e then weapon_therecanbeonlyone = 0 end
	end
end
